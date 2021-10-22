#ifndef TP_2021_2C_CONFIG_H
#define TP_2021_2C_CONFIG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

#include <shared/utils.h>

#include <readline/readline.h>

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

} t_config_memoria;

t_config_memoria* MEMORIA_CFG;
t_log* logger;

uint8_t cargar_configuracion(t_config_memoria*);
void cerrar_programa(t_log*,t_config_memoria*);
int init();

#endif