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
                uint32_t size_data;
                bool ret_code;

                if(recv_alloc_data(cliente_socket, &id_carpincho, &size_data)){
                //En MP
                //Necestio un HeapMetaData ---> 9bytes
                //Necesito el size que piden ---> n size_data
                    //TODO: Poner un hmd al principio o al final
                    uint32_t size_stream =  sizeof(hmd_t) * 2 + size_data;    
                //Primero deberia ver si entra o no entra en la mp 
                    if(entra_en_mp(size_stream)){
                        pthread_mutex_lock(&MUTEX_MP_BUSY);
                        ret_code = reservar_espacio_mp(size_stream); 
                        pthread_mutex_unlock(&MUTEX_MP_BUSY);                                   
                    }  else {
                        /*
                        ret_code = send_probar_en_swamp(size_stream, id_carpincho);
                        */
                    }

                    if(!ret_code){
                        log_error(logger, "Error al allocar un espacio\n");
                        break;
                    }   
                } 
                    log_info(logger, "ALLOCADO PA");
                    break;
                break;
            case MEM_FREE: 
                if(!liberar_espacio_mp(dir_logic_ini, size)) {
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR LIBERAR EL ESPACIO EN MEMORIA");    
                    break;
                }
                log_info(logger,"LIBERADO PADRE");
                break;
            case MEM_READ: 
                if(!leer_espacio_mp(dir_logic_ini)){
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR LEER LA MEMORIA");
                    break;
                }
                log_info(logger,"ANDA A SABER QUE ESTAS QUERIENDO LEER");
                break;
            case MEM_WRITE:
                if(!escribir_espacio_mp(dir_logic_ini)){
                    log_info(logger,"OCURRIO UN ERROR AL INTENTAR ESCRIBIR LA MEMORIA");   
                    break;
                }
                log_info(logger,"ESCRITO JEFE");
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