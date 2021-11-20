#include "../include/interfaz_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern t_list* tabla_frames;
extern t_log* logger;


void mate_init(unsigned long id){
    tp_carpincho_t* tp_carpincho = malloc(sizeof(tp_carpincho_t));
    tp_carpincho->id_carpincho = id;
    tp_carpincho->pages = 0;
    tp_carpincho->paginas = list_create();
    pthread_mutex_init(&tp_carpincho->mutex_paginas, NULL);
    if(MEMORIA_CFG->FIJA){
        //TODO: Si ocupar frames retorna false significa que esta algo raro con el grado de multiprogramacion
        bool ret_code = ocupar_frames(id);
        if(!ret_code){
            log_error(logger, "Te pasaste del grado de multiprogramacion - Incongruencia con Kernel\n");
            return;
        }
    }

    agregar_tabla_a_tp_carpinchos(tp_carpincho);
    return; 
}


bool allocar_carpincho(unsigned long id_carpincho, size_t size, uint32_t* direccion_logica){
    
    //Evaluo si es el primer alloc
    size_t size_stream;
    
    bool primer_alloc = tabla_vacia(id_carpincho);

    if(MEMORIA_CFG->FIJA){  
        return allocar_carpincho_fija(id_carpincho, size, primer_alloc, direccion_logica);
    }        
                               

    return true;
    
} 

uint32_t liberar_espacio_mp(unsigned long id_carpincho, uint32_t* direccion_logica){

    uint32_t nro_de_pagina = direccion_logica/ TAMAÑO_DE_PAGINA ; 
    if(nro_de_pagina >= list_size(find_tp_carpincho(id_carpincho) )){
        return 0;
    }
    liberar_Alloc(id_carpincho, direccion_logica));

    return 1;
}
uint32_t leer_espacio_mp(char* a){
    return 1;
}
uint32_t escribir_espacio_mp(){
    return 1;
}
