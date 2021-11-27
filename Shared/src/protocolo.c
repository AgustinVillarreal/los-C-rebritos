#include "../shared/protocolo.h"

bool send_handshake(int fd_server){
  return send_codigo_op(fd_server, HANDSHAKE);
}

bool send_handshake_swap(int fd_server, bool asignacion_fija){
  send_codigo_op(fd_server, HANDSHAKE);
  if(send(fd_server, &asignacion_fija, sizeof(bool), 0) == -1) {
    return false;
  }
  return true;
}

bool send_mate_init(int fd_server, int generar_id){
  void* stream = malloc(sizeof(int) + sizeof(op_code));
  op_code op = MATE_INIT;
  memcpy(stream, &op, sizeof(op_code));
  memcpy(stream + sizeof(op_code), &generar_id, sizeof(int));
  if(send(fd_server, stream, sizeof(int) + sizeof(op_code), 0) == -1) {
    free(stream);
    return false;
  }
  free(stream);
  return true;
}


bool send_codigo_op(int fd, op_code cop) {
  size_t size = sizeof(op_code);
  return send(fd,&cop,size,0) != -1;
}

bool send_alloc_data(int fd, unsigned long id, int size){
  void * stream = serializar_alloc_data(id, size);
  if(send(fd, stream, sizeof(long) + sizeof(int), 0) == -1){
    free(stream);
    return false;
  } 
  free(stream);
  return true;
}

bool send_data_mate_init(int fd, unsigned long id){
  return send(fd, &id, sizeof(long), 0) != -1;
}

bool send_memfree_data(int fd, unsigned long id, uint32_t direccion_logica){
  void * stream = serializar_memfree_data(id, direccion_logica);
  if(send(fd, stream, sizeof(long) + sizeof(uint32_t), 0) == -1){
    free(stream); 
    return false;
  } 
  free(stream);
  return true;
}

//SEMAFOROS

bool send_sem_init(int fd,  char* sem, unsigned int value){
  size_t size;
  void * stream = serializar_sem_init(&size, sem, value);
  if(send(fd, stream, size + sizeof(int), 0) == -1){
    free(stream);
    return false;
  } 
  free(stream);
  return true;
}

bool recv_sem_init(int fd, char** sem, int * value){
  size_t size;
  if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t)) {
      return false;
  }
  void* stream = malloc(size + sizeof(int));
  if (recv(fd, stream, size + sizeof(int), 0) != size + sizeof(int)) {
      free(stream);
      return false;
  }
  deserializar_sem_init(size, stream, sem, value);
  free(stream);
  return true;
} 

bool send_sem(int fd, char* sem){
  size_t size;
  void * stream = serializar_string(&size, sem);
  if(send(fd, stream, size, 0) == -1){
    free(stream);
    return false;
  } 
  free(stream);
  return true;
}

bool recv_sem(int fd, char** sem){
  size_t size;
  if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t)) {
      return false;
  }
  void* stream = malloc(size);
  if (recv(fd, stream, size, 0) != size) {
      free(stream);
      return false;
  }
  deserializar_sem_wait(size, stream, sem);
  free(stream);
  return true;
} 

//MEMORIA

bool send_memalloc(int fd_server){
  return send_codigo_op(fd_server, MEM_ALLOC);
}

bool send_memfree(int fd_server){
  return send_codigo_op(fd_server, MEM_FREE);
}

bool send_memread(int fd_server, uint32_t direccion_logica, int size){
  if(!send_codigo_op(fd_server, MEM_READ)){
    return false;
  }
  if(send(fd_server, &direccion_logica, sizeof(uint32_t), 0) == -1){
    return false;
  }
  if(send(fd_server, &size, sizeof(int), 0) == -1){
    return false;
  }
  return true;
}

bool send_memwrite(int fd_server, void* data, uint32_t direccion_logica, int size){
  if(!send_codigo_op(fd_server, MEM_WRITE)){
    return false;
  }
  if(send(fd_server, &direccion_logica, sizeof(uint32_t), 0) == -1){
    return false;
  }
  if(send(fd_server, &size, sizeof(int), 0) == -1){
    return false;
  }
  if(send(fd_server, data, size, 0) == -1){
    return false;
  }
  return true;
}

bool send_carpincho_ready(int fd, long id_carpincho){
  send_codigo_op(fd, CARPINCHO_READY);
  return send(fd, &id_carpincho, sizeof(long), 0) != -1;
}

bool recv_alloc_data(int fd, long* id_carpincho, int* size_data){
  void* stream = malloc(sizeof(long)+sizeof(int));
  if(recv(fd,stream,sizeof(long) + sizeof(int),0) != sizeof(long)+sizeof(int)){
    free(stream);
    return false;
  }
  long c_id;
  int c_size_data;
  deserializar_alloc_data(stream,&c_id,&c_size_data);

  *id_carpincho = c_id;
  *size_data = c_size_data;

  free(stream);
  return true;
}

