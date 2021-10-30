#include "../include/monitor_memoria.h"

extern t_config_memoria*  MEMORIA_CFG;

// uint32_t buscar_first_fit(int size){
//     int i=0,aux=0,primer_pos_libre;
//     while(memoria_principal[i] == 0 || aux != size){
//         aux++;
//         i++;
//         if(memoria_principal[i] == 1 && aux < size){
//             aux = 0;
//         }
//         if(aux == size){
//             primer_pos_libre = &memoria_principal[i - aux + 1];
//             return primer_pos_libre;
//         }
//     }
    
//     return NULL;
// }

uint32_t cant_frames_libres() {
    uint32_t libres = 0;
    pthread_mutex_lock(&MUTEX_FRAME_OCUPADO);
    for (uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++) {
        if (tabla_frames[i].libre == 1)
            libres++;
    }
    pthread_mutex_unlock(&MUTEX_FRAME_OCUPADO);
    return libres;
}

uint32_t cant_paginas(uint32_t size, size_t* rem){
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    *rem = size % t_pag;
    return (*rem) ? size/t_pag + 1 : size/t_pag;
}

frame_t buscar_frame_ff(){
    
}

void* algoritmo_mmu_clock_m (){
    return NULL;

}
void* algoritmo_mmu_lru(){
    return NULL;

}

void* algoritmo_tlb_fifo(){
    return NULL;

}
void* algoritmo_tlb_lru(){
    return NULL;

}

void* asignacion_fija(){
    return NULL;

}

void* asignacion_global(){
    return NULL;

}