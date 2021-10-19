#ifndef INCLUDE_COMUNICACION_H_
#define INCLUDE_COMUNICACION_H_

#define SERVERNAME "MEMORIA_SERVER"

#include <stdbool.h>
#include "config.h"
#include "conexion.h"
#include "interfaz_memoria.h"

#include <pthread.h>

#include <shared/sockets.h>
#include <shared/protocolo.h>

t_config_memoria* MEMORIA_CFG;

static void procesar_conexion(void* void_args);

#endif