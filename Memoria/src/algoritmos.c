#include "../include/algoritmos.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;
uint32_t posible_victima_global = 0;

void correr_algoritmo_clock_m (unsigned long id_carpincho, uint32_t* nro_frame){
    pthread_mutex_lock(&MUTEX_ALGORITMOS);
    t_list* victimas = posibles_victimas(id_carpincho);
    
    // yyy ordenarla por nro de frame
    bool frame_precede_frame(void* e1, void* e2) {
        algoritmo_t* entrada1 = e1;
        algoritmo_t* entrada2 = e2;
        return entrada1->entrada_tp->nro_frame < entrada2->entrada_tp->nro_frame;
    }
    list_sort(victimas, &frame_precede_frame);
    
    uint32_t* nro_frame_posible_victima = MEMORIA_CFG->FIJA ? victima_clock_fija(id_carpincho) : &posible_victima_global ;

    // Ahora si puedo aplicar el algoritmo del RELOJITO MODIFICADO
    const uint32_t cant_frames = list_size(victimas);

    while(1){
        for(uint32_t i = 0; i < cant_frames; i++){
            algoritmo_t* posible_victima_algoritmo = list_get(victimas, (*nro_frame_posible_victima) % cant_frames);
            entrada_tp_t* posible_victima = posible_victima_algoritmo->entrada_tp;
            pthread_mutex_lock(&(posible_victima->mutex_bits));
            if (posible_victima->algoritmo.clock_m->bit_U == 0 
                && posible_victima->bit_M == 0){
                posible_victima->bit_P = 0;
                pthread_mutex_unlock(&(posible_victima->mutex_bits));          
                *nro_frame = posible_victima->nro_frame;
                (*nro_frame_posible_victima)++;
                list_destroy_and_destroy_elements(victimas, (void*) free);
                pthread_mutex_unlock(&MUTEX_ALGORITMOS);                              
                return;
            }
            pthread_mutex_unlock(&(posible_victima->mutex_bits));
            (*nro_frame_posible_victima)++;
        }

        for(uint32_t i = 0; i < cant_frames; i++){
            algoritmo_t* posible_victima_algoritmo = list_get(victimas, (*nro_frame_posible_victima) % cant_frames);
            entrada_tp_t* posible_victima = posible_victima_algoritmo->entrada_tp;
            pthread_mutex_lock(&(posible_victima->mutex_bits));            
            if (posible_victima->algoritmo.clock_m->bit_U == 0 
                && posible_victima->bit_M == 1){
                posible_victima->bit_P = 0;
                pthread_mutex_unlock(&(posible_victima->mutex_bits));          
                //TODO: SWAPEAR
                *nro_frame = posible_victima->nro_frame;
                (*nro_frame_posible_victima)++;      
                list_destroy_and_destroy_elements(victimas, (void*) free);
                pthread_mutex_unlock(&MUTEX_ALGORITMOS);         
                return;
            }
            posible_victima->algoritmo.clock_m->bit_U = 0;
            pthread_mutex_unlock(&(posible_victima->mutex_bits));
            (*nro_frame_posible_victima)++;
        }
    }
}

//Siempre verificar antes de correr el algoritmo que la cantidad de paginas entran en swap
void correr_algoritmo_lru (unsigned long id, uint32_t* nro_frame){
    pthread_mutex_lock(&MUTEX_ALGORITMOS);    
    t_list* victimas = posibles_victimas(id);
    //TODO: Consultar sobre si es necesario un mutex en el global TUR
    void* minimo_TUR(void* pagina1, void* pagina2){
        return 
            ((algoritmo_t*) pagina1)->entrada_tp->algoritmo.TUR <= ((algoritmo_t*) pagina2)->entrada_tp->algoritmo.TUR ? 
            pagina1 : pagina2;
    }
    algoritmo_t* victima_algoritmo = list_get_minimum(victimas, minimo_TUR);
    entrada_tp_t* victima = victima_algoritmo->entrada_tp;
    pthread_mutex_lock(&(victima->mutex_bits));            
    victima->bit_P = 0;
    if(victima->bit_M){
        //TODO: Swapear
    }
    pthread_mutex_unlock(&(victima->mutex_bits));
    

    *nro_frame = victima->nro_frame;
    tabla_frames[*nro_frame].libre = 1;
    pthread_mutex_unlock(&MUTEX_ALGORITMOS);         
    return;
}

t_list* posibles_victimas(unsigned long id){
    if(MEMORIA_CFG->FIJA){
        tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
        t_list* paginas_presentes = paginas_presentes_local(tabla_carpincho);
        return paginas_presentes;
    } else {
        return paginas_presentes_dinamica();
    }
}

uint32_t* victima_clock_fija(unsigned long id_carpincho){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
    return &(tabla_carpincho->posible_victima_fija);
}