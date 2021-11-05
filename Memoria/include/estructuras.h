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
    uint32_t nro_frame;
    uint32_t nro_pagina;
} tlb_t;

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
    t_list* paginas;
} tp_carpincho_t;

typedef struct {
        uint16_t bit_U;     // bit de uso
        uint16_t bit_M;    // bit de modificación
} clock_m_t;

typedef struct {
    uint32_t nro_pagina;    
    uint32_t nro_frame;     
    union {
        clock_m_t* clock_m;
        uint32_t TUR;      // tiempo de ultima referencia, LRU
    };
    bool bit_P; // bit de presencia
} entrada_tp_t;


t_list* TLB_TABLE;
t_list* CARPINCHOS_TABLE; 

void init_memory_structs();
void destroy_memory_structs();

#endif
