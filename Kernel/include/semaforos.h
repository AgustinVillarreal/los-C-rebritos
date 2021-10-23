#ifndef TP_2021_2C_SEMAFOROS_H
#define TP_2021_2C_SEMAFOROS_H

#include <commons/collections/list.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


typedef struct {
    char* name;
    int value;
    sem_t* sem;
} t_semaforo;

t_list* LISTA_SEMAFOROS;

pthread_mutex_t MUTEX_LISTA_SEMAFOROS;

void inicializar_semaforos();
int sem_init_carpincho(char* sem, int value);
bool existe_semaforo(char* nombre_sem, t_semaforo* semaforo);

#endif