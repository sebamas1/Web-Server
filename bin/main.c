/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <ulfius.h>
#include <string.h>
#include <jansson.h>
//#include "json.h"

#define PORT 8080

/**
 * Callback function for the web application on /helloworld url call
 */

size_t i = 0;

int parse_json(json_t *root, const char **user_name, const char **user_pass){
    if(root){
        json_t *user_name_json = json_object_get(root, "user_name");
        json_t *user_pass_json = json_object_get(root, "password");
        if(user_name_json && user_pass_json){
            *user_name = json_string_value(user_name_json);
            *user_pass = json_string_value(user_pass_json);
            return 0;
        }
    }
    return -1;
}
//funcion que crea un body response en formato json de la forma:
//{
//      "id": 142,
//      "username": "myuser",
//      "created_at": "2019-06-22 02:19:59"
// }
json_t* create_json_response(int id, const char *username, const char *created_at){
    json_t *response = json_object();
    json_object_set_new(response, "id", json_integer(id));
    json_object_set_new(response, "username", json_string(username));
    json_object_set_new(response, "created_at", json_string(created_at));
    return response;
}


size_t contar_cifras_numero(size_t numero_a_contar)
{
    size_t contador = 1;
    size_t num = numero_a_contar;

    while (num / 10 > 0)
    {
        num = num / 10;
        contador++;
    }
    return contador;
}


int callback_usuarios_creados(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    char *inicio_respuesta = "Numero de usuarios nuevos creados: ";
    size_t largo_respuesta = strlen(inicio_respuesta);
    size_t cifras = contar_cifras_numero(i);
    char aux[cifras];
    memset(aux, '\0', cifras);

    char respuesta[largo_respuesta + cifras];
    memset(respuesta, '\0', largo_respuesta + cifras);
    strcat(respuesta, inicio_respuesta);

    sprintf(aux, "%lu", i);
    strcat(respuesta, aux);
    strcat(respuesta, "\n");

    ulfius_set_string_body_response(response, 400, respuesta);

    return U_CALLBACK_CONTINUE;
}

int callback_useradd(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    const char *user_name = NULL;
    const char *user_pass = NULL;

    json_t *mensaje_json = ulfius_get_json_body_request(request, NULL);
    if (mensaje_json == NULL)
    {
        ulfius_set_string_body_response(response, 400, "No se pudo obtener el json del body\n");
        return U_CALLBACK_CONTINUE;
    }

    if(parse_json(mensaje_json, &user_name, &user_pass)){
        ulfius_set_string_body_response(response, 400, "Se debe ingresar usuario y contraseña\n");
        return U_CALLBACK_CONTINUE;
    }

    ulfius_set_json_body_response(response, 200, create_json_response((int) i, user_name, "2019-06-22 02:19:59"));
    i++;
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
    ulfius_add_endpoint_by_val(&instance, "GET", "/contadorusuarios", NULL, 1, &callback_usuarios_creados, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/useradd", NULL, 0, &callback_useradd, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start framework on port %d\n", instance.port);

        // Wait for the user to press <enter> on the console to quit the application
        getchar();
    }
    else
    {
        fprintf(stderr, "Error starting framework\n");
    }
    printf("End framework\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}

// curl -v http://localhost:8080/helloworld