sf-example.o: sf-example.c
	gcc -c -fPIC sf-example.c -o sf-example.o -I ../

libsf-example.so.1.0.1: sf-example.o
	gcc -shared -Wl,-soname,libsf-example.so.1 -o libsf-example.so.1.0.1  sf-example.o

all: libsf-example.so.1.0.1 Makefile

clean:
	rm libsf-example.so.1.0.1 sf-example.o
