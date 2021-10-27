#include "../include/programador.h"

extern t_log* logger;

void listener_programacion(){
    while(1){
      sem_wait(&SEM_GRADO_MULTIPROGRAMACION);
      sem_wait(&SEM_CANTIDAD_A_READY);
      t_carpincho* carpincho;
      if(largo_cola_suspended_ready() > 0){
        carpincho = pop_cola_suspended_ready(); 
      } else {
        carpincho = pop_cola_new(); 
      }
      push_cola_ready(carpincho);
    }   
}
