#include "../include/monitor_tablas.h"

uint32_t static_pid;
uint32_t static_nro_frame;

extern t_config_memoria* MEMORIA_CFG;
extern uint64_t global_TUR;  


// Agrega una nueva entrada a la tabla de paginas, cuando se ocupa un nuevo frame, si estaba vacio
// (significa que se cargo una nueva pagina en memoria, total o parcialmente, ante un INICIAR_algo)
// Tambien actualiza cantidad total de paginas de la patota
// Contempla el caso en el que una pagina arranque cargada en SWAP
void list_add_page_frame_tppatotas(unsigned long pid, uint32_t nro_frame, size_t size, bool presente) {
    
    // /*log_info(logger, "list_add_page...: pid/nro_frame/size %" PRIu32 "/%" PRIu32 "/%" PRIu32 " (P%d)",
    //     pid, nro_frame, size, presente
    // );*/
    // //pthread_mutex_lock(&MUTEX_TP_TABLE);

    // //TODO: Ver si es necesario crear un pid estatico
    // static_pid = pid;


    // //tp_carpincho_t* res = list_find(TP_TABLE, &tp_carpincho_has_pid);

    // static_nro_frame = nro_frame;
    // entrada_tp_t* e_pagina = presente
    //     ? list_find(res->paginas, &has_nro_frame)
    //     : list_find(res->paginas, &has_nro_frame_swap);
    // if (e_pagina == NULL) {
    //     //Nuevo frame, nueva pagina!
    //     //log_info(logger, "Nuevo frame (%" PRIu32 ") (P%d)", nro_frame, presente);
    //     entrada_tp_t* e_pagina_new = malloc(sizeof(entrada_tp_t));
    //     e_pagina_new->nro_pagina = res->pages;
    //     e_pagina_new->nro_frame = nro_frame;

    //     if (MEMORIA_CFG->LRU_MMU)
    //         e_pagina_new->TUR = global_TUR++;
    //     else
    //         //TODO: Ver bit modificado
    //         e_pagina_new->clock_m->bit_U = 1;
        
    //     e_pagina_new->bit_P = presente;

    //     list_add(res->paginas, (void*) e_pagina_new);
    //     /*log_info(logger, "Agregada esta entrada_tp_t*: %" PRIu32 "|%" PRIu32 "|%d",
    //         e_pagina_new->nro_pagina, e_pagina_new->nro_frame, e_pagina_new->bit_P
    //     );*/

    //     if (!presente) {
    //         //TODO: Pasar a pitu
    //         // tabla_frames_swap[nro_frame].pid = pid;
    //         // tabla_frames_swap[nro_frame].nro_pagina = e_pagina_new->nro_pagina;
    //         // tabla_frames_swap[nro_frame].inicio += size;
    //     }

    //     res->pages++;
    // }
    // else {
    //     //TODO:
    //     // No nuevo frame, esta en SWAP
    //     //log_info(logger, "Not nuevo frame (%" PRIu32 ") (P%d)", nro_frame, presente);
    //     // if (!presente) tabla_frames_swap[nro_frame].inicio += size;
    // }
    // //pthread_mutex_unlock(&MUTEX_TP_TABLE);
    
}

void init_memory_structs(){
    TLB_TABLE = list_create();
    return;
}

void mutex_init_tablas(){
    pthread_mutex_init(&MUTEX_TP_CARPINCHO, NULL); 
}

void destroy_memory_structs(){
    list_destroy(TLB_TABLE);   
}

static bool tp_carpincho_has_pid(void* x) {
    tp_carpincho_t* elem = x;
    return elem->id_carpincho == static_pid;
}

static bool has_nro_frame(void* x) {
    entrada_tp_t* elem = x;
    return (elem->nro_frame == static_nro_frame) && (elem->bit_P==1);
}

static bool has_nro_frame_swap(void* x) {
    entrada_tp_t* elem = x;
    return (elem->nro_frame == static_nro_frame) && (elem->bit_P==0);
}

bool tabla_vacia(){
    /*
    bool esta_vacia;
    pthread_mutex_lock(&MUTEX_TP_CARPINCHO);
    esta_vacia = list_is_empty(tabla_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHO);
    return esta_vacia;
    */
}
