#ifndef SERIALIZACION_H_

#define SERIALIZACION_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void * serializar_alloc_data(unsigned long id, int size);

void * serializar_sem_init(size_t* size, char* sem, unsigned int value);
void * serializar_string(size_t* size, char* cadena);
void deserializar_sem_init(size_t size, void* stream, char** sem_aux, int* value_aux);

void deserializar_alloc_data(void* stream, long* id,int* size_data);


#endif