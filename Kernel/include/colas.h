#ifndef TP_2021_2C_COLAS_H
#define TP_2021_2C_COLAS_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdint.h>
#include "algoritmos.h"

//TODO ante la duda: extern

pthread_mutex_t MUTEX_LISTA_NEW;

t_queue* COLA_NEW;

void push_cola_new(t_running_thread*);
t_running_thread* pop_cola_new();
uint16_t largo_cola_new();
t_running_thread* buscar_cola_new(uint32_t tid);
void* remover_cola_new(uint32_t tid);

t_queue* COLA_READY;
t_queue* COLA_BLOCKED;



#endif
