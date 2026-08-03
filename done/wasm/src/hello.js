export function createHelloWasi() {
  let memory = null;
  const decoder = new TextDecoder("utf-8");

  function setMemory(wasmMemory) { memory = wasmMemory; }

  function view() {
    if (!memory) {
      throw new Error("WASM memory is not initialized");
    }
    return new DataView(memory.buffer);
  }

  function fd_write(fd, iovs, iovs_len, nwritten) {
    const mem = view();

    let text = "";
    let written = 0;

    for (let i = 0; i < iovs_len; i++) {
      const ptr = mem.getUint32(iovs + i * 8, true);
      const len = mem.getUint32(iovs + i * 8 + 4, true);

      const bytes = new Uint8Array(memory.buffer, ptr, len);
      text += decoder.decode(bytes);
      written += len;
    }

    if (fd === 1) {
      console.log(text.trimEnd());
    } else if (fd === 2) {
      console.error(text.trimEnd());
    }

    mem.setUint32(nwritten, written, true);
    return 0;
  }

  function args_sizes_get(argcPtr, argvBufSizePtr) {
    const mem = view();

    mem.setUint32(argcPtr, 0, true);
    mem.setUint32(argvBufSizePtr, 0, true);

    return 0;
  }

  function args_get(argvPtr, argvBufPtr) { return 0; }

  function environ_sizes_get(environCountPtr, environBufSizePtr) {
    const mem = view();

    mem.setUint32(environCountPtr, 0, true);
    mem.setUint32(environBufSizePtr, 0, true);

    return 0;
  }

  function environ_get(environPtr, environBufPtr) { return 0; }

  function proc_exit(code) { throw new Error(`WASI proc_exit(${code})`); }

  function random_get(bufPtr, bufLen) {
    const bytes = new Uint8Array(memory.buffer, bufPtr, bufLen);
    crypto.getRandomValues(bytes);
    return 0;
  }

  function clock_time_get(clockId, precision, timePtr) {
    const mem = view();

    const nowNs = BigInt(Date.now()) * 1000000n;
    mem.setBigUint64(timePtr, nowNs, true);

    return 0;
  }

  function fd_close(fd) { return 0; }

  function fd_seek(fd, offsetLow, offsetHigh, whence, newOffsetPtr) {
    const mem = new DataView(memory.buffer);
    mem.setUint32(newOffsetPtr, 0, true);     // low
    mem.setUint32(newOffsetPtr + 4, 0, true); // high
    return 0;
  }

  return {
    imports : {
      wasi_snapshot_preview1 : {
        fd_write,
        fd_close,
        fd_seek,
        args_sizes_get,
        args_get,
        environ_sizes_get,
        environ_get,
        proc_exit,
        random_get,
        clock_time_get,
      },
      env : {
        __main_argc_argv : () => 0,
      },
    },
    setMemory,
  };
}
