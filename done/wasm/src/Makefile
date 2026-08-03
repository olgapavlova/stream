
fib: fib.c
	clang \
		--target=wasm32 \
		-O3 \
		-nostdlib \
		-Wl,--no-entry \
		-Wl,--export-all \
		-o $@.wasm \
		$^


hello: hello.c
	emcc \
  		-O0 \
  		-s STANDALONE_WASM=1 \
  		-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
  		-Wl,--export=hello \
  		-o $@.wasm \
		$^


helloclang: hello.c
	clang \
		--target=wasm32 \
		-Wl,--no-entry \
		-Wl,--export-all \
		-o $@.wasm \
		$^

main: main.c
	emcc \
		-o $@.js \
		$^

s:
	python3 -m http.server
