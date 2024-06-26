#include "creadorusuarios.h"
#define PORT 8081

size_t write_data(__attribute__((unused)) void *buffer, size_t size, __attribute__((unused)) size_t nmemb, __attribute__((unused)) void *userp)
{
    return size * nmemb;
}

/*
Esta funcion hace un POST a incrementar del servicio contador. Se envia un json con la propiedad ip
del cliente de origen. Ademas, se setean los headers apropiados para el servicio contador.
*/

int incrementar_contador_usuarios(const char* ip_cliente)
{
    int ret = 0;
    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl)
    {

        char jsonObj[strlen("{\n\t\"ip\":\"") + 20];
        memset(jsonObj, 0, sizeof(jsonObj));
        strcat(jsonObj, "{\"ip\":\"");
        strncat(jsonObj, ip_cliente, strlen(ip_cliente));
        strcat(jsonObj, "\"\n}");
        
        char* json_enviar = jsonObj;

        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        curl_easy_setopt(curl, CURLOPT_URL, "http://contadorusuarios.com/contador/increment");
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_enviar); // aca meterias los parametros

        // setea curl para que no imprima en stdout
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1); // si no encuentra la pagina falla

        //setea la solicitud en application/json
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        // //agrega la ip real como un header
        // char ip_header[strlen("X-Real-IP: ") + 20];
        // memset(ip_header, 0, sizeof(ip_header));
        // strcat(ip_header, "X-Real-IP: ");
        // strncat(ip_header, ip_cliente, strlen(ip_cliente));
        // headers = curl_slist_append(headers, ip_header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            // fprintf(stderr, "curl_easy_perform() failed: %s\n",
            //         curl_easy_strerror(res));
            ret = -1;
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return ret;
}

int callback_usuarios_creados(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    if (leer_headers(request, response))
    {
        ulfius_set_string_body_response(response, 500, "Error al leer headers");
        return U_CALLBACK_ERROR;
    }
    int cantidad_usuarios = get_cantidad_usuarios();
    char *usuarios[cantidad_usuarios];

    obtener_usuarios(usuarios, cantidad_usuarios);

    json_t *response_json = create_json_array_usuarios(usuarios, cantidad_usuarios);

    ulfius_set_json_body_response(response, 200, response_json);

    // libera la memoria de los usuarios
    for (int i = 0; i < cantidad_usuarios; i++)
    {
        free(usuarios[i]);
    }

    int cifras = contar_cifras(cantidad_usuarios);
    char mensaje_log[(int)strlen("Usuarios creados: ") + cifras + 1];
    strcpy(mensaje_log, "Usuarios creados: ");
    sprintf(mensaje_log + strlen("Usuarios creados: "), "%d", cantidad_usuarios);
    loguear(mensaje_log, "<usuarios_creados>");

    return U_CALLBACK_CONTINUE;
}


int callback_useradd(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    if (leer_headers(request, response))
    {
        return U_CALLBACK_ERROR;
    }
    const char *user_name = NULL;
    const char *user_pass = NULL;

    json_t *mensaje_json = ulfius_get_json_body_request(request, NULL);
    if (mensaje_json == NULL)
    {
        ulfius_set_string_body_response(response, 400, "No se pudo obtener el json del body\n");
        return U_CALLBACK_CONTINUE;
    }

    if (parse_json(mensaje_json, &user_name, &user_pass))
    {
        ulfius_set_string_body_response(response, 400, "Se debe ingresar usuario y contraseña\n");
        return U_CALLBACK_CONTINUE;
    }
    if (validar_usuario(user_name))
    {
        ulfius_set_string_body_response(response, 400, "El nombre de usuario no puede contener caracteres especiales y debe tener menos de 10 caracteres.\n");
        return U_CALLBACK_CONTINUE;
    }
    if (validar_password(user_pass))
    {
        ulfius_set_string_body_response(response, 400, "La contraseña debe tener entre 5 y 10 numeros y debe estar compuesta unicamente por caracteres numericos\n");
        return U_CALLBACK_CONTINUE;
    }

    char useradd[20] = "useradd -m ";
    strncat(useradd, user_name, strlen(user_name));
    if (system(useradd))
    {
        ulfius_set_string_body_response(response, 400, "No se pudo crear el usuario\n");
        return U_CALLBACK_CONTINUE;
    }
    //concatena la contraseña junto a los comandos como se muestra en la siguiente linea
    //echo -e "contraseña\ncontraseña\n" | passwd Roberto
    //ademas concatena al final > /dev/null 2>&1 para que no muestre NADA en consola
    char passwd[strlen("echo \"") + 2*(strlen(user_pass)) + strlen("passwd ") + strlen(user_name) + strlen(" > /dev/null 2>&1")];
    memset(passwd, 0, sizeof(passwd));
    strcat(passwd, "echo \"");
    strncat(passwd, user_pass, strlen(user_pass));
    strcat(passwd, "\n");
    strncat(passwd, user_pass, strlen(user_pass));
    strcat(passwd, "\n\" | passwd ");
    strncat(passwd, user_name, strlen(user_name));
    strcat(passwd, " > /dev/null 2>&1");
    if(system(passwd))
    {
        ulfius_set_string_body_response(response, 400, "No se pudo cambiar la contraseña\n");
        return U_CALLBACK_CONTINUE;
    }

    char *created_at = get_time_string();
    ulfius_set_json_body_response(response, 200, create_json_respose_useradd(getid((char *)user_name), user_name, created_at));

   
   
       //obtiene el ip real del cliente leyendo la cabecera de nginx
    const char *ip_cliente = u_map_get(request->map_header, "X-Real-IP");

    if (incrementar_contador_usuarios(ip_cliente))
    {
        loguear("No se pudo incrementar el contador porque el servidor esta caido o el formato de envio es incorrecto", "<useradd>");
    }

    // convierte el id en un string
    int cifras = contar_cifras(getid((char *)user_name)) + 1;
    char id[cifras];
    sprintf(id, "%d", getid((char *)user_name));
    // concatena Usuario <id> creado
    char usuario_creado[strlen("Usuario ") + strlen(id) + strlen(" creado") + 1];
    strcpy(usuario_creado, "Usuario ");
    strncat(usuario_creado, id, strlen(id));
    strcat(usuario_creado, " creado");

    loguear(usuario_creado, "<useradd>");

    return U_CALLBACK_CONTINUE;
}

int main(void)
{
    struct _u_instance instance;

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK)
    {
        fprintf(stderr, "Error ulfius_init_instance, abort\n");
        return (1);
    }

    // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", "/api/users", NULL, 1, &callback_usuarios_creados, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/api/users", NULL, 0, &callback_useradd, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start framework on port %d creadorusuarios\n", instance.port);

        // Wait for the user to press <enter> on the console to quit the application
        while(1){
            
        }
        //getchar();
    }
    else
    {
        fprintf(stderr, "Error starting framework\n");
    }
    printf("End framework creadorusuarios\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}