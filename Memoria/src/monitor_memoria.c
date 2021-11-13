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
        //TODO: CAMBIAAAAAAAAAAAAAAAAAAAAAR A FALSE PQ MORIMOS
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
    
    if(dif > 0 ) {

        hmd->prevAlloc = 0;
        hmd->nextAlloc = (int) NULL;
        hmd->isFree = true;

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

uint32_t buscar_espacio_entre_hmd(unsigned long id_carpincho, size_t size, uint32_t* nro_pagina){
    tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
    //TODO: podria pasar por referencia un hmd
    uint32_t direccion_hmd = 0;
    void * hmd = malloc(sizeof(hmd_t));
    uint32_t tamanio_hmd = 0;
    bool ret_code;

    while(1){
        *nro_pagina = direccion_hmd/MEMORIA_CFG->TAMANIO_PAGINA;
        ret_code = entra_en_pagina(id_carpincho, list_get_pagina(tp_carpincho, *nro_pagina), size, &direccion_hmd, hmd, &tamanio_hmd);

        if(ret_code){
            return direccion_hmd + sizeof(hmd_t);
        }

        if(*nro_pagina == tp_carpincho->pages){
            log_info(logger, "Chau me fui");
            //Allocar en la ultima posicion
            return 0xFFFF;
        }
    }

    free(hmd);
}

bool entra_en_pagina(unsigned long id_carpincho, entrada_tp_t* entrada_tp, size_t size, uint32_t* direccion_hmd, void* hmd_void, uint32_t* tamanio_hmd){
   
    while(1){
        pthread_mutex_lock(&MUTEX_MP_BUSY);
        uint32_t offset = *direccion_hmd % (MEMORIA_CFG->TAMANIO_PAGINA);
        int32_t diff = MEMORIA_CFG->TAMANIO_PAGINA - offset;
        void* pos_frame = memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA; 
        log_info(logger, "ads-----------%d------------", diff);
        log_info(logger, "HMD:-----------%d------------", *direccion_hmd);
        if(diff < 9){
            memcpy(hmd_void, pos_frame + offset, diff);
            *tamanio_hmd = diff; 
            pthread_mutex_unlock(&MUTEX_MP_BUSY);    
            return false;
        } else {
            memcpy(hmd_void, pos_frame + offset + *tamanio_hmd, sizeof(hmd_t) - *tamanio_hmd);
            hmd_t* hmd = (hmd_t*) hmd_void;

            uint32_t direccion_aux = *direccion_hmd;
            if(hmd->isFree){
                if(!hmd->nextAlloc){
                    pthread_mutex_unlock(&MUTEX_MP_BUSY); 
                    return false;
                }
                int32_t tamanio_sobrante = hmd->nextAlloc - direccion_aux - size - sizeof(hmd_t);
                log_info(logger, "tamanio:--------------%d-----------", tamanio_sobrante);        
                if(tamanio_sobrante < 0){
                    log_info(logger, "UPA ENTRE ESTOS HMD NO ENTRE", *direccion_hmd);     
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    *direccion_hmd = hmd->nextAlloc; 
                    return false;
                //El alloc entra justo entre dos hmd
                } else if (tamanio_sobrante == 0){
                    log_info(logger, "ENTRE COMPLETITO", *direccion_hmd);             
                    //TODO: Aca hacer que este free en false, entra justo
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    return true;
                //El alloc entra pero hay que fragmentar una cantidad
                } else if (tamanio_sobrante >= 9){
                    log_info(logger, "DE QUE COLOR ES EL HMD?", *direccion_hmd);
                    hmd_t* hmd_fragmentacion = malloc(sizeof(hmd_t));
                    hmd_fragmentacion->nextAlloc = hmd->nextAlloc;
                    hmd_fragmentacion->prevAlloc = direccion_aux;
                    hmd_fragmentacion->isFree = true;
                    //Posicion Logica del hmd
                    uint32_t posicion_logica_hmd_fragmentacion = direccion_aux + sizeof(hmd_t) + size;
                    uint32_t nro_pagina_hmd_fragmentacion = posicion_logica_hmd_fragmentacion / (MEMORIA_CFG->TAMANIO_PAGINA);
                    uint32_t offset_hmd_fragmentacion = posicion_logica_hmd_fragmentacion % (MEMORIA_CFG->TAMANIO_PAGINA);
                    tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
                    uint32_t cant_paginas_hmd_fragmentacion = cant_paginas_relativa(posicion_logica_hmd_fragmentacion, sizeof(hmd_t));
                    uint32_t hmd_cortado = 0;
                    size_t size_aux = size;
                    entrada_tp_t* entrada_tp_aux = entrada_tp;

                    // hmd->nextAlloc = posicion_logica_hmd_fragmentacion;

                    // uint32_t cant_paginas_hmd_anterior = cant_paginas_relativa(direccion_aux, sizeof(hmd_t));
                    // uint32_t nro_pagina_hmd_anterior = direccion_aux / (MEMORIA_CFG->TAMANIO_PAGINA);
                    // for(uint32_t i = 0; i < cant_paginas_hmd_anterior; i++){
                    //     if(nro_pagina_hmd_anterior != entrada_tp_aux->nro_pagina){
                    //         entrada_tp_aux = list_get_pagina(tp_carpincho, nro_pagina_hmd_anterior);
                    //     }
                    //     alloc_hmd(0, entrada_tp_aux, &hmd_cortado, hmd, tp_carpincho);
                    // }

                    // hmd_cortado = 0;
                    
                    for(uint32_t i = 0; i < cant_paginas_hmd_fragmentacion; i++){
                        if(nro_pagina_hmd_fragmentacion != entrada_tp->nro_pagina){
                            entrada_tp = list_get_pagina(tp_carpincho, nro_pagina_hmd_fragmentacion);
                        }
                        alloc_hmd(&size_aux, entrada_tp, &hmd_cortado, hmd_fragmentacion, tp_carpincho);
                    }
             
                    pthread_mutex_unlock(&MUTEX_MP_BUSY);
                    free(hmd_fragmentacion);
                    return true;
                }
            } 
            *direccion_hmd = hmd->nextAlloc;    
            log_info(logger, "Direccion_hmd:------%d----------", *direccion_hmd);            
        }
    }     
}

//Retorna la cantidad de paginas, posicion es la logica
uint32_t cant_paginas_relativa(uint32_t posicion, uint32_t size){
    size_t rem;
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    rem = (size + posicion) % t_pag;
    return (rem) ? size/t_pag + 1 : size/t_pag;
}


//Alloc generico
//SE LLAMA SIEMPRE ENTRE MUTEX VIEJA (MUTEX_MP_BUSY)
void alloc_hmd(size_t* size_rest, entrada_tp_t* entrada_tp, uint32_t* hmd_cortado, hmd_t* hmd_a_crear, tp_carpincho_t* tp_carpincho){

    uint32_t nro_pagina = entrada_tp->nro_pagina;
    uint32_t nro_frame = entrada_tp->nro_frame;
    uint32_t offset = 0;

    if(!hmd_cortado){
        //TODO: ver mejor
        pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
        tabla_frames[nro_frame].libre = false;
        pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

        //Nadie me asegura que estoy en la misma pag
        offset = (hmd_a_crear->prevAlloc + sizeof(hmd_t)) % (MEMORIA_CFG->TAMANIO_PAGINA);
        bool misma_pagina = (uint32_t)((hmd_a_crear->prevAlloc + sizeof(hmd_t)) / (MEMORIA_CFG->TAMANIO_PAGINA)) == nro_pagina;

        if(!misma_pagina){
            nro_pagina++; 
            entrada_tp = list_get_pagina(tp_carpincho, nro_pagina);
            nro_frame = entrada_tp->nro_frame;

            pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
            tabla_frames[nro_frame].libre = false;
            pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);
        }
    }
    
    int32_t dif = MEMORIA_CFG->TAMANIO_PAGINA - offset - *size_rest;  
    
    if(dif > 0 ) {

        if(dif < 9){
            log_info(logger, "Allocando primer mitad del hmd en %d\n", nro_frame);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset + *size_rest, hmd_a_crear, dif);
            *hmd_cortado = 1;
            *size_rest = sizeof(hmd_t) - dif;
            return;        
        }
        if(*hmd_cortado){
            log_info(logger, "Allocando segunda mitad del hmd en %d\n", nro_frame);
            memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, ((void*) hmd_a_crear) + sizeof(hmd_t) - *size_rest, *size_rest);
            return;
        }
 
        log_info(logger, "Allocando todo el hmd en %d\n", nro_frame);        
        memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset + *size_rest, hmd_a_crear, sizeof(hmd_t));
        return;
    } 
    *size_rest -= MEMORIA_CFG->TAMANIO_PAGINA;
    return;
}
