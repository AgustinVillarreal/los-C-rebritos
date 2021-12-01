#include "../include/algoritmos.h"

t_carpincho* obtener_carpincho_HRRN(){
	t_carpincho* carpincho;
	pthread_mutex_lock(&MUTEX_LISTA_READY);
	list_sort(LISTA_READY, (void*)minimo_HRRN);
	carpincho = list_get(LISTA_READY, 0);
	list_remove(LISTA_READY, 0);
	// carpincho = queue_pop(LISTA_READY);
	pthread_mutex_unlock(&MUTEX_LISTA_READY);
	return carpincho;
}
t_carpincho* obtener_carpincho_SJF(){
	t_carpincho* carpincho;
	void mostrar_estimacion(t_carpincho* carpincho){
		log_info(logger, "Carpincho: %lu - Estimacion: %d", carpincho->id, carpincho->ultima_estimacion);
	}
	bool minima_estimacion(void* carpincho1, void* carpincho2){
	return ((t_carpincho*)carpincho1)->ultima_estimacion <= ((t_carpincho*)carpincho2)->ultima_estimacion;
	}
	pthread_mutex_lock(&MUTEX_LISTA_READY);
	list_sort(LISTA_READY, (void*)minima_estimacion);
	log_info(logger, "---------------------------");
	
	list_iterate(LISTA_READY, mostrar_estimacion);	
	log_info(logger, "---------------------------");

	
	carpincho = list_get(LISTA_READY, 0);
	log_info(logger, "--------sasasas-------------");
	mostrar_estimacion(carpincho);
	list_remove(LISTA_READY, 0);
	// carpincho = queue_pop(LISTA_READY);
	pthread_mutex_unlock(&MUTEX_LISTA_READY);
	return carpincho;
}



t_carpincho* minimo_HRRN(t_carpincho* carpincho1, t_carpincho* carpincho2){
	return HRRN(carpincho1) <= HRRN(carpincho2) ? carpincho1 : carpincho2;
}

double HRRN(t_carpincho* carpincho){
    time_t tiempoActual = time(NULL);
  
	double espera = difftime(carpincho->tiempo_ingreso_ready, tiempoActual);

	return (carpincho->ultima_estimacion + espera) / carpincho->ultima_estimacion;
}

void ejecutar_CPU(int numero_CPU){
	while(1){
		// TODO ver si los waits funcionan al reves, deberia ser lo correcto
		sem_wait(&SEM_CANTIDAD_EN_READY);
		sem_wait(&SEM_CPUs[numero_CPU]);		
		
		//Pido segun algoritmo
		t_carpincho* carpincho = obtener_carpincho();			

		carpincho->cpu_asignada = numero_CPU;
		carpincho->tiempo_ingreso_exec = time(NULL);
		//TODO: Lo pongo a ejecutar aca abajo
		sem_post(&carpincho->sem_pause);

	}
}