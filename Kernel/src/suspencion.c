#include "../include/suspencion.h"

extern t_log* logger;
extern t_config_kernel* KERNEL_CFG;

void listener_suspencion(){
    while(1){     
        sem_wait(&SEM_CARPINCHO_SUSPENCION);
        pthread_mutex_lock(&MUTEX_LISTA_READY); 
        bool cola_ready_empty = list_is_empty(LISTA_READY);
        pthread_mutex_unlock(&MUTEX_LISTA_READY);

        pthread_mutex_lock(&MUTEX_LISTA_NEW); 
        bool cola_new_not_empty = !queue_is_empty(COLA_NEW);
        pthread_mutex_unlock(&MUTEX_LISTA_NEW);  

        pthread_mutex_lock(&MUTEX_LISTA_BLOCKED); 
        bool cola_blocked_not_empty = !list_is_empty(LISTA_BLOCKED);
        pthread_mutex_unlock(&MUTEX_LISTA_BLOCKED); 

        if(cola_ready_empty && cola_new_not_empty && cola_blocked_not_empty){
            t_carpincho* carpincho = remove_lista_blocked_last(); 
            add_lista_suspended_blocked(carpincho);
            sem_post(&SEM_GRADO_MULTIPROGRAMACION);
            send_codigo_op(carpincho->memoria_fd, CARPINCHO_SWAP);      
            // TODO PASAR idcarpincho
            //Usamos send_data_mate_init pq ya estaba hecho, aunque el nombre sea medio choto para este caso
            send_data_mate_init(carpincho->memoria_fd, carpincho->id);
        }
    }
}