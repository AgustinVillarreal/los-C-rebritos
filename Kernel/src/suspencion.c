#include "../include/suspencion.h"

extern t_log* logger;

void listener_suspencion(){
    while(1){     
        sem_wait(&SEM_CARPINCHO_BLOCKED);
        // TODO
    }
}