bool recv_memfree_data(int fd, long* id_carpincho, uint32_t* direccion_logica){
  void* stream = malloc(sizeof(long)+sizeof(uint32_t));
  if(recv(fd,stream,sizeof(long) + sizeof(uint32_t),0) != sizeof(long)+sizeof(uint32_t)){
    free(stream);
    return false;
  }
  long c_id;
  uint32_t c_direccion_logica;
  deserializar_memfree_data(stream,&c_id,&c_direccion_logica);

  *id_carpincho = c_id;
  *direccion_logica = c_direccion_logica;

  free(stream);
  return true;
}


bool recv_memread_data(int fd, uint32_t* direccion_logica, int* size){
  if(recv(fd, direccion_logica, sizeof(uint32_t),0) != sizeof(uint32_t)){
    return false;
  }
  if(recv(fd, size, sizeof(int),0) != sizeof(int)){
    return false;
  }
  return true;
}

bool recv_memwrite_data(int fd, uint32_t* direccion_logica, void** data, int* size){
  if(recv(fd, direccion_logica, sizeof(uint32_t),0) != sizeof(uint32_t)){
    return false;
  }
  if(recv(fd, size, sizeof(int),0) != sizeof(int)){
    return false;
  }
  *data = malloc(*size);
  if(recv(fd, *data, *size,0) != *size){
    return false;
  }
  return true;
}

