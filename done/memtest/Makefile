$(CC) = gcc

%.run: %.o goal.o
	gcc $*.o goal.o -o $*.run

wrap: once.o goal.o wrapmalloc.o
	gcc $^ -Wl,--wrap=malloc -o $@.run

static: once.o
	gcc -static once.o -o once.run

dsi: once.run
	gcc -shared -fPIC dsi.c -o libdsi.so
	ld.so --preload ./libdsi.so ./once.run
	LD_PRELOAD=./libdsi.so ./once.run
	./once.run

dls: twice.o goal.o flag.o
	gcc -shared -fPIC flag.c -o libflag.so
	gcc -shared -fPIC dls.c -L. -lflag -o libdls.so
	gcc twice.o goal.o -L. -lflag -o twice.run
	LD_LIBRARY_PATH=. ld.so --preload ./libdls.so ./twice.run
	LD_LIBRARY_PATH=. LD_PRELOAD=./libdls.so ./twice.run
	LD_LIBRARY_PATH=. ./twice.run

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.run
