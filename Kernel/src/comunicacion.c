#include "../include/comunicacion.h"

extern t_log* logger;
extern t_config_kernel* KERNEL_CFG;
unsigned long global_id = 0;

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

    KERNEL_CFG->MEMORIA_FD = memoria_fd;

    t_carpincho * carpincho;
    carpincho = malloc(sizeof(t_carpincho));
    carpincho->memoria_fd = memoria_fd;
    carpincho->matelib_fd = cliente_socket;


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

            case MATE_INIT: ;
                unsigned long id;
                int value_init;
                pthread_mutex_lock(&MUTEX_IDS);
                id = global_id ++;
                pthread_mutex_unlock(&MUTEX_IDS);
                
                if(!recv(cliente_socket, &value_init, sizeof(int), 0)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }

                if(!send_mate_init(memoria_fd, 0)){
                    log_error(logger, "Error al enviar handshake desde kernel a matelib");
                    free(server_name);
                    return;
                }

                if(!send_data_mate_init(memoria_fd, id)){
                    log_error(logger, "Error al enviar id");
                    free(server_name);
                    return;
                }

                if(!send(cliente_socket, &id, sizeof(long), 0)){
                   log_error(logger, "Error al enviar id");
                   free(server_name);
                   return;
                }
                
                if (!send_codigo_op(cliente_socket, HANDSHAKE_KERNEL)){
                   log_error(logger, "Error al enviar handshake desde kernel a matelib");
                   free(server_name);
                   return;
                }

                carpincho_init(id, &carpincho);
                
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
                    log_info(logger, "Error haciendo sem_wait");
                    return;
                }
                t_semaforo* sem_wait_asignado;
                int result = sem_wait_carpincho(sem_name_wait, carpincho, &sem_wait_asignado);
                if(result == 1){
                    sem_wait(&carpincho->sem_pause);
                    result = 0;
                }
                sem_wait_asignado->carpincho_asignado = carpincho;
                if(!send(cliente_socket, &result, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem wait");
                   free(server_name);
                   return;
                }
                free(sem_name_wait);
                break;

            case SEM_POST: ;
                char * sem_name_post;
                log_info(logger, "Error haciendo sem_post %lu", carpincho->id);
                
                if(!recv_sem(cliente_socket, &sem_name_post)){
                    log_info(logger, "Error haciendo sem_post");
                    return;
                }
                int result_post = sem_post_carpincho(sem_name_post);
                if(!send(cliente_socket, &result_post, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem post");
                   free(server_name);
                   return;
                }
                free(sem_name_post);
                break;
            case SEM_DESTROY: ;
                char * sem_name_destroy;
                if(!recv_sem(cliente_socket, &sem_name_destroy)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                int result_destroy = sem_destroy_carpincho(sem_name_destroy);
                if(!send(cliente_socket, &result_destroy, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de sem wait");
                   free(server_name);
                   return;
                }
                free(sem_name_destroy);
                break;
            case IO: ;
                char* io;
                char* msg;
                if(!recv_sem(cliente_socket, &io)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                if(!recv_sem(cliente_socket, &msg)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                int bloqueo_salida_res = procesar_entrada_salida(carpincho, io, msg);
                if(bloqueo_salida_res){
                    sem_wait(&carpincho->sem_pause);                    
                }
                if(!send(cliente_socket, &bloqueo_salida_res, sizeof(int), 0)){
                   log_error(logger, "Error al enviar return code de io");
                   free(server_name);
                   return;
                }
                free(io);
                free(msg);                
                break;
            case MEM_ALLOC: ;
                long id_carpincho;
                int size_data;
                log_info(logger, "5");
                
                if(!recv_alloc_data(cliente_socket,&id_carpincho,&size_data)){
                    log_error(logger, "Error al recibir data de alloc");
                    free(server_name);
                    return;
                }

                log_info(logger, "6");
                
                send_memalloc(memoria_fd);
                send_alloc_data(memoria_fd,id_carpincho,size_data);
                uint32_t direccion_logica;
                if(!recv(memoria_fd, &direccion_logica, sizeof(uint32_t), 0)){
                    log_error(logger, "Error al recibir direccion logica");
                    free(server_name);
                    return;
                }
                log_info(logger, "7");
                
                if(!send(cliente_socket, &direccion_logica, sizeof(uint32_t), 0)){
                    log_error(logger, "Error al enviar direccion logica a Matelib");
                    free(server_name);
                    return;
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
		        sem_post(&SEM_CPUs[carpincho->cpu_asignada]);	
                push_cola_exit(carpincho);	 
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
