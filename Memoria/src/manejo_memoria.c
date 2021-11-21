#include "../include/manejo_memoria.h"
#define MIN(A,B) ((A)<(B)?(A):(B))

extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern t_config_memoria* MEMORIA_CFG;
extern t_log* logger;
extern frame_t* tabla_frames;
extern pthread_mutex_t MUTEX_FRAMES_BUSY;

bool allocar_carpincho_en_mp(unsigned long id_carpincho, size_t size, bool primer_alloc, uint32_t * direccion_logica){
    //TODO: cant_paginas no esta teniendo en cuenta el hmd
    // if(cant_frame_libres_fija(id_carpincho) < cant_paginas_relativa(0, size)) {
    //     //TODO: Buscar victima segun algoritmo y swapear la difrencia entre los frames que quiere y la cant de pags
    //     log_info(logger, "Cant_frames menor");
    //     return true;
    // }
    
    if(primer_alloc){
        uint32_t hmd_cortado = 0;
        uint32_t cantidad_de_paginas = cant_paginas_relativa(0, size + sizeof(hmd_t) * 2);
        //Esta función en swap reserva la cantidad de paginas de forma consecutiva a las que tenía
        // if(!entra_en_swap(id_carpincho, cantidad_de_paginas)){
        //   return false;
        // }
        for(uint32_t i = 0; i < cantidad_de_paginas; i++){
            
            uint32_t nro_frame = buscar_primer_frame_carpincho(id_carpincho);
            if(nro_frame == 0xFFFF){
              correr_algoritmo(id_carpincho, &nro_frame);              
            }
            append_frame_tp(id_carpincho, i, nro_frame);  
            tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
            entrada_tp_t* entrada_tp = list_get_pagina(tabla_carpincho, i);
            primer_memalloc_carpincho(id_carpincho, &size, entrada_tp, &hmd_cortado);
            *direccion_logica = sizeof(hmd_t);
        }
    } else {
        uint32_t nro_frame;
        uint32_t nro_pagina;
        hmd_t* hmd;
        entrada_tp_t* entrada_tp;
        uint32_t direccion_ultimo_hmd;
        *direccion_logica = buscar_recorriendo_hmd(id_carpincho, size, &hmd, &entrada_tp, &direccion_ultimo_hmd);
        if(*direccion_logica == 0xFFFF){
            //Actualizar ultimo hmd
            hmd_t* hmd_final = malloc(sizeof(hmd_t));
            hmd_final->isFree = true;
            hmd_final->prevAlloc = direccion_ultimo_hmd;
            hmd_final->nextAlloc = 0;
            uint32_t posicion_hmd_final = direccion_ultimo_hmd + sizeof(hmd_t) + size;
            hmd->nextAlloc = posicion_hmd_final;
            hmd->isFree = false;
            //Escribir el size y el nuevo hmd del final
            if(!allocar_al_final(id_carpincho, hmd, hmd_final, entrada_tp, direccion_ultimo_hmd, size)){
              //No hay lugar en SWAP
              return false;
            }
            *direccion_logica = direccion_ultimo_hmd + sizeof(hmd_t);
            free(hmd);
            free(hmd_final);
            return true;
        }
        free(hmd);  
    }

    return true;
}

