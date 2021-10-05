/*
 * conexion.c
 *
 *  Created on: 20 sep. 2021
 *      Author: utnso
 */

#include "../include/conexion.h"

extern t_log* logger;

bool generar_conexion(int* memoria_fd, t_config_kernel* kernel_cfg){
	
	*memoria_fd = crear_conexion(logger, "Server", kernel_cfg->IP_MEMORIA, kernel_cfg->PUERTO_MEMORIA);
	if(!*memoria_fd){
		data_destroy(kernel_cfg->IP_MEMORIA, kernel_cfg->PUERTO_MEMORIA, kernel_cfg);		
		log_destroy(logger);
		return EXIT_FAILURE;
	}


	return 1;
}

void data_destroy(char *uno, char *dos, t_config_kernel* cfg){
  free(uno);
  free(dos);
  cerrar_programa(logger, cfg);
}

