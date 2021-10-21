#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#define SERVERNAME "KERNEL_SERVER"

#include "config.h"
#include "conexion.h"
#include <shared/sockets.h>
#include <shared/protocolo.h>
#include "colas.h"

t_config_kernel* KERNEL_CFG;

static void procesar_conexion(void* void_args);

int server_escuchar(char* server_name, int server_socket, int memoria_fd);

#endif
