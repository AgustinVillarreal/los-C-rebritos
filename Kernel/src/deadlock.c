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

        // for(int i = 0; i < list_size(LISTA_SEMAFOROS); i++) {
        //     t_semaforo* sem_asignado = (t_semaforo*) list_get(LISTA_SEMAFOROS, i);
        //     t_list* lista_bloqueados = sem_asignado->COLA_BLOQUEADOS->elements;            
        //     for(int j = 0; j < list_size(sem_asignado->COLA_BLOQUEADOS->elements); j++) {
        //         t_carpincho* carp = list_get(lista_bloqueados, j);
        //         log_info(logger, "------i-%d--j-%d--name-%s--id-%ul----", i,j,sem_asignado->name, carp->id);
                    
        //     }
        // }
        
        for(int i = 0; i < list_size(LISTA_SEMAFOROS); i++) {
            t_semaforo* sem_asignado = (t_semaforo*) list_get(LISTA_SEMAFOROS, i);
            t_carpincho* asignado = sem_asignado->carpincho_asignado;

            bool esCarpincho(void* carpincho) {
                return asignado->id == ((t_carpincho*)carpincho)->id;
            }
            log_info(logger, "------i---%d-----", i);
            
            
            // estaba j = i + 1
            for(int j = 0; j < list_size(LISTA_SEMAFOROS); j++) {    
                if(j == i){
                    continue;            
                }
                log_info(logger, "------j---%d-----", j);
                
                t_semaforo* sem_bloqueado = (t_semaforo*) list_get(LISTA_SEMAFOROS, j);
                t_list* lista_bloqueados = sem_bloqueado->COLA_BLOQUEADOS->elements;
                log_info(logger, "------cant---%d-----", list_size(lista_bloqueados));
                log_info(logger, "------asignado---%s-----", sem_asignado->name);
                
               
                if(list_find(lista_bloqueados, esCarpincho) != NULL){
                    t_deadlock* posible_deadlock = malloc(sizeof(t_deadlock));
                    posible_deadlock->id_carpincho = asignado->id;
                    posible_deadlock->sem_asignado = strdup(sem_asignado->name);
                    posible_deadlock->sem_bloqueado = strdup(sem_bloqueado->name);                    
                    list_add(lista_posibles_bloqueados, posible_deadlock);
                }
            }
        }

        int cantidad_bloqueados = list_size(lista_posibles_bloqueados);
        log_info(logger, "AAAAAAAAAAAAAAAAAAAA");
        log_info(logger, "------blqueados---%d-----", cantidad_bloqueados);
        

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
                            log_info(logger, "-------------------Llegue-------------");
                        } else {
                            list_add(lista_bloqueados, posible_deadlock2);
                        }
                    } else {
                        list_remove(lista_posibles_bloqueados, 0);
                        list_clean(lista_bloqueados);
                        free(posible_deadlock);
                        break;
                    }
                    
                }
            } else {
                    list_remove(lista_posibles_bloqueados, 0);
                    list_clean(lista_bloqueados);
                    free(posible_deadlock);
            }
            
            
        }

        pthread_mutex_unlock(&MUTEX_LISTA_SEMAFOROS);
        
    }

    list_destroy(lista_posibles_bloqueados);
    list_destroy(lista_bloqueados);
    
}

void checkear_deadlock() {

}