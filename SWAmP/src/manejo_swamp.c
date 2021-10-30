#include "../include/manejo_swamp.h"

extern t_config_swamp* cfg

uint23_t cantidad_de_espacio_swamp_libre(void* swamp){

    uint23_t cant = 0;

    for(int i = 0 ; i < cfg->TAMANIO_SWAP; i++){   
        if(swamp[i] == '\0'){
            cant++;
        }
    }
    return cant;
}


void* swamp_con_mas_espacio(){

    int cant_swamps = list_size(areas_de_swap);
    void* elegido;

    for(int i = 0 ; i < cant_swamps ; i++){
        if(i == 0){
            elegido = list_get(areas_de_swap,i);
        }
        else{
            if(cantidad_de_espacio_swamp_libre(elegido) < cantidad_de_espacio_swamp_libre(list_get(areas_de_swap,i))){
                elegido = list_get(areas_de_swap,i);
            }
        }
    }

    return elegido;
}