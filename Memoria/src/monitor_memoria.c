#include "../include/monitor_memoria.h"

extern uint32_t memoria_disponible;
extern void* memoria_principal;

uint32_t buscar_first_fit(int size){
    int i=0,aux=0,primer_pos_libre;
    while(memoria_principal[i] == 0 || aux != size){
        aux++;
        i++;
        if(memoria_principal[i] == 1 && aux < size){
            aux = 0;
        }
        if(aux == size){
            primer_pos_libre = &memoria_principal[i - aux + 1];
            return primer_pos_libre;
        }
    }
    
    return NULL;
}