#ifndef INTERFAZ_SWAMP_H
#define INTERFAZ_SWAMP_H


#include "config.h"


/* TODO : Establecer parametros  */

void proceder_asignacion_fija();                
void proceder_asignacion_global();


void buscar_frame_en_swap(unsigned long id, uint32_t nro_pagina, void** data, bool esquema_asignacion);

void borrar_carpincho_swap(unsigned long carpincho_id);

#endif