bool allocar_al_final(unsigned long id_carpincho, hmd_t* hmd_inicial, hmd_t* hmd_final, entrada_tp_t* entrada_tp, uint32_t direccion_ultimo_hmd, size_t size){
  uint32_t tamanio_total_a_allocar = sizeof(hmd_t)*2 + size;
  uint32_t offset = direccion_ultimo_hmd % MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t pagina_ultimo_hmd = direccion_ultimo_hmd / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t espacio_restante = MEMORIA_CFG->TAMANIO_PAGINA - offset;
  
  bool nueva_pagina = table_size(id_carpincho) <= (hmd_inicial->nextAlloc + 9) / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t cant_paginas_a_allocar = cant_paginas_relativa(direccion_ultimo_hmd, size + sizeof(hmd_t) * 2);  
  uint32_t size_acum = 0;

  if(entrada_tp->nro_pagina != pagina_ultimo_hmd){
    entrada_tp = buscar_entrada_tp(id_carpincho, pagina_ultimo_hmd);
  }

  if(nueva_pagina){
    uint32_t ultima_pagina = entrada_tp->nro_pagina;
    /*Esta función en swap reserva la cantidad de paginas de forma consecutiva a las que tenía
    if(!entra_en_swap(id_carpincho, cant_paginas_a_allocar)){
      return false;
    }*/    
    void* data = malloc(tamanio_total_a_allocar);
    memcpy(data, (void*)hmd_inicial, sizeof(hmd_t));
    memset(data + sizeof(hmd_t), 0, size);
    memcpy(data + sizeof(hmd_t) + size, (void*)hmd_final, sizeof(hmd_t));
    
    uint32_t tamanio_a_allocar;
    for(uint32_t i = 0; i < cant_paginas_a_allocar; i++){
      tamanio_a_allocar = MIN(espacio_restante, tamanio_total_a_allocar - size_acum);    
      escritura_memcpy_size(data + size_acum, entrada_tp, offset, tamanio_a_allocar);
      size_acum += espacio_restante;
      if(size_acum != tamanio_total_a_allocar && i != (cant_paginas_a_allocar - 1)){
        if(table_size(id_carpincho) > (entrada_tp->nro_pagina + 1)){
          entrada_tp = buscar_entrada_tp(id_carpincho, entrada_tp->nro_pagina + 1);
        } else {
          entrada_tp = crear_nueva_pagina(id_carpincho);
        }
      }
      espacio_restante = MEMORIA_CFG->TAMANIO_PAGINA;
      offset = 0;
    }
    free(data);    
  } else {
    void* data = malloc(tamanio_total_a_allocar);
    memcpy(data, hmd_inicial, sizeof(hmd_t));
    memcpy(data + sizeof(hmd_t) + size, hmd_final, sizeof(hmd_t));
    uint32_t tamanio_a_allocar;
    for(uint32_t i = 0; i < cant_paginas_a_allocar; i++){
      tamanio_a_allocar = MIN(espacio_restante, tamanio_total_a_allocar - size_acum);
      escritura_memcpy_size(data + size_acum, entrada_tp, offset, tamanio_a_allocar);      
      size_acum += tamanio_a_allocar;
      if(table_size(id_carpincho) != (entrada_tp->nro_pagina + 1)){
        entrada_tp = buscar_entrada_tp(id_carpincho, entrada_tp->nro_pagina + 1);
      }
      espacio_restante = MEMORIA_CFG->TAMANIO_PAGINA;
      offset = 0;
    }
    free(data);        
  }
  return true;
}

entrada_tp_t* crear_nueva_pagina(unsigned long id_carpincho){
  tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id_carpincho);
  uint32_t nro_frame = buscar_primer_frame_carpincho(id_carpincho);
  if(nro_frame == 0xFFFF){
    correr_algoritmo(id_carpincho, &nro_frame);
  }
  uint32_t nueva_pagina = tabla_carpincho->pages;
  append_frame_tp(id_carpincho, nueva_pagina, nro_frame);
  return list_get_pagina(tabla_carpincho, nueva_pagina);
}

//Debería retornar la direccion lógica de la memoria
uint32_t buscar_recorriendo_hmd(unsigned long id_carpincho, size_t size, hmd_t** hmd, entrada_tp_t** entrada_tp, uint32_t* direccion_hmd){
  *direccion_hmd = 0;
  uint32_t nro_pagina = 0;
  uint32_t offset_hmd; 
  *entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina); 
  while(1){    
    nro_pagina = (*direccion_hmd) / MEMORIA_CFG->TAMANIO_PAGINA;
    offset_hmd = (*direccion_hmd) % MEMORIA_CFG->TAMANIO_PAGINA;    
    if(nro_pagina != (*entrada_tp)->nro_pagina){
      *entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    *hmd = leer_hmd(*entrada_tp, offset_hmd, id_carpincho);
    if(entra_en_hmd(*hmd, size, (*direccion_hmd))){
      //escribir en memoria principal y retornar direccion logica
      escribir_en_mp(*hmd, size, *entrada_tp, (*direccion_hmd), id_carpincho);

      //Retornar direccion logica    
      return *direccion_hmd + sizeof(hmd_t);
    } else {
      if((*hmd)->nextAlloc == 0){
        return 0xFFFF;
      }                                 
      *direccion_hmd = (*hmd)->nextAlloc;
    }
    free(*hmd);
  }
}

