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
} HeapMetadata;

typedef struct {
    uint32_t nro_frame;
    uint32_t nro_pagina;
} tlb_t;

typedef struct {
    uint32_t nro_pagina;    
    uint32_t nro_frame;     
    union {
        uint32_t bit_U;     //bit de uso FIFO
        uint32_t TUR;       //tiempo de ultima referencia LRU
    };
    bool bit_P; //bit de presencia
} tablaPaginas_t;

typedef union {
    uint64_t bytes;
    struct {
        unsigned libre          :  1;
        unsigned amedias        :  1;
        unsigned inicio_hueco   : 30;
        unsigned pid_ocupador   : 32;
    };
} frame_t;

#endif
