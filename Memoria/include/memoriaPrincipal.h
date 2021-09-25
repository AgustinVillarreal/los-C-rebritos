




typedef​ ​struct​ ​​{ 
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
} HeapMetadata;

typedef​ struct t_pagina {
    uint32_t tamanio;
    uint32_t dir_inicio;
    uint32_t dir_fin;
    ​HeapMetadata​* allocs;
}

uint32_t reservarMemoria(int cantidadDeMemoria);
uint32_t firstFit(); // tiene que ser de tipo unit32_t? de que tamanio es una direccion logica?
void liberarMemoria(HeapMetadata allocALiberar);
uint32_t leerMemoria(uint32_t direccionLogica, uint32_t* tablaDePaginas); //nose si tiene que ser de tipo uint32 la funcion porque no se que devuelve
void escribirMemoria(uint32_t direccionLogica, uint32_t* tablaDePaginas);