void escribir_en_mp(hmd_t* hmd, size_t size, entrada_tp_t* entrada_tp, uint32_t direccion_hmd, unsigned long id_carpincho){
  uint32_t offset_hmd = direccion_hmd % MEMORIA_CFG->TAMANIO_PAGINA;
  int32_t espacio_libre = hmd->nextAlloc - direccion_hmd - sizeof(hmd_t);
  uint32_t nro_pagina;

  hmd->isFree = false;
  if(espacio_libre >= 9){
    hmd_t* hmd_frag = malloc(sizeof(hmd_t));
    hmd_frag->nextAlloc = hmd->nextAlloc;
    hmd_frag->isFree = true;
    hmd_frag->prevAlloc = direccion_hmd;
    //Apunta a donde esta el hmd_frag
    uint32_t direccion_hmd_frag = direccion_hmd + size + sizeof(hmd_t);
    hmd->nextAlloc = direccion_hmd_frag;
    nro_pagina = direccion_hmd / MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina != entrada_tp->nro_pagina){
      entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    alloc_size_en_mp((void*)hmd, offset_hmd, sizeof(hmd_t), entrada_tp, id_carpincho);   
    nro_pagina = direccion_hmd_frag / MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina != entrada_tp->nro_pagina){
      entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    } 
    uint32_t offset_hmd_frag = direccion_hmd_frag % MEMORIA_CFG->TAMANIO_PAGINA;
    alloc_size_en_mp((void*)hmd_frag, offset_hmd_frag, sizeof(hmd_t), entrada_tp, id_carpincho);
    nro_pagina = hmd_frag->nextAlloc / MEMORIA_CFG->TAMANIO_PAGINA;
    uint32_t offset = hmd_frag->nextAlloc % MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina != entrada_tp->nro_pagina){
      entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    hmd_t* hmd_siguiente = leer_hmd(entrada_tp, offset, id_carpincho);
    hmd_siguiente->prevAlloc = direccion_hmd_frag;
    offset_hmd = hmd_frag->nextAlloc % MEMORIA_CFG->TAMANIO_PAGINA;
    if(nro_pagina != entrada_tp->nro_pagina){
      entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
    }
    alloc_size_en_mp((void*)hmd_siguiente, offset_hmd, sizeof(hmd_t), entrada_tp, id_carpincho);
    
    free(hmd_frag);
    free(hmd_siguiente);
  } else {
    //escribir en memoria principal
    //TODO: que pasa si alloco el size del hmd y además lo que tendría de resto
    alloc_size_en_mp((void*)hmd, offset_hmd, sizeof(hmd_t), entrada_tp, id_carpincho);
  }
}

//Alloca literalmente lo que le tiro sin agregar nada
void alloc_size_en_mp(void* data, uint32_t offset, size_t size, entrada_tp_t* entrada_tp, unsigned long id_carpincho){
  
  uint32_t cant_paginas_a_allocar = cant_paginas_relativa(offset, size);
  uint32_t size_a_allocar;
  uint32_t size_acum = 0;
  for(uint32_t i = 0; i < cant_paginas_a_allocar; i++){
    size_a_allocar = MIN(size, MEMORIA_CFG->TAMANIO_PAGINA - offset);
    escritura_memcpy_size(data + size_acum, entrada_tp, offset, size_a_allocar);
    // escritura_memcpy_size_hmd(data, entrada_tp->nro_frame, offset, size_a_allocar);
    if(table_size(id_carpincho) != (entrada_tp->nro_pagina + 1) && cant_paginas_a_allocar > 1){
      entrada_tp = buscar_entrada_tp(id_carpincho, (entrada_tp->nro_pagina) + 1);
    }
    offset = 0;
    size_acum += size_a_allocar;
    size -= size_a_allocar;
  }
}

