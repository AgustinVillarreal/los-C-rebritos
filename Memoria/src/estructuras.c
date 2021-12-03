#include "../include/estructuras.h"

void init_memory_structs(){
    TLB_TABLE = list_create();
    CARPINCHOS_TABLE = list_create();
    return;
}


void destroy_memory_structs(){
    list_destroy(TLB_TABLE);
    list_destroy(CARPINCHOS_TABLE);   
}

