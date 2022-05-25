#include "comun.h"
/*
Funcion loguear toma como parametro un char* y lo loguea en el archivo log.txt
En caso de que no exista lo crea, y en caso de que si exista agrega el nuevo mensaje
El mensaje tomado como argumento es modificado para que tenga la siguiente forma
"<fecha> | <Nombre del servicio> | <mensaje>"
El nombre del servicio se lo toma como argumento y la fecha se la obtiene de la funcion *get_time_string
*/
void loguear(char *mensaje, char *servicio)
{
    FILE *log;
    char *time_string;
    time_string = get_time_string();
    log = fopen("/var/log/tp6_mascasariego_2022/log_tp6", "a");
    if (log == NULL)
    {
        printf("Error al abrir el archivo log.txt\n");
        return;
    }
    fprintf(log, "%s | %s | %s\n", time_string, servicio, mensaje);
    fclose(log);
}

int leer_headers(const struct _u_request *request, struct _u_response *response)
{
    struct _u_map *headers = u_map_copy(request->map_header);
    if (u_map_has_key(headers, "Content-Type"))
    {
        if (strcmp(u_map_get(headers, "Content-Type"), "application/json") == 0)
        {
            return 0;
        }
        else
        {
            ulfius_set_string_body_response(response, 200, "Content-Type no es application/json");
            return -1;
        }
    }
    else
    {
        ulfius_set_string_body_response(response, 200, "No se encontro Content-Type");
        return -1;
    }
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