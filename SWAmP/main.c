#include "include/main.h"

extern t_config_swamp* cfg;
extern t_log* logger;

extern t_list* areas_de_swap;
extern t_list* tablas_de_frames_swap;

extern sem_t SEM_COMPACTACION_DONE;
extern sem_t SEM_COMPACTACION_START;

int swamp_server;

/* typedef struct {
    uint32_t pid;
    uint32_t nro_pagina;
    uint32_t inicio;
    uint32_t nro_swap;
    bool libre;
} frame_swap_t; */

//void mostrar_tabla_swap(t_list* tabla){

//   bool filtrar_x_swap0(frame_swap_t* frame){
//     return frame->nro_swap == 0;
//   }
//   bool filtrar_x_swap1(frame_swap_t* frame){
//     return frame->nro_swap == 1;
//   }
  
//   t_list* aux1 = list_filter(tabla,(void*)filtrar_x_swap0);
//   t_list* aux2 = list_filter(tabla,(void*)filtrar_x_swap1);

//   log_info(logger,"------------------------");
//   log_info(logger,"Para SWAP 0");
//   log_info(logger,"------------------------");

//   if(list_size(aux1) == 0){
//     log_info(logger,"---------NADA---------");
//   }

//   for(int i = 0 ; i < list_size(aux1) ; i++){
//     frame_swap_t* frame = list_get(aux1,i);
//     log_info(logger,"PID: %d",frame->pid);
//     log_info(logger,"NRO PAGINA: %d",frame->nro_pagina);
//     log_info(logger,"INICIO: %d",frame->inicio);
//     log_info(logger,"NRO SWAP: %d",frame->nro_swap);
//     log_info(logger,"------------------------");
//   }
//     log_info(logger,"------------------------");
//     log_info(logger,"Para SWAP 1");
//     log_info(logger,"------------------------");

//     if(list_size(aux2) == 0){
//       log_info(logger,"---------NADA---------");
//     }

//     for(int i = 0 ; i < list_size(aux2) ; i++){
//     frame_swap_t* frame = list_get(aux2,i);
//     log_info(logger,"PID: %d",frame->pid);
//     log_info(logger,"NRO PAGINA: %d",frame->nro_pagina);
//     log_info(logger,"INICIO: %d",frame->inicio);
//     log_info(logger,"NRO SWAP: %d",frame->nro_swap);
//     log_info(logger,"------------------------");
//   }

//   list_destroy(aux1);
//   list_destroy(aux2);

// }

