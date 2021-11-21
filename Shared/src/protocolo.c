#include "../shared/protocolo.h"

bool send_handshake(int fd_server){
  return send_codigo_op(fd_server, HANDSHAKE);
}

bool send_memalloc(int fd_server){
  return send_codigo_op(fd_server, MEM_ALLOC);
}

bool send_memfree(int fd_server){
  return send_codigo_op(fd_server, MEM_FREE);
}

bool send_memread(int fd_server){
  return send_codigo_op(fd_server, MEM_READ);
}

bool send_memwrite(int fd_server){
  return send_codigo_op(fd_server, MEM_WRITE);
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
  void * stream = serializar_alloc_data(id, direccion_logica);
  if(send(fd, stream, sizeof(long) + sizeof(direccion_logica), 0) == -1){
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
  int c_direccion_logica;
  deserializar_alloc_data(stream,&c_id,&direccion_logica);

  *id_carpincho = c_id;
  *direccion_logica = c_direccion_logica;

  free(stream);
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

bool send_pagina(int fd, long carpincho_id, uint32_t nro_pagina, void* data){
  return true;
}
bool recv_pagina(int fd, long* carpincho_id, uint32_t* nro_pagina, void** data){
  return true;
}

bool recv_id(int cliente_socket, unsigned long* carpincho_id){
  return true;
}// TODO: Recibe el id del carpincho

bool recv_lectura(int cliente_socket, unsigned long* carpincho_id, uint32_t*  nro_pagina){
  return true;
}// TODO: Recibe el pedido de lectura de memoria

bool recv_ecritura(int cliente_socket, unsigned long* carpincho_id, uint32_t* nro_pagina, void *data, bool* asigancion_fija){
  return true;
}// TODO: Recibe el pedido de escritura de memoria

bool recv_solicitud_espacio_libre(int cliente_socket, unsigned long* carpincho_id,uint32_t* cant_paginas, bool* asignacion_fija){
  return true;
}// TODO: Recibe de solicitud para saber si hay espacio libre para la cant_paginas
