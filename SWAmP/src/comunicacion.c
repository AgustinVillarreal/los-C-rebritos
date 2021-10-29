
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

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, STR(DISCONNECT!));
            return;
        }

        switch (cop) {
            case HANDSHAKE:
                log_info(logger, "Me llego el HANDSHAKE con MEMORIA!");
                break;

            case GUARDAR_SWAMP:
            {
                if (/* Aca va la funcion que rcibe la operacion de guardar en SWAmP */) {
                   
                }
                else {
                    log_error(logger, "Error guardando en SWAmP");
                    send_ack(cliente_socket, false);
                }
                break;
            }

            case PEDIR_SWAMP:
            {
                if (/* Aca va la funcion para recibir el pedido de memoria */) {
                    
                }
                }
                else {
                    log_error(logger, "Error recibiendo PEDIR_SWAMP en SWAmP");
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
        procesar_conexion();
        return 1;
    }
    return 0;
}
