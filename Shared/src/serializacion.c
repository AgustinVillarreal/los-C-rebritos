#include "../shared/serializacion.h"

// static void* serializar_carpincho(uint32_t id_tripulante, t_status estado) {
//     void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) + sizeof(t_status));
//     op_code cop = CAMBIO_ESTADO;
//     memcpy(stream, &cop, sizeof(op_code));
//     memcpy(stream+sizeof(op_code), &id_tripulante, sizeof(uint32_t));
//     memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &estado, sizeof(t_status));
//     return stream;
// }

void * serializar_alloc_data(unsigned long id, int size){
    void * stream = malloc(sizeof(int) + sizeof(long));
    memcpy(stream, &id, sizeof(long));
    memcpy(stream + sizeof(long), &size, sizeof(int));    
    return stream;
}