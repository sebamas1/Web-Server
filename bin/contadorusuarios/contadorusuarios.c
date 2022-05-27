#include "contadorusuarios.h"
#define PORT 8080
int numero_usuarios = 0;

int callback_contador_value(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    if (leer_headers(request, response))
    {
        return U_CALLBACK_CONTINUE;
    }
    json_t *response_json = json_object();
    json_object_set_new(response_json, "description", json_integer((long long int)numero_usuarios));
    ulfius_set_json_body_response(response, 200, response_json);
    return U_CALLBACK_CONTINUE;
}

int callback_contador_increment(__attribute__((unused)) const struct _u_request *request, __attribute__((unused)) struct _u_response *response, __attribute__((unused)) void *user_data)
{
    if (leer_headers(request, response))
    {
        return U_ERROR_NOT_FOUND;
    }
    numero_usuarios++;

    //obtiene el json del header y luego lee el valor de la propiedad "ip"
    json_error_t* error = NULL;
    json_t *mensaje_json = ulfius_get_json_body_request(request, error);
    
    if (mensaje_json == NULL)
    {
        ulfius_set_string_body_response(response, 400, "No se pudo obtener el json del body\n");
        return U_CALLBACK_CONTINUE;
    }
    const char *ip_cliente = json_string_value(json_object_get(mensaje_json, "ip"));

    // concatena Contador incrementado desde: y el ip del cliente
    char mensaje_log[(int)strlen("Contador incrementado desde: ") + INET_ADDRSTRLEN + 1];
    strcpy(mensaje_log, "Contador incrementado desde: ");
    strcat(mensaje_log, ip_cliente);
    loguear(mensaje_log, "<contador_increment>");

    return U_CALLBACK_CONTINUE;
}

int main(void)
{
    numero_usuarios = get_cantidad_usuarios();
    struct _u_instance instance;

    // Initialize instance with the port number
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK)
    {
        fprintf(stderr, "Error ulfius_init_instance, abort\n");
        return (1);
    }

    // Endpoint list declaration
    ulfius_add_endpoint_by_val(&instance, "GET", "/contador/value", NULL, 0, &callback_contador_value, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/contador/increment", NULL, 0, &callback_contador_increment, NULL);

    // Start the framework
    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start framework on port %d contadorusuarios\n", instance.port);
        while(1){
            
        }

        // Wait for the user to press <enter> on the console to quit the application
        getchar();
    }
    else
    {
        fprintf(stderr, "Error starting framework\n");
    }
    printf("End framework contadorusuarios\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}