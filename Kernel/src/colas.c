#include "../include/colas.h"


static unsigned long obj_tid = 0;

void iniciar_mutex(int grado_multiprogramacion){
  pthread_mutex_init(&MUTEX_LISTA_NEW, NULL);
  sem_init(&SEM_GRADO_MULTIPROGRAMACION,0, grado_multiprogramacion);
  sem_init(&SEM_CANTIDAD_A_READY, 0, 0);
}

void destroy_mutex(){
  pthread_mutex_destroy(&MUTEX_LISTA_NEW);
  sem_destroy(&SEM_GRADO_MULTIPROGRAMACION);
  sem_destroy(&SEM_CANTIDAD_A_READY);
}

bool filter_t_carpincho_by_tid(void *item){
  t_carpincho* t_r = (t_carpincho*) item;
  return t_r->id == obj_tid;
}

void push_cola_new(t_carpincho* carpincho){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  queue_push(COLA_NEW, carpincho);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
}

t_carpincho* pop_cola_new(){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  t_carpincho* element = (t_carpincho*) queue_pop(COLA_NEW);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  sem_post(&SEM_CANTIDAD_A_READY);
  return element;
}

uint16_t largo_cola_new() {
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  uint16_t ret = queue_size(COLA_NEW);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

t_carpincho* buscar_cola_new(unsigned long tid){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  obj_tid = tid;
  t_carpincho* ret = list_find(COLA_NEW->elements, filter_t_carpincho_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

void* remover_cola_new(unsigned long tid){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  obj_tid = tid;
  void* ret = list_remove_by_condition(COLA_NEW->elements, filter_t_carpincho_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

void carpincho_init(unsigned long id){
  t_carpincho* carpincho = malloc(sizeof(t_carpincho));
  carpincho->id = id;
  push_cola_new(carpincho);
  return;
}

