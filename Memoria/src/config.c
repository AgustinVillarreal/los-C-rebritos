#include "../include/logs.h"
#include "../include/config.h"

extern t_log* logger;

void cerrar_programa(t_log* logger, t_config_memoria* cfg){
  log_destroy(logger);
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

    log_info(logger,"CONFIGURACION CARGADA EXITOSAMENTE");

    config_destroy(cfg);

    return 1;
}