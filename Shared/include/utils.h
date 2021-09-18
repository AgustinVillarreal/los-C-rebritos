#ifndef UTILS_H_
#define UTILS_H_

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "frees.h"

uint16_t string_split_len(char**);
bool config_has_all_properties(t_config*, char**);

//Estos tenemos implementados
t_list* extraer_dispositivos(char**);
t_list* extraer_duraciones(char**);

bool string_is_number(char*);

char* leer_archivo_completo(char*);

void ciclo();

char F(); // por si hay que poner F en el chat

#endif
