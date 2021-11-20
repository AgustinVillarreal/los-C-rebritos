#ifndef INCLUDE_ALGORITMOS_H_
#define INCLUDE_ALGORITMOS_H_


#include <stdbool.h>
#include "config.h"
#include "monitor_memoria.h"

t_list* posibles_victimas(unsigned long id);
uint32_t* victima_clock_fija(unsigned long id_carpincho);


#endif