CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all : creadorusuarios contadorusuarios

config: 
	mkdir -p /var/log/tp6_mascasariego_2022
	cp conf/tp6_mascasariego_2022 /etc/logrotate.d
	cp conf/tp6.com /etc/nginx/sites-available/
	cp conf/contadorusuarios.com /etc/nginx/sites-available/
	ln -s /etc/nginx/sites-available/tp6.com /etc/nginx/sites-enabled/tp6.com
	ln -s /etc/nginx/sites-available/contadorusuarios.com /etc/nginx/sites-enabled/contadorusuarios.com
	nginx -s reload

creadorusuarios: obj/creadorusuarios.o obj/util.o obj/comun.o
	gcc $(CFLAGS) -o creadorusuarios $^ -lulfius -ljansson -lcurl

contadorusuarios: obj/contadorusuarios.o obj/comun.o
	gcc $(CFLAGS) -o contadorusuarios $^ -lulfius -ljansson

obj/creadorusuarios.o: bin/creadorusuarios/creadorusuarios.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/creadorusuarios/creadorusuarios.c -o obj/creadorusuarios.o

obj/contadorusuarios.o: bin/contadorusuarios/contadorusuarios.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/contadorusuarios/contadorusuarios.c -o obj/contadorusuarios.o

obj/comun.o: bin/comun/comun.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/comun/comun.c -o obj/comun.o

obj/util.o: bin/creadorusuarios/util.c
	mkdir -p obj
	gcc $(CFLAGS) -c bin/creadorusuarios/util.c -o obj/util.o

clean:
	rm -f -r obj/*
	rm -f tp6
	rm -f tp6.com
	rm -f contadorusuarios.com
	rm -f contadorusuarios
	rm -f creadorusuarios
	rm /etc/nginx/sites-enabled/tp6.com
	rm /etc/nginx/sites-enabled/contadorusuarios.com
	rm /etc/nginx/sites-available/tp6.com
	rm /etc/nginx/sites-available/contadorusuarios.com
	rmdir obj