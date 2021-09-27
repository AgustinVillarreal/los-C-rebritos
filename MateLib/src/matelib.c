#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){
	
	t_log* logger;
	int servidor_fd;
	t_config* cfg = config_create(config);
	char* IP = strdup(config_get_string_value(cfg, "IP"));
	char* PUERTO = strdup(config_get_string_value(cfg, "PUERTO"));
	char*	NIVEL_LOGEO = strdup(config_get_string_value(cfg, "NIVEL_LOGEO"));
	logger = log_create("mateLib.log", "mateLib", true, log_string_enum(NIVEL_LOGEO));

	//TODO ver nombre_server
	servidor_fd = crear_conexion(logger, "Server", IP, PUERTO);
	if(!servidor_fd){
		return EXIT_FAILURE;
	}

	//Logger se guarda en la lib_ref



	free(IP);
	free(PUERTO);
	free(NIVEL_LOGEO);
	config_destroy(cfg);
	return 0;
}

int mate_close(mate_instance *lib_ref){

}
