#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <stdlib.h>

t_log_level log_string_enum(char* string);
void data_destroy(char *uno, char *dos, char *tres, t_config* cfg);

#endif
