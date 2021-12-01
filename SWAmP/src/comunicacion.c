
#include "../include/comunicacion.h"

extern t_log* logger;
extern t_config_swamp* cfg;
extern t_list* tablas_de_frames_swap;


#define STR(_) #_

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;


//BORRAR

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

  if(list_size(aux1) == 0){
    log_info(logger,"---------NADA---------");
  }

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

    if(list_size(aux2) == 0){
      log_info(logger,"---------NADA---------");
    }

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

/* Procesa los mensajes que le manda la MEMORIA */

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);
    bool asigancion_fija;

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, STR(DISCONNECT!));
            return;
        }

        switch (cop) {
            case HANDSHAKE:
                log_info(logger, "Me llego el HANDSHAKE con MEMORIA!");
                if(recv_esquema_asignacion(cliente_socket,&asigancion_fija)){
                    log_info(logger,"Se recibio el esquema de asigancion coreectamente");
                }
                else{
                    log_error(logger, "Error recibiendo ESQUEMA ASIGNACION en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            
            case ALLOCAR_EN_SWAP:
            {
                long carpincho_id;
                uint32_t cant_paginas;

                if (recv_allocar(cliente_socket, &carpincho_id, &cant_paginas)) {
                   
                    proceder_allocar(cliente_socket,carpincho_id,cant_paginas,asigancion_fija);
                    log_info(logger,"Se allocaron %d marcos para el carpincho %d correctamente",cant_paginas,carpincho_id);
                }
                else {
                    log_error(logger, "Error recibiendo ESCRITURA en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case ESCRITURA_SWAMP:
            {
                long carpincho_id;
                uint32_t nro_pagina;
                void* data;

                if (recv_ecritura(cliente_socket, &carpincho_id, &nro_pagina, &data, cfg->TAMANIO_PAGINA)) {
                   
                    proceder_escritura(cliente_socket,carpincho_id,nro_pagina,data);
                    log_info(logger,"Se escribio la pagina %d del carpincho %d en swap",nro_pagina,carpincho_id);
                }
                else {
                    log_error(logger, "Error recibiendo ESCRITURA en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case LECTURA_SWAMP:
            {
                unsigned long carpincho_id;
                uint32_t nro_pagina;
                void* data;

                if (recv_lectura(cliente_socket, &carpincho_id, &nro_pagina)) {
                    /* Aca necesito saber el pid y el numero de pagina del carpicho para buscarlo en mi listas de frames */
                    /* Debo usar serializacion para desempaquetarlo y sacar la info que necesito */
                    buscar_frame_en_swap(cliente_socket,carpincho_id, nro_pagina, &data);
                    free(data);
                    log_info(logger,"Se leyo la swap correctamente");
                }
                else {
                    free(data);
                    log_error(logger, "Error recibiendo LECTURA en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case FINALIZAR_CARPINCHO:
            {
                unsigned long carpincho_id;

                if (recv_id(cliente_socket, &carpincho_id)) {
                    borrar_carpincho_swap(cliente_socket,carpincho_id);
                    log_info(logger,"Se elimnino el carpincho %d correctamente",carpincho_id);
                }
                else {
                    log_error(logger, "Error recibiendo FINALIZAR_CARPINCHO en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }
            case ESPACIO_LIBRE:
            {
                unsigned long carpincho_id;
                uint32_t cant_paginas;

                if (recv_solicitud_espacio_libre(cliente_socket, &carpincho_id, &cant_paginas)) {
                    bool respuesta = revisar_espacio_libre(cliente_socket,carpincho_id,cant_paginas,asigancion_fija);
                    usleep(1000*cfg->RETARDO_SWAP);
                    send_ack(cliente_socket,respuesta);
                    log_info(logger,"La solicitud se realizo correctamente");
                }
                else {
                    log_error(logger, "Error recibiendo ESPACIO_LIBRE en SWAmP");
                    usleep(1000*cfg->RETARDO_SWAP);
                    send_ack(cliente_socket, false);
                }
                break;
            }
            case LIBERAR_MARCOS:
            {
                unsigned long carpincho_id;
                uint32_t cant_paginas;
                mostrar_tabla_swap(tablas_de_frames_swap);

                if (recv_solicitud_liberar_marcos(cliente_socket, &carpincho_id, &cant_paginas)) {
                    liberar_marcos(cliente_socket,carpincho_id,cant_paginas);
                    send_ack(cliente_socket,true);
                    mostrar_tabla_swap(tablas_de_frames_swap);
                    log_info(logger,"Se elimino los marcos correctamente");
                }
                else {
                    log_error(logger, "Error recibiendo LIBERAR_MARCOS en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }
            // Errores
            case -1:
                log_error(logger, "Cliente desconectado de SWAmP...");
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de SWAmp");
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

#define ARROBA @


/*  Atiende a la conexion del cliente  */

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        procesar_conexion(args);
        return 1;
    }
    return 0;
}
