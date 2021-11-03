#ifndef INCLUDE_MONITOR_MEMORIA_H_
#define INCLUDE_MONITOR_MEMORIA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

#include "monitor_memoria.h"
#include "config.h"

t_list* TLB_TABLE;

bool entra_en_mp(uint32_t size_stream); 
bool esta_en_tlb(unsigned long id_carpincho);
void init_memory_structs();
uint32_t alloc_carpincho_en_mp(size_t size, unsigned long pid, t_list* tp_carp, bool* nuevapag);
static bool meter_pagina_en_mp(size_t size, unsigned long pid, uint32_t iter, bool offset);


#endif