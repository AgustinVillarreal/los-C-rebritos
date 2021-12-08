#include "../include/interfaz_memoria.h"
#define MIN(A,B) ((A)<(B)?(A):(B))

extern t_config_memoria* MEMORIA_CFG;
extern frame_t* tabla_frames;
extern t_log* logger;


void mate_init(unsigned long id){
    tp_carpincho_t* tp_carpincho = malloc(sizeof(tp_carpincho_t));
    tp_carpincho->id_carpincho = id;
    tp_carpincho->pages = 0;
    tp_carpincho->paginas = list_create();
    tp_carpincho->posible_victima_fija = 0;
    tp_carpincho->cant_hits = 0;
    tp_carpincho->cant_miss = 0;    
    pthread_mutex_init(&tp_carpincho->mutex_paginas, NULL);
    pthread_mutex_init(&tp_carpincho->mutex_hits, NULL);
    pthread_mutex_init(&tp_carpincho->mutex_miss, NULL);
    
    agregar_tabla_a_tp_carpinchos(tp_carpincho);
    return; 
}

void ocupar_frames_carpincho(unsigned long id){
    if(MEMORIA_CFG->FIJA){
        //TODO: Si tiene la tabla de paginas vacia significa que no tiene ninguna estructura en memoria --> la creo, si no, no ocupo nada
        if(/*tabla_vacia(id)*/ !tiene_frames(id)){
            //TODO: Si ocupar frames retorna false significa que esta algo raro con el grado de multiprogramacion
            bool ret_code = ocupar_frames(id);
            if(!ret_code){
                log_error(logger, "Te pasaste del grado de multiprogramacion - Incongruencia con Kernel\n");
                return;
            }
        }   
    }
}

bool allocar_carpincho(unsigned long id_carpincho, size_t size, uint32_t* direccion_logica, int swap_fd){
    
    //Evaluo si es el primer alloc
    size_t size_stream;
    
    bool primer_alloc = tabla_vacia(id_carpincho);

    //TODO: En caso de emergencia revisar aca xd
    return allocar_carpincho_en_mp(id_carpincho, size, primer_alloc, direccion_logica, swap_fd);    
} 

uint32_t liberar_espacio_mp(unsigned long id_carpincho, uint32_t* direccion_logica, int swap_fd ){
    uint32_t nro_de_pagina = *direccion_logica / MEMORIA_CFG->TAMANIO_PAGINA ; 
    
    tp_carpincho_t* carpincho = find_tp_carpincho(id_carpincho);
    if(carpincho == NULL){
        log_warning(logger, "---------------------dfdfdfdf");
    }
    if(nro_de_pagina > list_size(carpincho-> paginas)){
        return 0;
    }

    liberar_Alloc( id_carpincho, direccion_logica,swap_fd);

    return 1;
}


//Asumimos que la dirección lógica que llega de memoria es valida y es donde comienza la data del carpincho
bool read_carpincho(unsigned long id_carpincho, void** dest, size_t size, uint32_t direccion_logica){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
    uint32_t posicion_hmd_a_leer = direccion_logica - sizeof(hmd_t);
    //Calculo el offset segun la posicion_logica del hmd porque despues tengo que tener en cuenta al leerlo (podría llegar a cambiar la página si el hmd esta cortado)
    uint32_t offset_hmd_a_leer =  posicion_hmd_a_leer % MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t cant_paginas_a_leer = cant_paginas_relativa(offset_hmd_a_leer, size + sizeof(hmd_t));
    
    uint32_t nro_pagina = posicion_hmd_a_leer / MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina > tabla_carpincho->pages){
        log_error(logger, "Estas buscando una pagina que no existe pa");
        return false; 
    }
    
    //TODO: Verificar, mi cabeza ya no funciona son las 12 menos 15
    uint32_t offset_data = (offset_hmd_a_leer + sizeof(hmd_t)) % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_t* entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    hmd_t* hmd = leer_hmd(entrada_tp, offset_hmd_a_leer, id_carpincho);
    if(hmd->nextAlloc - posicion_hmd_a_leer - sizeof(hmd_t) < size){
        log_error(logger, "El size a leer es mayor que el que puedo darte en esta posicion");
        free(hmd);
        return false;
    }
    if(!offset_data){
        nro_pagina++;
    }
    if(nro_pagina != entrada_tp->nro_pagina){
        entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    uint32_t size_acum = 0;
    uint32_t size_a_leer = MIN(MEMORIA_CFG->TAMANIO_PAGINA - offset_data, size);
    u_int32_t size_rest = size;
    *dest = malloc(size);  
    for(uint32_t i=0; i< cant_paginas_a_leer ; i++){
        //El leer hmd me deja la entrada tp en la pagina para comenzar a leer
        lectura_memcpy_size(entrada_tp, offset_data, (*dest) + size_acum, size_a_leer);
        size_rest -= size_a_leer;
        size_acum += size_a_leer;
        offset_data = 0;
        size_a_leer = MIN(size_rest, MEMORIA_CFG->TAMANIO_PAGINA); 
        
        if(i != (cant_paginas_a_leer - 1)){
            entrada_tp = buscar_entrada_tp(id_carpincho, entrada_tp->nro_pagina + 1);       
        }
    //La entrada tp me deja parado al final del hmd (si se leyo mitad y mitad me deja en la segunda mitad), entonces supongo que queda donde comienza la data
    }
    free(hmd); 
    return true;
}

