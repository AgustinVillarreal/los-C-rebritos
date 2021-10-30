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
#include "main.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;
 

uint32_t buscar_first_fit();
uint32_t cant_frames_libres();
uint32_t cant_paginas(uint32_t size, size_t* rem);

void* (*algoritmo_reemplazo_mmu) (void);
void* algoritmo_mmu_clock_m ();
void* algoritmo_mmu_lru();

void* (*algoritmo_reemplazo_tlb) (void);
void* algoritmo_tlb_fifo();
void* algoritmo_tlb_lru();

void* (*tipo_asignacion) (void);
void* asignacion_fija();
void* asignacion_global();



#endif