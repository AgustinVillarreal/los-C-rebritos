#ifndef TP_2021_2C_ALGORITMOS_H
#define TP_2021_2C_ALGORITMOS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  unsigned int id;
  pthread_t thread;
  sem_t sem_pause;
  bool blocked;
} t_carpincho;

void ejecutar_algoritmo_HRRN(t_carpincho* thread_data);
void ejecutar_algoritmo_SJF(t_carpincho* thread_data);

#endif
