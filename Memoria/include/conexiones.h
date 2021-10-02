#ifndef INCLUDE_CONEXION_H_
#define INCLUDE_CONEXION_H_

#define SERVERNAME "MEMORIA_SERVER"

#include <stdbool.h>
#include "config.h"

#include "../../Shared/include/sockets.h"
#include "../../Shared/include/protocolo.h"

t_config_memoria* MEMORIA_CFG;

static void procesar_conexion(void* void_args);

int server_escuchar(char* server_name, int server_socket);

bool generar_conexiones(int*,t_config_kernel*);

#endif