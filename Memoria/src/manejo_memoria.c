#include "../include/manejo_memoria.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern t_config_memoria* MEMORIA_CFG;
extern t_log* logger;
extern pthread_mutex_t MUTEX_FRAMES_BUSY;


bool allocar_carpincho_fija(unsigned long id_carpincho, size_t size, bool primer_alloc, uint32_t * direccion_logica){

    //TODO: cant_paginas no esta teniendo en cuenta el hmd
    if(cant_frame_libres_fija(id_carpincho) < cant_paginas(size)) {
        //TODO: Buscar victima segun algoritmo y swapear la difrencia entre los frames que quiere y la cant de pags
        log_info(logger, "Cant_frames menor");
        return true;
    }
    

    if(primer_alloc){
        uint32_t hmd_cortado = 0;
        uint32_t cantidad_de_paginas = cant_paginas(size + sizeof(hmd_t) * 2);

        for(uint32_t i = 0; i < cantidad_de_paginas; i++){
            
            uint32_t nro_frame = buscar_primer_frame_carpincho(id_carpincho);
            if(nro_frame == 0xFFFF){
                bool ret_code = correr_algoritmo(id_carpincho, &nro_frame);
                if(!ret_code){
                    return false;
                }
            }
            //TODO: Penasr de que forma se puede generalizar, en este caso la i corresdponde a la pagina de al tp
            primer_memalloc_carpincho(id_carpincho, &size, direccion_logica, nro_frame, i, &hmd_cortado);
            //TODO: Cuando se mete un frame que ya esta hay que poner el bit de presencia del anterior en 0
            append_frame_tp(id_carpincho, i, nro_frame);    
        }
    } else {
        uint32_t nro_frame;
        uint32_t nro_pagina;
        *direccion_logica = buscar_espacio_entre_hmd(id_carpincho, size, &nro_frame, &nro_pagina);
        if(*direccion_logica == 0xFFFF){
            log_info(logger, "no hay espacio entre hmd\n");
            return true;
        }
        log_info(logger, "hay espacio entre hmd");
        
        
    }

    log_info(logger, "Cant_frames mayor");
    //TODO
    return true;
}



uint32_t cant_frame_libres_fija(unsigned long id_carpincho) {
    uint32_t presentes_en_mp = 0;

    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);

    for(uint32_t j = 0; j < table_size(id_carpincho); j++){
        tp_carpincho_t * tabla_carpincho = find_tp_carpincho(id_carpincho);
        entrada_tp_t* entrada_tp = list_get_pagina(tabla_carpincho, j);
        
        if(entrada_tp->bit_P){
            presentes_en_mp++;
        }
    }
    uint32_t frames_disponibles_carpincho = MEMORIA_CFG->MARCOS_POR_PROCESO - presentes_en_mp;
    
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    return frames_disponibles_carpincho;
}

uint32_t cant_paginas(size_t size){
    size_t rem;
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;
    rem = size % t_pag;
    return (rem) ? size/t_pag + 1 : size/t_pag;
}
