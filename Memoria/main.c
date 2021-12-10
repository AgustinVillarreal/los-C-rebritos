#include "include/main.h"

extern t_log* logger;

//SIGNAL HANDLER
void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
			log_info(logger, "Senial SIGUSR1");
			log_info(logger, "-------------");
			dumpear_TLB(); 
            break;
        case SIGUSR2:
			log_info(logger, "Senial SIGUSR2");
			log_info(logger, "-------------");
			borrar_entradas_TLB();
            break;
        case SIGINT:
			log_info(logger, "Senial SIGINT");
			log_info(logger, "-------------");
			imprimir_metricas_TLB();
        	exit(EXIT_SUCCESS);
			break;
		
    }
}

int main(int argc, char *argv[]){

	signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);
    signal(SIGINT , sighandler);
	
	MEMORIA_CFG = initialize_cfg();

	int memoria_server;
	int swap_fd;
	// hasta que no este el swap el generar conexiones no va a compilar
	if( !init() | !cargar_configuracion(MEMORIA_CFG, argv[1]) || !cargar_memoria(MEMORIA_CFG) | !generar_conexion(&swap_fd, MEMORIA_CFG)){
		cerrar_programa(logger,MEMORIA_CFG);
		return EXIT_FAILURE;
	}
	
	memoria_server = iniciar_servidor(logger, SERVERNAME, MEMORIA_CFG->IP, MEMORIA_CFG->PUERTO);

	if(!memoria_server){
		cerrar_programa(logger,MEMORIA_CFG);
		printf("Falle :(");
		return EXIT_FAILURE;
	}

	if(!send_handshake_swap(swap_fd, MEMORIA_CFG->FIJA)){
		cerrar_programa(logger,MEMORIA_CFG);
		printf("Falle :(");
		return EXIT_FAILURE;
	}
	

	while(server_escuchar(SERVERNAME, memoria_server, swap_fd));

	liberar_conexion(&memoria_server);

	cerrar_programa(logger,MEMORIA_CFG);
	return 0;
}
