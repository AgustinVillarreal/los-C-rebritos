#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "config.h"
#include "conexion.h"
#include "../../Shared/include/sockets.h"

t_config_kernel* KERNEL_CFG;

char* SERVERNAME = "KERNEL_SERVER";

int server_escuchar(char* server_name, int server_socket);

#endif
