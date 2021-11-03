#include "../include/interfaz_memoria.h"

extern t_config_memoria* MEMORIA_CFG;	 

bool allocar_carpincho(t_list* tabla_carpincho, unsigned long id_carpincho, size_t size){
    //Evaluo si es el primer alloc
    // size_t size_stream;
    // bool primer_alloc = tabla_vacia(tabla_carpincho);
    // bool primerPagina;
    // if(primer_alloc){
    //     primerPagina = true;
    //     size_stream =  sizeof(hmd_t)*2 + size_data;    
    // } else {
    //     primerPagina = false;
    //     size_stream = sizeof(hmd_t) + size_data;
    // }
    // bool nuevapag;
    // //Primero deberia ver si entra o no entra en la mp 
    // if(entra_en_mp(size_stream)){
    //     pthread_mutex_lock(&MUTEX_MP_BUSY);
    //     ret_code = alloc_carpincho_en_mp(size_stream, id_carpincho, tabla_carpincho, &nuevapag); 
    //     pthread_mutex_unlock(&MUTEX_MP_BUSY);                                   
    // }  else {
        /*
    ret_code = send_probar_en_swamp(size_stream, id_carpincho);
    */
    //}

    
} 


// uint32_t reservar_espacio_mp(size_t size_stream, unsigned long id, t_list* tabla_carpincho, bool primerPagina){
//     if(primerPagina){
//         entrada_tp_t* entrada_tp = malloc(sizeof(entrada_tp_t));
//         entrada_tp->nro_pagina = 0;
//         entrada_tp->nro_frame = buscar_frame_ff();
//         entrada_tp->bit_P = 1;
//         if(MEMORIA_CFG->LRU){
//             //TODO: ver lo de LRU
//         } else {
//             //TODO: ver lo del clock
//         }

//     }

//     bool nuevaPagina;
//     alloc_carpincho_en_mp(size_stream, id, &nuevaPagina);
//     //Pongo un hmd al principio y uno al final creo, TODO: consultar
//     void* size_data = malloc(sizeof(int) + sizeof(hmd_t)*2);



//     printf("Se reservo %d \n", size_stream);
//     return 1;
// }

uint32_t liberar_espacio_mp(char* a, uint32_t* b){
    return 1;
}
uint32_t leer_espacio_mp(char* a){
    return 1;
}
uint32_t escribir_espacio_mp(){
    return 1;
}
