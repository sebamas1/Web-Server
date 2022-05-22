/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <ctype.h>
#include <ulfius.h>
#include <string.h>
#include <jansson.h>

#include <errno.h>
#include <sys/types.h>
#include <pwd.h>

#define PORT 8080
int numero_usuarios = 0;

// esta funcion imprime el nombre de todos los usuarios presentes en el sistema usando getpwent a
// partir del usuario sebastian
void obtener_usuarios(char **usuarios, int cantidad_usuarios)
{
    struct passwd *pw;
    int i = 0;
    setpwent();
    while (strcmp((pw = getpwent())->pw_name, "sebastian"))
    {
    }
    do
    {
       // usuarios[i] = pw->pw_name;
        usuarios[i] = malloc(strlen(pw->pw_name) + 1);
        sprintf(usuarios[i], "%s", pw->pw_name);
        i++;

    } while ((pw = getpwent()) != NULL && i < cantidad_usuarios);
    endpwent();
}

int get_cantidad_usuarios(void)
{
    struct passwd *pw;
    int i = 0;
    setpwent();
    while (strcmp((pw = getpwent())->pw_name, "sebastian"))
    {
    }
    do
    {
        i++;
    } while ((pw = getpwent()) != NULL);
    endpwent();
    return i;
}

int getid(char *name)
{
    struct passwd *p;
    if ((p = getpwnam(name)) == NULL)
    {
        perror(name);
        return EXIT_FAILURE;
    }
    return (int)p->pw_uid;
}

// devuelve un string con año-mes-dia hora:minuto:segundo
char *get_time_string()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *time_string = malloc(sizeof(char) * 20);
    sprintf(time_string, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return time_string;
}

int validar_password(const char *password)
{
    size_t largo_password;
    largo_password = strlen(password);
    if (largo_password < 5)
    {
        return -1;
    }
    if (largo_password > 10)
    {
        return -2;
    }
    for (size_t i = 0; i < largo_password; i++)
    {
        if (!isdigit(password[i]))
        {
            return -1;
        }
    }

    if (strchr(password, ' ') != NULL)
    {
        return -1;
    }
    return 0;
}

int validar_usuario(const char *usuario)
{
    size_t largo_usuario;
    char *caracteres_prohibidos = "\\/:*?\"<>|&;";
    largo_usuario = strlen(usuario);
    if (largo_usuario > 10)
    {
        return -1;
    }
    for (size_t i = 0; i < largo_usuario; i++)
    {
        if (strchr(caracteres_prohibidos, usuario[i]) != NULL)
        {
            return -1;
        }
    }
    return 0;
}

int parse_json(json_t *root, const char **user_name, const char **user_pass)
{
    if (root)
    {
        json_t *user_name_json = json_object_get(root, "user_name");
        json_t *user_pass_json = json_object_get(root, "password");
        if (user_name_json && user_pass_json)
        {
            *user_name = json_string_value(user_name_json);
            *user_pass = json_string_value(user_pass_json);
            return 0;
        }
    }
    return -1;
}
// funcion que crea un body response en formato json de la forma:
//{
//       "id": 142,
//       "username": "myuser",
//       "created_at": "2019-06-22 02:19:59"
//  }
json_t *create_json_respose_useradd(int id, const char *username, const char *created_at)
{
    json_t *response = json_object();
    json_object_set_new(response, "id", json_integer(id));
    json_object_set_new(response, "username", json_string(username));
    json_object_set_new(response, "created_at", json_string(created_at));
    return response;
}
//Esta funcion crea un json array donde cada elemento lista username y user_id
json_t *create_json_array_usuarios(char **usuarios, int cantidad_usuarios)
{
    json_t *array = json_array();
    for (int i = 0; i < cantidad_usuarios; i++)
    {
        json_t *element = json_object();
        json_object_set_new(element, "username", json_string(usuarios[i]));
        json_object_set_new(element, "user_id", json_integer(getid(usuarios[i])));
        json_array_append_new(array, element);
    }
    return array;
}

int callback_usuarios_creados(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data)
{
    int cantidad_usuarios = get_cantidad_usuarios();
    char * usuarios[cantidad_usuarios];

    obtener_usuarios(usuarios, cantidad_usuarios);

    json_t *response_json = create_json_array_usuarios(usuarios, cantidad_usuarios);

    ulfius_set_json_body_response(response, 200, response_json);

    return U_CALLBACK_CONTINUE;
}

int callback_contador_increment(__attribute__((unused)) const struct _u_request *request, __attribute__((unused)) struct _u_response *response, __attribute__((unused)) void *user_data){
    numero_usuarios++;
    // json_t *response_json = json_object();
    // json_object_set_new(response_json, "description", json_integer((long long int) numero_usuarios));
    // ulfius_set_json_body_response(response, 200, response_json);
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
    char *created_at = get_time_string();

    ulfius_set_json_body_response(response, 200, create_json_respose_useradd(getid("sebastian"), user_name, created_at));

    char useradd[20] = "useradd ";
    strncat(useradd, user_name, strlen(user_name));
    if(system(useradd)){
        ulfius_set_string_body_response(response, 400, "No se pudo crear el usuario\n");
        return U_CALLBACK_CONTINUE;
    }

    callback_contador_increment(request, response, (void*) NULL);
    
    return U_CALLBACK_CONTINUE;
}
int callback_contador_value(__attribute__((unused)) const struct _u_request *request, struct _u_response *response, __attribute__((unused)) void *user_data){
    json_t *response_json = json_object();
    json_object_set_new(response_json, "description", json_integer((long long int) numero_usuarios));
    ulfius_set_json_body_response(response, 200, response_json);
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
    ulfius_add_endpoint_by_val(&instance, "GET", "/api/users", NULL, 1, &callback_usuarios_creados, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/api/users", NULL, 0, &callback_useradd, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/contador/value", NULL, 0, &callback_contador_value, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/contador/increment", NULL, 0, &callback_contador_increment, NULL);

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