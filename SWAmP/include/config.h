#ifndef SWAMP_CONFIG_H_
#define SWAMP_CONFIG_H_


#include <stdint.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/list.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

// Al menos tenemos uno solo de este struct blasfemico
typedef struct {
    char* IP;
    uint32_t PUERTO;
    uint32_t TAMANIO_SWAP;
    uint32_t TAMANIO_PAGINA;
    char* ARCHIVOS_SWAP;
    uint32_t MARCOS_POR_CARPINCHO;
    uint32_t RETARDO_SWAP;
} t_config_swamp;



static t_config_mrhq* initialize_cfg() {
    t_config_mrhq* cfg = malloc(sizeof(t_config_mrhq));
    return cfg;
}

#include "../../shared/include/utils.h"
#include "../../shared/include/sockets.h"

#define MODULENAME "SWAmP"

uint8_t init();                 // inicializa loger, cfg, y semaforos
uint8_t cargar_configuracion(); // carga cfg en strut cfg
uint8_t cargar_swamp();       // Init de segmentos_libres

void cerrar_programa();
int server_escuchar(char*, int);




#endif