#include "../include/monitor_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;
extern void* memoria_principal;
extern t_log* logger;

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


uint32_t buscar_primer_frame_carpincho(){
    uint32_t nro_frame = 0xFFFF;
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    for(uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++){
        if(tabla_frames[i].libre) {
            pthread_mutex_unlock(&MUTEX_FRAMES_BUSY); 
            return i;
        }
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return nro_frame;
}

void primer_memalloc_carpincho(unsigned long id_carpincho, size_t* size_rest, uint32_t direccion_logica, uint32_t nro_frame, uint32_t nro_pagina, uint32_t* hmd_cortado){
    //TODO: liberar en algun lado
    hmd_t* hmd = malloc(sizeof(hmd_t));
    if(nro_pagina == 0){
        hmd->prevAlloc = NULL;
        hmd->nextAlloc = sizeof(hmd_t) + *size_rest;
        hmd->isFree = true;

        pthread_mutex_lock(&MUTEX_MP_BUSY);
        memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd, sizeof(hmd_t));
        pthread_mutex_unlock(&MUTEX_MP_BUSY);
        
        * size_rest += sizeof(hmd_t);       
    }

    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = false;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    int32_t dif = MEMORIA_CFG->TAMANIO_PAGINA - *size_rest;  

    hmd->prevAlloc = 0;
    hmd->nextAlloc = NULL;
    hmd->isFree = true;
    
    if(dif > 0 ) {

        if(dif < 9){
            pthread_mutex_lock(&MUTEX_MP_BUSY);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, dif);
            pthread_mutex_unlock(&MUTEX_MP_BUSY);
            *hmd_cortado = 1;
            *size_rest = sizeof(hmd_t) - dif;
            return;        
        }
        if(*hmd_cortado){

            pthread_mutex_lock(&MUTEX_MP_BUSY);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd + sizeof(hmd_t) - *size_rest, *size_rest);
            pthread_mutex_unlock(&MUTEX_MP_BUSY);
            return;
        }

        pthread_mutex_lock(&MUTEX_MP_BUSY);
        memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, sizeof(hmd_t));
        pthread_mutex_unlock(&MUTEX_MP_BUSY);

        return;
    } 

    *size_rest -= MEMORIA_CFG->TAMANIO_PAGINA;

    return;
}

//Se llama siempre entre mutex
uint32_t primer_frame_libre(){
    uint32_t frame = 0;
    for(;frame < MEMORIA_CFG->CANT_PAGINAS; frame++){
        if(!tabla_frames[frame].ocupado){
            return frame;
        }
    }
    return frame;
}

bool ocupar_frames(unsigned long id){
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);    
    uint32_t primer_frame_libre_ = primer_frame_libre();
    log_info(logger, "--------------%d-----------\n", primer_frame_libre_);
    if(primer_frame_libre_ == MEMORIA_CFG->CANT_PAGINAS){
        return false;
    }
    for(uint32_t i = 0; i < MEMORIA_CFG->MARCOS_POR_PROCESO; i++){
        tabla_frames[primer_frame_libre_ + i].ocupado = 1;
        tabla_frames[i].id_carpincho = id;            
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return true;
}
