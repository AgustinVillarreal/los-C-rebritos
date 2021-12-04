#include "../include/entrada_salida.h"

extern t_config_kernel* KERNEL_CFG;
extern t_log* logger;


int procesar_entrada_salida(t_carpincho* carpincho, char* io, char* msg){
    add_lista_blocked(carpincho);
	sem_post(&SEM_CPUs[carpincho->cpu_asignada]);
    bool es_dispositivo(void* dispositivo_comp){
        t_dispositivo_io* dispositivo = dispositivo_comp;
        return !strcmp(io, (char*) dispositivo->nombre);
    }
    t_dispositivo_io* dispositivo = list_find(KERNEL_CFG->DISPOSITIVOS_IO, es_dispositivo);
    if(dispositivo == NULL){
        return 0;
    }
    log_info(logger, "Esperando dispositivo: %lu", carpincho->id);
    sem_wait(&dispositivo->sem);
    log_info(logger, "Dispositivo asignado: %lu", carpincho->id);
    
    usleep(dispositivo->duracion * 1000);
    if(existe_en_lista_blocked(carpincho)){
        remove_lista_blocked(carpincho);
        push_cola_ready(carpincho);    
    } else {
        remove_lista_suspended_blocked(carpincho);
        push_cola_suspended_ready(carpincho);
    }
    sem_post(&dispositivo->sem);
    return 1;
}