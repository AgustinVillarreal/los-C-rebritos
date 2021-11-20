#include "../include/interfaz_swamp.h"

extern t_log* logger;
extern t_config_swamp* cfg;

extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;

void proceder_asignacion_fija(int fd,unsigned long id,uint32_t pagina,void* data){

    uint32_t pos_swap = swamp_con_mas_espacio();
    void* swap = list_get(areas_de_swap,pos_swap);

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id;
    }

    //Me fijo si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){

        // Me fijo si hay espacio 
        if(cantidad_de_espacio_swamp_libre(swap) < cfg->TAMANIO_PAGINA*cfg->MARCOS_POR_CARPINCHO){
            log_error(logger,"No espacio sufieciente en swap");
            send_ack(fd,false);
            return;
        }

        // Si no esta lo agrego a la tabla
        uint32_t pos_libre = posicion_primer_byte_libre(swap);

        for(int i = 0 ;i < cfg->MARCOS_POR_CARPINCHO ; i++){

            if(i==0){
                frame_swap_t* frame = malloc(sizeof(frame_swap_t));
                frame->pid = id;
                frame->nro_pagina = pagina;
                frame->inicio = pos_libre + (i*cfg->TAMANIO_PAGINA);
                frame->nro_swap = pos_swap;
                frame->libre = false;

                list_add(tablas_de_frames_swap,frame);
            }
            else{  
                frame_swap_t* frame = malloc(sizeof(frame_swap_t));
                frame->pid = id;
                frame->nro_pagina = pagina;
                frame->inicio = pos_libre + (i*cfg->TAMANIO_PAGINA);
                frame->nro_swap = pos_swap;
                frame->libre = true;

                list_add(tablas_de_frames_swap,frame);
            }
        }

        // Y ecribo la pagina que me mando en swap
        memcpy(swap + pos_libre , data , cfg->TAMANIO_PAGINA);

    }
    else{
        // El carpincho esta en swap 
        // Filtro los carpinchos que necesito


        bool cmp_nro_pagina(frame_swap_t* f1,frame_swap_t* f2){
            return f1->nro_pagina < f2->nro_pagina;
        }

        t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);
        list_sort(aux,(void*)cmp_nro_pagina);

        // Me fijo si se alcanzo la cantidad maxima de carpinchos
        if(!hay_marcos_libres(aux)){
            log_error(logger,"Se alcanzo la cantidad maxima de marcos por carpincho");
            send_ack(fd,false);
            return;
        }

        // Busco el marco libre y lo agrego a swap
        for(int i = 0 ;i < cfg->MARCOS_POR_CARPINCHO ; i++){

            frame_swap_t* f = list_get(aux,i);

            if(((char*)swap)[f->inicio] == '\0' ){

                memcpy(swap + f->inicio , data , cfg->TAMANIO_PAGINA);
                f->libre = false;
                break;
            }
        }

    }

    // Envio que se realizo la escritura
    send_ack(fd,true);

}                
void proceder_asignacion_global(int fd,unsigned long id,uint32_t pagina,void* data){

    bool buscar_x_id(frame_swap_t* f){
        return f->pid == id;
    }

    //Me fijo si el carpincho esta en swap
    if(!list_any_satisfy(tablas_de_frames_swap,(void*)buscar_x_id)){

        uint32_t pos_swap = swamp_con_mas_espacio();
        void* swap = list_get(areas_de_swap,pos_swap);
    

        // Me fijo si hay espacio 
        if(cantidad_de_espacio_swamp_libre(swap) < cfg->TAMANIO_PAGINA){
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
        frame->libre = false;

        list_add(tablas_de_frames_swap,frame);

        // Y ecribo la pagina que me mando en swap
        memcpy(swap + pos_libre , data , cfg->TAMANIO_PAGINA);

    }
    else{
        // Esta el frame en swap
        // Tengo que averiguar en que swap esta
        t_list* aux = list_filter(tablas_de_frames_swap,(void*)buscar_x_id);
        frame_swap_t* frame = list_get(aux,0);

        //Tomo la swap correspondiente

        void* swap = list_get(areas_de_swap,frame->nro_swap);

        // Busco el primer byte libre
        uint32_t pos_libre = posicion_primer_byte_libre(swap);

        // Pongo la referncia en la tabla y lo agrego a la swap
        frame_swap_t* f = malloc(sizeof(frame_swap_t));
        f->pid = id;
        f->nro_pagina = pagina;
        f->inicio = pos_libre;
        f->nro_swap = frame->nro_swap;
        f->libre = false;

        list_add(tablas_de_frames_swap,frame);

        memcpy(swap + pos_libre , data , cfg->TAMANIO_PAGINA);



    }

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

            uint32_t paginas_libres = cantidad_de_espacio_swamp_libre(swap) / cfg->TAMANIO_PAGINA;

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

            uint32_t libres = 0;

            for(int i = 0 ; i<cfg->MARCOS_POR_CARPINCHO ;i++){
                frame_swap_t* f =  list_get(aux,i);
                
                if(f->libre){
                    libres++;
                }
            }

            return libres >= cant_paginas;

        }
        else{
            frame_swap_t* frame = list_find(tablas_de_frames_swap,(void*)buscar_x_id);
            void* swap = list_get(areas_de_swap, frame->nro_swap);

            uint32_t paginas_libres = cantidad_de_espacio_swamp_libre(swap) / cfg->TAMANIO_PAGINA;

            return paginas_libres >= cant_paginas;

        }
    }
}