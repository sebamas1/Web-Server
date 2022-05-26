#ifndef UTIL_H
#define UTIL_H

#include <jansson.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>

#endif

int contar_cifras(int numero);
void obtener_usuarios(char **usuarios, int cantidad_usuarios);
int getid(char *name);
int validar_password(const char *password);
int validar_usuario(const char *usuario);
json_t *create_json_respose_useradd(int id, const char *username, const char *created_at);
json_t *create_json_array_usuarios(char **usuarios, int cantidad_usuarios);