Para instalar nginx se debe seguir las instrucciones presentes en [nginx](https://nginx.org/en/linux_packages.html)

## Configuracion del nginx

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


Fuentes:

[Logrotate](https://www.digitalocean.com/community/tutorials/how-to-manage-logfiles-with-logrotate-on-ubuntu-16-04)

[Video explicacion](https://www.youtube.com/watch?v=SI3rHpVXrdc)
