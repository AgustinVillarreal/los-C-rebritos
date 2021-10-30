#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){
	
	lib_ref->group_info = malloc(sizeof(mate_inner_structure));

	op_code cop;
	
	t_log* logger;
	int servidor_fd;
	t_config* cfg = config_create(config);
	char* IP = NULL;
	char* PUERTO = NULL;
	char* NIVEL_LOGEO = strdup("LOG_LEVEL_INFO");

	IP = strdup(config_get_string_value(cfg, "IP"));
	PUERTO = strdup(config_get_string_value(cfg, "PUERTO"));
	//TODO ver que si no encuentra el nivel de logeo siga funcioando
	if(config_has_property(cfg, "NIVEL_LOGUEO")){
		strcpy(NIVEL_LOGEO, config_get_string_value(cfg, "NIVEL_LOGEO"));
	}
	logger = log_create("mateLib.log", "mateLib", true, log_string_enum(NIVEL_LOGEO));
	free(NIVEL_LOGEO);

	//TODO ver nombre_server
	servidor_fd = crear_conexion(logger, "Server", IP, PUERTO);
	if(!servidor_fd){
		data_destroy(IP, PUERTO, cfg);		
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	if(!send_handshake(servidor_fd)){
		data_destroy(IP, PUERTO, cfg);	
		log_destroy(logger);	
		return EXIT_FAILURE;
	}

	if(recv(servidor_fd,  &cop, sizeof(op_code), 0) == -1){
		log_error(logger, "Error en el handshake");
		data_destroy(IP, PUERTO, cfg);
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	mate_inner_structure* inner_structure = lib_ref->group_info;
	inner_structure->logger = logger;
	inner_structure->IP = IP;
	inner_structure->PUERTO = PUERTO;
	inner_structure->servidor_fd = servidor_fd;
	inner_structure->kernel_connected = cop == HANDSHAKE_KERNEL;
	inner_structure->id= generate_id();

	if(inner_structure->kernel_connected){
		if(!send_poner_cola_new(servidor_fd)){
			data_destroy(IP, PUERTO, cfg);	
			log_destroy(logger);	
			return EXIT_FAILURE;
		}
		if(!send_data_cola_new(servidor_fd, inner_structure->id)){
			log_error(logger, "Error enviando");
			data_destroy(IP, PUERTO, cfg);
			log_destroy(logger);
			return EXIT_FAILURE;
		}
	}
	
	if(recv(servidor_fd, &cop, sizeof(op_code), 0) == -1){
		log_error(logger, "Error en la espera de poner en exec");
		data_destroy(IP, PUERTO, cfg);
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	data_destroy(IP, PUERTO, cfg);
	return 0;
}

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!send_codigo_op(inner_structure->servidor_fd, SEM_INIT)){
		free(inner_structure->IP);
		free(inner_structure->PUERTO);	
		log_destroy(inner_structure->logger);	
		return EXIT_FAILURE;
	}
	if(!send_sem_init(inner_structure->servidor_fd, sem, value)){
		free(inner_structure->IP);
		free(inner_structure->PUERTO);	
		log_destroy(inner_structure->logger);	
		return EXIT_FAILURE;
	}




	return 0;
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!send_codigo_op(inner_structure->servidor_fd, SEM_WAIT)){
		free(inner_structure->IP);
		free(inner_structure->PUERTO);	
		log_destroy(inner_structure->logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!send_codigo_op(inner_structure->servidor_fd, SEM_POST)){
		free(inner_structure->IP);
		free(inner_structure->PUERTO);	
		log_destroy(inner_structure->logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!send_codigo_op(inner_structure->servidor_fd, SEM_DESTROY)){
		free(inner_structure->IP);
		free(inner_structure->PUERTO);	
		log_destroy(inner_structure->logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

mate_pointer mate_memalloc(mate_instance *lib_ref, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	 
	if(!send_memalloc(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return -1;
	}
	send_alloc_data(inner_structure->servidor_fd, inner_structure->id, size);
	return 0;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr){
	mate_inner_structure* inner_structure = lib_ref->group_info;	
	if(!send_memfree(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;		
	if(!send_memread(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 0;
}


int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;			
	if(!send_memwrite(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	return 1;
}


int mate_close(mate_instance *lib_ref){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	log_destroy(inner_structure->logger);
	free(lib_ref->group_info);
}
