#include "creadorusuarios.h"
#define PORT 8081

// int incrementar_contador_usuarios()
// {
//     CURL *curl;
//     CURLcode res;

//     /* In windows, this will init the winsock stuff */
//     curl_global_init(CURL_GLOBAL_ALL);

//     /* get a curl handle */
//     curl = curl_easy_init();
//     if (curl)
//     {
//         /* First set the URL that is about to receive our POST. This URL can
//            just as well be a https:// URL if that is what should receive the
//            data. */
//         curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1/contador/increment");
//         /* Now specify the POST data */
//         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

//         /* Perform the request, res will get the return code */
//         res = curl_easy_perform(curl);
//         /* Check for errors */
//         if (res != CURLE_OK)
//             fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                     curl_easy_strerror(res));

//         /* always cleanup */
//         curl_easy_cleanup(curl);
//     }
//     curl_global_cleanup();
//     return 0;
// }

int callback_usuarios_creados(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    if (leer_headers(request, response))
    {
        return U_CALLBACK_CONTINUE;
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
        return U_CALLBACK_CONTINUE;
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

    char useradd[20] = "useradd ";
    strncat(useradd, user_name, strlen(user_name));
    if (system(useradd))
    {
        ulfius_set_string_body_response(response, 400, "No se pudo crear el usuario\n");
        return U_CALLBACK_CONTINUE;
    }

    char *created_at = get_time_string();
    ulfius_set_json_body_response(response, 200, create_json_respose_useradd(getid((char *)user_name), user_name, created_at));

    //incrementar_contador_usuarios();

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
        getchar();
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