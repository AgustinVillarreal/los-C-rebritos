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

  for(int i = 0 ; i < list_size(tabla) ; i++){
    frame_swap_t* frame = list_get(tabla,i);
    log_info(logger,"PID: %d",frame->pid);
    log_info(logger,"NRO PAGINA: %d",frame->nro_pagina);
    log_info(logger,"INICIO: %d",frame->inicio);
    log_info(logger,"NRO SWAP: %d",frame->nro_swap);
    log_info(logger,"LIBRE: %d",frame->libre);
    log_info(logger,"------------------------");

  }

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

    char* data1 = "1111111111111111111111111111111111111111111111111111111111111111";
    char* data2 = "2222222222222222222222222222222222222222222222222222222222222222";
    char* data3 = "3333333333333333333333333333333333333333333333333333333333333333";
    char* data4 = "4444444444444444444444444444444444444444444444444444444444444444";
    /* proceder_asignacion_fija(0,1,1,(void*)data1);
    proceder_asignacion_fija(0,2,1,(void*)data2);
    proceder_asignacion_fija(0,3,1,(void*)data3);
    proceder_asignacion_fija(0,1,2,(void*)data1);
    proceder_asignacion_fija(0,1,4,(void*)data1);
    proceder_asignacion_fija(0,1,9,(void*)data1);
    proceder_asignacion_fija(0,1,10,(void*)data1);
    proceder_asignacion_fija(0,1,7,(void*)data1);
    proceder_asignacion_fija(0,1,8,(void*)data1);
    proceder_asignacion_fija(0,1,5,(void*)data1);
    proceder_asignacion_fija(0,1,3,(void*)data1);
    proceder_asignacion_fija(0,3,2,(void*)data3);
    proceder_asignacion_fija(0,4,1,(void*)data4);
    proceder_asignacion_fija(0,3,3,(void*)data3); */
    /* proceder_asignacion_fija(0,1,6,(void*)data); */

    proceder_asignacion_global(0,2,1,(void*)data2);
    proceder_asignacion_global(0,1,1,(void*)data1);
    proceder_asignacion_global(0,3,1,(void*)data3);
    proceder_asignacion_global(0,1,2,(void*)data1);
    proceder_asignacion_global(0,2,1,(void*)data2);
/*     proceder_asignacion_global(0,1,4,(void*)data1);
    proceder_asignacion_global(0,1,9,(void*)data1);
    proceder_asignacion_global(0,1,10,(void*)data1);
    proceder_asignacion_global(0,1,7,(void*)data1);
    proceder_asignacion_global(0,1,8,(void*)data1);
    proceder_asignacion_global(0,1,5,(void*)data1); 
    proceder_asignacion_global(0,1,3,(void*)data1);
    proceder_asignacion_global(0,3,2,(void*)data3);
    proceder_asignacion_global(0,4,1,(void*)data4);
    proceder_asignacion_global(0,3,3,(void*)data3); */

    
  /*   proceder_asignacion_fija(0,2,2,(void*)data);
    proceder_asignacion_fija(0,3,1,(void*)data);
    proceder_asignacion_fija(0,4,1,(void*)data);
    proceder_asignacion_fija(0,1,4,(void*)data); */

/*     mem_hexdump(list_get(areas_de_swap,0),4096);
    mem_hexdump(list_get(areas_de_swap,1),4096);
 */
    mostrar_tabla_swap(tablas_de_frames_swap);


    liberar_conexion(&swamp_server);
    cerrar_programa();

    return EXIT_SUCCESS;
}
