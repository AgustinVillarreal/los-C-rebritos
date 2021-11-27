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
pthread_mutex_t MUTEX_GLOBAL_TUR_TLB;
pthread_mutex_t MUTEX_TOTAL_HITS;
pthread_mutex_t MUTEX_TOTAL_MISS;

entrada_tp_t* list_get_pagina(tp_carpincho_t* tabla_carpincho ,uint32_t index);
tp_carpincho_t* find_tp_carpincho(unsigned long id);
bool tabla_vacia(unsigned long id);
void agregar_tabla_a_tp_carpinchos(void* tp_carpincho);
void mutex_init_tabla();
uint32_t table_size(unsigned long id);
void append_frame_tp(unsigned long id, uint32_t nro_pagina, uint32_t nro_frame);

//TLB
bool buscar_en_TLB(unsigned long id_carpincho, uint32_t nro_pagina, entrada_tp_t** entrada_buscada);
void crear_en_TLB(unsigned long id_carpincho, entrada_tp_t* entrada);
tlb_t* tlb_get_entrada_tlb(unsigned long id_carpincho, uint32_t nro_pagina);
void actualizarTUR(tlb_t* entrada_tlb);
void actualizar_hits(unsigned long id_carpincho);
void actualizar_miss(unsigned long id_carpincho);
void list_add_tlb(tlb_t* entrada_tlb);
uint32_t get_tlb_size();
char* stringify_tlb();
char* entrada_tlb_a_string_de_dump(tlb_t* entrada, int nro_entrada, bool ocupado);

void actualizar_bits(entrada_tp_t* entrada_tp, bool se_modifica);
bool esta_presente(void* pagina);
t_list* paginas_presentes_local(tp_carpincho_t* tabla_carpincho);
t_list* paginas_presentes_dinamica();

#endif