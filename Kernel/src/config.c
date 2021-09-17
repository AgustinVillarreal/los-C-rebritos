#include "../include/logs.h"
#include "../include/config.h"


uint8_t cargar_configuracion(t_config_kernel* config){

  t_config* cfg = config_create("kernel.config");

  if(cfg == NULL) {
      log_error(main_log, "No se encontro kernel.config");
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
    NULL
  };

  // Falta alguna propiedad
  if(!config_has_all_properties(cfg, properties)) {
    log_error(main_log, "Propiedades faltantes en el archivo de configuracion");
    config_destroy(cfg);
    return 0;
  }



  config->IP_MEMORIA = strdup(config_get_string_value(cfg, "IP_MEMORIA"));
  config->PUERTO_MEMORIA = config_get_int_value(cfg, "PUERTO_MEMORIA");
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

  // if(strncmp(config->ALGORITMO, "RR", 2) == 0)
  //     correr_tripulante = correr_tripulante_RR;
  // else
  //     correr_tripulante = correr_tripulante_FIFO;
  // log_info(main_log, "Archivo de configuracion cargado correctamente");
  // config_destroy(cfg);

  return 1;

}