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
#include "monitor_memoria.h"

typedef struct {
    char* IP;
    char* PUERTO;
    char* IP_SWAP;
    char* PUERTO_SWAP;
    uint32_t TAMANIO;
    uint32_t TAMANIO_PAGINA;
    uint32_t CANT_PAGINAS;
    char* ALGORITMO_REEMPLAZO_MMU;
    char* TIPO_ASIGNACION;
    uint16_t MARCOS_POR_PROCESO;
    uint16_t CANTIDAD_ENTRADAS_TLB;
    char* ALGORITMO_REEMPLAZO_TLB;
    uint16_t RETARDO_ACIERTO_TLB;
    uint16_t RETARDO_FALLO_TLB;
    char* PATH_DUMP_TLB;
} t_config_memoria;

t_config_memoria* MEMORIA_CFG;
t_log* logger;
uint32_t memoria_disponible;
frame_t* tabla_frames;
uint32_t global_TUR;
void* memoria_principal;
t_list* tp_carpinchos;


pthread_mutex_t MUTEX_FRAME_OCUPADO;
pthread_mutex_t MUTEX_MP_BUSY; 


uint8_t cargar_configuracion(t_config_memoria*);
void cerrar_programa(t_log*,t_config_memoria*);
uint8_t init();
void init_mutex();
uint8_t cargar_memoria(t_config_memoria*);


#endif