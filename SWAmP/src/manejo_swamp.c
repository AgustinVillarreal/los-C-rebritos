#include "../include/manejo_swamp.h"

extern t_config_swamp* cfg;
extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;
extern t_log* logger;


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

uint32_t swamp_con_mas_espacio(){

    int cant_swamps = list_size(areas_de_swap);
    uint32_t pos; 
    void* elegido;


    for(int i = 0 ; i < cant_swamps ; i++){
        if(i == 0){
            elegido = list_get(areas_de_swap,i);
            pos = i;
        }
        else{

            bool buscar_x_nro_swap1(frame_swap_t* f){
                return f->nro_swap == pos;
            }

            bool buscar_x_nro_swap2(frame_swap_t* f){
                return f->nro_swap == i;
            }

            t_list* aux1 = list_filter(tablas_de_frames_swap,(void*)buscar_x_nro_swap1);
            t_list* aux2 = list_filter(tablas_de_frames_swap,(void*)buscar_x_nro_swap2);

            /* log_info(logger,"Elementos en swap 1: %d",list_size(aux1));
            log_info(logger,"Elementos en swap 2: %d",list_size(aux2)); */

            if(list_size(aux1) > list_size(aux2)){
                pos = i;
                elegido = list_get(areas_de_swap,i);
            }

            list_destroy(aux1);
            list_destroy(aux2);
        }
    }

    

    return pos;
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
    log_info(logger, "Primer byte libre: %d", pos);
    return pos;
}

void insertar_global(void* data , void* swap){

    uint32_t pos = posicion_primer_byte_libre(swap);
    memcpy(swap + pos , data , cfg->TAMANIO_PAGINA);

}

bool hay_marcos_libres(t_list* list){

    for(int i = 0 ; i < list_size(list) ; i++){
        frame_swap_t* f = list_get(list, i);
        if(f->libre){
            return true;
        }
    }

    return false;
}

uint32_t primer_byte_no_asignado(uint32_t nro_swap){

    bool cmp_nro_byte(frame_swap_t* f1, frame_swap_t* f2){
        return (f1->inicio - f2->inicio) < 0 ;
    }
    bool buscar_x_nro_swap(frame_swap_t* f){
        return f->nro_swap == nro_swap;
    }


    t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_nro_swap);
    list_sort(aux, (void*)cmp_nro_byte);

    log_info(logger,"Tamanio de lista AUX; %d", list_size(aux));

    uint32_t primero = 0; 
    uint32_t segundo = 0;

    for(int i = 0 ; i < list_size(aux) - 1 ; i++){
        
        frame_swap_t* frame1 = list_get(aux, i);
        frame_swap_t* frame2 = list_get(aux, i+1);

        primero = frame1->inicio;
        segundo = frame2->inicio;

        if(segundo - primero > cfg->TAMANIO_PAGINA){
            list_destroy(aux);
            return primero + cfg->TAMANIO_PAGINA;
        }

    }

    if(primero == 0 && segundo == 0){
        list_destroy(aux);
        return 0;
    }

    list_destroy(aux);
    return segundo + cfg->TAMANIO_PAGINA;
}