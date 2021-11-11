#ifndef INCLUDE_CONEXION_H_
#define INCLUDE_CONEXION_H_

#include <stdbool.h>
#include "config.h"
#include <shared/sockets.h>
bool generar_conexion(int*,t_config_memoria*);
void data_destroy(char*,char*,t_config_memoria*);

#endif