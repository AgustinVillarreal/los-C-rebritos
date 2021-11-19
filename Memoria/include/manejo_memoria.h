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
uint32_t cant_frame_libres_fija(unsigned long id_carpincho);
uint32_t cant_frame_libres();
uint32_t buscar_primer_frame_carpincho();

uint32_t buscar_recorriendo_hmd(unsigned long id_carpincho, size_t size, hmd_t** hmd, entrada_tp_t** entrada_tp, uint32_t* direccion_hmd);
entrada_tp_t* buscar_entrada_tp(unsigned long id_carpincho, uint32_t nro_pagina);
hmd_t* leer_hmd(entrada_tp_t* entrada_tp, uint32_t offset, unsigned long id_carpincho);
void alloc_size_en_mp(void* data, uint32_t offset, size_t size, entrada_tp_t* entrada_tp, unsigned long id_carpincho);
bool entra_en_hmd(hmd_t* hmd, size_t size, uint32_t direccion_hmd);
void escribir_en_mp(hmd_t* hmd, size_t size, entrada_tp_t* entrada_tp, uint32_t offset_hmd, unsigned long id_carpincho);

bool allocar_al_final(unsigned long id_carpincho, hmd_t* hmd_inicial, hmd_t* hmd_final, entrada_tp_t* entrada_tp, uint32_t posicion_hmd_final, size_t size);
entrada_tp_t* crear_nueva_pagina(unsigned long id_carpincho);

#endif