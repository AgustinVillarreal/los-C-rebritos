#include "../include/semaforos.h"

extern t_log* logger;

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
        semaforo->carpincho_asignado = NULL;
        list_add(LISTA_SEMAFOROS, semaforo);
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    free(sem);
    return 0;
}

bool existe_semaforo(char* nombre_sem, t_semaforo* semaforo){
    return !strcmp(nombre_sem, semaforo->name);
}

//WAIT 

int sem_wait_carpincho(char* sem, t_carpincho* carpincho, t_semaforo** sem_wait){

    bool carpincho_blocked = false;

    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem, semaforo);
    } 
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
        return -1;
    }
    *sem_wait = list_find(LISTA_SEMAFOROS, existe_semaforo_nombre);

    (*sem_wait)->value --;
    if((*sem_wait)->value < 0){
        queue_push((*sem_wait)->COLA_BLOQUEADOS, carpincho);
        //TODO: Ver que pasa con CPU
        add_lista_blocked(carpincho);
        carpincho_blocked = true;
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    if(carpincho_blocked){
		sem_post(&SEM_CPUs[carpincho->cpu_asignada]);		        
        return 1;
    }
    return 0;
}   


//POST 

int sem_post_carpincho(char* sem_name_post){
    t_carpincho * carpincho;
    bool desbloquea_carpincho = false;
    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem_name_post, semaforo);
    }
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);        
        return -1;
    }
    t_semaforo* sem_post_c = list_find(LISTA_SEMAFOROS, existe_semaforo_nombre);
    sem_post_c->value ++;
    if(sem_post_c->value <= 0){
        carpincho = queue_pop(sem_post_c->COLA_BLOQUEADOS);
        desbloquea_carpincho = true;
        //TODO: ver si es Region critica
    }
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    if(desbloquea_carpincho){
        if(existe_en_lista_blocked(carpincho)){
            remove_lista_blocked(carpincho);
            push_cola_ready(carpincho);    
        } else {
            remove_lista_suspended_blocked(carpincho);
            push_cola_suspended_ready(carpincho);
        }       
    }
    return 0;
}

// DESTROY
int sem_destroy_carpincho(char* sem_name_destroy){
    t_carpincho * carpincho;
    bool desbloquea_carpincho = false;
    bool existe_semaforo_nombre(void* semaforo){
        return existe_semaforo(sem_name_destroy, semaforo);
    }
    pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
    if(!list_any_satisfy(LISTA_SEMAFOROS, existe_semaforo_nombre)){
        pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
        return -1;
    }
    t_semaforo* sem_destroy_c = list_find(LISTA_SEMAFOROS, existe_semaforo_nombre);
    queue_destroy(sem_destroy_c->COLA_BLOQUEADOS);
    char * name = sem_destroy_c->name;
    list_remove_by_condition(LISTA_SEMAFOROS, existe_semaforo_nombre);
    free(name);
    pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
    return 0;
}
