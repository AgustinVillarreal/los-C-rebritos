#include "include/main.h"

t_log* main_log;

static t_config_memoria* initialize_cfg(){
	t_config_memoria* cfg = malloc(sizeof(t_config_memoria));
	return cfg;
}


int main(){

	MEMORIA_CFG = initialize_cfg();
	main_log = log_create("memoria.log","memoria",true,LOG_LEVEL_INFO);

	int memoria_server;

	if(!cargar_configuracion(MEMORIA_CFG)){
		cerrar_programa(main_log,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	memoria_server = iniciar_servidor(main_log,SERVERNAME,MEMORIA_CFG->IP,MEMORIA_CFG->PUERTO);
	if(!memoria_server){
		cerrar_programa(main_log,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	while(server_escuchar(SERVERNAME,memoria_server));

	liberar_conexion(&memoria_server);

	printf("Hola mundo");
	cerrar_programa(main_log,MEMORIA_CFG);
	return 0;
}
