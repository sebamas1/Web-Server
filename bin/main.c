/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <ulfius.h>
#include <string.h>

#define PORT 8080

/**
 * Callback function for the web application on /helloworld url call
 */

size_t i = 0;

size_t contar_cifras_numero(size_t numero_a_contar)
{
    size_t contador=1;
    size_t num = numero_a_contar;
 
    while(num/10>0)
    {
        num = num/10;
        contador++;
    }
    return contador;
}

int callback_imprimir(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    char *inicio_respuesta = "El contador esta en el valor de: ";
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


int callback_incrementar(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    size_t incremento = (unsigned) atol(u_map_get(request->map_post_body, "incremento"));

    if(incremento == 0)
    {
        ulfius_set_string_body_response(response, 400, "No se pudo obtener el incremento, o el mismo es nulo\n");
        return U_CALLBACK_CONTINUE;
    }
    
    char *inicio_respuesta = "Incrementando en ";
    size_t largo_respuesta = strlen(inicio_respuesta);
    size_t cifras = contar_cifras_numero(incremento);
    char aux[incremento];
    memset(aux, '\0', cifras);

    char respuesta[largo_respuesta + cifras];
    memset(respuesta, '\0', largo_respuesta + cifras);
    strcat(respuesta, inicio_respuesta);
    
    sprintf(aux, "%lu", incremento);
    strcat(respuesta, aux);


    strcat(respuesta, " el contador...\n");

    ulfius_set_string_body_response(response, 400, respuesta);
    i += incremento;
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
    ulfius_add_endpoint_by_val(&instance, "GET", "/contadorusuarios", NULL, 1, &callback_imprimir, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/useradd", NULL, 0, &callback_incrementar, NULL);

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