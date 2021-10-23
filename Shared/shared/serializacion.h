#ifndef SERIALIZACION_H_

#define SERIALIZACION_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void * serializar_alloc_data(unsigned long id, int size);
void deserializar_alloc_data(void* stream, long* id,int* size_data);

#endif