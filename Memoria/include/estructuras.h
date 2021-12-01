#ifndef INCLUDE_ESTRUCTURAS_H_
#define INCLUDE_ESTRUCTURAS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
} __attribute__((packed)) 
    hmd_t;

typedef struct {
    unsigned long id_carpincho;
    //TODO
    uint8_t ocupado;
    uint8_t libre;
} frame_t;

//TODO: Struct para poder administrar los carpinchos con sus tablas
typedef struct {
    unsigned long id_carpincho;
    uint32_t pages;
    pthread_mutex_t mutex_paginas;
    pthread_mutex_t mutex_hits;
    pthread_mutex_t mutex_miss;
    t_list* paginas;
    uint32_t posible_victima_fija;
    uint32_t cant_hits;
    uint32_t cant_miss;
} tp_carpincho_t;

typedef struct {
        uint16_t bit_U;     // bit de uso
} clock_m_t;

typedef struct {
    uint32_t nro_pagina;    
    uint32_t nro_frame; 
    pthread_mutex_t mutex_bits;
    uint16_t bit_M;    // bit de modificación    
    bool bit_P; // bit de presencia
    union {
        clock_m_t* clock_m;
        uint32_t TUR;      // tiempo de ultima referencia, LRU
    } algoritmo;
} entrada_tp_t;

typedef struct {
    unsigned long id_carpincho;
    entrada_tp_t* entrada_tp;
    uint32_t TUR;
} tlb_t;

typedef struct {
    entrada_tp_t* entrada_tp;
    unsigned long id_carpincho;
} algoritmo_t;

t_list* TLB_TABLE;
t_list* CARPINCHOS_TABLE; 

void init_memory_structs();
void destroy_memory_structs();
void destroy_tlb_entry(tlb_t* tlb_entry);
#endif
