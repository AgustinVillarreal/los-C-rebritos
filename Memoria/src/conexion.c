#include "../include/conexion.h"

extern t_log* logger;

bool generar_conexiones(int* swap_fd, t_config_memoria* memoria_cfg){
	
	*swap_fd = crear_conexion(logger,"Server",memoria_cfg->IP_SWAP,memoria_cfg->PUERTO_SWAP);
	if(!*swap_fd){
		data_destroy(memoria_cfg->IP_SWAP,memoria_cfg->PUERTO_SWAP, memoria_cfg);
		log_destroy(logger);
		return EXIT_FAILURE;
	}
	return 1;

}

void data_destroy(char* uno, char* dos, t_config_memoria* cfg){
	free(uno),
	free(dos),
	cerrar_programa(logger,cfg);
}