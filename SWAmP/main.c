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

void mostrar_tabla_swap(t_list* tabla){

  bool filtrar_x_swap0(frame_swap_t* frame){
    return frame->nro_swap == 0;
  }
  bool filtrar_x_swap1(frame_swap_t* frame){
    return frame->nro_swap == 1;
  }
  
  t_list* aux1 = list_filter(tabla,(void*)filtrar_x_swap0);
  t_list* aux2 = list_filter(tabla,(void*)filtrar_x_swap1);

  log_info(logger,"------------------------");
  log_info(logger,"Para SWAP 0");
  log_info(logger,"------------------------");

  for(int i = 0 ; i < list_size(aux1) ; i++){
    frame_swap_t* frame = list_get(aux1,i);
    log_info(logger,"PID: %d",frame->pid);
    log_info(logger,"NRO PAGINA: %d",frame->nro_pagina);
    log_info(logger,"INICIO: %d",frame->inicio);
    log_info(logger,"NRO SWAP: %d",frame->nro_swap);
    log_info(logger,"------------------------");
  }
    log_info(logger,"------------------------");
    log_info(logger,"Para SWAP 1");
    log_info(logger,"------------------------");

    for(int i = 0 ; i < list_size(aux2) ; i++){
    frame_swap_t* frame = list_get(aux2,i);
    log_info(logger,"PID: %d",frame->pid);
    log_info(logger,"NRO PAGINA: %d",frame->nro_pagina);
    log_info(logger,"INICIO: %d",frame->inicio);
    log_info(logger,"NRO SWAP: %d",frame->nro_swap);
    log_info(logger,"------------------------");
  }

  list_destroy(aux1);
  list_destroy(aux2);

}

int main(){

	
	if(!init() || !cargar_configuracion("swamp.config") || !cargar_swamp()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }

    /* ****** CREACION DEL SERVIDOR ****** */
    char* puerto = string_itoa(cfg->PUERTO);
    swamp_server = iniciar_servidor(logger, SERVERNAME, "0.0.0.0", puerto);
    free(puerto);


  	//while(server_escuchar(SERVERNAME, swamp_server));

    char* data1 = "11111111111111111111111111111111111111111111111111\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    char* data2 = "2222222222222222222222222222222222222222222222222222222222222222";
    char* data3 = "3333333333333333333333333333333333333333333333\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    char* data4 = "4444444444444444444444444444444444444444444444444\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    char* dataX = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0\0\0\0";
    char* dataY = "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY";
 
/* Prueba asignacion global  */

    bool flag = false;

    log_info(logger,"Mando marco de Carpincho 2");
    proceder_asignacion(0,2,1,(void*)data2,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,1,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 3");
    proceder_asignacion(0,3,1,(void*)data3,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,2,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 2");
    proceder_asignacion(0,2,1,(void*)data2,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,4,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,9,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,10,(void*)dataY,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,7,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,8,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,5,(void*)data1,flag); 
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,3,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 3");
    proceder_asignacion(0,3,2,(void*)dataX,flag);
    log_info(logger,"Mando marco de Carpincho 4");
    proceder_asignacion(0,4,1,(void*)data4,flag);
    log_info(logger,"Mando marco de Carpincho 3");
    proceder_asignacion(0,3,3,(void*)data3,flag);
    log_info(logger,"Mando marco de Carpincho 3");
    proceder_asignacion(0,3,15,(void*)data3,flag);
    log_info(logger,"Mando marco de Carpincho 3");
    proceder_asignacion(0,3,24,(void*)data3,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,13,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,14,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,15,(void*)data1,flag);
    log_info(logger,"Mando marco de Carpincho 1");
    proceder_asignacion(0,1,20,(void*)data1,flag);
   

    mem_hexdump(list_get(areas_de_swap,0),512);
    mem_hexdump(list_get(areas_de_swap,1),512);

    mostrar_tabla_swap(tablas_de_frames_swap);

  /* Ahora pruebo eliminar dos carpinchos */

    log_info(logger,"Elimino al carpincho 4");
    eliminar_carpincho_de_memoria(4);

    log_info(logger,"Elimino al carpincho 2");
    eliminar_carpincho_de_memoria(2);

    log_info(logger,"Mando marco de Carpincho 5");
    proceder_asignacion(0,5,1,(void*)dataY,flag);
    log_info(logger,"Mando marco de Carpincho 5");
    proceder_asignacion(0,5,2,(void*)dataY,flag);
    log_info(logger,"Mando marco de Carpincho 5");
    proceder_asignacion(0,5,3,(void*)dataY,flag);
    log_info(logger,"Mando marco de Carpincho 2");
    proceder_asignacion(0,2,30,(void*)data2,flag);



    mem_hexdump(list_get(areas_de_swap,0),512);
    mem_hexdump(list_get(areas_de_swap,1),512);

    mostrar_tabla_swap(tablas_de_frames_swap);

    /* Ahora pruebo leer unos marcos */

    void* marco1 = tomar_frame_swap(3, 2);
    void* marco2 = tomar_frame_swap(1, 10);

    log_info(logger,"-------------------------------");
    
    mem_hexdump(marco1,64);
    mem_hexdump(marco2,64);
    
    log_info(logger,"-------------------------------");

    free(marco1);
    free(marco2);

    /* Ahora pregunto si hay espacio disponible en swap */

    if(revisar_espacio_libre(1,3,false)){
      log_info(logger,"Hay espacio para a locar el carpincho 1 (asigancion global)");
    }
    if(revisar_espacio_libre(1,3,true)){
      log_info(logger,"No hay espacio para a locar el carpincho 1 (asigancion fija)");
    }
    if(revisar_espacio_libre(6,3,true)){
      log_info(logger,"Hay espacio para a locar el carpincho 5 (asigancion fija)");
    }
    if(revisar_espacio_libre(6,3,false)){
      log_info(logger,"Hay espacio para a locar el carpincho 5 (asigancion global)");
    }

    /* Mato toda la los carpinchos */

    log_info(logger,"Elimino al carpincho 1");
    eliminar_carpincho_de_memoria(1);

    log_info(logger,"Elimino al carpincho 2");
    eliminar_carpincho_de_memoria(2);

    log_info(logger,"Elimino al carpincho 3");
    eliminar_carpincho_de_memoria(3);

    log_info(logger,"Elimino al carpincho 5");
    eliminar_carpincho_de_memoria(5);

    mem_hexdump(list_get(areas_de_swap,0),512);
    mem_hexdump(list_get(areas_de_swap,1),512);

    mostrar_tabla_swap(tablas_de_frames_swap);

    liberar_conexion(&swamp_server);
    cerrar_programa();

    return EXIT_SUCCESS;
}
