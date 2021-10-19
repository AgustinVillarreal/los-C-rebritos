#include "../include/algoritmos.h"

t_carpincho* obtener_carpincho_HRRN(){
	//TODO
}
t_carpincho* obtener_carpincho_SJF(){

	

	return (t_carpincho*) list_get_minimum();
}

void ejecutar_CPU(int numero_CPU){
	while(1){
		sem_wait(&SEM_CANTIDAD_EN_READY);
		log_info(logger, "pase una vez %d", numero_CPU);
		//Pido segun algoritmo
		t_carpincho* carpincho = obtener_carpincho();
		//TODO: Lo pongo a ejecutar aca abajo


		sem_wait(&SEM_CPUs[numero_CPU]);
	}
}