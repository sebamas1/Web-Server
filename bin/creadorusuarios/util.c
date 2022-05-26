#include "util.h"

// cuenta las cifras del int pasado como argumento y devuelve ese valor
int contar_cifras(int numero)
{
    int contador = 0;
    while (numero != 0)
    {
        numero = numero / 10;
        contador++;
    }
    return contador;
}

// esta funcion imprime el nombre de todos los usuarios presentes en el sistema usando getpwent a
// partir del usuario sebastian
void obtener_usuarios(char **usuarios, int cantidad_usuarios)
{
    struct passwd *pw;
    int i = 0;
    setpwent();
    while (strcmp((pw = getpwent())->pw_name, getpwuid(1000)->pw_name)) //esto no es muy portable a futuras versiones pero que importa
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

json_t *create_json_respose_useradd(int id, const char *username, const char *created_at)
{
    json_t *response = json_object();
    json_object_set_new(response, "id", json_integer(id));
    json_object_set_new(response, "username", json_string(username));
    json_object_set_new(response, "created_at", json_string(created_at));
    return response;
}

// Esta funcion crea un json array donde cada elemento lista username y user_id
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