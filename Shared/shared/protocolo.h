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

//JAJAJAJJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJA

typedef enum {
    HANDSHAKE,
    HANDSHAKE_KERNEL,
    HANDSHAKE_MEMORIA,
    MEM_FREE,
    MEM_READ,
    MEM_WRITE,
    SEM_INIT,
    SEM_WAIT,
    SEM_POST,
    SEM_DESTROY,
    FREE_CARPINCHO,
    HANDSHAKE_SWAMP,
    MEM_ALLOC,
    GUARDAR_SWAMP,
    PEDIR_SWAMP,
    MATE_INIT,
    IO,
    CARPINCHO_SWAP,
} op_code;


bool send_mate_init(int fd_server);
bool send_codigo_op(int fd, op_code cop);

bool send_alloc_data(int fd, unsigned long id, int size);
bool recv_alloc_data(int fd, long* id_carpincho, int* size_data);

bool send_data_mate_init(int fd, unsigned long id);

//SEMAFOROS
bool recv_sem_init(int fd, char** sem, int * value);
bool send_sem_init(int fd,  char* sem, unsigned int value);

bool send_sem(int fd, char* sem);
bool recv_sem(int fd, char** sem);

//IO
bool send_io(int fd, char* io, char* msg);


//MEMORIA
bool send_probar_en_swamp(uint32_t size, unsigned long id);
bool send_handshake(int fd_server);
bool send_memalloc(int fd_server);
bool send_memwrite(int fd_server);
bool send_memread(int fd_server);
bool send_memfree(int fd_server);

bool send_ack(int fd, bool ack);
bool recv_ack(int fd, bool* ack);

#endif