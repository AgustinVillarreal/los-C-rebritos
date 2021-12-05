#include "../include/comunicacion.h"

extern t_log* logger;
unsigned long global_id_mem = 1;
extern t_config_memoria* MEMORIA_CFG;

typedef struct {
    int fd;
    char* server_name;
    int swap_fd;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args){
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    int swap_fd = args->swap_fd;
    MEMORIA_CFG->SWAP_FD = swap_fd;
    unsigned long id_carpincho;
    
    free(args);

    // Mientras la conexion este abierta
    op_code cop;

    //Tabla del carpincho que se conecta

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
            case MATE_INIT: ;
                int value;
                if(!recv(cliente_socket, &value, sizeof(int), 0)){
                    log_info(logger, "Error iniciando semaforo");
                    return;
                }
                if(value == 0) {
                    if (!recv(cliente_socket, &id_carpincho, sizeof(long), 0)){
                        log_error(logger, "Error al iniciar el carpincho en memoria");
                        // return EXIT_FAILURE;
                        break;
                    }
                } else {
                    pthread_mutex_lock(&MUTEX_IDS);
                    id_carpincho = global_id_mem ++;
                    pthread_mutex_unlock(&MUTEX_IDS);             
                    if(!send(cliente_socket, &id_carpincho, sizeof(long), 0)){
                        log_error(logger, "Error al enviar id");
                        free(server_name);
                        return;
                    }
                }
                mate_init(id_carpincho);
                // Aca logeo la conexion del carpincho
                log_warning(logger, "\nSe conecto el carpincho: %lu \n", id_carpincho);            
                break;
            case CARPINCHO_READY: ;
                if (!recv(cliente_socket, &id_carpincho, sizeof(long), 0)){
                    log_error(logger, "Error al crear estructuras de carpincho en ready");
                    // return EXIT_FAILURE;
                    break;
                }
                ocupar_frames_carpincho(id_carpincho);
                
                break; 

            case MEM_ALLOC: ;
                size_t size_data;
                uint32_t direccionLogica;                
                if(!recv_alloc_data(cliente_socket, &id_carpincho, &size_data)){            
                    log_error(logger, "Error al enviar data para allocar");
                    // return EXIT_FAILURE;
                    break;
                }                
                log_info(logger, "Alocando size: %d del carpincho: %lu", size_data, id_carpincho);
                
                if(!allocar_carpincho(id_carpincho, size_data, &direccionLogica, swap_fd)){
                    log_info(logger, "No se pudo allocar carpincho");
                } 
                //TODO: Hacer un send de la direccionLogica, si es 0xFFFF esta mal
                send(cliente_socket, &direccionLogica, sizeof(uint32_t), 0);

                break;
            case MEM_FREE: ;
                uint32_t error;
                uint32_t direccion_logica;  
                if(!recv_memfree_data(cliente_socket, &id_carpincho, &direccion_logica)){            
                    log_error(logger, "Error al enviar data para allocar");
                    // return EXIT_FAILURE;
                    break;
                }  

                log_info(logger, "Liberando direccion logica: %d del carpincho: %lu", direccion_logica, id_carpincho);

                uint32_t estado_free = liberar_espacio_mp(id_carpincho, &direccion_logica,swap_fd); 
                send(cliente_socket,&estado_free,sizeof(uint32_t),0);      
                break;
            case MEM_READ: ; 
                int size;
                void* buff;
                if(!recv_memread_data(cliente_socket, &direccion_logica, &size)){
                    log_error(logger, "Error al recibir data para leer");
                    break;
                }
                uint8_t ret_code = read_carpincho(id_carpincho, &buff, size, direccion_logica);

                if (send(cliente_socket, &ret_code, sizeof(uint8_t), 0) == -1){
                   log_error(logger, "Error al enviar ret_code a cliente desde memread");
                   break;
                }
                
                if(ret_code){
                    if(send(cliente_socket, buff, size, 0) == -1){
                        log_error(logger, "Error al enviar buff a cliente desde memread");
                        break;
                    }
                }
                free(buff);
                break;
            case MEM_WRITE: ;
                int size_w;
                void* data;
                if(!recv_memwrite_data(cliente_socket, &direccion_logica, &data, &size_w)){
                    log_error(logger, "Error al recibir data para escribir");
                    break;
                }
                uint8_t ret_code_write = write_carpincho(id_carpincho, &data, size_w, direccion_logica);

                if(send(cliente_socket, &ret_code_write, sizeof(uint8_t), 0) == -1){
                   log_error(logger, "Error al enviar ret_code a cliente desde memwrite");
                   break;
                }

                free(data);
                break;
            //TODO: Liberar cosas aca
            case FREE_CARPINCHO:
                log_warning(logger, "\nSe desconecto el carpincho: %lu \n", id_carpincho);     
                finalizar_carpincho(id_carpincho);
                send_finalizar_carpincho(swap_fd, id_carpincho);       
                break;
            case CARPINCHO_SWAP: ;
                unsigned long id_a_swapear;
                if(!recv_id(cliente_socket, &id_a_swapear)){
                    log_error(logger, "Error swapeando el id");
                }
                suspender_carpincho(id_a_swapear);
                
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

int server_escuchar(char* server_name, int server_fd, int cliente_fd) {
    int cliente_socket = esperar_cliente(logger, server_name, server_fd);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        args->swap_fd = cliente_fd;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}