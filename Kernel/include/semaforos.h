#ifndef TP_2021_2C_SEMAFOROS_H
#define TP_2021_2C_SEMAFOROS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "config.h"
#include "colas.h"


typedef struct {
    char* name;
    int value;
    pthread_mutex_t MUTEX_COLA_BLOQUEADOS;
    t_queue* COLA_BLOQUEADOS;
    // TODO Es carpincho y no una lista porque se asume deadlock ante un mutex
    t_carpincho* carpincho_asignado;
} t_semaforo;

t_list* LISTA_SEMAFOROS;

pthread_mutex_t MUTEX_LISTA_SEMAFOROS;

void inicializar_semaforos();
int sem_init_carpincho(char* sem, int value);
bool existe_semaforo(char* nombre_sem, t_semaforo* semaforo);
int sem_wait_carpincho(char* sem, t_carpincho* carpincho, t_semaforo** sem_wait);
int sem_post_carpincho(char* sem_name_post);
int sem_destroy_carpincho(char* sem_name_destroy);


#endif
