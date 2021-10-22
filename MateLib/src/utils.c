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

void data_destroy(char *uno, char *dos, t_config* cfg){
  free(uno);
  free(dos);
  config_destroy(cfg);
}

unsigned long generate_id(){
  struct timeval t;
  unsigned long id;
  gettimeofday(&t,NULL);
  id = ((t.tv_sec * 1000 * 1000) + (t.tv_usec * 1000)) << 24;
  return id;
}