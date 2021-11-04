#include "../include/interfaz_memoria.h"

extern t_config_memoria* MEMORIA_CFG;

void mate_init(unsigned long id){
    tp_carpincho_t* tp_carpincho = malloc(sizeof(tp_carpincho_t));
    tp_carpincho->id_carpincho = id;
    tp_carpincho->pages = 0;
    tp_carpincho->paginas = list_create();
    pthread_mutex_init(&tp_carpincho->mutex_paginas, NULL);
    agregar_tabla_a_tp_carpinchos(tp_carpincho);
    return; 
}


bool allocar_carpincho(unsigned long id_carpincho, size_t size, uint32_t* direccionLogica){
    
    //Evaluo si es el primer alloc
    size_t size_stream;
    
    bool primer_alloc = tabla_vacia(id_carpincho);

    if(primer_alloc){
        if(MEMORIA_CFG->FIJA){  
            *direccionLogica = allocar_carpincho_fija(id_carpincho, size);
        }        
    }
                               

    return direccionLogica;
    
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
