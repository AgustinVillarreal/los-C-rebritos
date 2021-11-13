#ifndef INCLUDE_MONITOR_MEMORIA_H_
#define INCLUDE_MONITOR_MEMORIA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "config.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;

pthread_mutex_t MUTEX_TLB_BUSY;
pthread_mutex_t MUTEX_MP_BUSY;
pthread_mutex_t MUTEX_FRAMES_BUSY;

void mutex_init_memoria();
bool ocupar_frames(unsigned long id);
uint32_t cant_frame_libres();
uint32_t buscar_primer_frame_carpincho(unsigned long id_carpincho);
void primer_memalloc_carpincho(unsigned long id_carpincho, size_t* size_rest, uint32_t direccion_logica, uint32_t nro_frame, uint32_t nro_pagina, uint32_t* hmd_cortado);
uint32_t primer_frame_libre();
bool entra_en_pagina(unsigned long id_carpincho, entrada_tp_t* entrada_tp, size_t size, uint32_t* direccion_hmd, void* hmd_void, uint32_t* tamanio_hmd);
uint32_t buscar_espacio_entre_hmd(unsigned long id_carpincho, size_t size, uint32_t* nro_pagina);
uint32_t cant_paginas_relativa(uint32_t posicion, uint32_t size);
void alloc_hmd(size_t* size_rest, entrada_tp_t* entrada_tp, uint32_t* hmd_cortado, hmd_t* hmd_a_crear, tp_carpincho_t* tp_carpincho);

#endif