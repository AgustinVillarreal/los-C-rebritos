#ifndef PROTOCOLO_H_

#define PROTOCOLO_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/log.h>

#include "utils.h"
#include "frees.h"

///JAJAJAJJAJAJAJAJAJAJAJAJA

typedef enum {
    HANDSHAKE,
    HANDSHAKE_KERNEL,
    HANDSHAKE_MEMORIA,
    HANDSHAKE_SWAMP,
    MEM_ALLOC,
    GUARDAR_SWAMP,
    PEDIR_SWAMP
} op_code;


bool send_handshake(int fd_server);
bool send_memalloc(int fd_server);
bool send_codigo_op(int fd, op_code cop);

#endif