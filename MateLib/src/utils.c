#include "../include/utils.h"

t_log_level log_string_enum(char* string){

  if(strcmp("LOG_LEVEL_TRACE", string) == 0){
    return LOG_LEVEL_TRACE;
  } else if(strcmp("LOG_LEVEL_DEBUG", string) == 0){
    return LOG_LEVEL_DEBUG;    
  } else if(strcmp("LOG_LEVEL_WARNING", string) == 0){
    return LOG_LEVEL_WARNING;
  } else if(strcmp("LOG_LEVEL_ERROR", string) == 0){
    return LOG_LEVEL_ERROR;
  } else {
    return LOG_LEVEL_INFO;
  }
  //Por defecto retorna LOG_LEVEL_INFO
}

void data_destroy(char *uno, char *dos, char *tres, t_config* cfg){
  free(uno);
  free(dos);
  free(tres);
  config_destroy(cfg);
}
