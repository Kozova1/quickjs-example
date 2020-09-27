CC := gcc
CFLAGS := -fPIC -flto -std=gnu99 -O2

.PHONY: build
build: .obj/lib.o quickjs/libquickjs.a
	$(CC) main.c .obj/lib.o QuickJS/libquickjs.a -lm -o main ${CFLAGS}

.obj/lib.o: clean
	$(CC) -c lib.c -o .obj/lib.o ${CFLAGS}

.PHONY: clean
clean:
	-rm .obj/* main

quickjs/libquickjs.a:
	$(MAKE) -C QuickJS
