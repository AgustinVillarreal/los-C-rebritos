/*
 * conexion.h
 *
 *  Created on: 20 sep. 2021
 *      Author: utnso
 */

#ifndef INCLUDE_CONEXION_H_
#define INCLUDE_CONEXION_H_

#include <stdbool.h>
#include "config.h"
#include <shared/sockets.h>

void data_destroy(char *uno, char *dos, t_config_kernel* cfg);
bool generar_conexion(int*,t_config_kernel*);


#endif /* INCLUDE_CONEXION_H_ */
