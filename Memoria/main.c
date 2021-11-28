#include "include/main.h"

extern t_log* logger;

//SIGNAL HANDLER
void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
			dumpear_TLB(); 
			log_info(logger, "hola");
            break;
        case SIGUSR2:
        //TODO: ver que hacer
            break;
        case SIGINT:
			imprimir_metricas_TLB();
        	exit(EXIT_SUCCESS);
			break;
		
    }
}

int main(){

	signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);
    signal(SIGINT , sighandler);
	
	MEMORIA_CFG = initialize_cfg();

	int memoria_server;
	int swap_fd;
	// hasta que no este el swap el generar conexiones no va a compilar
	if( !init() | !cargar_configuracion(MEMORIA_CFG) || !cargar_memoria(MEMORIA_CFG) | !generar_conexion(&swap_fd, MEMORIA_CFG)){
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
