#include "../shared/protocolo.h"

bool send_handshake(int fd_server){
  send_codigo_op(fd_server, HANDSHAKE);
}

bool send_memalloc(int fd_server){
  send_codigo_op(fd_server, MEM_ALLOC);
}

bool send_memfree(int fd_server){
  send_codigo_op(fd_server, MEM_FREE);
}

bool send_memread(int fd_server){
  send_codigo_op(fd_server, MEM_READ);
}

bool send_memwrite(int fd_server){
  send_codigo_op(fd_server, MEM_WRITE);
}

bool send_poner_cola_new(int fd_server){
  send_codigo_op(fd_server, PONER_COLA_NEW);
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

bool send_data_cola_new(int fd, unsigned long id){
  return send(fd, &id, sizeof(long), 0) != -1;
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