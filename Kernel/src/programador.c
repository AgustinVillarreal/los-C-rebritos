#include "../include/programador.h"

void listener_programacion(){
    while(1){
      sem_wait(&SEM_GRADO_MULTIPROGRAMACION);
      sem_wait(&SEM_CANTIDAD_A_READY);
    }
   
}
