#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){

	op_code cop;
	lib_ref = malloc(sizeof(mate_instance));
	
	t_log* logger;
	int servidor_fd;
	t_config* cfg = config_create(config);
	char* IP = NULL;
	char* PUERTO = NULL;
	char* NIVEL_LOGEO = NULL;

	IP = strdup(config_get_string_value(cfg, "IP"));
	PUERTO = strdup(config_get_string_value(cfg, "PUERTO"));
	//TODO ver que si no encuentra el nivel de logeo siga funcioando
	NIVEL_LOGEO = strdup(config_get_string_value(cfg, "NIVEL_LOGEO"));
	logger = log_create("mateLib.log", "mateLib", true, log_string_enum(NIVEL_LOGEO));


	//TODO ver nombre_server
	servidor_fd = crear_conexion(logger, "Server", IP, PUERTO);
	if(!servidor_fd){
		data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);		
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	if(!send_handshake(servidor_fd)){
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

	lib_ref->logger = logger;
	lib_ref->servidor_fd = servidor_fd;
	lib_ref->kernel_connected = cop == HANDSHAKE_KERNEL;
	
	// Logger se guarda en la lib_ref
	log_destroy(logger);

	data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);
	return 0;
}

mate_pointer mate_memalloc(mate_instance *lib_ref, int size){

	if(!send_memalloc(lib_ref->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return -1;
	}
	send_alloc_data(lib_ref->servidor_fd, lib_ref->id, size);
	return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr){
	if(!send_memfree(lib_ref->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size){
	if(!send_memread(lib_ref->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}


int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size){
	if(!send_memwrite(lib_ref->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 1;
}


int mate_close(mate_instance *lib_ref){

}
