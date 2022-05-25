#ifndef COMUN_H
#define COMUN_H

#include <jansson.h>
#include <time.h>
#include <ulfius.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#endif

void loguear(char* mensaje, char* servicio);
int leer_headers(const struct _u_request * request, struct _u_response *response);
char *get_time_string();
int parse_json(json_t *root, const char **user_name, const char **user_pass);
int get_cantidad_usuarios(void);