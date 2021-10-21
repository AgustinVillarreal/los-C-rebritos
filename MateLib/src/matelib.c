#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){
	
	lib_ref->group_info = malloc(sizeof(mate_inner_structure));

	op_code cop;
	
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
	mate_inner_structure* inner_structure = lib_ref->group_info;
	inner_structure->logger = logger;
	inner_structure->servidor_fd = servidor_fd;
	inner_structure->kernel_connected = cop == HANDSHAKE_KERNEL;
	inner_structure->id= generate_id();

	if(inner_structure->kernel_connected){
		if(!send_poner_cola_new(servidor_fd)){
			data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
			log_destroy(logger);	
			return EXIT_FAILURE;
		}
		if(!send_data_cola_new(servidor_fd, inner_structure->id)){
			log_error(logger, "Error enviando");
			data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);
			log_destroy(logger);
			return EXIT_FAILURE;
		}
	}

	  
	
	// Logger se guarda en la lib_ref
	log_destroy(logger);

	data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);
	return 0;
}

mate_pointer mate_memalloc(mate_instance *lib_ref, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	 
	if(!send_memalloc(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return -1;
	}
	send_alloc_data(inner_structure->servidor_fd, inner_structure->id, size);
	return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr){
	mate_inner_structure* inner_structure = lib_ref->group_info;	
	if(!send_memfree(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;		
	if(!send_memread(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}


int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;			
	if(!send_memwrite(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, NIVEL_LOGEO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 1;
}


int mate_close(mate_instance *lib_ref){
	free(lib_ref->group_info);
}
