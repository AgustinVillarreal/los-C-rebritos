#include "../include/monitor_tablas.h"

uint32_t static_pid;
uint32_t static_nro_frame;

extern t_config_memoria* MEMORIA_CFG;
extern uint64_t global_TUR;  
extern t_list* CARPINCHOS_TABLE;

pthread_mutex_t MUTEX_TP_CARPINCHOS;



void agregar_tabla_a_tp_carpinchos(void* tp_carpincho) {
    pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
    list_add(CARPINCHOS_TABLE, tp_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);
    return;
}

bool tabla_vacia(unsigned long id){
    //TODO: Pensar si va este mutex
    
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);

    uint32_t is_empty;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    is_empty = list_is_empty(tabla_carpincho->paginas);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);
    
    return is_empty;
}

tp_carpincho_t* find_tp_carpincho(unsigned long id){
    bool es_tabla_carpincho(void* tabla){
        return ((tp_carpincho_t *) tabla)->id_carpincho == id;
    }
    
    pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
    tp_carpincho_t* tabla_carpincho =  (tp_carpincho_t*) list_find(CARPINCHOS_TABLE, es_tabla_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);

    return tabla_carpincho;
}

entrada_tp_t* list_get_pagina(tp_carpincho_t* tabla_carpincho ,uint32_t index){   
    entrada_tp_t* entrada_tp;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    entrada_tp = list_get(tabla_carpincho->paginas, index);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);    
    return entrada_tp;
}

bool buscar_en_TLB(unsigned long id_carpincho, uint32_t nro_pagina, entrada_tp_t* entrada_buscada){
    //TODO: busca en tlb 
    return false;
}

void mutex_init_tabla(){
    pthread_mutex_init(&MUTEX_TP_CARPINCHOS, NULL); 
}

uint32_t table_size(unsigned long id){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
    uint32_t size;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    size = list_size(tabla_carpincho->paginas);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);   
    return size;
}

void append_frame_tp(unsigned long id, uint32_t nro_pagina, uint32_t nro_frame){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
    entrada_tp_t* entrada_tp = malloc(sizeof(entrada_tp_t));
    entrada_tp->nro_pagina = nro_pagina;
    entrada_tp->nro_frame = nro_frame;
    entrada_tp->bit_P = 1;
    //TODO: Ver mi TUR
    if(MEMORIA_CFG->LRU_MMU){
        entrada_tp->algoritmo.TUR = global_TUR++;
    } else {
        entrada_tp->algoritmo.clock_m = malloc(sizeof(clock_m_t));
        entrada_tp->algoritmo.clock_m->bit_U = 1;
        entrada_tp->bit_M = 1; 
    }

    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    tabla_carpincho->pages++;
    list_add(tabla_carpincho->paginas, entrada_tp);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas); 

    return;
}



