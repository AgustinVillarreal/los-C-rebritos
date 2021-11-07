
#include "../include/comunicacion.h"

extern t_log* logger;
extern t_config_swamp* cfg;


#define STR(_) #_

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;


/* Procesa los mensajes que le manda la MEMORIA */

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code cop;
    while (cliente_socket != -1) {

        if (1/* recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code) */) {
            log_info(logger, STR(DISCONNECT!));
            return;
        }

        switch (cop) {
            case HANDSHAKE:
                log_info(logger, "Me llego el HANDSHAKE con MEMORIA!");
                break;

            case ESCRITURA_SWAMP:
            {
                long carpincho_id;
                uint32_t nro_pagina;
                void* data;
                bool asigancion_fija;

                if (recv_ecritura(cliente_socket, &carpincho_id, &nro_pagina, &data, &asigancion_fija)) {
                    if(asigancion_fija){
                        proceder_asignacion_fija(cliente_socket,carpincho_id,nro_pagina,data);
                    }
                    else{
                        proceder_asignacion_global(cliente_socket,carpincho_id,nro_pagina,data);
                    }
                }
                else {
                    log_error(logger, "Error ESCRITURA en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case LECTURA_SWAMP:
            {
                unsigned long carpincho_id;
                uint32_t nro_pagina;
                void* data;
                bool asignacion_fija;

                if (recv_lectura(cliente_socket, &carpincho_id, &nro_pagina)) {
                    /* Aca necesito saber el pid y el numero de pagina del carpicho para buscarlo en mi listas de frames */
                    /* Debo usar serializacion para desempaquetarlo y sacar la info que necesito */
                    buscar_frame_en_swap(carpincho_id, nro_pagina, &data, asignacion_fija);
                }
                else {
                    log_error(logger, "Error LECTURA en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case FINALIZAR_CARPINCHO:
            {
                unsigned long carpincho_id;

                if (recv_id(cliente_socket, &carpincho_id)) {
                    borrar_carpincho_swap(carpincho_id);
                }
                else {
                    log_error(logger, "Error recibiendo FINALIZAR_CARPINCHO en SWAmP");
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
