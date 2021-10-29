#include "../include/suspencion.h"

extern t_log* logger;
extern t_config_kernel* KERNEL_CFG;

void listener_suspencion(){
    while(1){     
        sem_wait(&SEM_CARPINCHO_BLOCKED);
        pthread_mutex_lock(&MUTEX_LISTA_READY); 
        bool cola_ready_empty = queue_is_empty(COLA_READY);
        pthread_mutex_unlock(&MUTEX_LISTA_READY);
        pthread_mutex_lock(&MUTEX_LISTA_NEW); 
        bool cola_new_not_empty = !queue_is_empty(COLA_NEW);
        pthread_mutex_unlock(&MUTEX_LISTA_NEW);   
        if(cola_ready_empty && cola_new_not_empty){
            t_carpincho* carpincho = remove_lista_blocked_last(); 
            add_lista_suspended_blocked(carpincho);
            sem_post(&SEM_GRADO_MULTIPROGRAMACION);
            send_codigo_op(KERNEL_CFG->MEMORIA_FD, CARPINCHO_SWAP);

            // TODO PASAR idcarpincho
        }
    }
}