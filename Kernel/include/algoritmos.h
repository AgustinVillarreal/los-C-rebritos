#ifndef TP_2021_2C_ALGORITMOS_H
#define TP_2021_2C_ALGORITMOS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>


typedef struct {
  uint32_t tid;
} t_carpincho;

typedef struct {
  t_carpincho* c;
  pthread_t thread;
  sem_t sem_pause;
  bool blocked;
} t_running_thread;

void ejecutar_algoritmo_HRRN(t_running_thread* thread_data);
void ejecutar_algoritmo_SJF(t_running_thread* thread_data);

#endif
