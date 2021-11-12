#ifndef INCLUDE_COMUNICACION_H_
#define INCLUDE_COMUNICACION_H_

#define SERVERNAME "MEMORIA_SERVER"

#include <stdbool.h>
#include "config.h"
#include "conexion.h"
#include "interfaz_memoria.h"
#include "manejo_memoria.h"

#include <pthread.h>

#include <shared/sockets.h>
#include <shared/protocolo.h>


static void procesar_conexion(void* void_args);
int server_escuchar(char* server_name, int server_fd);

#endif