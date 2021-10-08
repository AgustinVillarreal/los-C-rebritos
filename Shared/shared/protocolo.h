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
#include "serializacion.h"

///JAJAJAJJAJAJAJAJAJAJAJAJAJAJAJAJAJAJA

typedef enum {
    HANDSHAKE,
    HANDSHAKE_KERNEL,
    HANDSHAKE_MEMORIA,
    MEM_ALLOC,
    MEM_FREE,
    MEM_READ,
    MEM_WRITE,
} op_code;


bool send_handshake(int fd_server);
bool send_memalloc(int fd_server);
bool send_memwrite(int fd_server);
bool send_memread(int fd_server);
bool send_memfree(int fd_server);
bool send_codigo_op(int fd, op_code cop);
bool send_alloc_data(int fd, unsigned long id, int size);

#endif