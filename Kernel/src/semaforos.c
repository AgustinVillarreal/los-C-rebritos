#include "../include/semaforos.h"

void inicializar_semaforos(){
    LISTA_SEMAFOROS = list_create();
    pthread_mutex_init(&MUTEX_LISTA_SEMAFOROS, NULL);    
}


int sem_init_carpincho(char* sem, int value){
    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem, semaforo);
    }
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        t_semaforo* semaforo = malloc(sizeof(t_semaforo));
        semaforo->name = strdup(sem);
        semaforo->value = value;
        list_add(LISTA_SEMAFOROS, semaforo);
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    printf("---------%d--------", list_size(LISTA_SEMAFOROS));
    return 0;
}

bool existe_semaforo(char* nombre_sem, t_semaforo* semaforo){
    return !strcmp(nombre_sem, semaforo->name);
}
