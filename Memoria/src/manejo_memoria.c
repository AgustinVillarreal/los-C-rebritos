#include "../include/manejo_memoria.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern t_config_memoria* MEMORIA_CFG;
extern t_log* logger;


uint32_t allocar_carpincho_fija(unsigned long id_carpincho, size_t size){

    if(cant_frame_libres_fija(id_carpincho) <= cant_paginas(size)) {
        log_info(logger, "Cant_frames menor");
        return 1;
    }
    log_info(logger, "Cant_frames mayor");
    //TODO
    return 1;
}



uint32_t cant_frame_libres_fija(unsigned long id_carpincho) {
    uint32_t presentes_en_mp = 0;

    //TODO: Estos mutex no andan, revisar
    // pthread_mutex_lock(&MUTEX_FRAMES_BUSY);

    for(uint32_t j = 0; j < table_size(id_carpincho); j++){
        tp_carpincho_t * tabla_carpincho = find_tp_carpincho(id_carpincho);
        entrada_tp_t* entrada_tp = list_get_pagina(tabla_carpincho->paginas, j);
        
        if(entrada_tp->bit_P){
            presentes_en_mp++;
        }
    }

    uint32_t frames_disponibles_carpincho = MEMORIA_CFG->MARCOS_POR_PROCESO - presentes_en_mp;
    uint32_t frames_disponibles_en_mp = cant_frame_libres();

    if(frames_disponibles_carpincho > frames_disponibles_en_mp){
        return frames_disponibles_en_mp;
    } 

    return frames_disponibles_carpincho;
}

uint32_t cant_paginas(size_t size){
    size_t rem;
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    rem = size % t_pag;
    return (rem) ? size/t_pag + 1 : size/t_pag;
}