//Asumimos que la dirección lógica que llega de memoria es valida y es donde comienza la data del carpincho
bool write_carpincho(unsigned long id_carpincho, void** dest, size_t size, uint32_t direccion_logica){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
    uint32_t posicion_hmd_a_leer = direccion_logica - sizeof(hmd_t);
    //Calculo el offset segun la posicion_logica del hmd porque despues tengo que tener en cuenta al leerlo (podría llegar a cambiar la página si el hmd esta cortado)
    uint32_t offset_hmd_a_leer =  posicion_hmd_a_leer % MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t cant_paginas_a_leer = cant_paginas_relativa(offset_hmd_a_leer, size + sizeof(hmd_t));
    
    uint32_t nro_pagina = posicion_hmd_a_leer / MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina > tabla_carpincho->pages){
        log_error(logger, "Estas buscando una pagina que no existe pa");
        return false; 
    }
    
    //TODO: Verificar, mi cabeza ya no funciona son las 12 menos 15
    uint32_t offset_data = (offset_hmd_a_leer + sizeof(hmd_t)) % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_t* entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    hmd_t* hmd = leer_hmd(entrada_tp, offset_hmd_a_leer, id_carpincho);
    if(hmd->nextAlloc - posicion_hmd_a_leer - sizeof(hmd_t) < size){
        log_error(logger, "El size a leer es mayor que el que puedo darte en esta posicion");
        free(hmd);
        return false;
    }
    if(!offset_data){
        nro_pagina++;
    }
    if(nro_pagina != entrada_tp->nro_pagina){
        entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    uint32_t size_acum = 0;
    uint32_t size_a_leer = MIN(MEMORIA_CFG->TAMANIO_PAGINA - offset_data, size);
    u_int32_t size_rest = size;
    for(uint32_t i=0; i< cant_paginas_a_leer ; i++){
        //El leer hmd me deja la entrada tp en la pagina para comenzar a leer
        escritura_memcpy_size((*dest) + size_acum, entrada_tp, offset_data, size_a_leer);
        size_rest -= size_a_leer;
        size_acum += size_a_leer;
        offset_data = 0;
        size_a_leer = MIN(size_rest, MEMORIA_CFG->TAMANIO_PAGINA); 
        
        if(i != (cant_paginas_a_leer - 1)){
            entrada_tp = buscar_entrada_tp(id_carpincho, entrada_tp->nro_pagina + 1);       
        }
    //La entrada tp me deja parado al final del hmd (si se leyo mitad y mitad me deja en la segunda mitad), entonces supongo que queda donde comienza la data
    }
    free(hmd); 
    return true;
}

void suspender_carpincho(unsigned long id){
    log_info(logger, "Suspendiendo: %lu", id);
    uint32_t nro_frame;

    for(uint32_t i = 0; i < table_size(id); i++){
        swapear_pagina(id, i, &nro_frame);
        suspender_frame(nro_frame);
    }
}
// TODO esto se deberia cambiar por los tlb hits
void finalizar_carpincho(unsigned long id_carpincho){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
    if(tabla_carpincho){
        //Sacar la tabla_carpincho de la tabla de tablas carpincho
        eliminar_tabla(tabla_carpincho);
        //Sacar las entradas carpinchos de la TLB
        sacar_entradas_TLB(id_carpincho);
        //Liberar entradas tp y adentro el clock_m
        void destructor_loco(void* pagina){
            entrada_tp_t* entrada_tp = pagina;
            if(!MEMORIA_CFG->LRU_MMU){
                free(entrada_tp->algoritmo.clock_m);
            }
            free(entrada_tp);
        }
        list_destroy_and_destroy_elements(tabla_carpincho->paginas, destructor_loco);  
        free(tabla_carpincho);
    }
    
}
