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

//Esto debería llamarse siempre entre mutex
uint32_t cant_frame_libres(){
    uint32_t libres = 0;
    for(uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++){
        if(tabla_frames[i].libre) libres++;
    }
    return libres; 
}


uint32_t buscar_primer_frame_carpincho(unsigned long id_carpincho){
    uint32_t nro_frame = 0xFFFF;
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    for(uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++){
        if(tabla_frames[i].libre && tabla_frames[i].id_carpincho == id_carpincho) {
            pthread_mutex_unlock(&MUTEX_FRAMES_BUSY); 
            log_info(logger, "El frame es el %d \n", i);
            return i;
        }
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return nro_frame;
}

void primer_memalloc_carpincho(unsigned long id_carpincho, size_t* size_rest, uint32_t direccion_logica, uint32_t nro_frame, uint32_t nro_pagina, uint32_t* hmd_cortado){
    hmd_t* hmd = malloc(sizeof(hmd_t));
    if(nro_pagina == 0){
        hmd->prevAlloc = (int) NULL;
        hmd->nextAlloc = sizeof(hmd_t) + *size_rest;
        hmd->isFree = false;

        pthread_mutex_lock(&MUTEX_MP_BUSY);
        memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd, sizeof(hmd_t));
        pthread_mutex_unlock(&MUTEX_MP_BUSY);
        
        * size_rest += sizeof(hmd_t);       
    }

    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = false;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    int32_t dif = MEMORIA_CFG->TAMANIO_PAGINA - *size_rest;  
    
    if(dif > 0 ) {

        hmd->prevAlloc = 0;
        hmd->nextAlloc = (int) NULL;
        hmd->isFree = false;

        if(dif < 9){
            log_info(logger, "Allocando primer mitad del hmd en %d\n", nro_frame);
            pthread_mutex_lock(&MUTEX_MP_BUSY);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, dif);
            pthread_mutex_unlock(&MUTEX_MP_BUSY);
            *hmd_cortado = 1;
            *size_rest = sizeof(hmd_t) - dif;
            free(hmd);
            
            return;        
        }
        if(*hmd_cortado){
            log_info(logger, "Allocando segunda mitad del hmd en %d\n", nro_frame);
            pthread_mutex_lock(&MUTEX_MP_BUSY);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, ((void*) hmd) + sizeof(hmd_t) - *size_rest, *size_rest);
            pthread_mutex_unlock(&MUTEX_MP_BUSY);
            free(hmd);
            
            return;
        }
 
        log_info(logger, "Allocando todo el hmd en %d\n", nro_frame);        
        pthread_mutex_lock(&MUTEX_MP_BUSY);
        memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, sizeof(hmd_t));
        pthread_mutex_unlock(&MUTEX_MP_BUSY);
        free(hmd);

        return;
    } 

    *size_rest -= MEMORIA_CFG->TAMANIO_PAGINA;

    free(hmd);

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
        pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);        
        return false;
    }    
    for(uint32_t i = 0; i < MEMORIA_CFG->MARCOS_POR_PROCESO; i++){
        tabla_frames[primer_frame_libre_ + i].ocupado = 1;
        tabla_frames[primer_frame_libre_ + i].id_carpincho = id;            
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return true;
}

bool buscar_espacio_entre_hmd(unsigned long id_carpincho, size_t size, uint32_t* nro_pagina){
    tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
    //TODO: podria pasar por referencia un hmd
    uint32_t direccion_hmd = 0;
    void * hmd = malloc(sizeof(hmd_t));
    uint32_t tamanio_hmd;
    bool ret_code;

    while(1){
        *nro_pagina = direccion_hmd/MEMORIA_CFG->TAMANIO_PAGINA;
        ret_code = entra_en_pagina(list_get_pagina(tp_carpincho, *nro_pagina), size, &direccion_hmd, hmd, &tamanio_hmd);

        if(ret_code){
            return true;
        }

        if(*nro_pagina == tp_carpincho->pages){
            return false;
        }
    }

    free(hmd);
}

bool entra_en_pagina(entrada_tp_t* entrada_tp, size_t size, uint32_t* direccion_hmd, void* hmd, uint32_t* tamanio_hmd){
   
    while(1){
    pthread_mutex_lock(&MUTEX_MP_BUSY);
        uint32_t offset = *direccion_hmd % (MEMORIA_CFG->TAMANIO_PAGINA);
        int32_t diff = MEMORIA_CFG->TAMANIO_PAGINA - offset;
        uint32_t pos_frame = memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA; 
        if(diff > 0 && diff < 9){
            memcpy(hmd, pos_frame + offset, diff);
            *tamanio_hmd = diff; 
            pthread_mutex_unlock(&MUTEX_MP_BUSY);    
            return false;
        } else {
            memcpy(hmd, pos_frame + offset + *tamanio_hmd, sizeof(hmd_t) - *tamanio_hmd);
            hmd_t* hmd = (hmd_t*) hmd;

            *direccion_hmd = hmd->nextAlloc;
            if(hmd->isFree){
                int32_t tamanio_sobrante = hmd->nextAlloc - hmd->prevAlloc - size;
                if(tamanio_sobrante < 0){
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    return false;
                } else if (tamanio_sobrante <= 9){
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    return true;
                } else {
                    hmd_t* hmd_fragmentacion = malloc(sizeof(hmd_t));
                    hmd_fragmentacion->prevAlloc = *direccion_hmd;
                    hmd_fragmentacion->nextAlloc = hmd->nextAlloc;
                    hmd_fragmentacion->isFree = true;
                    //ME LO DIJO PITU
                    hmd->nextAlloc = *direccion_hmd + size;
                    memcpy(memoria_disponible + pos_frame + offset + sizeof(hmd_t) + size, hmd_fragmentacion, sizeof(hmd_t));
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    return true;
                }
            }      
        }      
    }
}

