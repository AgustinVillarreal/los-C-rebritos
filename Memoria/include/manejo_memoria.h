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
#include "algoritmos.h"



bool entra_en_mp(uint32_t size_stream); 
bool esta_en_tlb(unsigned long id_carpincho);
void init_memory_structs();
bool allocar_carpincho_fija(unsigned long id_carpincho, size_t sizes, bool primer_alloc, uint32_t * direccion_logica);
uint32_t cant_paginas(size_t size);
uint32_t cant_frame_libres_fija(unsigned long id_carpincho);
uint32_t cant_frame_libres();
uint32_t buscar_primer_frame_carpincho();

#endif