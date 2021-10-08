#ifndef TP_2021_2C_COLAS_H
#define TP_2021_2C_COLAS_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include "algoritmos.h"
#include <stdlib.h>

//TODO ante la duda: extern

pthread_mutex_t MUTEX_LISTA_NEW;

//TODO: Signal en suspended y en exit
sem_t SEM_GRADO_MULTIPROGRAMACION;

//TODO: Signal cuando se crea new y cuando suspended block -> suspended ready
sem_t SEM_CANTIDAD_A_READY;

t_queue* COLA_NEW;

void push_cola_new(t_carpincho* carpincho);
t_carpincho* pop_cola_new();
uint16_t largo_cola_new();
t_carpincho* buscar_cola_new(unsigned long id);
void* remover_cola_new(unsigned long id);
void carpincho_init(unsigned long id);
t_queue* COLA_READY;
t_queue* COLA_BLOCKED;



#endif
