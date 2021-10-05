#include "../shared/protocolo.h"

bool send_handshake(int fd_server){
  send_codigo_op(fd_server, HANDSHAKE);
}

bool send_memalloc(int fd_server){
  send_codigo_op(fd_server, MEM_ALLOC);
}

bool send_codigo_op(int fd, op_code cop) {
    size_t size = sizeof(op_code);
    return send(fd,&cop,size,0) != -1;
}