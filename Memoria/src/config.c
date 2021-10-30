#include "../include/config.h"

extern t_log* logger;

void cerrar_programa(t_log* logger, t_config_memoria* cfg){
    log_destroy(logger);
    free(cfg->ALGORITMO_REEMPLAZO_MMU);
    free(cfg->TIPO_ASIGNACION);
    free(cfg->ALGORITMO_REEMPLAZO_TLB);
    free(cfg->PATH_DUMP_TLB);
    free(cfg->IP);
    free(cfg->PUERTO);
    free(cfg->IP_SWAP);
    free(cfg->PUERTO_SWAP);
    free(cfg);

  //TODO Destruir colas e hilos
  
}

uint8_t cargar_configuracion(t_config_memoria* config){
    t_config* cfg = config_create("memoria.config");

    if(cfg == NULL) {
        log_error(logger,"NO SE ENCONTRO memoria.config");
        return 0;
    }

    char* properties[] = {
        "IP",
        "PUERTO",
        "IP_SWAP",
        "PUERTO_SWAP",
        "TAMANIO",
        "TAMANIO_PAGINA",
        "ALGORITMO_REEMPLAZO_MMU",
        "TIPO_ASIGNACION",
        "MARCOS_POR_PROCESO",
        "CANTIDAD_ENTRADAS_TLB",
        "ALGORITMO_REEMPLAZO_TLB",
        "RETARDO_ACIERTO_TLB",
        "RETARDO_FALLO_TLB",
        "PATH_DUMP_TLB",
        NULL
    };

    if(!config_has_all_properties(cfg,properties)){
        log_error(logger,"NO SE ENCUENTRAN TODAS LAS PROPIEDADES DEFINIDAS EN EL CONFIG");
        config_destroy(cfg);
        return 0;
    }

    config->IP = strdup(config_get_string_value(cfg,"IP"));
    config->PUERTO = strdup(config_get_string_value(cfg,"PUERTO"));
    config->IP_SWAP = strdup(config_get_string_value(cfg,"IP_SWAP"));
    config->PUERTO_SWAP = strdup(config_get_string_value(cfg,"PUERTO_SWAP"));
    config->TAMANIO = config_get_int_value(cfg,"TAMANIO");
    config->TAMANIO_PAGINA = config_get_int_value(cfg,"TAMANIO_PAGINA");
    config->ALGORITMO_REEMPLAZO_MMU = strdup(config_get_string_value(cfg,"ALGORITMO_REEMPLAZO_MMU"));
    config->TIPO_ASIGNACION = strdup(config_get_string_value(cfg,"TIPO_ASIGNACION"));
    config->MARCOS_POR_PROCESO = config_get_int_value(cfg,"MARCOS_POR_PROCESO");
    config->CANTIDAD_ENTRADAS_TLB = config_get_int_value(cfg,"CANTIDAD_ENTRADAS_TLB");
    config->ALGORITMO_REEMPLAZO_TLB = strdup(config_get_string_value(cfg,"ALGORITMO_REEMPLAZO_TLB"));
    config->RETARDO_ACIERTO_TLB = config_get_int_value(cfg,"RETARDO_ACIERTO_TLB");
    config->RETARDO_FALLO_TLB = config_get_int_value(cfg,"RETARDO_FALLO_TLB");
    config->PATH_DUMP_TLB = strdup(config_get_string_value(cfg,"PATH_DUMP_TLB"));
    config->CANT_PAGINAS = config->TAMANIO / config->TAMANIO_PAGINA;

    if(!strcmp(config->ALGORITMO_REEMPLAZO_MMU, "CLOCK-M")){
        algoritmo_reemplazo_mmu = algoritmo_mmu_clock_m;
    } else {
        algoritmo_reemplazo_mmu = algoritmo_mmu_lru;
    }

    if(!strcmp(config->ALGORITMO_REEMPLAZO_TLB, "FIFO")){
        algoritmo_reemplazo_tlb = algoritmo_tlb_fifo;
    } else {
        algoritmo_reemplazo_tlb = algoritmo_tlb_lru;
    }

    if(!strcmp(config->TIPO_ASIGNACION, "FIJA")){
        tipo_asignacion = asignacion_fija;
    } else {
        tipo_asignacion = asignacion_global;
    }

    log_info(logger,"CONFIGURACION CARGADA EXITOSAMENTE");

    config_destroy(cfg);

    return 1;
}

static t_config_memoria* initialize_cfg(){
	t_config_memoria* cfg = malloc(sizeof(t_config_memoria));
    cfg->ALGORITMO_REEMPLAZO_MMU = NULL;
    cfg->TIPO_ASIGNACION = NULL;
    cfg->ALGORITMO_REEMPLAZO_TLB = NULL;
    cfg->PATH_DUMP_TLB = NULL;
    cfg->IP = NULL;
    cfg->PUERTO = NULL;
    cfg->IP_SWAP = NULL;
    cfg->PUERTO_SWAP = NULL;
	return cfg;
}

void init_mutex(){
    pthread_mutex_init(&MUTEX_FRAME_OCUPADO, NULL);
    pthread_mutex_init(&MUTEX_MP_BUSY, NULL);
}

uint8_t init(){
	MEMORIA_CFG = initialize_cfg();
	logger = log_create("memoria.log", "memoria", true, LOG_LEVEL_INFO);
    //TODO: iniciar mutex aca 
    init_mutex();
    return 1;
}

uint8_t cargar_memoria(t_config_memoria* cfg) {
    memoria_principal = malloc(cfg->TAMANIO); 
    if (memoria_principal == NULL) {
        log_error(logger, "Fallo en el malloc a memoria_principal");
        return 0;
    }
    memset(memoria_principal, 0, cfg->TAMANIO);
    memoria_disponible = cfg->TAMANIO; 

    global_TUR = 0;
        
        tp_carpinchos = list_create();
        if (tp_carpinchos == NULL) {
            log_error(logger, "Fallo creando tp_carpinchos");
            return 0;
        }

        tabla_frames = malloc(cfg->CANT_PAGINAS * sizeof(frame_t));
        if (tabla_frames == NULL) {
            log_error(logger, "Fallo creando tabla_frames");
            return 0;
        }
        for (int i=0; i<cfg->CANT_PAGINAS; i++) {
            tabla_frames[i].bytes = 0;
            tabla_frames[i].libre = 1;
        }
    return 1;
}