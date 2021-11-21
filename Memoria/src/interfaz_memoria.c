#include "../include/interfaz_memoria.h"

extern t_config_memoria* MEMORIA_CFG;
extern t_list* tabla_frames;
extern t_log* logger;


void mate_init(unsigned long id){
    tp_carpincho_t* tp_carpincho = malloc(sizeof(tp_carpincho_t));
    tp_carpincho->id_carpincho = id;
    tp_carpincho->pages = 0;
    tp_carpincho->paginas = list_create();
    tp_carpincho->posible_victima_fija = 0;
    pthread_mutex_init(&tp_carpincho->mutex_paginas, NULL);
    agregar_tabla_a_tp_carpinchos(tp_carpincho);
    return; 
}

void ocupar_frames_carpincho(unsigned long id){
    if(MEMORIA_CFG->FIJA){
        //TODO: Si tiene la tabla de paginas vacia significa que no tiene ninguna estructura en memoria --> la creo, si no, no ocupo nada
        if(tabla_vacia(id)){
            //TODO: Si ocupar frames retorna false significa que esta algo raro con el grado de multiprogramacion
            bool ret_code = ocupar_frames(id);
            if(!ret_code){
                log_error(logger, "Te pasaste del grado de multiprogramacion - Incongruencia con Kernel\n");
                return;
            }
        }    
    }
}



bool allocar_carpincho(unsigned long id_carpincho, size_t size, uint32_t* direccion_logica){
    
    //Evaluo si es el primer alloc
    size_t size_stream;
    
    bool primer_alloc = tabla_vacia(id_carpincho);

    //TODO: En caso de emergencia revisar aca xd
    return allocar_carpincho_en_mp(id_carpincho, size, primer_alloc, direccion_logica);    
} 

uint32_t liberar_espacio_mp(unsigned long id_carpincho, uint32_t* direccion_logica){

    uint32_t nro_de_pagina = (*direccion_logica) / MEMORIA_CFG->TAMANIO_PAGINA ; 
    tp_carpincho_t* carpincho = find_tp_carpincho(id_carpincho);
    if(nro_de_pagina > list_size(carpincho-> paginas)){
        return 0;
    }

    liberar_Alloc( id_carpincho, direccion_logica);

    return 1;
}
uint32_t leer_espacio_mp(char* a){
    return 1;
}
uint32_t escribir_espacio_mp(){
    return 1;
}
