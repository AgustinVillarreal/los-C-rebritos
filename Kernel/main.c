#include "include/main.h"

t_log* logger;

static t_config_kernel* initialize_cfg() {
    t_config_kernel* cfg            = malloc(sizeof(t_config_kernel));
    cfg->IP_KERNEL                  = NULL;
    cfg->PUERTO_KERNEL              = NULL;
    cfg->IP_MEMORIA                 = NULL;
    cfg->PUERTO_KERNEL              = NULL;
    cfg->ALGORITMO_PLANIFICACION    = NULL;
    cfg->DISPOSITIVOS_IO            = NULL;
    return cfg;
}

int main(){
	KERNEL_CFG = initialize_cfg();
    logger = log_create("Kernel.log", "Kernel", true, LOG_LEVEL_INFO);
    //TODO: Ver donde liberar esta memoria
    COLA_NEW               = queue_create();
    LISTA_READY             = list_create();
    COLA_EXIT              = queue_create();
    COLA_SUSPENDED_READY   = queue_create();
    LISTA_SUSPENDED_BLOCKED = list_create();
    LISTA_BLOCKED           = list_create();
    inicializar_semaforos();

    int memoria_fd;
    int kernel_server;

    if(!cargar_configuracion(KERNEL_CFG) || !generar_conexion(&memoria_fd, KERNEL_CFG)) {
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    iniciar_mutex(KERNEL_CFG->GRADO_MULTIPROGRAMACION);    

    kernel_server = iniciar_servidor(logger, SERVERNAME, KERNEL_CFG->IP_KERNEL, KERNEL_CFG->PUERTO_KERNEL);
    if(!kernel_server){
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    pthread_t LISTENER_PROGRAMACION;
    if(!pthread_create(&LISTENER_PROGRAMACION, NULL, (void*)listener_programacion, NULL)){
        pthread_detach(LISTENER_PROGRAMACION);
    } else {
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    SEM_CPUs = malloc(sizeof(sem_t) * KERNEL_CFG->GRADO_MULTIPROCESAMIENTO);
    for(int i = 0; i < KERNEL_CFG->GRADO_MULTIPROCESAMIENTO; i++){
        pthread_t CPU;
        if(!pthread_create(&CPU, NULL, (void*)ejecutar_CPU, (void*)i)){
            pthread_detach(CPU);
            sem_init(&SEM_CPUs[i], 0, 1);
        } else {
            cerrar_programa(logger, KERNEL_CFG);
            return EXIT_FAILURE;
        } 
    }

    pthread_t LISTENER_SUSPENCION;
    if(!pthread_create(&LISTENER_SUSPENCION, NULL, (void*)listener_suspencion, NULL)){
        pthread_detach(LISTENER_SUSPENCION);
    } else {
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    pthread_t LISTENER_DEADLOCK;
    if(!pthread_create(&LISTENER_DEADLOCK, NULL, (void*)listener_deadlock, NULL)){
        pthread_detach(LISTENER_DEADLOCK);
    } else {
        cerrar_programa(logger, KERNEL_CFG);
        return EXIT_FAILURE;
    }

    while (server_escuchar(SERVERNAME, kernel_server, memoria_fd));

    liberar_conexion(&kernel_server);

	printf("Hola mundo");

	cerrar_programa(logger, KERNEL_CFG);

	return 0;
}
