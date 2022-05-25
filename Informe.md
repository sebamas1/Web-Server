## Lo que falta

- Falta sacar el hardcodeo ese de que se leen los usuarios a partir de "sebastian"
- Falta probar lo del SSH con el usuario




## Requerimientos

Se deben tener las siguientes herramientas instaladas para poder compilar y correr el programa.

- nginx
- curl
- system D
- logrotation

## Configuracion del nginx

Para instalar nginx se debe seguir las instrucciones presentes en [nginx](https://nginx.org/en/linux_packages.html)

Primero se entra a /etc/nginx/sites-available y se debe crear 1 archivo por cada servidor que se desea hacer.

~~~
cd /etc/nginx/sites-available
cp default tp6.com
cp default contadorusuarios.com 
~~~

En este caso se creo el archivo tp6.com y contadorusuarios.com copiando el archivo que viene por defecto y luego modificando estos dos archivos segun se desea. Luego de configurar para cada archivo el nombre del servidor y el puerto al que escucha(que va a ser el puerto 80) y de configurar de donde saca su index.html, se le configura el proxy pass, para redireccionar las rutas a los endpoints de ulfius.
Una vez hecho esto se debe configurar estos mismos archivos como links simbolicos dentro de la carpeta sites-enabled, que se hace con el comando

~~~
cd sites-enabled
ln -s ../sites-available/tp6.com
ln -s ../sites-available/contadorusuarios.com
~~~

Luego se cargan las configuraciones de nginx con 

~~~
nginx -s reload
~~~

Ademas se le debe decir al host local que cuando intente resolver los nombres de **tp6.com** y **contadorusuarios.com** los redireccione a la direccion de loopback, eso se hace con el comando

~~~
nano /etc/hosts
~~~

Y luego escribe manualmente el dominio y la direccion a la que se lo quiere asociar.

Fuentes:

[Config inicial de nginx y de sus servidores](https://www.youtube.com/watch?v=_LQv96MdtCk)

[Proxy pass](https://www.youtube.com/watch?v=fVM658GMbTo)

## Configuracion del logrotation

Se creo un archivo dentro de la carpeta conf llamado tp6_mascasariego_2022 que es la configuracion necesaria para el logrotation. Este archivo es copiado en /etc/logrotate.d, y cuando el sistema operativo realice un logrotation programado, va a ejecutar la configuracion copiada. Esta configuracion rota el log guardado en /var/log/tp6_mascasariego_2022.
La creacion de las carpetas necesarias y de la configuracion es realizada por el makefile, ejecutando el comando:

~~~
make config
~~~
**Es posible que se deba usar esto con privilegios de sudo**

Para forzar una rotacion de log en debug:

~~~
sudo logrotate /etc/logrotate.conf --debug
~~~

Los logs son guardados en **/var/log/tp6_mascasariego_2022**.


Fuentes:

[Logrotate](https://www.digitalocean.com/community/tutorials/how-to-manage-logfiles-with-logrotate-on-ubuntu-16-04)

[Video explicacion](https://www.youtube.com/watch?v=SI3rHpVXrdc)

## Configuracion de systemd

Se crearon 2 nuevos archivos de servicio que son los que toma el systemd para iniciar los programas.
Estos dos archivos se copian mediante el makefile en **/etc/systemd/system** con lo que ahora systemd 
sabe que archivo de configuracion abrir para iniciar los programas. 
Los ejecutables deben estar en una carpeta que este presente en todos los linux, por lo que se eligio poner los ejecutables en **/usr/local** y crear ahi una carpeta para guardar los ejecutables.

Una vez puestos los ejecutables en /usr/local y los archivos de configuracion en /etc/systemd/system, se usaran 4 comandos para manejar el servicio

- systemctl start <nombre_servicio> -> este comando inicia el servicio
- systemctl stop <nombre_servicio> -> detiene el servicio
- systemctl enable <nombre_servicio> -> habilita el servicio para que se inicie al encender la computadora
- systemctl disable <nombre_servicio> -> deshabilita el servicio y ya no es ejecutado al inicio de la computadora

Es posible usar el comando **systemctl status <nombre_servicio>** te muestra info sobre el servicio

Los nombres de los servicios es 
- contadorusuarios_mascasariego
- creadorusuarios_mascasariego
