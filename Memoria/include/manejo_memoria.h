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



bool entra_en_mp(uint32_t size_stream); 
bool esta_en_tlb(unsigned long id_carpincho);
void init_memory_structs();
uint32_t allocar_carpincho_fija(unsigned long id_carpincho, size_t size);
uint32_t cant_paginas(size_t size);
uint32_t cant_frame_libres_fija(unsigned long id_carpincho);

#endif