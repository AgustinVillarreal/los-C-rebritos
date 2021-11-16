#include "../include/manejo_memoria.h"
#define MIN(A,B) ((A)<(B)?(A):(B))

extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern t_config_memoria* MEMORIA_CFG;
extern t_log* logger;
extern pthread_mutex_t MUTEX_FRAMES_BUSY;


bool allocar_carpincho_fija(unsigned long id_carpincho, size_t size, bool primer_alloc, uint32_t * direccion_logica){

    //TODO: cant_paginas no esta teniendo en cuenta el hmd
    if(cant_frame_libres_fija(id_carpincho) < cant_paginas_relativa(0, size)) {
        //TODO: Buscar victima segun algoritmo y swapear la difrencia entre los frames que quiere y la cant de pags
        log_info(logger, "Cant_frames menor");
        return true;
    }
    

    if(primer_alloc){
        uint32_t hmd_cortado = 0;
        uint32_t cantidad_de_paginas = cant_paginas_relativa(0, size + sizeof(hmd_t) * 2);

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
        *direccion_logica = buscar_recorriendo_hmd(id_carpincho, size);
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

//Debería retornar la direccion lógica de la memoria
uint32_t buscar_recorriendo_hmd(unsigned long id_carpincho, size_t size){
  uint32_t direccion_hmd = 0;
  uint32_t nro_pagina;
  uint32_t offset_hmd; 
  while(1){
    nro_pagina = direccion_hmd / MEMORIA_CFG->TAMANIO_PAGINA;
    offset_hmd = direccion_hmd % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_t* entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    hmd_t* hmd = leer_hmd(entrada_tp, offset_hmd, id_carpincho);
    if(entra_en_hmd(hmd, size, direccion_hmd)){
      //escribir en memoria principal y retornar direccion logica
      escribir_en_mp(hmd, size, entrada_tp, offset_hmd, id_carpincho);

      //Retornar direccion logica
      return direccion_hmd + sizeof(hmd_t);
    } else {
      if(hmd->nextAlloc == 0){
        return 0xFFFF;
      }
      direccion_hmd = hmd->nextAlloc;
    }
  }
}

void escribir_en_mp(hmd_t* hmd, size_t size, entrada_tp_t* entrada_tp, uint32_t offset_hmd, unsigned long id_carpincho){
  int32_t espacio_libre = hmd->nextAlloc - offset_hmd - sizeof(hmd_t);
  hmd->isFree = false;
  if(espacio_libre >= 9){
    hmd_t* hmd_frag = malloc(sizeof(hmd_t));
    hmd_frag->nextAlloc = hmd->nextAlloc;
    hmd_frag->isFree = true;
    hmd_frag->prevAlloc = offset_hmd;
    //Apunta a donde esta el hmd_frag
    uint32_t direccion_hmd_frag = offset_hmd + size;
    hmd->nextAlloc = direccion_hmd_frag;
    alloc_size_en_mp((void*)hmd, offset_hmd, sizeof(hmd_t), entrada_tp, id_carpincho);
    log_info(logger, "hmd_frag->nextAlloc: %d", hmd_frag->nextAlloc);
    alloc_size_en_mp((void*)hmd_frag, direccion_hmd_frag, sizeof(hmd_t), entrada_tp, id_carpincho);
    hmd_t* hmd_siguiente = leer_hmd(entrada_tp, hmd_frag->nextAlloc, id_carpincho);
    hmd_siguiente->prevAlloc = direccion_hmd_frag;
    alloc_size_en_mp((void*)hmd_siguiente, hmd_frag->nextAlloc, sizeof(hmd_t), entrada_tp, id_carpincho);
  } else {
    //escribir en memoria principal
    //TODO: que pasa si alloco el size del hmd y además lo que tendría de resto
    alloc_size_en_mp((void*)hmd, offset_hmd, sizeof(hmd_t), entrada_tp, id_carpincho);
  }
}

void alloc_size_en_mp(void* data, uint32_t offset, size_t size, entrada_tp_t* entrada_tp, unsigned long id_carpincho){
  uint32_t cant_paginas_a_allocar = cant_paginas_relativa(size, offset);
  uint32_t size_a_allocar;
  for(uint32_t i = 0; i < cant_paginas_a_allocar; i++){
    size_a_allocar = MIN(size, MEMORIA_CFG->TAMANIO_PAGINA - offset);
    escritura_memcpy_size(data, entrada_tp->nro_frame, offset, size_a_allocar);
    if(cant_paginas_a_allocar > 1){
      entrada_tp = buscar_entrada_tp(id_carpincho, (entrada_tp->nro_pagina) + 1);
      offset = 0;
    }
  }
}

bool entra_en_hmd(hmd_t* hmd, size_t size, uint32_t direccion_hmd){
  bool entra = false;
  if(hmd->isFree){
    int32_t espacio_libre = hmd->nextAlloc - direccion_hmd - sizeof(hmd_t);
    if(espacio_libre == size || espacio_libre >= 9){
    log_info(logger, "Esta free");
        entra = true;
    }
  }
  return entra;
}


hmd_t* leer_hmd(entrada_tp_t* entrada_tp, uint32_t offset, unsigned long id_carpincho){
  bool pedir_nueva_pagina = (MEMORIA_CFG->TAMANIO_PAGINA - offset) < sizeof(hmd_t);
  log_info(logger, "--------------%d-------", pedir_nueva_pagina);
  log_info(logger, "--------------entrada_tp->nro_pagina: %d-------", entrada_tp->nro_pagina);
  
  void* hmd_buff = malloc(sizeof(hmd_t)); 
  uint32_t size_a_leer = pedir_nueva_pagina ? MEMORIA_CFG->TAMANIO_PAGINA - offset : sizeof(hmd_t);
  entrada_tp_t* entrada_tp_aux = malloc(sizeof(entrada_tp_t));
  *entrada_tp_aux = *entrada_tp;

  uint32_t pagina_a_leer = offset /  MEMORIA_CFG->TAMANIO_PAGINA;
  if(pagina_a_leer != entrada_tp->nro_pagina){
    entrada_tp_aux = buscar_entrada_tp(id_carpincho, pagina_a_leer);
  }

  if(pedir_nueva_pagina){
    //pedir nueva pagina
    for(uint32_t i = 0; i < 2; i++){
      lectura_memcpy_size(entrada_tp_aux->nro_frame, offset, hmd_buff, size_a_leer);
      if(!i){
        entrada_tp_aux = buscar_entrada_tp(id_carpincho, entrada_tp_aux->nro_pagina + 1);
        size_a_leer = sizeof(hmd_t) - size_a_leer;
        offset = 0;
      }
    }
  } else {
    //leer de memoria principal
    lectura_memcpy_size(entrada_tp_aux->nro_frame, offset, hmd_buff, size_a_leer);
  }
  free(entrada_tp_aux);
  return (hmd_t*) hmd_buff;
}

entrada_tp_t* buscar_entrada_tp(unsigned long id_carpincho, uint32_t nro_pagina){
    entrada_tp_t* entrada_buscada = NULL;
    if(!buscar_en_TLB(id_carpincho, nro_pagina, entrada_buscada)){
        tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
        log_info(logger, "---------%lu en pagina %d---------", tp_carpincho->id_carpincho, nro_pagina);
        entrada_buscada = list_get_pagina(tp_carpincho, nro_pagina);
    }
    return entrada_buscada;
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

/*
//suponemos que la direccion logica que nos pasan es la de un hmd
void liberar_Alloc(unsigned long id_carpincho, uint32_t* direccion_logica){
  uint32_t nro_pagina = direccion_logica / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd = direccion_logica % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
  hmd_t* hmd = leer_hmd(entrada_tp, offset_hmd, id_carpincho);
  
  uint32_t direccion_logica_aux = hmd->prevAlloc;
  uint32_t nro_pagina_aux= direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd_aux = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux);
  hmd_t* hmd_aux = leer_hmd(entrada_tp_aux, offset_hmd_aux, id_carpincho);
  uint32_t nro_frame = *entrada_tp_aux-> nro_frame;
  
  while(hmd_aux-> isFree){
    
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = true;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    hmd_aux -> nextAlloc = hmd -> nextAlloc;
    uint32_t diff = MEMORIA_CFG->TAMANIO_PAGINA - offset_hmd_aux;
    if( diff > 9){
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd_aux, sizeof(hmd_t));
      pthread_mutex_unlock(&MUTEX_MP_BUSY);  
    }else{
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset_hmd_aux, hmd_aux, dif);
      pthread_mutex_unlock(&MUTEX_MP_BUSY);
      *entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux+1);
      offset_hmd_aux = 0
      nro_frame = *entrada_tp_aux-> nro_frame;
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, ((void*) hmd_aux) + diff , sizeof(hmd_t) - diff);
      pthread_mutex_unlock(&MUTEX_MP_BUSY); 
    }
    uint32_t direccion_logica_aux = hmd_aux->prevAlloc;
    uint32_t nro_pagina_aux= direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t offset_hmd_aux = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_t* entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux);
    hmd_t* hmd_aux = leer_hmd(entrada_tp_aux, offset_hmd_aux, id_carpincho);
    nro_frame = *entrada_tp_aux-> nro_frame;
    hmd -> prevAlloc = hmd_aux -> prevAlloc;
  }

  uint32_t direccion_logica_aux_next = hmd->nextAlloc;
  uint32_t nro_pagina_aux_next = direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd_aux_next = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp_aux_next = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next);
  hmd_t* hmd_aux_next = leer_hmd(entrada_tp_aux_next, offset_hmd_aux_next, id_carpincho);
  nro_frame = *entrada_tp_aux_next -> nro_frame;

  

  while(hmd_aux_next-> isFree){
  
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = true;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    hmd_aux_next -> prevAlloc = hmd -> prevAlloc;
    uint32_t diff = MEMORIA_CFG->TAMANIO_PAGINA - offset_hmd_aux_next;
    if( diff > 9){
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, hmd_aux_next, sizeof(hmd_t));
      pthread_mutex_unlock(&MUTEX_MP_BUSY);  
    }else{
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA + offset_hmd_aux_next, hmd_aux_next, dif);
      pthread_mutex_unlock(&MUTEX_MP_BUSY);
      *entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next+1);
      offset_hmd_aux_next = 0
      nro_frame = *entrada_tp_aux-> nro_frame;
      pthread_mutex_lock(&MUTEX_MP_BUSY);
      memcpy(memoria_principal + nro_frame * MEMORIA_CFG->TAMANIO_PAGINA, ((void*) hmd_aux_next) + diff , sizeof(hmd_t) - diff);
      pthread_mutex_unlock(&MUTEX_MP_BUSY); 
    }
    uint32_t direccion_logica_aux_next = hmd_aux->nextAlloc;
    uint32_t nro_pagina_aux_next = direccion_logica_aux_next / MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t offset_hmd_aux_next = direccion_logica_aux_next % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_t* entrada_tp_aux_next = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next);
    hmd_t* hmd_aux_next = leer_hmd(entrada_tp_aux, offset_hmd_aux_next, id_carpincho);
    uint32_t nro_frame = *entrada_tp_aux_next-> nro_frame;
    hmd -> nextAlloc = hmd_next -> nextAlloc;
    hmd_aux -> nextAlloc = hmd -> nextAlloc;
  }
  
  // NOS QUEDAMOS ACA


*/

  hmd_t* hmd_frag = malloc(sizeof(hmd_t));
    hmd_frag->nextAlloc = hmd->nextAlloc;
    hmd_frag->isFree = true;
    hmd_frag->prevAlloc = offset_hmd;

}

