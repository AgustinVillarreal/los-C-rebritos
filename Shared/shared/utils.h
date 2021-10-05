#ifndef UTILS_H
#define UTILS_H

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include "frees.h"

bool config_has_all_properties(t_config*, char**);
t_list* extraer_dispositivos(char**);
t_list* extraer_duraciones(char**);

#endif
