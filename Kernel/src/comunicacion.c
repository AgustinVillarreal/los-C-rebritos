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

    t_carpincho * carpincho;


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
                carpincho_init(id, &carpincho);
                if (!send_codigo_op(cliente_socket, HANDSHAKE_KERNEL)){
                   log_error(logger, "Error al enviar handshake desde kernel a matelib");
                   free(server_name);
                   return;
                }
                break;
            
            case SEM_INIT: ;
                char * sem;
                int value;
                if(!recv_sem_init(cliente_socket, &sem, &value)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                int return_code = sem_init_carpincho(sem, value);
                if (!send(cliente_socket, &return_code, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem init");
                   free(server_name);
                   return;
                }
                break;
            case SEM_WAIT: ;
                char * sem_name_wait;
                if(!recv_sem(cliente_socket, &sem_name_wait)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                int result = sem_wait_carpincho(sem_name_wait, carpincho);
                if(result == 1){
                    sem_wait(&carpincho->sem_pause);
                }
                if(!send(cliente_socket, &result, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem wait");
                   free(server_name);
                   return;
                }
                break;

            case SEM_POST: ;
                char * sem_name_post;
                if(!recv_sem(cliente_socket, &sem_name_post)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                int result = sem_post_carpincho(sem_name_post);
                /* 
                if(result == 1){
                    sem_wait(&carpincho->sem_pause);
                }
                if(!send(cliente_socket, &result, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem wait");
                   free(server_name);
                   return;
                }*/
                break;
            case SEM_DESTROY:
                break;

            case MEM_ALLOC: ;
                long id_carpincho;
                int size_data;
                if(recv_alloc_data(cliente_socket,&id_carpincho,&size_data)){
                    send_memalloc(memoria_fd);
                    send_alloc_data(memoria_fd,id_carpincho,size_data);
                } else {
                    //TODO
                }
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
