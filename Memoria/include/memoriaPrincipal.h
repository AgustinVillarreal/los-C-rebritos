#ifndef INCLUDE_MEMORIAPRINCIPAL_H_
#define INCLUDE_MEMORIAPRINCIPAL_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>

// typedef struct​ {
// uint32_t prevAlloc,
// uint32_t nextAlloc,
// uint8_t isFree,
// } ​HeapMetadata;


// typedef struct {
//     uint32_t tamanio;
//     uint32_t dir_inicio;
//     uint32_t dir_fin;
//     HeapMetadata​* allocs;
// } t_pagina;

// uint32_t reservarMemoria(int cantidadDeMemoria);
// uint32_t firstFit(); // tiene que ser de tipo unit32_t? de que tamanio es una direccion logica?
// void liberarMemoria(HeapMetadata allocALiberar);
// uint32_t leerMemoria(uint32_t direccionLogica, t_pagina* tablaDePaginas); //nose si tiene que ser de tipo uint32 la funcion porque no se que devuelve
// void escribirMemoria(uint32_t direccionLogica, t_pagina* tablaDePaginas);

#endif // INCLUDE_MEMORIAPRINCIPAL_H_