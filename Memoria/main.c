#include "include/main.h"

extern t_config_memoria*  MEMORIA_CFG;
extern t_log* logger;

//SIGNAL HANDLER
void sighandler(int x) {
    switch (x) {
        case SIGUSR1:
		//TODO: ver que hacer
            break;
        case SIGUSR2:
        //TODO: ver que hacer
            break;
        case SIGINT:
        //TODO: ver que hacer
        	exit(EXIT_SUCCESS);
		
    }
}

int main(){

	signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);
    signal(SIGINT , sighandler);

	int memoria_server;
	int swap_fd;
	// hasta que no este el swap el generar conexiones no va a compilar
	if( !init() | !cargar_configuracion(MEMORIA_CFG) || !cargar_memoria(MEMORIA_CFG)){
		cerrar_programa(logger,MEMORIA_CFG);
		return EXIT_FAILURE;
	}

	memoria_server = iniciar_servidor(logger,SERVERNAME,MEMORIA_CFG->IP,MEMORIA_CFG->PUERTO);

	if(!memoria_server){
		cerrar_programa(logger,MEMORIA_CFG);
		printf("Falle :(");
		return EXIT_FAILURE;
	}

	while(server_escuchar(SERVERNAME,memoria_server));

	liberar_conexion(&memoria_server);

	cerrar_programa(logger,MEMORIA_CFG);
	return 0;
}
