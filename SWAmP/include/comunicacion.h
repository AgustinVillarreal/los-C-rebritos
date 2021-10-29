#ifndef COMUNICACION_SWAMP_H_
#define COMUNICACION_SWAMP_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include <commons/config.h>

#include "config.h"

#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

int server_escuchar(char* server_name, int server_socket);

#endif
