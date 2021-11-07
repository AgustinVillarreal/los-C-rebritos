#ifndef INTERFAZ_SWAMP_H
#define INTERFAZ_SWAMP_H


#include "config.h"
#include "manejo_swamp.h"
#include "../../Shared/shared/protocolo.h"


/* TODO : Establecer parametros  */

void proceder_asignacion_fija(int fd,unsigned long id,uint32_t pagina,void* data);                
void proceder_asignacion_global(int fd,unsigned long id,uint32_t pagina,void* data);


void buscar_frame_en_swap(unsigned long id, uint32_t nro_pagina, void** data, bool esquema_asignacion);

void borrar_carpincho_swap(unsigned long carpincho_id);

#endif