int main(int argc, char *argv[]){

	
	if(!init() || !cargar_configuracion(argv[1]) || !cargar_swamp()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    /* ****** CREACION DEL SERVIDOR ****** */
    char* puerto = string_itoa(cfg->PUERTO);
    swamp_server = iniciar_servidor(logger, SERVERNAME, "0.0.0.0", puerto);
    free(puerto);

  server_escuchar(SERVERNAME, swamp_server);
  //while(server_escuchar(SERVERNAME, swamp_server));

  // char* data1 = "11111111111111111111111111111111111111111111111111\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  // char* data2 = "2222222222222222222222222222222222222222222222222222222222222222";
  // char* data3 = "3333333333333333333333333333333333333333333333\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  // char* data4 = "4444444444444444444444444444444444444444444444444\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  // char* dataX = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0\0\0\0";
  // char* dataY = "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY";
 
  // /* Pruebo allocar memoria en swap */

  // bool flag = true;
  
  // log_info(logger,"Alloco 3 paginas del carpincho 1");
  // proceder_allocar(0,1,3,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 1 paginas del carpincho 4");
  // proceder_allocar(0,4,1,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 4 paginas del carpincho 2");
  // proceder_allocar(0,2,4,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 2 paginas del carpincho 3");
  // proceder_allocar(0,3,2,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 2 paginas del carpincho 1");
  // proceder_allocar(0,1,2,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 1 paginas del carpincho 1");
  // proceder_allocar(0,1,2,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 2 paginas del carpincho 3");
  // proceder_allocar(0,3,2,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // log_info(logger,"Alloco 1 paginas del carpincho 3");
  // proceder_allocar(0,3,1,flag);
  // mostrar_tabla_swap(tablas_de_frames_swap);

  // mem_hexdump(list_get(areas_de_swap,0),512);
  // mem_hexdump(list_get(areas_de_swap,1),512);

  // mostrar_tabla_swap(tablas_de_frames_swap);

  // getchar();
  // log_info(logger,"------------------------");
  // /* Prueba escritura  */

  //   log_info(logger,"Escribo marco 0 de Carpincho 4");
  //   proceder_escritura(0,4,0,(void*)data4);
  //   log_info(logger,"Escribo marco 0 de Carpincho 1");
  //   proceder_escritura(0,1,0,(void*)data1);
  //   log_info(logger,"Escribo marco 1 de Carpincho 1");
  //   proceder_escritura(0,1,1,(void*)data1);
  //   log_info(logger,"Escribo marco 2 de Carpincho 1");
  //   proceder_escritura(0,1,2,(void*)data1);
  //   log_info(logger,"Escribo marco 3 de Carpincho 1");
  //   proceder_escritura(0,1,3,(void*)data1);
  //   log_info(logger,"Escribo marco 4 de Carpincho 1");
  //   proceder_escritura(0,1,4,(void*)data1);
  //   log_info(logger,"Escribo marco 3 de Carpincho 2");
  //   proceder_escritura(0,2,3,(void*)data2);
  //   log_info(logger,"Escribo marco 0 de Carpincho 3");
  //   proceder_escritura(0,3,0,(void*)data3);
  //   log_info(logger,"Escribo marco 1 de Carpincho 3");
  //   proceder_escritura(0,3,1,(void*)data3);
  //   log_info(logger,"Escribo marco 1 de Carpincho 4");
  //   proceder_escritura(0,4,1,(void*)data4);
  //   log_info(logger,"Escribo marco 3 de Carpincho 2");
  //   proceder_escritura(0,2,3,(void*)dataX);
   

  //   mem_hexdump(list_get(areas_de_swap,0),512);
  //   mem_hexdump(list_get(areas_de_swap,1),512);

  //   mostrar_tabla_swap(tablas_de_frames_swap);

  //   getchar();
  //   log_info(logger,"------------------------");

  // /* Ahora elimino unas paginas del carpincho 1 */

  //   liberar_marcos(0,3,4);
  //   liberar_marcos(0,1,4);

  //   mem_hexdump(list_get(areas_de_swap,0),512);
  //   mem_hexdump(list_get(areas_de_swap,1),512);

  //   mostrar_tabla_swap(tablas_de_frames_swap);

  //   getchar();
  //   log_info(logger,"------------------------");
  // /* Ahora pruebo eliminar dos carpinchos */

  //   log_info(logger,"Elimino al carpincho 4");
  //   eliminar_carpincho_de_memoria(4);

  //   log_info(logger,"Elimino al carpincho 2");
  //   eliminar_carpincho_de_memoria(2);

  //   log_info(logger,"Elimino al carpincho 5");
  //   eliminar_carpincho_de_memoria(5);

  //   log_info(logger,"Mando marco de Carpincho 5");
  //   proceder_escritura(0,5,1,(void*)dataY);
  //   log_info(logger,"Mando marco de Carpincho 5");
  //   proceder_escritura(0,5,2,(void*)dataY);
  //   log_info(logger,"Mando marco de Carpincho 5");
  //   proceder_escritura(0,5,3,(void*)dataY);
  //   log_info(logger,"Mando marco de Carpincho 2");
  //   proceder_escritura(0,2,30,(void*)data2);


  //   mem_hexdump(list_get(areas_de_swap,0),512);
  //   mem_hexdump(list_get(areas_de_swap,1),512);

  //   mostrar_tabla_swap(tablas_de_frames_swap);

  //   getchar();
  //   log_info(logger,"------------------------");

  //   /* Ahora pruebo leer unos marcos */

  //   void* marco1 = tomar_frame_swap(3, 2);
  //   void* marco2 = tomar_frame_swap(1, 10);

  //   log_info(logger,"-------------------------------");
    
  //   if(marco1 != NULL) mem_hexdump(marco1,64);
  //   if(marco2 != NULL) mem_hexdump(marco2,64);
    
  //   log_info(logger,"-------------------------------");

  //   free(marco1);
  //   free(marco2);

  //   getchar();
  //   log_info(logger,"------------------------");

  //   /* Ahora pregunto si hay espacio disponible en swap */

  //   if(revisar_espacio_libre(0,1,3,false)){
  //     log_info(logger,"Hay espacio para allocar 3 paginas para el carpincho 1 (asigancion global)");
  //   }
  //   if(revisar_espacio_libre(0,1,3,true)){
  //     log_info(logger,"No hay espacio para allocar 3 paginas para el carpincho 1 (asigancion fija)");
  //   }
  //   if(revisar_espacio_libre(0,5,3,true)){
  //     log_info(logger,"Hay espacio para allocar 3 paginas para el carpincho 5 (asigancion fija)");
  //   }
  //   if(revisar_espacio_libre(0,5,3,false)){
  //     log_info(logger,"Hay espacio para allocar 3 paginas para el carpincho 5 (asigancion global)");
  //   }

  //   getchar();
  //   log_info(logger,"------------------------");

  //   /* Mato toda la los carpinchos */

  //   log_info(logger,"Elimino al carpincho 1");
  //   eliminar_carpincho_de_memoria(1);

  //   log_info(logger,"Elimino al carpincho 2");
  //   eliminar_carpincho_de_memoria(2);

  //   log_info(logger,"Elimino al carpincho 3");
  //   eliminar_carpincho_de_memoria(3);

  //   log_info(logger,"Elimino al carpincho 5");
  //   eliminar_carpincho_de_memoria(5);

  //   mem_hexdump(list_get(areas_de_swap,0),512);
  //   mem_hexdump(list_get(areas_de_swap,1),512);

  //   mostrar_tabla_swap(tablas_de_frames_swap);

    liberar_conexion(&swamp_server);
    cerrar_programa();

    return EXIT_SUCCESS;
}