// se le pasa direccion NO EL OFFSET
bool entra_en_hmd(hmd_t* hmd, size_t size, uint32_t direccion_hmd){
  bool entra = false;
  if(hmd->isFree && hmd->nextAlloc != 0){
    int32_t espacio_libre = hmd->nextAlloc - direccion_hmd - sizeof(hmd_t);
    int32_t resta_reloca = espacio_libre - size;
    if(espacio_libre == size || resta_reloca >= 9){
        entra = true;
    }
  }
  return entra;
}

// se le pasa el offset y la entrada_tp que corresponde
hmd_t* leer_hmd(entrada_tp_t* entrada_tp, uint32_t offset, unsigned long id_carpincho){
  bool pedir_nueva_pagina = (MEMORIA_CFG->TAMANIO_PAGINA - offset) < sizeof(hmd_t);  
  void* hmd_buff = malloc(sizeof(hmd_t)); 
  uint32_t size_a_leer = pedir_nueva_pagina ? MEMORIA_CFG->TAMANIO_PAGINA - offset : sizeof(hmd_t);
  entrada_tp_t* entrada_tp_aux = malloc(sizeof(entrada_tp_t));
  *entrada_tp_aux = *entrada_tp;
  uint32_t size_acum = 0;
  if(pedir_nueva_pagina){
    //pedir nueva pagina
    for(uint32_t i = 0; i < 2; i++){
      lectura_memcpy_size(entrada_tp_aux, offset, hmd_buff + size_acum, size_a_leer);
      if(!i){
        *entrada_tp_aux = *(buscar_entrada_tp(id_carpincho, entrada_tp_aux->nro_pagina + 1));
        size_acum = size_a_leer;
        size_a_leer = sizeof(hmd_t) - size_a_leer;
        offset = 0;
      }
    }
  } else {
    //leer de memoria principal
    lectura_memcpy_size(entrada_tp_aux, offset, hmd_buff, size_a_leer);
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
        if(!entrada_buscada->bit_P){
          uint32_t nro_frame_a_asignar;
          correr_algoritmo(id_carpincho, &nro_frame_a_asignar);
          entrada_buscada->nro_frame = nro_frame_a_asignar;
        }
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


//suponemos que la direccion logica que nos pasan es la de un hmd
void liberar_Alloc(unsigned long id_carpincho, uint32_t* direccion_logica){
  direccion_logica - sizeof(hmd_t);
  uint32_t nro_pagina = *direccion_logica / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd = *direccion_logica % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp = buscar_entrada_tp(id_carpincho, nro_pagina);
  hmd_t* hmd = leer_hmd(entrada_tp, offset_hmd, id_carpincho);
  
  uint32_t direccion_logica_aux = hmd->prevAlloc;
  uint32_t nro_pagina_aux= direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd_aux = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux);
  hmd_t* hmd_aux = leer_hmd(entrada_tp_aux, offset_hmd_aux, id_carpincho);
  uint32_t nro_frame = entrada_tp_aux-> nro_frame;

  pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
  tabla_frames[nro_frame].libre = true;
  pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);


  while(hmd_aux-> isFree && (hmd_aux -> prevAlloc =! 0) ){
    
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = true;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    hmd_aux -> nextAlloc = hmd -> nextAlloc;
    direccion_logica_aux = hmd_aux->prevAlloc;
    nro_pagina_aux= direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
    offset_hmd_aux = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux);
    hmd_aux = leer_hmd(entrada_tp_aux, offset_hmd_aux, id_carpincho);
    nro_frame = entrada_tp_aux-> nro_frame;
    hmd -> prevAlloc = hmd_aux -> prevAlloc;
  }

  uint32_t direccion_logica_aux_next = hmd->nextAlloc;
  uint32_t nro_pagina_aux_next = direccion_logica_aux / MEMORIA_CFG->TAMANIO_PAGINA;
  uint32_t offset_hmd_aux_next = direccion_logica_aux % MEMORIA_CFG->TAMANIO_PAGINA;
  entrada_tp_t* entrada_tp_aux_next = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next);
  hmd_t* hmd_aux_next = leer_hmd(entrada_tp_aux_next, offset_hmd_aux_next, id_carpincho);
  nro_frame = entrada_tp_aux_next -> nro_frame;

  
   

  while(hmd_aux_next-> isFree && (hmd_aux_next -> nextAlloc != NULL) ){
  
    pthread_mutex_lock(&MUTEX_FRAMES_BUSY);
    tabla_frames[nro_frame].libre = true;
    pthread_mutex_unlock(&MUTEX_FRAMES_BUSY);

    hmd_aux_next -> prevAlloc = hmd -> prevAlloc;
    
    direccion_logica_aux_next = hmd_aux->nextAlloc;
    nro_pagina_aux_next = direccion_logica_aux_next / MEMORIA_CFG->TAMANIO_PAGINA;
    offset_hmd_aux_next = direccion_logica_aux_next % MEMORIA_CFG->TAMANIO_PAGINA;
    entrada_tp_aux_next = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next);
    hmd_aux_next = leer_hmd(entrada_tp_aux, offset_hmd_aux_next, id_carpincho);
    nro_frame = entrada_tp_aux_next-> nro_frame;
    hmd -> nextAlloc = hmd_aux_next -> nextAlloc;
    hmd_aux -> nextAlloc = hmd -> nextAlloc;
  }

  uint32_t diff = MEMORIA_CFG->TAMANIO_PAGINA - offset_hmd_aux_next;
  if( diff > 9){
    escritura_memcpy_size(hmd_aux_next,entrada_tp_aux_next, offset_hmd_aux_next, sizeof(hmd_t)) ;
  }else{
    escritura_memcpy_size(hmd_aux_next,entrada_tp_aux_next, offset_hmd_aux_next, diff);
    entrada_tp_aux_next = buscar_entrada_tp(id_carpincho, nro_pagina_aux_next+1);
    offset_hmd_aux_next = 0;
    escritura_memcpy_size(hmd_aux_next + diff,entrada_tp_aux_next, offset_hmd_aux_next, sizeof(hmd_t) - diff);
  }

  diff = MEMORIA_CFG->TAMANIO_PAGINA - offset_hmd_aux;
  if( diff > 9){
    escritura_memcpy_size(hmd_aux,entrada_tp_aux, offset_hmd_aux, sizeof(hmd_t));
  }else{
    escritura_memcpy_size(hmd_aux,entrada_tp_aux, offset_hmd_aux, diff);
    entrada_tp_aux = buscar_entrada_tp(id_carpincho, nro_pagina_aux+1);
    offset_hmd_aux = 0;
    escritura_memcpy_size(hmd_aux + diff,entrada_tp_aux, offset_hmd_aux, sizeof(hmd_t) - diff);
  }


  diff = hmd_aux -> nextAlloc - hmd_aux_next -> prevAlloc ;
  uint32_t cantidad_paginas_libres = diff / MEMORIA_CFG->TAMANIO_PAGINA;
  if(hmd_aux_next -> nextAlloc == NULL && (cantidad_paginas_libres > 1)){
    quitar_paginas_a_carpinchos(id_carpincho,cantidad_paginas_libres);
  }
}

  // NOS QUEDAMOS ACA











//   hmd_t* hmd_frag = malloc(sizeof(hmd_t));
//     hmd_frag->nextAlloc = hmd->nextAlloc;
//     hmd_frag->isFree = true;
//     hmd_frag->prevAlloc = offset_hmd;

// }

