#ifndef TP_2021_2C_ALGORITMOS_H
#define TP_2021_2C_ALGORITMOS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <commons/log.h>
#include "config.h"
#include "colas.h"

extern t_log * logger;

//TODO ante la duda: extern

double HRRN(t_carpincho* carpincho);
t_carpincho* minima_estimacion(t_carpincho* carpincho1, t_carpincho* carpincho2);
t_carpincho* minimo_HRRN(t_carpincho* carpincho1, t_carpincho* carpincho2);
void ejecutar_CPU(int numero_CPU);



#endif

