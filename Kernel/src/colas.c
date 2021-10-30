#include "../include/colas.h"

//TODO: TODO ESTO SEGURO SE PUEDE ABSTRAEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEER

static unsigned long obj_tid = 0;

//Esto es prueba
uint16_t estimacion = 80;

void iniciar_mutex(int grado_multiprogramacion){

  pthread_mutex_init(&MUTEX_LISTA_NEW, NULL);
  pthread_mutex_init(&MUTEX_LISTA_READY, NULL);
  pthread_mutex_init(&MUTEX_LISTA_SUSPENDED_READY, NULL);
  pthread_mutex_init(&MUTEX_LISTA_SUSPENDED_BLOCKED, NULL);
  pthread_mutex_init(&MUTEX_LISTA_BLOCKED, NULL);

  sem_init(&SEM_GRADO_MULTIPROGRAMACION,0, grado_multiprogramacion);
  sem_init(&SEM_CANTIDAD_A_READY, 0, 0);
  sem_init(&SEM_CANTIDAD_EN_READY, 0, 0);
}

void carpincho_init(unsigned long id){
  t_carpincho* carpincho = malloc(sizeof(t_carpincho));
  carpincho->id = id;
  // TODO se elimina esto, es solo para pruebas
  // if(estimacion = 80){
  //   estimacion = 1; 
  // }
  // carpincho->ultima_estimacion = estimacion; 
  carpincho->ultima_estimacion = KERNEL_CFG->ESTIMACION_INICIAL;
  sem_init(&carpincho->sem_pause, 0, 0);
  push_cola_new(carpincho);
  sem_wait(&carpincho->sem_pause);
  return;
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

// COSAS DE COLA NEW 

void push_cola_new(t_carpincho* carpincho){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  queue_push(COLA_NEW, carpincho);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
  sem_post(&SEM_CANTIDAD_A_READY);    
}

t_carpincho* pop_cola_new(){
  pthread_mutex_lock(&MUTEX_LISTA_NEW);
  t_carpincho* element = (t_carpincho*) queue_pop(COLA_NEW);
  pthread_mutex_unlock(&MUTEX_LISTA_NEW);
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

//COSAS COLA SUSPENDED READY

void push_cola_suspended_ready(t_carpincho* carpincho){
  pthread_mutex_lock(&MUTEX_LISTA_SUSPENDED_READY);
  queue_push(COLA_SUSPENDED_READY, carpincho);
  pthread_mutex_unlock(&MUTEX_LISTA_SUSPENDED_READY);
  sem_post(&SEM_CANTIDAD_A_READY);    
}

t_carpincho* pop_cola_suspended_ready(){
  pthread_mutex_lock(&MUTEX_LISTA_SUSPENDED_READY);
  t_carpincho* element = (t_carpincho*) queue_pop(COLA_SUSPENDED_READY);
  pthread_mutex_unlock(&MUTEX_LISTA_SUSPENDED_READY);
  return element;
}

uint16_t largo_cola_suspended_ready() {
  pthread_mutex_lock(&MUTEX_LISTA_SUSPENDED_READY);
  uint16_t ret = queue_size(COLA_SUSPENDED_READY);
  pthread_mutex_unlock(&MUTEX_LISTA_SUSPENDED_READY);
  return ret;
}

t_carpincho* buscar_cola_suspended_ready(unsigned long tid){
  pthread_mutex_lock(&MUTEX_LISTA_SUSPENDED_READY);
  obj_tid = tid;
  t_carpincho* ret = list_find(COLA_SUSPENDED_READY->elements, filter_t_carpincho_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_SUSPENDED_READY);
  return ret;
}

void* remover_cola_suspended_ready(unsigned long tid){
  pthread_mutex_lock(&MUTEX_LISTA_SUSPENDED_READY);
  obj_tid = tid;
  void* ret = list_remove_by_condition(COLA_SUSPENDED_READY->elements, filter_t_carpincho_by_tid);
  pthread_mutex_unlock(&MUTEX_LISTA_SUSPENDED_READY);
  return ret;
}

//COSAS COLA READY

void push_cola_ready(t_carpincho* carpincho){
  pthread_mutex_lock(&MUTEX_LISTA_READY);
  queue_push(COLA_READY, carpincho);
  carpincho->tiempo_ingreso_ready = time(NULL);  
  pthread_mutex_unlock(&MUTEX_LISTA_READY);
}

uint16_t largo_cola_ready() {
  pthread_mutex_lock(&MUTEX_LISTA_READY);
  uint16_t ret = queue_size(COLA_READY);
  pthread_mutex_unlock(&MUTEX_LISTA_READY);
  return ret;
}
