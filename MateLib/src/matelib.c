#include "../include/matelib.h"

#define ERROR -1


//------------------General Functions---------------------/

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
	log_info(logger, "Handshake");

	mate_inner_structure* inner_structure = lib_ref->group_info;
	inner_structure->logger = logger;
	inner_structure->IP = IP;
	inner_structure->PUERTO = PUERTO;
	inner_structure->servidor_fd = servidor_fd;
	inner_structure->kernel_connected = cop == HANDSHAKE_KERNEL;

	if(!send_mate_init(servidor_fd, -1)){
		data_destroy(IP, PUERTO, cfg);	
		log_destroy(logger);	
		return EXIT_FAILURE;
	}
	
	if(recv(servidor_fd, &(inner_structure->id), sizeof(long), 0) == -1){
		log_error(logger, "Error recibiendo id");
		data_destroy(IP, PUERTO, cfg);
		log_destroy(logger);
		return EXIT_FAILURE;
	}
		
	if(inner_structure->kernel_connected){
		if(recv(servidor_fd, &cop, sizeof(op_code), 0) == -1){
			log_error(logger, "Error en la espera de poner en exec");
			data_destroy(IP, PUERTO, cfg);
			log_destroy(logger);
			return EXIT_FAILURE;
		}	
	}
	

	data_destroy(IP, PUERTO, cfg);
	return 0;
}

int mate_close(mate_instance *lib_ref){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!send_codigo_op(inner_structure->servidor_fd, FREE_CARPINCHO)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return EXIT_FAILURE;
	}
	log_destroy(inner_structure->logger);
	free(lib_ref->group_info);
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value){
	int result_code;
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(inner_structure->kernel_connected){
		if(!send_codigo_op(inner_structure->servidor_fd, SEM_INIT)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		if(!send_sem_init(inner_structure->servidor_fd, sem, value)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		if(recv(inner_structure->servidor_fd, &result_code, sizeof(int), 0) == -1){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
	} else {
		log_error(inner_structure->logger, "No podes usar semaforos si no estas conectado al kernel\n");
		return ERROR;	
	}

	if(result_code == -1) {
		return ERROR;
	}else {
		return 0;
	}
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	int result_code;
	if(inner_structure->kernel_connected){
		if(!send_codigo_op(inner_structure->servidor_fd, SEM_WAIT)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		if(!send_sem(inner_structure->servidor_fd, sem)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}

		if(recv(inner_structure->servidor_fd, &result_code, sizeof(int), 0) == -1){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}

		if(result_code == -2) {
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);
			free(inner_structure);
			exit(ERROR);
		}
	}	else {
		log_error(inner_structure->logger, "No podes usar semaforos si no estas conectado al kernel\n");
		return ERROR;	
	}

	

	return result_code;
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	int result_code;
	if(inner_structure->kernel_connected){
		if(!send_codigo_op(inner_structure->servidor_fd, SEM_POST)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		if(!send_sem(inner_structure->servidor_fd, sem)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}

		if(recv(inner_structure->servidor_fd, &result_code, sizeof(int), 0) == -1){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		
	}	else {
		log_error(inner_structure->logger, "No podes usar semaforos si no estas conectado al kernel\n");
		return ERROR;	
	}
	return result_code;
}
// TODO ABSTRAER WAIT, POST Y DESTROY

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	int result_code;
	if(inner_structure->kernel_connected){
		if(!send_codigo_op(inner_structure->servidor_fd, SEM_DESTROY)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		if(!send_sem(inner_structure->servidor_fd, sem)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}

		if(recv(inner_structure->servidor_fd, &result_code, sizeof(int), 0) == -1){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return ERROR;
		}
		
	}	else {
		log_error(inner_structure->logger, "No podes usar semaforos si no estas conectado al kernel\n");
		return ERROR;	
	}
	return result_code;
}

// //--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	int result_code;	
	if(inner_structure->kernel_connected){
		if(!send_codigo_op(inner_structure->servidor_fd, IO)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return EXIT_FAILURE;
		}
		if(!send_io(inner_structure->servidor_fd, io, msg)){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return EXIT_FAILURE;
		}
		if(recv(inner_structure->servidor_fd, &result_code, sizeof(int), 0) == -1){
			free(inner_structure->IP);
			free(inner_structure->PUERTO);	
			log_destroy(inner_structure->logger);	
			return EXIT_FAILURE;
		}
	} else {
		log_error(inner_structure->logger, "No podes usar dispositivos de entrada salida si no estas conectado al kernel\n");
		return EXIT_FAILURE;
	}
	return result_code == 0;
}

// //--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size){
	mate_inner_structure* inner_structure = lib_ref->group_info;
	if(!inner_structure->kernel_connected){
  		send_carpincho_ready(inner_structure->servidor_fd, inner_structure->id);
	}
	if(!send_memalloc(inner_structure->servidor_fd)){
		// data_destroy(IP, PUERTO, cfg);	
		// log_destroy(logger);	
		return -1;
	}
	send_alloc_data(inner_structure->servidor_fd, inner_structure->id, size);
	uint32_t direccion;
	recv(inner_structure->servidor_fd, &direccion, sizeof(uint32_t), 0);
	if(direccion == 0xFFFF){
		return NULL;
	}
	
	return direccion;
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


