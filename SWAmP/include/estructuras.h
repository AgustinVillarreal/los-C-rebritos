#ifndef ESTRUCTURAS_SWAMP_H
#define ESTRUCTURAS_SWAMP_H

#include <stdint.h>
#include <stdlib.h>


typedef struct {
    uint32_t pid;
    uint32_t nro_pagina;
    uint32_t inicio;
    uint32_t nro_swap;
} frame_swap_t;




void destroy_frame(frame_swap_t* frame);


#endif