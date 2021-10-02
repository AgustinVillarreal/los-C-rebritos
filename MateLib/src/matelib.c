#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){

	op_code cop;
	
	t_log* logger;
	int servidor_fd;
	t_config* cfg = config_create(config);
	char* IP = NULL;
	IP = strdup(config_get_string_value(cfg, "IP"));
	char* PUERTO = NULL;
	PUERTO = strdup(config_get_string_value(cfg, "PUERTO"));
	//TODO ver que si no encuentra el nivel de logeo siga funcioando
	char* NIVEL_LOGEO = NULL;
	NIVEL_LOGEO = strdup(config_get_string_value(cfg, "NIVEL_LOGEO"));
	logger = log_create("mateLib.log", "mateLib", true, log_string_enum(NIVEL_LOGEO));


	//TODO ver nombre_server
	servidor_fd = crear_conexion(logger, "Server", IP, PUERTO);
	if(!servidor_fd){
		data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);		
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	if(send_handshake(servidor_fd) == -1){
		data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		log_destroy(logger);	
		return EXIT_FAILURE;
	}

	if(recv(servidor_fd,  &cop, sizeof(op_code), 0) == -1){
		log_error(logger, "Error en el handshake");
		data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	lib_ref->kernel_connected = cop == HANDSHAKE_KERNEL;
	
	// Logger se guarda en la lib_ref
	log_destroy(logger);

	data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);
	return 0;
}

int mate_close(mate_instance *lib_ref){

}
