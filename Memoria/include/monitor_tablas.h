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


entrada_tp_t* list_get_pagina(tp_carpincho_t* tabla_carpincho ,uint32_t index);
tp_carpincho_t* find_tp_carpincho(unsigned long id);
bool tabla_vacia(unsigned long id);
void agregar_tabla_a_tp_carpinchos(void* tp_carpincho);
void mutex_init_tabla();
uint32_t table_size(unsigned long id);

#endif