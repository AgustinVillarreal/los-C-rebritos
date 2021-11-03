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

pthread_mutex_t MUTEX_TP_CARPINCHO;

t_list* TLB_TABLE;

void destroy_memory_structs();
static bool tp_carpincho_has_pid(void* x);
void init_memory_structs();
void list_add_page_frame_tppatotas(unsigned long pid, uint32_t nro_frame, size_t size, bool presente);
static bool has_nro_frame(void* x);
static bool has_nro_frame_swap(void* x);
bool tabla_vacia();
void mutex_init_tablas();

#endif