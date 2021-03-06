#ifndef TP_2021_2C_CONFIG_H
#define TP_2021_2C_CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <shared/sockets.h>


#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

#include <shared/utils.h>
#include <readline/readline.h>

#include "estructuras.h" 
#include "monitor_tablas.h"
#include "monitor_memoria.h"


typedef struct {
    char* IP;
    char* PUERTO;
    char* IP_SWAP;
    char* PUERTO_SWAP;
    uint32_t TAMANIO;
    uint32_t TAMANIO_PAGINA;
    char* ALGORITMO_REEMPLAZO_MMU;
    char* TIPO_ASIGNACION;
    uint16_t MARCOS_POR_PROCESO;
    uint16_t CANTIDAD_ENTRADAS_TLB;
    char* ALGORITMO_REEMPLAZO_TLB;
    uint16_t RETARDO_ACIERTO_TLB;
    uint16_t RETARDO_FALLO_TLB;
    char* PATH_DUMP_TLB;
    //Agregado:
    uint32_t CANT_PAGINAS;
    bool LRU_MMU;
    bool LRU_TLB;    
    bool FIJA;
    int SWAP_FD;
} t_config_memoria;


pthread_mutex_t MUTEX_IDS;
pthread_mutex_t MUTEX_SWAP;


uint8_t cargar_configuracion(t_config_memoria*, char*);
void cerrar_programa(t_log*,t_config_memoria*);
uint8_t init();
uint8_t cargar_memoria(t_config_memoria*);
t_config_memoria* initialize_cfg();

//MMU
void (*correr_algoritmo) (unsigned long id, uint32_t* nro_frame, uint32_t nro_pagina);
void correr_algoritmo_lru (unsigned long id, uint32_t* nro_frame, uint32_t nro_pagina);
void correr_algoritmo_clock_m (unsigned long id_carpincho, uint32_t* nro_frame, uint32_t nro_pagina);



#endif