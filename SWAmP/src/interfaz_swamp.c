#include "../include/interfaz_swamp.h"

extern t_log* logger;
extern t_config_swamp* cfg;

extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;
              
void proceder_asignacion(int fd,unsigned long id,uint32_t pagina,void* data,bool asignacion_fija){

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id;
    }

    //Me fijo si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){

        uint32_t pos_swap = swamp_con_mas_espacio();
        void* swap = list_get(areas_de_swap,pos_swap);


        // Me fijo si hay espacio 
        if(cantidad_de_espacio_swamp_libre(pos_swap) < cfg->TAMANIO_PAGINA){
            log_error(logger,"No espacio sufieciente en swap");
            send_ack(fd,false);
            return;
        }


        // Si no esta lo agrego a la tabla
        uint32_t pos_libre = posicion_primer_byte_libre(swap);

        frame_swap_t* frame = malloc(sizeof(frame_swap_t));
        frame->pid = id;
        frame->nro_pagina = pagina;
        frame->inicio = pos_libre;
        frame->nro_swap = pos_swap;

        list_add(tablas_de_frames_swap,frame);

        // Y ecribo la pagina que me mando en swap
        memcpy(swap + pos_libre , data , cfg->TAMANIO_PAGINA);

    }
    else{
        
        if(asignacion_fija && !hay_marcos_disponibles(id)){
            log_error(logger,"Se alcanzo la cantidad maxima de marcos por carpincho");
            send_ack(fd,false);
            return;
        }
        // Esta el frame en swap
        // Tengo que averiguar en que swap esta
        t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);
        frame_swap_t* frame = list_get(aux,0);

        //Tomo la swap correspondiente

        void* swap = list_get(areas_de_swap,frame->nro_swap);


        log_info(logger,"Cantidad de swap libre: %d",cantidad_de_espacio_swamp_libre(frame->nro_swap));
        // Me fijo si hay espacio 
        if(cantidad_de_espacio_swamp_libre(frame->nro_swap) < cfg->TAMANIO_PAGINA){
            list_destroy(aux);
            log_error(logger,"No espacio sufieciente en swap");
            send_ack(fd,false);
            return;
        }

        // Busco el primer byte libre
        uint32_t pos_libre = posicion_primer_byte_libre(swap);
        

        // Pongo la referncia en la tabla y lo agrego a la swap
        frame_swap_t* f = malloc(sizeof(frame_swap_t));
        f->pid = id;
        f->nro_pagina = pagina;
        f->inicio = pos_libre;
        f->nro_swap = frame->nro_swap;

        list_add(tablas_de_frames_swap,f);

        memcpy(swap + pos_libre , data , cfg->TAMANIO_PAGINA);

        list_destroy(aux);
    }

    // Envio que se realizo la escritura
    send_ack(fd,true);
}


void buscar_frame_en_swap(unsigned long id, uint32_t nro_pagina, void** data, bool esquema_asignacion){

}

void borrar_carpincho_swap(unsigned long carpincho_id){

}

bool revisar_espacio_libre(unsigned long carpincho_id, uint32_t cant_paginas, bool asignacion_fija){
    
    bool buscar_x_id(frame_swap_t* f){
        return f->pid == carpincho_id;
    }

    //Pregunto si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*) buscar_x_id)){
        //El caprincho no esta en swap busco si hay espacio en alguno de los swap para las paginas
        bool respuesta = false;

        for(int i = 0 ; i < list_size(areas_de_swap); i++){

            void* swap = list_get(areas_de_swap,i);

            uint32_t paginas_libres = cantidad_de_espacio_swamp_libre(i) / cfg->TAMANIO_PAGINA;

            if(paginas_libres >= cant_paginas){
                respuesta = true;
                break;
            }
        }

        return respuesta;

    }
    else{

        if(asignacion_fija){

            t_list* aux = list_filter(tablas_de_frames_swap,(void*) buscar_x_id);

            if(list_size(aux) >= cfg->MARCOS_POR_CARPINCHO){
                list_destroy(aux);
                return false;
            }

            frame_swap_t* frame = list_get(aux,0);

            void* swap = list_get(areas_de_swap,frame->nro_swap);

            uint32_t libres = cantidad_de_espacio_swamp_libre(frame->nro_swap) / cfg->TAMANIO_PAGINA;

            list_destroy(aux);

            return libres >= cant_paginas;

        }
        else{

            frame_swap_t* frame = list_find(tablas_de_frames_swap,(void*)buscar_x_id);

            void* swap = list_get(areas_de_swap, frame->nro_swap);

            uint32_t paginas_libres = cantidad_de_espacio_swamp_libre(frame->nro_swap) / cfg->TAMANIO_PAGINA;

            return paginas_libres >= cant_paginas;

        }
    }
}