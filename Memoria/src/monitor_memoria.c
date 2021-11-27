#include "../include/monitor_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;
extern void* memoria_principal;
extern t_log* logger;

void mutex_init_memoria(){
    //TODO: iniciar mutex aca 
    pthread_mutex_init(&MUTEX_FRAMES_BUSY, NULL);
    pthread_mutex_init(&MUTEX_MP_BUSY, NULL);
    pthread_mutex_init(&MUTEX_GLOBAL_TUR, NULL);
    pthread_mutex_init(&MUTEX_ALGORITMOS, NULL);
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


// Se busca y se ocupa el frame
uint32_t buscar_primer_frame_carpincho(unsigned long id_carpincho){
    uint32_t nro_frame = 0xFFFF;
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    for(uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++){
        if(tabla_frames[i].libre) {
            if(MEMORIA_CFG->FIJA && tabla_frames[i].id_carpincho == id_carpincho){
                tabla_frames[i].libre = false;
                pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);
                log_info(logger, "El frame es el %d \n", i);                                 
                return i;
            } else if (!MEMORIA_CFG->FIJA){
                tabla_frames[i].libre = false;   
                pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);                              
                return i;
            }
        }
    }
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return nro_frame;
}


void primer_memalloc_carpincho(unsigned long id_carpincho, size_t* size_rest,entrada_tp_t* entrada_tp, uint32_t* hmd_cortado){
    hmd_t* hmd = malloc(sizeof(hmd_t));
    if(entrada_tp->nro_pagina == 0){
        hmd->prevAlloc = (int) NULL;
        hmd->nextAlloc = sizeof(hmd_t) + *size_rest;
        hmd->isFree = false;

        escritura_memcpy_size(hmd, entrada_tp, 0, sizeof(hmd_t));
        
        // pthread_mutex_lock(&MUTEX_MP_BUSY);
        // memcpy(memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd, sizeof(hmd_t));
        // pthread_mutex_unlock(&MUTEX_MP_BUSY);
        
        * size_rest += sizeof(hmd_t);       
    }

    int32_t dif = MEMORIA_CFG->TAMANIO_PAGINA - *size_rest;  
    
    if(dif > 0 ) {

        hmd->prevAlloc = 0;
        hmd->nextAlloc = (int) NULL;
        hmd->isFree = true;

        if(dif < 9){
            log_info(logger, "Allocando primer mitad del hmd en %d\n", entrada_tp->nro_frame);
            
            escritura_memcpy_size((void*)hmd, entrada_tp, *size_rest, dif);
            // pthread_mutex_lock(&MUTEX_MP_BUSY);
            // memcpy(memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, dif);
            // pthread_mutex_unlock(&MUTEX_MP_BUSY);

            *hmd_cortado = 1;
            *size_rest = sizeof(hmd_t) - dif;
            free(hmd);
            
            return;        
        }
        if(*hmd_cortado){
            log_info(logger, "Allocando segunda mitad del hmd en %d\n", entrada_tp->nro_frame);
            escritura_memcpy_size(((void*) hmd) + sizeof(hmd_t) - *size_rest, entrada_tp, 0, *size_rest);            
            // pthread_mutex_lock(&MUTEX_MP_BUSY);
            // memcpy(memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, ((void*) hmd) + sizeof(hmd_t) - *size_rest, *size_rest);
            // pthread_mutex_unlock(&MUTEX_MP_BUSY);
            free(hmd);
            
            return;
        }
 
        log_info(logger, "Allocando todo el hmd en %d\n", entrada_tp->nro_frame);  
        escritura_memcpy_size((void*) hmd, entrada_tp, *size_rest, sizeof(hmd_t));            
              
        // pthread_mutex_lock(&MUTEX_MP_BUSY);
        // memcpy(memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + *size_rest, hmd, sizeof(hmd_t));
        // pthread_mutex_unlock(&MUTEX_MP_BUSY);
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
//Retorna la cantidad de paginas, posicion es la logica
uint32_t cant_paginas_relativa(uint32_t posicion, size_t size){
    size_t rem;
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t offset = posicion % t_pag;
    rem = (size + posicion) % t_pag;
    uint32_t pags_parcial = (size + offset)/t_pag;
    uint32_t cant_paginas = (rem) ? pags_parcial + 1 : pags_parcial;
    // if(posicion + size > MEMORIA_CFG->TAMANIO_PAGINA) {
    //     cant_paginas++;
    // }
    
    return cant_paginas;
}

void lectura_memcpy_size(entrada_tp_t* entrada_tp, uint32_t offset, void* destino, size_t size){
  //leer de memoria principal
  actualizar_bits(entrada_tp, false);
  pthread_mutex_lock(&MUTEX_MP_BUSY);
  memcpy(destino, (void*) (memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset), size);
  pthread_mutex_unlock(&MUTEX_MP_BUSY);
  //TODO: Comento este log porque estaba tirando error de valgrind
  //log_info(logger, "aaaa: %s", (char*) destino);
}
 
void escritura_memcpy_size(void* data, entrada_tp_t* entrada_tp, uint32_t offset, size_t size){
  //escribir en memoria principal
  actualizar_bits(entrada_tp, true);
  pthread_mutex_lock(&MUTEX_MP_BUSY);
  memcpy((void*) (memoria_principal + entrada_tp->nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset), data, size);
  pthread_mutex_unlock(&MUTEX_MP_BUSY);
}


// void escritura_memcpy_size_hmd(hmd_t* data, uint32_t nro_frame, uint32_t offset, size_t size){
//   //escribir en memoria principal
//   pthread_mutex_lock(&MUTEX_MP_BUSY);
//   memcpy((void*) (memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset), data, size);
//   pthread_mutex_unlock(&MUTEX_MP_BUSY);
// }

