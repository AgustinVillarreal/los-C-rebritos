#include "include/main.h"


t_log* logger;

static t_config_kernel* initialize_cfg() {
    t_config_kernel* cfg            = malloc(sizeof(t_config_kernel));
    cfg->IP_KERNEL                  = NULL;
    cfg->PUERTO_KERNEL              = NULL;
    cfg->IP_MEMORIA                 = NULL;
    cfg->PUERTO_KERNEL              = NULL;
    cfg->ALGORITMO_PLANIFICACION    = NULL;
    cfg->DISPOSITIVOS_IO            = NULL;
    cfg->DURACIONES_IO              = NULL;
    return cfg;
}

int main(){
	KERNEL_CFG = initialize_cfg();
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_INFO);

    int memoria_fd;
    int kernel_server;

    //TODO generar_conexiones esta vacio
    if(!cargar_configuracion(KERNEL_CFG) || !generar_conexiones(&memoria_fd, KERNEL_CFG)) {
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    kernel_server = iniciar_servidor(logger, SERVERNAME, KERNEL_CFG->IP_KERNEL, KERNEL_CFG->PUERTO_KERNEL);
    if(!kernel_server){
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    while (server_escuchar(SERVERNAME, kernel_server));

    liberar_conexion(&kernel_server);

	printf("Hola mundo");

	cerrar_programa(logger, KERNEL_CFG);

	return 0;
}
