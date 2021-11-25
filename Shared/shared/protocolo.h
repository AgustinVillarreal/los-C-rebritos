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

//JAJAJAJJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJA

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
    CARPINCHO_READY,
    ESCRITURA_SWAMP,
    LECTURA_SWAMP,
    FINALIZAR_CARPINCHO,
    ESPACIO_LIBRE,
    ALLOCAR_EN_SWAP,
    PAGINA_DE_SWAP
} op_code;


bool send_mate_init(int fd_server, int generar_id);
bool send_codigo_op(int fd, op_code cop);

bool send_alloc_data(int fd, unsigned long id, int size);
bool recv_alloc_data(int fd, long* id_carpincho, int* size_data);

bool send_memfree_data(int fd, unsigned long id, uint32_t direccion_logica);
bool recv_memfree_data(int fd, long* id_carpincho, uint32_t* direccion_logica);

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
bool send_memread(int fd_server, uint32_t direccion_logica, int size);
bool send_memwrite(int fd_server, void* data, uint32_t direccion_logica, int size);
bool send_memfree(int fd_server);

bool recv_memread_data(int fd, uint32_t* direccion_logica, int* size);
bool recv_memwrite_data(int fd, uint32_t* direccion_logica, void** data, int* size);

bool send_ack(int fd, bool ack);
bool recv_ack(int fd, bool* ack);
bool send_carpincho_ready(int fd, long id_carpincho);

bool send_finalizar_carpincho(int fd, unsigned long id);

//SWAMP

bool send_pagina(int fd, long carpincho_id, uint32_t nro_pagina, void* data, uint32_t tamanio_pagina);
bool recv_pagina(int fd, long* carpincho_id, uint32_t* nro_pagina, void** data);

bool recv_id(int cliente_socket, unsigned long* carpincho_id);

bool recv_lectura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina);

bool recv_ecritura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina, void *data, uint32_t tamanio_pagina);

bool recv_solicitud_espacio_libre(int cliente_socket, unsigned long* carpincho_id,uint32_t* cant_paginas);

bool recv_allocar(int cliente_socket, unsigned long* carpincho_id, uint32_t* cant_paginas);

bool recv_esquema_asignacion(int cliente_socket, bool* asignacion_fija);
#endif