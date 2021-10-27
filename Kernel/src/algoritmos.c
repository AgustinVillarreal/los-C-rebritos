#include "../include/algoritmos.h"

t_carpincho* obtener_carpincho_HRRN(){
	log_info(logger, "-----------------------------SE TODO HRRN-------------------------\n");
	t_carpincho* carpincho;
	pthread_mutex_lock(&MUTEX_LISTA_READY);
	list_sort(COLA_READY->elements, (void*)minimo_HRRN);
	carpincho = queue_pop(COLA_READY);
	pthread_mutex_unlock(&MUTEX_LISTA_READY);
	return carpincho;
}
t_carpincho* obtener_carpincho_SJF(){
	log_info(logger, "-----------------------------SE TODO-------------------------\n");
	t_carpincho* carpincho;
	pthread_mutex_lock(&MUTEX_LISTA_READY);
	list_sort(COLA_READY->elements, (void*)minima_estimacion);
	carpincho = queue_pop(COLA_READY);
	pthread_mutex_unlock(&MUTEX_LISTA_READY);
	return carpincho;
}

t_carpincho* minima_estimacion(t_carpincho* carpincho1, t_carpincho* carpincho2){
	return carpincho1->ultima_estimacion <= carpincho2->ultima_estimacion ? carpincho1 : carpincho2;
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
		// sleep(10);
		sem_wait(&SEM_CANTIDAD_EN_READY);
		sem_wait(&SEM_CPUs[numero_CPU]);		
		
		log_info(logger, "pase una vez %d", numero_CPU);
		//Pido segun algoritmo
		t_carpincho* carpincho = obtener_carpincho();

		carpincho->cpu_asignada = numero_CPU;
		//TODO: Lo pongo a ejecutar aca abajo
		sem_post(&carpincho->sem_pause);

	}
}