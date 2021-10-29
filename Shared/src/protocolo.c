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