//SEND PARA AVISAR QUE LLEGO TODO FLAMA
bool send_ack(int fd, bool ack) {
    void* stream = malloc(sizeof(bool));
    memcpy(stream, &ack, sizeof(bool));
    if (send(fd, stream, sizeof(bool), 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

//IO
bool send_io(int fd, char* io, char* msg){
  size_t size, size2;
  void * stream = serializar_string(&size, io);
  if(send(fd, stream, size, 0) == -1){
    free(stream);    
    return false;
  } 
  void * stream2 = serializar_string(&size2, msg);
  if(send(fd, stream2, size2, 0) == -1){
    free(stream2);    
    return false;
  } 
  free(stream);
  free(stream2);
   
  return true;
}

//MEMORIA
bool send_probar_en_swamp(uint32_t size, unsigned long id){
  return true;
}

bool recv_ack(int fd, bool* ack) {
    void* stream = malloc(sizeof(bool));
    if (recv(fd, stream, sizeof(bool), 0) != sizeof(bool)) {
        free(stream);
        return false;
    }
    memcpy(ack, stream, sizeof(bool));

    free(stream);
    return true;
}


bool send_finalizar_carpincho(int fd, unsigned long id) {
  void* stream = malloc(sizeof(op_code) + sizeof(long));
  op_code op = FREE_CARPINCHO;
  memcpy(stream, &op, sizeof(op_code));
  memcpy(stream + sizeof(op_code), &id, sizeof(long));
  if(send(fd, stream, sizeof(op_code) + sizeof(long), 0) == -1){
    free(stream);    
    return false;
  }
  free(stream);    
  return true;
}



//SWAmP

/* TODO: A implementar */

/* Preguntar si necesita el op code */
/* Envia la pagina a memoria */
bool send_pagina(int fd, long carpincho_id, uint32_t nro_pagina, void* data, uint32_t tamanio_pagina){
  void* stream = malloc(sizeof(long) + sizeof(nro_pagina) + tamanio_pagina);
  op_code op = PAGINA_DE_SWAP;

  memcpy(stream, &carpincho_id, sizeof(long));
  memcpy(stream + sizeof(long), &nro_pagina, sizeof(uint32_t));
  memcpy(stream + sizeof(long) + sizeof(uint32_t), data, tamanio_pagina);
  if(send(fd,stream,sizeof(long) + sizeof(nro_pagina) + tamanio_pagina, 0) == -1){
    free(stream);
    return false;
  }

  free(stream);
  return true;

  return true;
}

/* Recibe el ID de un carpincho */
bool recv_id(int cliente_socket, unsigned long* carpincho_id){
  void* stream = malloc(sizeof(long));

  if(recv(cliente_socket,stream,sizeof(long),0) != sizeof(long)){
    free(stream);
    return false;
  }

  memcpy(carpincho_id,stream,sizeof(long));
  free(stream);
  return true;
}// TODO: Recibe el id del carpincho 

bool recv_lectura(int cliente_socket, unsigned long* carpincho_id, uint32_t*  nro_pagina){

  void* stream = malloc(sizeof(long) + sizeof(uint32_t));
  if(recv(cliente_socket,stream,sizeof(long) + sizeof(uint32_t),0) != sizeof(long) + sizeof(uint32_t)){
    free(stream);
    return false;
  }
  memcpy(carpincho_id,stream,sizeof(long));
  memcpy(nro_pagina,stream + sizeof(long) ,sizeof(uint32_t));
  free(stream);
  return true;
}// TODO: Recibe el pedido de lectura de memoria

bool recv_ecritura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina, void *data, uint32_t tamanio_pagina){
  void* stream = malloc(sizeof(long) + sizeof(uint32_t) + tamanio_pagina);
  if(recv(cliente_socket,stream,sizeof(long) + sizeof(uint32_t) + tamanio_pagina,0) != sizeof(long) + sizeof(uint32_t) + tamanio_pagina){
    free(stream);
    return false;
  }
  memcpy(carpincho_id, stream, sizeof(long));
  memcpy(nro_pagina,stream + sizeof(long) ,sizeof(uint32_t));
  memcpy(data,stream + sizeof(long) + sizeof(uint32_t) , tamanio_pagina);
  free(stream);
  return true;
}

bool recv_solicitud_espacio_libre(int cliente_socket, unsigned long* carpincho_id, uint32_t* cant_paginas){
  void* stream = malloc(sizeof(long) + sizeof(uint32_t));
  if(recv(cliente_socket,stream,sizeof(long) + sizeof(uint32_t),0) != sizeof(long) + sizeof(uint32_t)){
    free(stream);
    return false;
  }
  memcpy(carpincho_id,stream,sizeof(long));
  memcpy(cant_paginas,stream + sizeof(long) ,sizeof(uint32_t));
  free(stream);
  return true;
}

bool entra_en_swap(unsigned long id_carpincho, uint32_t cantidad_de_paginas, int fd_swamp){
  send_codigo_op(fd_swamp, ESPACIO_LIBRE);
  void* stream = malloc(sizeof(long) + sizeof(uint32_t));
  memcpy(stream, &id_carpincho, sizeof(long));
  memcpy(stream + sizeof(long), &cantidad_de_paginas, sizeof(uint32_t));
  if(send(fd_swamp, stream, sizeof(uint32_t) + sizeof(long), 0) == -1){
    free(stream);    
    return false;
  }
  
  bool ack;
  recv_ack(fd_swamp, &ack);  
  if(ack){
    send_codigo_op(fd_swamp,ALLOCAR_EN_SWAP);
    send(fd_swamp, stream, sizeof(uint32_t) + sizeof(long), 0);
    free(stream);  
    return true;
  } 
  free(stream);  
  return false;
}

bool quitar_en_swap(unsigned long id_carpincho, uint32_t cantidad_de_paginas, int fd_swamp){
  send_codigo_op(fd_swamp, LIBERAR_MARCOS);
  void* stream = malloc(sizeof(long) + sizeof(uint32_t));
  memcpy(stream, &id_carpincho, sizeof(long));
  memcpy(stream + sizeof(long), &cantidad_de_paginas, sizeof(uint32_t));
  if(send(fd_swamp, stream, sizeof(uint32_t) + sizeof(long), 0) == -1){
    free(stream);    
    return false;
  }
  free(stream);  
  bool ack;
  recv_ack(fd_swamp, &ack);  
  if(ack){
    return true;
  } 
  return false;
}



bool recv_allocar(int cliente_socket, unsigned long* carpincho_id, uint32_t* cant_paginas){
  void* stream = malloc(sizeof(long) + sizeof(uint32_t) );
  if(recv(cliente_socket,stream,sizeof(long) + sizeof(uint32_t),0) != sizeof(long) + sizeof(uint32_t)){
    free(stream);
    return false;
  }
  memcpy(carpincho_id,stream,sizeof(long));
  memcpy(cant_paginas,stream + sizeof(long) ,sizeof(uint32_t));
  free(stream);
  return true;
}

bool recv_esquema_asignacion(int cliente_socket, bool* asignacion_fija){
  void* stream = malloc(sizeof(bool));
  if(recv(cliente_socket,stream,sizeof(bool),0) != sizeof(bool)){
    free(stream);
    return false;
  }
  memcpy(asignacion_fija,stream,sizeof(bool));
  free(stream);
  return true;
}

bool recv_solicitud_liberar_marcos(int cliente_socket, unsigned long* carpincho_id, uint32_t* cant_paginas){
  void* stream = malloc(sizeof(long) + sizeof(uint32_t) );
  if(recv(cliente_socket,stream,sizeof(long) + sizeof(uint32_t),0) != sizeof(long) + sizeof(uint32_t)){
    free(stream);
    return false;
  }
  memcpy(carpincho_id,stream,sizeof(long));
  memcpy(cant_paginas,stream + sizeof(long) ,sizeof(uint32_t));
  free(stream);
  return true;
}