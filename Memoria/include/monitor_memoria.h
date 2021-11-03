#ifndef INCLUDE_MONITOR_MEMORIA_H_
#define INCLUDE_MONITOR_MEMORIA_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include "config.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;

pthread_mutex_t MUTEX_TLB_BUSY;
pthread_mutex_t MUTEX_FRAME_BUSY;
pthread_mutex_t MUTEX_MP_BUSY;


uint32_t cant_frames_libres();
uint32_t cant_paginas(uint32_t size, size_t* rem);
uint32_t primer_frame_libre_framo(uint32_t pid, uint32_t* inicio, t_list* tp_carp);
void mutex_init();






#endif