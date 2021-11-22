#ifndef INTERFAZ_SWAMP_H
#define INTERFAZ_SWAMP_H


#include "config.h"
#include "manejo_swamp.h"
#include "../../Shared/shared/protocolo.h"


/* TODO : Establecer parametros  */

void proceder_asignacion(int fd,unsigned long id,uint32_t pagina,void* data,bool asignacion_fija);                

void buscar_frame_en_swap(unsigned long id, uint32_t nro_pagina, void** data, bool esquema_asignacion);

void borrar_carpincho_swap(unsigned long carpincho_id);

bool revisar_espacio_libre(unsigned long carpincho_id, uint32_t cant_paginas, bool asignacion_fija);

#endif