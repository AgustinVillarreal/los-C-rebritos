#ifndef INCLUDE_INTERFAZ_MEMORIA_H_
#define INCLUDE_INTERFAZ_MEMORIA_H_

int server_escuchar(char* server_name, int server_socket);
char reservar_espacio_memoria(int*);
void liberar_espacio_memoria(char*,int*);
char leer_espacio_memoria(char*);
void escribir_espacio_memoria();

#endif
