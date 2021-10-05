#include "include/main.h"

t_log* logger;

static t_config_memoria* initialize_cfg(){
	t_config_memoria* cfg = malloc(sizeof(t_config_memoria));
	cfg->IP = NULL;
    cfg->PUERTO = NULL;
    cfg->ALGORITMO_REEMPLAZO_MMU = NULL;
    cfg->TIPO_ASIGNACION = NULL;
    cfg->ALGORITMO_REEMPLAZO_TLB = NULL;
	return cfg;
}


int main(){

	MEMORIA_CFG = initialize_cfg();
	logger = log_create("memoria.log","memoria",true,LOG_LEVEL_INFO);

	int memoria_server;

	if(!cargar_configuracion(MEMORIA_CFG)){
		cerrar_programa(logger,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	memoria_server = iniciar_servidor(logger,SERVERNAME,MEMORIA_CFG->IP,MEMORIA_CFG->PUERTO);
	if(!memoria_server){
		cerrar_programa(logger,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	while(server_escuchar(SERVERNAME,memoria_server));

	liberar_conexion(&memoria_server);

	printf("Hola mundo soy ian");
	cerrar_programa(logger,MEMORIA_CFG);
	return 0;
}
