#ifndef TP_2021_2C_UNDEFINED_CONFIG_H
#define TP_2021_2C_UNDEFINED_CONFIG_H

#include <stdint.h>
#include <commons/collections/list.h>
typedef struct {
    char* IP_MEMORIA;
    uint16_t PUERTO_MEMORIA;
    char* ALGORITMO_PLANIFICACION;
    t_list* DISPOSITIVOS_IO;
    t_list* DURACIONES_IO;
    uint16_t RETARDO_CPU;
    uint16_t GRADO_MULTIPROGRAMACION;
    uint16_t GRADO_MULTIPROCESAMIENTO;
} t_config_kernel;

#endif

