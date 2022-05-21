CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all : tp6

tp6: obj/main.o
	gcc $(CFLAGS) -o tp6 obj/main.o -lulfius -ljansson

obj/main.o: bin/main.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/main.c -o obj/main.o

clean:
	rm -f obj/*
	rm -f tp6
	rmdir obj