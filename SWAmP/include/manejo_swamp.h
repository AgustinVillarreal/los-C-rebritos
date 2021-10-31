#ifndef MANEJO_SWAMP_H
#define  MANEJO_SWAMP_H


#include <stdint.h>
#include "config.h"



uint32_t cantidad_de_espacio_swamp_libre(void* swamp);
void* swamp_con_mas_espacio();

#endif