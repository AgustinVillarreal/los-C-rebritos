#include "../include/deadlock.h"

extern t_config_kernel* KERNEL_CFG;
extern t_log* logger;

typedef struct {
    unsigned long id_carpincho;
    char* sem_asignado;
    char* sem_bloqueado;
} t_deadlock;


void listener_deadlock(){

    t_list* lista_posibles_bloqueados = list_create();
    t_list* lista_bloqueados = list_create();


    while(1){
        usleep(KERNEL_CFG->TIEMPO_DEADLOCK * 1000);

        pthread_mutex_lock(&MUTEX_LISTA_SEMAFOROS);
        
        for(int i = 0; i < list_size(LISTA_SEMAFOROS); i++) {
            t_semaforo* sem_asignado = (t_semaforo*) list_get(LISTA_SEMAFOROS, i);
            t_carpincho* asignado = sem_asignado->carpincho_asignado;

            bool esCarpincho(void* carpincho) {
                return asignado->id == ((t_carpincho*)carpincho)->id;
            }
            
            if(asignado != NULL) {
                for(int j = 0; j < list_size(LISTA_SEMAFOROS); j++) {    
                    if(j == i){
                        continue;            
                    }
                    
                    t_semaforo* sem_bloqueado = (t_semaforo*) list_get(LISTA_SEMAFOROS, j);
                    t_list* lista_bloqueados = sem_bloqueado->COLA_BLOQUEADOS->elements;
                    
                
                    if(list_find(lista_bloqueados, esCarpincho) != NULL){
                        t_deadlock* posible_deadlock = malloc(sizeof(t_deadlock));
                        posible_deadlock->id_carpincho = asignado->id;
                        posible_deadlock->sem_asignado = strdup(sem_asignado->name);
                        posible_deadlock->sem_bloqueado = strdup(sem_bloqueado->name);                    
                        list_add(lista_posibles_bloqueados, posible_deadlock);
                    }
                }
            }
            
        }

        pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);

        checkear_deadlock(lista_posibles_bloqueados, lista_bloqueados);
        
    }

    list_destroy(lista_posibles_bloqueados);
    list_destroy(lista_bloqueados);
    
}

void checkear_deadlock(t_list* lista_posibles_bloqueados, t_list* lista_bloqueados) {

    int cantidad_bloqueados = list_size(lista_posibles_bloqueados);

    for(int i = 0; i < cantidad_bloqueados; i++) {
        t_deadlock* posible_deadlock = list_get(lista_posibles_bloqueados, 0);

        list_add(lista_bloqueados, posible_deadlock);
        bool tiene_recurso(void* bloqueado) {
            return !strcmp(posible_deadlock->sem_bloqueado, ((t_deadlock*) bloqueado)->sem_asignado);
        }
        t_deadlock* posible_deadlock2 = list_find(lista_posibles_bloqueados, tiene_recurso);
        if(posible_deadlock2 != NULL) {
            
            list_add(lista_bloqueados, posible_deadlock2);
            for(int j = 0; j < list_size(lista_posibles_bloqueados) - i - 1; j++) {
                bool tiene_recurso(void* bloqueado) {
                    return !strcmp(posible_deadlock2->sem_bloqueado, ((t_deadlock*) bloqueado)->sem_asignado);
                }
                posible_deadlock2 = list_find(lista_posibles_bloqueados, tiene_recurso);
                if(posible_deadlock2 != NULL) {
                    if(posible_deadlock2->id_carpincho == posible_deadlock->id_carpincho) {
                        killear_id_mas_alto(lista_bloqueados, lista_posibles_bloqueados);
                        return;
                    } else {
                        list_add(lista_bloqueados, posible_deadlock2);
                    }
                } else {
                    list_remove(lista_posibles_bloqueados, 0);
                    list_clean(lista_bloqueados);
                    free(posible_deadlock->sem_bloqueado);
                    free(posible_deadlock->sem_asignado);
                    free(posible_deadlock);
                    break;
                }
                
            }
        } else {
                list_remove(lista_posibles_bloqueados, 0);
                free(posible_deadlock->sem_bloqueado);
                free(posible_deadlock->sem_asignado);
                free(posible_deadlock);
        }
        
    }

    void destruir(void* aa) {
        t_deadlock* deadlock = aa;
        free(deadlock->sem_bloqueado);
        free(deadlock->sem_asignado);
        free(deadlock);            
    }

    list_clean_and_destroy_elements(lista_posibles_bloqueados, destruir);
    list_clean(lista_bloqueados);

    return;


}

void killear_id_mas_alto(t_list* lista_bloqueados, t_list* lista_posibles_bloqueados){
    void* es_mayor(void* d1, void* d2) {
        return ((t_deadlock*) d1)->id_carpincho >= ((t_deadlock*) d2)->id_carpincho ? d1 : d2;
    }
    void* aca = list_get_maximum(lista_bloqueados, es_mayor);
   
    t_deadlock* id_mayor = (t_deadlock*)aca;

    // Saco lista de bloqueados
    bool es_deadlock(void* d) {
        return id_mayor->id_carpincho == ((t_deadlock*)d)->id_carpincho;
    }
    list_remove_by_condition(lista_bloqueados, es_deadlock);

    // Libero el semaforo asignado
    bool es_sem_asig(void * sem) {
        return !strcmp(id_mayor->sem_asignado, ((t_semaforo*)sem)->name);
    }
    t_semaforo* sem_asig = list_find(LISTA_SEMAFOROS, es_sem_asig);
    t_carpincho* carpincho_deadlock = sem_asig->carpincho_asignado;
    sem_asig->carpincho_asignado = NULL;

    sem_post_carpincho(sem_asig->name);

    // Saco al carpincho de la lista de bloqueados del semaforo
    bool es_sem_blocked(void * sem) {
        return !strcmp(id_mayor->sem_bloqueado, ((t_semaforo*)sem)->name);
    }
    t_semaforo* sem_blocked = list_find(LISTA_SEMAFOROS, es_sem_blocked);
    bool es_carpincho(void* carpincho) {
        return id_mayor->id_carpincho == ((t_carpincho*)carpincho)->id;
    }
    list_remove_by_condition(sem_blocked->COLA_BLOQUEADOS->elements, es_carpincho);

    // Saco al carpincho de la cola de bloqueados general
    remove_lista_blocked(carpincho_deadlock);

    // Agrego a cola exit
    push_cola_exit(carpincho_deadlock);
    
    // Mandar a matelib y memoria que se finalizó al carpincho
    finalizar_carpincho(carpincho_deadlock);

    // se llama nuevamente a chekear si hay deadlock
    checkear_deadlock(lista_bloqueados, lista_posibles_bloqueados);
    
}

void finalizar_carpincho(t_carpincho* carpincho) {
    // Lo tiene que recibir memoria
    send_finalizar_carpincho(carpincho->memoria_fd, carpincho->id);
    int error = -2;
    send(carpincho->matelib_fd, &error, sizeof(int), 0);
}