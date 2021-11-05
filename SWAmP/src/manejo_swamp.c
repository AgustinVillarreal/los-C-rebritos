#include "../include/manejo_swamp.h"

extern t_config_swamp* cfg;
extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;


/*   Me devuelve los bytes libres de swap que tiene uno de los espacios de swap */

uint32_t cantidad_de_espacio_swamp_libre(void* swamp){

    uint32_t cant = 0;

    for(int i = 0 ; i < cfg->TAMANIO_SWAP; i++){   
        if(((char*)swamp)[i] == '\0'){
            cant++;
        }
    }

    return cant;
}

/* Busca la swap con mas espacio libre */

void* swamp_con_mas_espacio(){

    int cant_swamps = list_size(areas_de_swap);
    void* elegido;

    for(int i = 0 ; i < cant_swamps ; i++){
        if(i == 0){
            elegido = list_get(areas_de_swap,i);
        }
        else{
            if(cantidad_de_espacio_swamp_libre(elegido) < cantidad_de_espacio_swamp_libre(list_get(areas_de_swap,i))){
                elegido = list_get(areas_de_swap,i);
            }
        }
    }

    return elegido;
}

/* Esto deberia darme un void* en -> "dest" que representa la pagina que guarde */
/* Esto deberia poder solucionar el problema de leer de memoria */

void tomar_frame_swap(unsigned long id_carpincho, uint32_t nro_pagina, void* dest){

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id_carpincho && f->nro_pagina == nro_pagina;
    }

    frame_swap_t* frame = list_find(tablas_de_frames_swap, (void*)buscar_x_id);

    void* swap = list_get(areas_de_swap, frame->nro_swap);

    memcpy(dest, swap + frame->inicio, cfg->TAMANIO_PAGINA);

}

/* Setea la pagina en swap con '\0' cuando se quiere borrar al carpincho  */

void formatear_pagina_swap(uint32_t inicio,void* swap){

    memset(swap+inicio,'\0',cfg->TAMANIO_PAGINA);

}

/* Elimina al carpincho de la lista de frames y setea todas las paginas que ocupaba con '\0' */

void eliminar_carpincho_de_memoria(unsigned long id_carpincho){

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id_carpincho;
    }

    t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);

    for(int i = 0 ; i < list_size(aux) ; i++){

        frame_swap_t* frame = list_get(aux,i);

        void* swap = list_get(areas_de_swap, frame->nro_swap);

        formatear_pagina_swap(frame->inicio,swap);

    }

    list_remove_and_destroy_all_by_condition(tablas_de_frames_swap,(void*)buscar_x_id,(void*)destroy_frame);

    free(aux); // --> No se si deberia liberarlo
}


uint32_t posicion_primer_byte_libre(void* data){
    uint32_t pos = 0;

    for(int i = 0;((char*)data)[i]!='\0'; i++){
        pos++;
    }

    return pos;
}

void insertar_global(void* data , void* swap){

    uint32_t pos = posicion_primer_byte_libre(swap);
    memcpy(swap + pos , data , cfg->TAMANIO_PAGINA);

}

