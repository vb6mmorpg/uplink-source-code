CC=gcc -Wall

.PHONY: all

all: normal-test glib-test

normal-test: normal-test.c ../generate.pl ../*.c
	cd .. && ./generate.pl normal tests/normal-binreloc.c tests/normal-binreloc.h
	$(CC) normal-test.c normal-binreloc.c -o normal-test

glib-test: glib-test.c ../generate.pl ../*.c
	cd .. && ./generate.pl glib tests/glib-binreloc.c tests/glib-binreloc.h
	$(CC) glib-test.c glib-binreloc.c -o glib-test `pkg-config --cflags --libs glib-2.0`

clean:
	rm -f *-binreloc.* normal-test glib-test
