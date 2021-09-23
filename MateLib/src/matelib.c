#include "../include/matelib.h"

int mate_init (mate_instance *lib_ref, char *config){
	t_config* cfg = config_create(config);
	char* IP = strdup(config_get_string_value(cfg, "IP"));
	char* PUERTO = strdup(config_get_string_value(cfg, "PUERTO"));
//	lib_ref
	// TODO UUID
	config_destroy(cfg);
	return 0;
}
