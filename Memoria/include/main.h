#ifndef MAIN_MEMORIA_H_
#define MAIN_MEMORIA_H_

#include "conexion.h"
#include "comunicacion.h"
#include <signal.h>
#include <shared/sockets.h>
#include "config.h"
#include "seniales.h"

t_config_memoria* MEMORIA_CFG;	


void sighandler(int x);




#endif