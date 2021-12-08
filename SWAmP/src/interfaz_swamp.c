#include "../include/interfaz_swamp.h"

extern t_log* logger;
extern t_config_swamp* cfg;

extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;
              
void proceder_escritura(int fd,unsigned long id,uint32_t pagina,void* data){


    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id;
    }

    bool buscar_x_id_y_pagina(frame_swap_t* f){
        return f->pid == id && f->nro_pagina == pagina;
    }

        /* Me fijo si el carpincho esta en swap */
    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){

        log_error(logger,"El carpincho nro %d no esta en swap",id);
        // send_ack(fd,false);
        usleep(1000*cfg->RETARDO_SWAP);
        return;    
    }
    else{
        
        /* Chequear que tabien esta la pagina */

        if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id_y_pagina)){
            log_error(logger,"La pagina nro %d no existe para el carpincho %d",pagina,id);
            usleep(1000*cfg->RETARDO_SWAP);
            // send_ack(fd,false);
            return;
        }

        /* Ahora si puedo buscarlo y escribir el contenido */

        frame_swap_t* frame = list_find(tablas_de_frames_swap,(void*)buscar_x_id_y_pagina);
        void* swap = list_get(areas_de_swap,frame->nro_swap);        
        memcpy(swap + frame->inicio , data , cfg->TAMANIO_PAGINA);
    }
  

    // Envio que se realizo la escritura
    usleep(1000*cfg->RETARDO_SWAP);
    // send_ack(fd,true);
}


void buscar_frame_en_swap(int fd,unsigned long id, uint32_t nro_pagina, void** data){
    *data = tomar_frame_swap(id,nro_pagina);
    usleep(1000*cfg->RETARDO_SWAP);
    // send_ack(fd,true);
}

void borrar_carpincho_swap(int fd,unsigned long carpincho_id){
    eliminar_carpincho_de_memoria(carpincho_id);
    // usleep(1000*cfg->RETARDO_SWAP);
    // send_ack(fd,true);
}

bool revisar_espacio_libre(int fd,unsigned long carpincho_id, uint32_t cant_paginas, bool asignacion_fija){
    
    bool buscar_x_id(frame_swap_t* f){
        return f->pid == carpincho_id;
    }

    log_warning(logger, "id: %lu cant %d asig fija %d", carpincho_id, cant_paginas, asignacion_fija);

    //Pregunto si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*) buscar_x_id)){
        //El caprincho no esta en swap busco si hay espacio en alguno de los swap para las paginas
        bool respuesta = false;

        for(int i = 0 ; i < list_size(areas_de_swap); i++){

            log_warning(logger, "Vuelta %d", i);

            void* swap = list_get(areas_de_swap,i);

            uint32_t paginas_libres = cantidad_de_espacio_swamp_libre(i) / cfg->TAMANIO_PAGINA;

            log_warning(logger, "paginas_libres %d", paginas_libres);
            

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


void proceder_allocar(int fd,unsigned long id, uint32_t cant_paginas, bool asignacion_fija){
    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id;
    }

    //Me fijo si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){

        /* Busco con que swap deberia ir */
        uint32_t pos_swap = swamp_con_mas_espacio();

        /* Me fijo si hay espacio */ 
        if(cantidad_de_espacio_swamp_libre(pos_swap) < cfg->TAMANIO_PAGINA * cant_paginas){
            log_error(logger,"No espacio sufieciente en swap para meter %d paginas",cant_paginas);
            usleep(1000*cfg->RETARDO_SWAP);
            // send_ack(fd,false);
            return;
        }

        if(asignacion_fija && !hay_marcos_disponibles(id,cant_paginas)){
            log_error(logger,"Se alcanzo la cantidad maxima de marcos por carpincho");
            usleep(1000*cfg->RETARDO_SWAP);
            // send_ack(fd,false);
            return;
        }

        /* Creo la cantidad de paginas correspondientes */
        for(int i = 0 ; i < cant_paginas ;i++){

            /* Si no esta lo agrego a la tabla */
            uint32_t pos_libre = posicion_primer_byte_libre(pos_swap);

            frame_swap_t* frame = malloc(sizeof(frame_swap_t));
            frame->pid = id;
            frame->nro_pagina = i;
            frame->inicio = pos_libre;
            frame->nro_swap = pos_swap;

            list_add(tablas_de_frames_swap,frame);
        }
    }
    else{

        frame_swap_t* cmp_nro_pagina(frame_swap_t* frame1, frame_swap_t* frame2){
            return frame1->nro_pagina < frame2->nro_pagina ? frame2 : frame1 ;
        }
        
        if(asignacion_fija && !hay_marcos_disponibles(id,cant_paginas)){
            log_error(logger,"Se alcanzo la cantidad maxima de marcos por carpincho");
            usleep(1000*cfg->RETARDO_SWAP);
            // send_ack(fd,false);
            return;
        }

        /* Esta el frame en swap */
        // Tengo que averiguar en que swap esta
        t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);
        frame_swap_t* frame = list_get_maximum(aux, (void*)cmp_nro_pagina);

        uint32_t next_pag = frame->nro_pagina + 1;

        // Me fijo si hay espacio 
        if(cantidad_de_espacio_swamp_libre(frame->nro_swap) < cfg->TAMANIO_PAGINA * cant_paginas){
            list_destroy(aux);
            log_error(logger,"No espacio sufieciente en swap para meter %d paginas",cant_paginas);
            usleep(1000*cfg->RETARDO_SWAP);
            // send_ack(fd,false);
            return;
        }

        for(int i = next_pag ; i < cant_paginas + next_pag ; i++){
            // Busco el primer byte libre
            uint32_t pos_libre = posicion_primer_byte_libre(frame->nro_swap);
        
            // Pongo la referncia en la tabla y lo agrego a la swap
            frame_swap_t* f = malloc(sizeof(frame_swap_t));
            f->pid = id;
            f->nro_pagina = i;
            f->inicio = pos_libre;
            f->nro_swap = frame->nro_swap;

            list_add(tablas_de_frames_swap,f);
        }
        list_destroy(aux);
    }

    // Envio que se realizo la escritura
    usleep(1000*cfg->RETARDO_SWAP);
    // send_ack(fd,true);
}

