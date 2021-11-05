#include "../include/monitor_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;

void mutex_init_memoria(){
    //TODO: iniciar mutex aca 
    pthread_mutex_init(&MUTEX_FRAMES_BUSY, NULL);
    pthread_mutex_init(&MUTEX_MP_BUSY, NULL);
    return;
}

uint32_t cant_frame_libres(){
    uint32_t libres = 0;
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    for(uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++){
        if(tabla_frames[i].libre) libres++;
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);   
    return libres; 
}
