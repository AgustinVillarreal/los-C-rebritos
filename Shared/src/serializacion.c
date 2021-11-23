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



void * serializar_memfree_data(unsigned long id, uint32_t direccion_logica){
    void * stream = malloc(sizeof(uint32_t) + sizeof(long));
    memcpy(stream, &id, sizeof(long));
    memcpy(stream + sizeof(long), &direccion_logica, sizeof(uint32_t));    
    return stream;
}

void * serializar_sem_init(size_t* size, char* sem, unsigned int value){

    size_t sz_stream_sem;
    void* stream_sem = serializar_string(&sz_stream_sem, sem);

    *size = sz_stream_sem;
    void* stream = malloc(*size + sizeof(int));

    memcpy(stream, stream_sem, sz_stream_sem);
    memcpy(stream + *size, &value, sizeof(int));
    
    free(stream_sem);
    return stream;
}

void deserializar_sem_init(size_t size, void* stream, char** sem_aux, int* value_aux) {
    char* r_sem = malloc(size);
    memcpy(r_sem, stream, size);

    *sem_aux = r_sem;

    memcpy(value_aux, size + stream, sizeof(int));    
}


void* serializar_string(size_t* size, char* cadena) {
    size_t sz_cadena = strlen(cadena) + 1;
    void* stream = malloc(sizeof(size_t) + sz_cadena);

    memcpy(stream, &sz_cadena, sizeof(size_t));
    memcpy(stream+sizeof(size_t), cadena, sz_cadena);

    *size = sizeof(size_t) + sz_cadena;

    return stream;
}

void deserializar_alloc_data(void* stream, long* id,int* size_data){
  memcpy(id,stream,sizeof(long));
  memcpy(size_data,stream+sizeof(long),sizeof(int));
}

void deserializar_memfree_data(void* stream, long* id,uint32_t* direccion_logica){
  memcpy(id,stream,sizeof(long));
  memcpy(direccion_logica,stream+sizeof(long),sizeof(uint32_t));
}


void deserializar_sem_wait(size_t size, void* stream, char** sem_aux) {
    char* r_sem = malloc(size);
    memcpy(r_sem, stream, size);
    *sem_aux = r_sem;
}
