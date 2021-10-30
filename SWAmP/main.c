#include "include/main.h"

extern t_config_swamp* cfg;
extern t_log* logger;

extern t_list* areas_de_swap;

extern sem_t SEM_COMPACTACION_DONE;
extern sem_t SEM_COMPACTACION_START;

int swamp_server;

int main(){

	
	if(!init() || !cargar_configuracion("swamp.config") || !cargar_swamp()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    /* ****** CREACION DEL SERVIDOR ****** */
    char* puerto = string_itoa(cfg->PUERTO);
    swamp_server = iniciar_servidor(logger, SERVERNAME, "0.0.0.0", puerto);
    free(puerto);


  	while(server_escuchar(SERVERNAME, swamp_server));

    liberar_conexion(&swamp_server);
    cerrar_programa();

    return EXIT_SUCCESS;
}
