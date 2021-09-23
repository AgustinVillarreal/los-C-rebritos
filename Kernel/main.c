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
    main_log = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_INFO);

    int memoria_fd;

    if(!cargar_configuracion(KERNEL_CFG) || !generar_conexiones(&memoria_fd, KERNEL_CFG)) {
        cerrar_programa(main_log, KERNEL_CFG);
        return EXIT_FAILURE;
    }
	printf("Hola mundo");

	cerrar_programa(main_log, KERNEL_CFG);

	return 0;
}
