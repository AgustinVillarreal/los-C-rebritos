#include "../include/interfaz_memoria.h"


int reservar_espacio_mp(int* size_stream, unsigned long id){

    //Habria que elegir un alloc por FirstFit 
    

    //Probablemente habría que hacer validaciones antes
    // frame_t frame_libre = buscar_frame_ff();

    //Reservo ese cacho de memoria 

    //Pongo un hmd al principio y uno al final creo, TODO: consultar
    void* size_data = malloc(sizeof(int) + sizeof(hmd_t)*2);
    
    printf("Se reservo %d \n",*size_stream);
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
