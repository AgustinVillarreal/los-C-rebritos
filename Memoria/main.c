#include "include/main.h"

t_log* main_log;

static t_config_memoria* initialize_cfg(){
	t_config_memoria* cfg = malloc(sizeof(t_config_memoria));
	return cfg;
}


int main(){

	MEMORIA_CFG = initialize_cfg();
	main_log = log_create("memoria.log","memoria",true,LOG_LEVEL_INFO);

	if(!cargar_configuracion(MEMORIA_CFG)){
		cerrar_programa(main_log,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	printf("Hola mundo");
	cerrar_programa(main_log,MEMORIA_CFG);
	return 0;
}
