CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all : tp6

config: 
	mkdir -p /var/log/tp6_mascasariego_2022
	cp conf/tp6_mascasariego_2022 /etc/logrotate.d
	cp conf/tp6.com /etc/nginx/sites-available/
	cp conf/contadorusuarios.com /etc/nginx/sites-available/
	cd /etc/nginx/sites-enabled/
	ln -s ../sites-available/tp6.com
	ln -s ../sites-available/contadorusuarios.com
	nginx -s reload

tp6: obj/main.o
	gcc $(CFLAGS) -o tp6 obj/main.o -lulfius -ljansson

obj/main.o: bin/main.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/main.c -o obj/main.o

clean:
	rm -f obj/*
	rm -f tp6
	rmdir obj