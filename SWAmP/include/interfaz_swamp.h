#ifndef INTERFAZ_SWAMP_H
#define INTERFAZ_SWAMP_H


#include "config.h"
#include "manejo_swamp.h"
#include "../../Shared/shared/protocolo.h"


/* TODO : Establecer parametros  */

void proceder_escritura(int fd,unsigned long id,uint32_t pagina,void* data);                

void buscar_frame_en_swap(int fd,unsigned long id, uint32_t nro_pagina, void** data);

void borrar_carpincho_swap(int fd,unsigned long carpincho_id);

bool revisar_espacio_libre(int fd,unsigned long carpincho_id, uint32_t cant_paginas, bool asignacion_fija);

void proceder_allocar(int cliente_socket,unsigned long carpincho_id, uint32_t nro_pagina, bool asigancion_fija);

#endif