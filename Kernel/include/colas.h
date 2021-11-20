#ifndef TP_2021_2C_COLAS_H
#define TP_2021_2C_COLAS_H

#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"

extern t_config_kernel* KERNEL_CFG;

pthread_mutex_t MUTEX_LISTA_NEW;
pthread_mutex_t MUTEX_LISTA_READY;
pthread_mutex_t MUTEX_LISTA_SUSPENDED_READY;
pthread_mutex_t MUTEX_LISTA_SUSPENDED_BLOCKED;
pthread_mutex_t MUTEX_LISTA_BLOCKED;
pthread_mutex_t MUTEX_LISTA_EXIT;

sem_t * SEM_CPUs;


//TODO: Signal en suspended y en exit
sem_t SEM_GRADO_MULTIPROGRAMACION;


//TODO: Signal cuando se crea new y cuando suspended block -> suspended ready
sem_t SEM_CANTIDAD_A_READY;

//TODO: Signal en block y de new a ready
sem_t SEM_CANTIDAD_EN_READY;

sem_t SEM_CARPINCHO_SUSPENCION;

t_queue* COLA_NEW;
t_queue* COLA_READY;
t_queue* COLA_EXIT;
t_queue* COLA_SUSPENDED_READY;
t_list* LISTA_SUSPENDED_BLOCKED;
t_list*  LISTA_BLOCKED;

void carpincho_init(unsigned long id, t_carpincho** carpincho);
void iniciar_mutex(int grado_multiprogramacion);
void destroy_mutex();
uint16_t calcular_estimacion(t_carpincho* carpincho);

//COSAS DE COLA NEW
void push_cola_new(t_carpincho* carpincho);
t_carpincho* pop_cola_new();
uint16_t largo_cola_new();
t_carpincho* buscar_cola_new(unsigned long id);
void* remover_cola_new(unsigned long id);

//COSAS DE COLA SUSPENDED READY
void push_cola_suspended_ready(t_carpincho* carpincho);
t_carpincho* pop_cola_suspended_ready();
uint16_t largo_cola_suspended_ready();
t_carpincho* buscar_cola_suspended_ready(unsigned long id);
void* remover_cola_suspended_ready(unsigned long id);

//COSAS DE COLA READY
void push_cola_ready(t_carpincho* carpincho);
t_carpincho* pop_cola_ready();
uint16_t largo_cola_ready();
t_carpincho* buscar_cola_ready(unsigned long id);
void* remover_cola_ready(unsigned long id);

//COSAS DE LISTA BLOCKED
void remove_lista_blocked(t_carpincho* carpincho);
void add_lista_blocked(t_carpincho* carpincho);
bool existe_en_lista_blocked(t_carpincho* carpincho);
t_carpincho* remove_lista_blocked_last();

//COSAS DE LISTA SUSPENDED BLOCKED
void remove_lista_suspended_blocked(t_carpincho* carpincho);
void add_lista_suspended_blocked(t_carpincho* carpincho);

//COSAS DE COLA EXIT
void push_cola_exit(t_carpincho* carpincho);

#endif
