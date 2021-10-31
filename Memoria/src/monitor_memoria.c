#include "../include/monitor_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;



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

void mutex_init(){
    //TODO: iniciar mutex aca 
    pthread_mutex_init(&MUTEX_FRAME_BUSY, NULL);
    pthread_mutex_init(&MUTEX_MP_BUSY, NULL);
    return;
}

uint32_t cant_frames_libres() {
    uint32_t libres = 0;
    pthread_mutex_lock(&MUTEX_FRAME_BUSY);
    for (uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++) {
        if (tabla_frames[i].libre == 1)
            libres++;
    }
    pthread_mutex_unlock(&MUTEX_FRAME_BUSY);
    return libres;
}

uint32_t cant_paginas(uint32_t size, size_t* rem){
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    *rem = size % t_pag;
    return (*rem) ? size/t_pag + 1 : size/t_pag;
}

uint32_t primer_frame_libre_framo(uint32_t pid, uint32_t* inicio) {
    uint32_t primero_vacio = 0xFFFF;

    pthread_mutex_lock(&MUTEX_FRAME_BUSY);
    for (uint32_t i = 0; i < MEMORIA_CFG->CANT_PAGINAS; i++) {
        if (primero_vacio == 0xFFFF && tabla_frames[i].libre)
            primero_vacio = i;

        if (tabla_frames[i].amedias && (tabla_frames[i].pid_ocupador == pid)) {
            pthread_mutex_unlock(&MUTEX_FRAME_BUSY);

            *inicio = tabla_frames[i].inicio_hueco;
            return i;
        }
    }
    pthread_mutex_unlock(&MUTEX_FRAME_BUSY);

    *inicio = 0;
    return primero_vacio;
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