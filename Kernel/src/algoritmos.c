#include "../include/algoritmos.h"

t_carpincho* obtener_carpincho_HRRN(){
	//TODO
}
t_carpincho* obtener_carpincho_SJF(){

	t_carpincho* carpincho;
	sem_wait(COLA_READY);
	carpincho = list_get_minimum(COLA_READY, (void*)minima_estimacion);
	sem_post(COLA_READY);
	return carpincho;
}

t_carpincho* minima_estimacion(t_carpincho* carpincho1, t_carpincho* carpincho2){
	return carpincho1->ultima_estimacion <= carpincho2->ultima_estimacion ? carpincho1 : carpincho2;
}

void ejecutar_CPU(int numero_CPU){
	while(1){
		sem_wait(&SEM_CANTIDAD_EN_READY);
		log_info(logger, "pase una vez %d", numero_CPU);
		//Pido segun algoritmo
		t_carpincho* carpincho = obtener_carpincho;
		//TODO: Lo pongo a ejecutar aca abajo


		sem_wait(&SEM_CPUs[numero_CPU]);
	}
}