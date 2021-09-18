#include "include/main.h"

t_log* main_log;

static t_config_kernel* initialize_cfg() {
    t_config_kernel* cfg = malloc(sizeof(t_config_kernel));
    cfg->IP_MEMORIA = NULL;
    cfg->ALGORITMO_PLANIFICACION = NULL;
    cfg->DISPOSITIVOS_IO = NULL;
    cfg->DURACIONES_IO = NULL;
    return cfg;
}

int main(){
	KERNEL_CFG = initialize_cfg();
	cargar_configuracion(KERNEL_CFG);

//	if(! || !generar_conexiones(&mi_ram_hq_fd, KERNEL_CFG)) {
//	        cerrar_programa(main_log, main_log_inv, DISCORDIADOR_CFG);
//	        return EXIT_FAILURE;
//	}
	printf("Hola mundo");

	return 0;
}
