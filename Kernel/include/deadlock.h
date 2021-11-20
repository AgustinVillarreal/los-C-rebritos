#ifndef TP_2021_2C_DEADLOCK_H
#define TP_2021_2C_DEADLOCK_H

#include "config.h"
#include "semaforos.h"
#include <string.h>

void listener_deadlock();
void killear_id_mas_alto(t_list* lista_bloqueados, t_list* lista_posibles_bloqueados);
void checkear_deadlock(t_list* lista_posibles_bloqueados, t_list* lista_bloqueados);
void finalizar_carpincho(t_carpincho* carpincho);

#endif

