#include "../include/manejo_memoria.h"

bool entra_en_mp(uint32_t tamanio) {
    size_t rem;
    return memoria_disponible >= tamanio || cant_frames_libres() >= cant_paginas(tamanio, &rem);
}
