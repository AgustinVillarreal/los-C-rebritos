#ifndef TP_2021_2C_ALGORITMOS_H
#define TP_2021_2C_ALGORITMOS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <commons/log.h>
#include "config.h"

extern t_log * logger;

typedef struct {
  unsigned int id;
  pthread_t thread;
  sem_t sem_pause;
  bool blocked;
  uint16_t ultima_estimacion;
  struct tm * tiempo_inicio;
} t_carpincho;

//TODO ante la duda: extern

pthread_mutex_t MUTEX_LISTA_NEW;
pthread_mutex_t MUTEX_LISTA_READY;
pthread_mutex_t MUTEX_LISTA_SUSPENDED_READY;
pthread_mutex_t MUTEX_LISTA_SUSPENDED_BLOCKED;
pthread_mutex_t MUTEX_LISTA_BLOCKED;

sem_t * SEM_CPUs;


//TODO: Signal en suspended y en exit
sem_t SEM_GRADO_MULTIPROGRAMACION;


//TODO: Signal cuando se crea new y cuando suspended block -> suspended ready
sem_t SEM_CANTIDAD_A_READY;

//TODO: Signal en block y de new a ready
sem_t SEM_CANTIDAD_EN_READY;


t_carpincho* obtener_carpincho_HRRN();
t_carpincho* obtener_carpincho_SJF();
void ejecutar_CPU(int numero_CPU);

#endif

