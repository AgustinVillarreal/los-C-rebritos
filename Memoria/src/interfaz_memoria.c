#include "../include/interfaz_memoria.h"


int reservar_espacio_mp(int* a){
    //Habria que elegir un alloc por FirstFit 
    uint32_t dir_espacio_libre = buscar_first_fit(a);

    //Reservo ese cacho de memoria 

    //Pongo un hmd al principio y uno al final creo, TODO: consultar
    void* size_data = malloc(sizeof(int) + sizeof(hmd_t)*2);
    
    printf("Se reservo %d \n",a);
    return 1;
}
int liberar_espacio_mp(char* a,int* b){
    return 1;
}
int leer_espacio_mp(char* a){
    return 1;
}
int escribir_espacio_mp(){
    return 1;
}
