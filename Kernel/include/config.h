#ifndef TP_2021_2C_CONFIG_H
#define TP_2021_2C_CONFIG_H

#include <stdint.h>
#include <stdlib.h>

#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>

#include <readline/readline.h>
#include <shared/utils.h>

typedef struct {
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* ALGORITMO_PLANIFICACION;
    t_list* DISPOSITIVOS_IO;
    t_list* DURACIONES_IO;
    uint16_t RETARDO_CPU;
    uint16_t GRADO_MULTIPROGRAMACION;
    uint16_t GRADO_MULTIPROCESAMIENTO;
    uint16_t ESTIMACION_INICIAL;
    double ALFA;
} t_config_kernel;

typedef struct {
  unsigned int id;
  pthread_t thread;
  sem_t sem_pause;
  bool blocked;
  uint16_t ultima_estimacion;
  time_t tiempo_ingreso_ready;
} t_carpincho;


uint8_t cargar_configuracion(t_config_kernel*);
void cerrar_programa(t_log*, t_config_kernel*);

t_carpincho* (*obtener_carpincho)(void);
t_carpincho* obtener_carpincho_HRRN();
t_carpincho* obtener_carpincho_SJF();

#endif

