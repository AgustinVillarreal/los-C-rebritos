#include "../include/logs.h"
#include "../include/config.h"

extern t_log* logger;

void cerrar_programa(t_log* logger, t_config_kernel* cfg){
  log_destroy(logger);

  free(cfg->IP_KERNEL);
  free(cfg->PUERTO_KERNEL);
  free(cfg->IP_MEMORIA);
  free(cfg->PUERTO_MEMORIA);
  free(cfg->ALGORITMO_PLANIFICACION);

  list_destroy(cfg->DISPOSITIVOS_IO);
  list_destroy(cfg->DURACIONES_IO);

  free(cfg);


  //TODO Destruir colas e hilos
  
}
 


uint8_t cargar_configuracion(t_config_kernel* config){

  t_config* cfg = config_create("kernel.config");

  if(cfg == NULL) {
      log_error(logger, "No se encontro kernel.config");
      return 0;
  }

  char* properties[] = {
    "IP_MEMORIA",
    "PUERTO_MEMORIA",
    "ALGORITMO_PLANIFICACION",
    "DISPOSITIVOS_IO",
    "DURACIONES_IO",
    "RETARDO_CPU",
    "GRADO_MULTIPROGRAMACION",
    "GRADO_MULTIPROCESAMIENTO",
    "ESTIMACION_INICIAL",
    "ALFA",
    NULL
  };

  // Falta alguna propiedad
  if(!config_has_all_properties(cfg, properties)) {
    log_error(logger, "Propiedades faltantes en el archivo de configuracion");
    config_destroy(cfg);
    return 0;
  }

  config->IP_KERNEL = strdup(config_get_string_value(cfg, "IP_KERNEL"));
  config->PUERTO_KERNEL = strdup(config_get_string_value(cfg, "PUERTO_KERNEL"));

  config->IP_MEMORIA = strdup(config_get_string_value(cfg, "IP_MEMORIA"));
  config->PUERTO_MEMORIA = strdup(config_get_string_value(cfg, "PUERTO_MEMORIA"));
  config->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(cfg, "ALGORITMO_PLANIFICACION"));

  char ** dispositivos_IO = config_get_array_value(cfg, "DISPOSITIVOS_IO");
  config->DISPOSITIVOS_IO = extraer_dispositivos(dispositivos_IO);
  config_free_array_value(&dispositivos_IO);

  char ** duraciones_IO = config_get_array_value(cfg, "DURACIONES_IO");
  config->DURACIONES_IO = extraer_duraciones(duraciones_IO);
  config_free_array_value(&duraciones_IO);

  config->RETARDO_CPU = config_get_int_value(cfg, "RETARDO_CPU");
  config->GRADO_MULTIPROGRAMACION = config_get_int_value(cfg, "GRADO_MULTIPROGRAMACION");
  config->GRADO_MULTIPROCESAMIENTO = config_get_int_value(cfg, "GRADO_MULTIPROCESAMIENTO");

  config->ESTIMACION_INICIAL = config_get_int_value(cfg, "ESTIMACION_INICIAL");
  config->ALFA = config_get_double_value(cfg, "ALFA");

  //Evaluar el Algoritmo con un if
  if(strcmp(config->ALGORITMO_PLANIFICACION, "SJF")){
    algoritmo_planificacion = ejecutar_algoritmo_SJF;
  } else {
    algoritmo_planificacion = ejecutar_algoritmo_HRRN;
  }

  config_destroy(cfg);

  return 1;
}
