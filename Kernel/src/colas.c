#include "../include/colas.h"


static uint32_t obj_tid = 0;

bool filter_t_running_thread_by_tid(void *item){
  t_running_thread* t_r = (t_running_thread*) item;
  return t_r->c->tid == obj_tid;
}

void push_cola_new(t_running_thread* n){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  queue_push(COLA_NEW, n);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
}

t_running_thread* pop_cola_new(){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  t_running_thread* element = (t_running_thread*) queue_pop(COLA_NEW);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return element;
}

uint16_t largo_cola_new() {
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  uint16_t ret = queue_size(COLA_NEW);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

t_running_thread* buscar_cola_new(uint32_t tid){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  obj_tid = tid;
  t_running_thread* ret = list_find(COLA_NEW->elements, filter_t_running_thread_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

void* remover_cola_new(uint32_t tid){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  obj_tid = tid;
  void* ret = list_remove_by_condition(COLA_NEW->elements, filter_t_running_thread_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  return ret;
}

