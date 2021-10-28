#include "../include/interfaz_memoria.h"

int reservar_espacio_memoria(int* a){
    uint32_t dir_espacio_libre = buscar_first_fit();
    void* size_data = malloc(a+9);
    
    printf("Se reservo %d \n",a);
    return 1;
}
int liberar_espacio_memoria(char* a,int* b){
    return 1;
}
int leer_espacio_memoria(char* a){
    return 1;
}
int escribir_espacio_memoria(){
    return 1;
}
