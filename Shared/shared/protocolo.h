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

//JAJAJAJJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJA

typedef enum {
    HANDSHAKE,
    HANDSHAKE_KERNEL,
    HANDSHAKE_MEMORIA,
    PONER_COLA_NEW,
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
    ESCRITURA_SWAMP,
    LECTURA_SWAMP,
    FINALIZAR_CARPINCHO
} op_code;


bool send_handshake(int fd_server);
bool send_memalloc(int fd_server);
bool send_memwrite(int fd_server);
bool send_memread(int fd_server);
bool send_memfree(int fd_server);

bool send_codigo_op(int fd, op_code cop);

bool send_alloc_data(int fd, unsigned long id, int size);
bool recv_alloc_data(int fd, long* id_carpincho, int* size_data);

bool send_poner_cola_new(int fd);
bool send_data_cola_new(int fd, unsigned long id);

//SEMAFOROS
bool send_sem_init(int fd,  char* sem, unsigned int value);
bool recv_sem_init(int fd, char** sem, int * value);

bool send_sem_init(int fd,  char* sem, unsigned int value);


//MEMORIA
bool send_probar_en_swamp(uint32_t size, unsigned long id);

bool send_ack(int fd, bool ack);
bool recv_ack(int fd, bool* ack);


//SWAmP

bool send_pagina(int fd, long carpincho_id, uint32_t nro_pagina, void* data);
bool recv_pagina(int fd, long* carpincho_id, uint32_t* nro_pagina, void** data);

bool recv_id(int cliente_socket, unsigned long* carpincho_id);

bool recv_lectura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina);

bool recv_ecritura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina, void *data, bool* asigancion_fija);

#endif