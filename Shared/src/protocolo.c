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



bool recv_alloc_data(int fd, long* id_carpincho, int* size_data){
  void* stream = malloc(sizeof(long)+sizeof(int));
  if(recv(fd,&stream,sizeof(stream),0) != sizeof(stream)){
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


