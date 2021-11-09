#include "../include/algoritmos.h"

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;


bool correr_algoritmo_clock_m (unsigned long id, uint32_t* nro_frame){

}

bool correr_algoritmo_lru (unsigned long id, uint32_t* nro_frame){
    t_list* victimas = posibles_victimas(id);
    //TODO: Consultar sobre si es necesario un mutex en el global TUR
    void* minimo_TUR(void* pagina1, void* pagina2){
        return 
            ((entrada_tp_t*) pagina1)->algoritmo.TUR <= ((entrada_tp_t*) pagina2)->algoritmo.TUR ? 
            pagina1 : pagina2;
    }
    entrada_tp_t* victima = list_get_minimum(victimas, minimo_TUR);
    victima->bit_P = 0;
    if(victima->bit_M){
        //TODO: Swapear
        //Retorna false si falla el swapear
    }

    *nro_frame = victima->nro_frame;
    tabla_frames[*nro_frame].libre = 1;

    return true;
}

t_list* posibles_victimas(unsigned long id){
    bool esta_presente(void* pagina){
        return ((entrada_tp_t*)pagina)->bit_P;
    }
    if(MEMORIA_CFG->FIJA){
        tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
        t_list* paginas_presentes = list_filter(tabla_carpincho->paginas, esta_presente);
        return paginas_presentes;
    } else {
        //TODO: Dinamica
        return NULL;
    }
}