void liberar_marcos(int cliente_socket,unsigned long carpincho_id,uint32_t cant_paginas){

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == carpincho_id;
    }

    bool cmp_numero_pagina(frame_swap_t* f1,frame_swap_t* f2){
        return f1->nro_pagina > f2->nro_pagina;
    }

    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){
        log_error(logger,"No esta el carpincho %d en swap",carpincho_id);
        usleep(1000*cfg->RETARDO_SWAP);
        // send_ack(cliente_socket,false);
        return;
    }

    t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);
    list_sort(aux,(void*)cmp_numero_pagina);
    frame_swap_t* f = list_get(aux , 0);
    void* swap = list_get(areas_de_swap,f->nro_swap);


    uint32_t pos_ult_pagina = list_size(aux) - 1;

    log_info(logger,"El tamanio de aux es %d y la ultima pos es %d", list_size(aux),pos_ult_pagina);

    if(cant_paginas > pos_ult_pagina + 1){
        log_error(logger,"EL carpincho %d tiene %d paginas y se quiere eliminar %d marcos",carpincho_id,pos_ult_pagina + 1,cant_paginas);
        usleep(1000*cfg->RETARDO_SWAP);
        // send_ack(cliente_socket,false);
        list_destroy(aux);
        return;
    }

    for(int i = 0 ; i < cant_paginas ; i++ ){
        log_info(logger,"%d",i);
        frame_swap_t* frame = list_get(aux , i);
        formatear_pagina_swap(frame->inicio,frame->nro_swap);
    }

    for(int i = 0 ; i < cant_paginas ; i++ ){
        
        bool buscar_x_id_y_nro_pagina(frame_swap_t* f){
            return f->pid == carpincho_id && f->nro_pagina == pos_ult_pagina;
        }

        list_remove_and_destroy_by_condition(tablas_de_frames_swap,(void*)buscar_x_id_y_nro_pagina,(void*)destroy_frame);
        pos_ult_pagina--;
    }

    list_destroy(aux);

    usleep(1000*cfg->RETARDO_SWAP);

}