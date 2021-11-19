#ifndef INCLUDE_MONITOR_TABLAS_H_
#define INCLUDE_MONITOR_TABLAS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/collections/list.h>

#include "estructuras.h"
#include "config.h"

pthread_mutex_t MUTEX_TLB_BUSY;
pthread_mutex_t MUTEX_GLOBAL_TUR;
pthread_mutex_t MUTEX_TP_CARPINCHOS;


entrada_tp_t* list_get_pagina(tp_carpincho_t* tabla_carpincho ,uint32_t index);
tp_carpincho_t* find_tp_carpincho(unsigned long id);
bool tabla_vacia(unsigned long id);
void agregar_tabla_a_tp_carpinchos(void* tp_carpincho);
void mutex_init_tabla();
uint32_t table_size(unsigned long id);
void append_frame_tp(unsigned long id, uint32_t nro_pagina, uint32_t nro_frame);
bool buscar_en_TLB(unsigned long id_carpincho, uint32_t nro_pagina, entrada_tp_t* entrada_buscada);

void actualizar_bits(entrada_tp_t* entrada_tp, bool se_modifica);

#endif