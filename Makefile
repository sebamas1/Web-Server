CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11

all : /usr/local/tp6_mascasariego/creadorusuarios /usr/local/tp6_mascasariego/contadorusuarios

config: 
	mkdir -p /var/log/tp6_mascasariego_2022
	cp conf/tp6_mascasariego_2022 /etc/logrotate.d
	cp conf/tp6.com /etc/nginx/sites-available/
	cp conf/contadorusuarios.com /etc/nginx/sites-available/
	ln -s /etc/nginx/sites-available/tp6.com /etc/nginx/sites-enabled/tp6.com
	ln -s /etc/nginx/sites-available/contadorusuarios.com /etc/nginx/sites-enabled/contadorusuarios.com
	nginx -s reload
	grep -qxF '127.0.0.1 	tp6.com' /etc/hosts || echo '127.0.0.1 	tp6.com' >> /etc/hosts
	grep -qxF '127.0.0.1	contadorusuarios.com' /etc/hosts || echo '127.0.0.1	contadorusuarios.com' >> /etc/hosts
	cp conf/contadorusuarios_mascasariego.service /etc/systemd/system
	cp conf/creadorusuarios_mascasariego.service /etc/systemd/system
	systemctl daemon-reload
	

/usr/local/tp6_mascasariego/creadorusuarios: obj/creadorusuarios.o obj/util.o obj/comun.o
	mkdir -p /usr/local/tp6_mascasariego
	gcc $(CFLAGS) -o /usr/local/tp6_mascasariego/creadorusuarios $^ -lulfius -ljansson -lcurl

/usr/local/tp6_mascasariego/contadorusuarios: obj/contadorusuarios.o obj/comun.o
	mkdir -p /usr/local/tp6_mascasariego
	gcc $(CFLAGS) -o /usr/local/tp6_mascasariego/contadorusuarios $^ -lulfius -ljansson

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
	rm -f contadorusuarios
	rm -f creadorusuarios
	rm /etc/nginx/sites-enabled/tp6.com
	rm /etc/nginx/sites-enabled/contadorusuarios.com
	rm /etc/nginx/sites-available/tp6.com
	rm /etc/nginx/sites-available/contadorusuarios.com
	sed -i '/127.0.0.1 	tp6.com/d' /etc/hosts
	sed -i '/127.0.0.1	contadorusuarios.com/d' /etc/hosts
	rm /usr/local/tp6_mascasariego/creadorusuarios
	rm /usr/local/tp6_mascasariego/contadorusuarios
	rm /etc/systemd/system/creadorusuarios_mascasariego.service
	rm /etc/systemd/system/contadorusuarios_mascasariego.service
	rmdir /usr/local/tp6_mascasariego
	rmdir obj