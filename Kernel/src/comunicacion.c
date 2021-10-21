#include "../include/comunicacion.h"

extern t_log* logger;

typedef struct {
    int fd;
    char* server_name;
    int memoria_fd;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args){
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    int memoria_fd = args->memoria_fd;
    free(args);

    // Mientras la conexion este abierta
    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) == 0) {
        	log_info(logger, "DISCONNECT!");
            return;
        }

        switch (cop) {
            case HANDSHAKE:
               if (!send_codigo_op(cliente_socket, HANDSHAKE_KERNEL)){
                   log_error(logger, "Error al enviar handshake desde kernel a matelib");
                   free(server_name);
                   return;
               }
               log_info(logger, "HANDSHAKE");
               break;

            case PONER_COLA_NEW: ;
                unsigned long id;
                if(!recv(cliente_socket, &id, sizeof(long), 0)){
                    log_info(logger, "Error recibiendo msj de encolamiento new");
                    return;
                }
                carpincho_init(id);
                if (!send_codigo_op(cliente_socket, HANDSHAKE_KERNEL)){
                   log_error(logger, "Error al enviar handshake desde kernel a matelib");
                   free(server_name);
                   return;
                }
                printf("--------asdasd- %d------", largo_cola_new());               
                break;

            case MEM_ALLOC: ;
                void * stream = malloc(sizeof(int) * 2);
                //TODO: Abstraccion                
                if(!recv(cliente_socket, stream, sizeof(int)*2, 0)){
                    log_info(logger, "Error en mem_alloc");
                    return;
                }
                send_memalloc(memoria_fd);  
                free(stream);                                              
                break;

            case MEM_FREE:
                send_memfree(memoria_fd);
                break;
            case MEM_READ:
                send_memread(memoria_fd);
                break;
            case MEM_WRITE:
                send_memwrite(memoria_fd);
                break;

            //TODO ver donde se libera
            case FREE_CARPINCHO:
                break;
            case -1:
                log_info(logger, "Cliente desconectado de Kernel");
                free(server_name);
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de Kernel");
                free(server_name);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    free(server_name);
    return;
}

int server_escuchar(char* server_name, int server_fd, int memoria_fd) {
    int cliente_socket = esperar_cliente(logger, server_name, server_fd);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        args->memoria_fd = memoria_fd;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}
