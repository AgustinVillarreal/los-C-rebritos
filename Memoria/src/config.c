#include "../include/logs.h"
#include "../include/config.h"

void cerrar_programa(t_log* logger, t_config_kernel* cfg){
  log_destroy(logger);
  free(cfg);

  //TODO Destruir colas e hilos
  
}

uint8_t cargar_configuracion(t_config_memoria* config){
    t_config* cfg = config_create("memoria.config");

    if(cfg == NULL) {
        log_error(main_log,"NO SE ENCONTRO memoria.config");
        return 0;
    }

    char* properties[] = {
        "IP",
        "PUERTO",
        "TAMANIO",
        "ALGORITMO_REEMPLAZO_MMU",
        "TIPO_ASIGNACION",
        "MARCOS_MAXIMOS",
        "CANTIDAD_ENTRADAS_TLB",
        "ALGORITMO_REEMPLAZO_TLB",
        "RETARDO_ACIERTO_TLB",
        "RETARDO_FALLO_TLB",
        NULL
    };

    if(!config_has_all_properties(cfg,properties)){
        log_error(main_log,"NO SE ENCUENTRAN TODAS LAS PROPIEDADES DEFINIDAS EN EL CONFIG");
        config_destroy(cfg);
        return 0;
    }

    config->IP = strdup(config_get_string_value(cfg,"IP"));
    config->PUERTO = config_get_int_value(cfg,"PUERTO");
    config->TAMANIO = config_get_int_value(cfg,"TAMANIO");
    config->ALGORITMO_REEMPLAZO_MMU = strdup(config_get_string_value(cfg,"ALGORITMO_REEMPLAZO_MMU"));
    config->TIPO_ASIGNACION = strdup(config_get_string_value(cfg,"TIPO_ASIGNACION"));
    config->MARCOS_MAXIMOS = config_get_int_value(cfg,"MARCOS_MAXIMOS");
    config->CANTIDAD_ENTRADAS_TLB = config_get_int_value(cfg,"CANTIDAD_ENTRADAS_TLB");
    config->ALGORITMO_REEMPLAZO_TLB = strdup(config_get_string_value(cfg,"ALGORITMO_REEMPLAZO_TLB"));
    config->RETARDO_ACIERTO_TLB = config_get_int_value(cfg,"RETARDO_ACIERTO_TLB");
    config->RETARDO_FALLO_TLB = config_get_int_value(cfg,"RETARDO_FALLO_TLB");

    log_info(main_log,"CONFIGURACION CARGADA EXITOSAMENTE");

    config_destroy(cfg);

    return 1;
}