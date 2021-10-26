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
        semaforo->COLA_BLOQUEADOS = queue_create();
        list_add(LISTA_SEMAFOROS, semaforo);
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    return 0;
}

bool existe_semaforo(char* nombre_sem, t_semaforo* semaforo){
    return !strcmp(nombre_sem, semaforo->name);
}

//WAIT 

int sem_wait_carpincho(char* sem, t_carpincho* carpincho){

    bool carpincho_blocked = false;

    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem, semaforo);
    }
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        return -1;
    }
    t_semaforo* sem_wait = list_find(LISTA_SEMAFOROS, existe_semaforo_nombre);
    sem_wait->value --;
    if(sem_wait->value < 0){
        queue_push(sem_wait->COLA_BLOQUEADOS, carpincho);
        //TODO: Ver que pasa con CPU
        add_lista_blocked(carpincho);
        carpincho_blocked = true;
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    if(carpincho_blocked){
        return 1;
    }
    return 0;
}   


//POST 

int sem_post_carpincho(char* sem_name_post){
    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem, semaforo);
    }
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        return -1;
    }
    t_semaforo* sem_post = list_find(LISTA_SEMAFOROS, existe_semaforo_nombre);
    sem_post->value ++;
    if(sem_post->value <= 0){
        t_carpincho * carpincho = queue_pop(sem_post->COLA_BLOQUEADOS);
        //TODO: ver si es Region critica
        remove_lista_blocked(carpincho);
        push_cola_ready(carpincho);
        sem_post(&SEM_CANTIDAD_EN_READY);        
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    return 0;
}
