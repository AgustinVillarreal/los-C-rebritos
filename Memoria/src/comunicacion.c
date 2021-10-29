#include "../include/comunicacion.h"

extern t_log* logger;

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args){
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);

    // Mientras la conexion este abierta
    op_code cop;

    // TODO VER DONDE VA ESTO
    char* dir_logic_ini;
    int* size;

    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) == 0) {
        	log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case HANDSHAKE:
               if (!send_codigo_op(cliente_socket, HANDSHAKE_MEMORIA)){
                   log_error(logger, "Error al enviar handshake desde memoria a swap");
                   //return EXIT_FAILURE;
                   break;
               }
               log_info(logger, "HANDSHAKE");
               break;
            case MEM_ALLOC: ;
                long id_carpincho;
                int size_data;
                if(recv_alloc_data(cliente_socket,&id_carpincho,&size_data)){

                    int size_stream =  9 + size_data;     
                    if(!reservar_espacio_memoria(size_stream)){
                        log_info(logger, "OCURRIO UN ERROR AL INTENTAR RESERVAR UN ESPACIO EN MEMORIA");
                        break;
                    }

                    log_info(logger, "ALLOCADO PA");
                    break;
                } 
                break;
            case MEM_FREE: 
                if(!liberar_espacio_memoria(dir_logic_ini, size)) {
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR LIBERAR EL ESPACIO EN MEMORIA");    
                    break;
                }
                log_info(logger,"LIBERADO PADRE");
                break;
            case MEM_READ: 
                if(!leer_espacio_memoria(dir_logic_ini)){
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR LEER LA MEMORIA");
                    break;
                }
                log_info(logger,"ANDA A SABER QUE ESTAS QUERIENDO LEER");
                break;
            case MEM_WRITE:
                if(!escribir_espacio_memoria(dir_logic_ini)){
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR ESCRIBIR LA MEMORIA");   
                    break;
                }
                log_info(logger,"ESCRITO JEFE");
                break;
            case CARPINCHO_SWAP:
                log_info(logger, "SWAPEADO PAAAA");  
                break;              
            case -1:
                log_info(logger, "Cliente desconectado de Memoria");
                free(server_name);
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de Memoria");
                free(server_name);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(char* server_name, int server_fd) {
    int cliente_socket = esperar_cliente(logger, server_name, server_fd);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}