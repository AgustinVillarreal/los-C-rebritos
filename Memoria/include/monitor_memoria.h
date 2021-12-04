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

pthread_mutex_t MUTEX_MP_BUSY;
pthread_mutex_t MUTEX_FRAMES_BUSY;
pthread_mutex_t MUTEX_ALGORITMOS;
pthread_mutex_t MUTEX_ALGORITMOS_TLB;


void mutex_init_memoria();
bool no_tiene_frames(unsigned long id);
bool ocupar_frames(unsigned long id);
uint32_t cant_frame_libres();
uint32_t buscar_primer_frame_carpincho(unsigned long id_carpincho);
void primer_memalloc_carpincho(unsigned long id_carpincho, size_t* size_rest, entrada_tp_t* entrada_tp, uint32_t* hmd_cortado);
uint32_t primer_frame_libre();

uint32_t buscar_espacio_entre_hmd(unsigned long id_carpincho, size_t size, uint32_t* nro_pagina);
uint32_t cant_paginas_relativa(uint32_t posicion, size_t size);
void escritura_memcpy_size(void* data, entrada_tp_t* entrada_tp, uint32_t offset, size_t size);
void lectura_memcpy_size(entrada_tp_t* entrada_tp, uint32_t offset, void* destino, size_t size);
void quitar_paginas_a_carpinchos(unsigned long  id_carpincho, uint32_t cantidad_paginas_finales, int swap_fd);
void lectura_pagina_completa(entrada_tp_t* entrada_tp, void* destino);
void suspender_frame(uint32_t nro_frame);

#endif