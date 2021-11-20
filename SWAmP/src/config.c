#include "../include/config.h"

// Varibles necesarias para loggear y leer el archivo de config
t_log* logger;
t_config_swamp* cfg;

t_list* areas_de_swap;
t_list* tablas_de_frames_swap;

/* Funciones para iniciar el modulo  */

/* Reserva memoria para la estructura de config crea el logger y el mutex necesario */

uint8_t init() {
    cfg = initialize_cfg();
    logger = log_create("swamp.log", MODULENAME, true, LOG_LEVEL_INFO);
    return 1;
}

/* Carga el archivo de configuracion -> Retorna 0 si falla y 1 si sale todo bien*/

uint8_t cargar_configuracion(char* path) {
    t_config* cfg_file = config_create(path);

    if(cfg_file == NULL) {
        log_error(logger, "No se encontro swamp.config");
        return 0;
    }

/*  
    ---  Ejemplo ---
    IP=127.0.0.1
    PUERTO=5001
    TAMANIO_SWAP=4096
    TAMANIO_PAGINA=64
    ARCHIVOS_SWAP=[/home/utnso/swap1.bin, /home/utnso/swap2.bin]
    MARCOS_POR_CARPINCHO=10
    RETARDO_SWAP=500
 */

    char* properties[] = {
        "IP",
        "PUERTO",
        "TAMANIO_SWAP",
        "TAMANIO_PAGINA",
        "ARCHIVOS_SWAP",
        "MARCOS_POR_CARPINCHO",
        "RETARDO_SWAP",
        NULL
    };

    // Falta alguna propiedad
    if(!config_has_all_properties(cfg_file, properties)) {
        log_error(logger, "Propiedades faltantes en el archivo de configuracion");
        config_destroy(cfg_file);
        return 0;
    }

    cfg->IP = string_duplicate(config_get_string_value(cfg_file, "IP"));
    cfg->PUERTO = config_get_int_value(cfg_file, "PUERTO");
    cfg->TAMANIO_SWAP = config_get_int_value(cfg_file, "TAMANIO_SWAP");
    cfg->TAMANIO_PAGINA = config_get_int_value(cfg_file, "TAMANIO_PAGINA");
    cfg->ARCHIVOS_SWAP =  string_duplicate(config_get_string_value(cfg_file, "ARCHIVOS_SWAP"));
    cfg->MARCOS_POR_CARPINCHO = config_get_int_value(cfg_file, "MARCOS_POR_CARPINCHO");
    cfg->RETARDO_SWAP = config_get_int_value(cfg_file, "RETARDO_SWAP");


    /*  -----   No se que hace  -------
        proximo_hueco = strcmp(cfg->CRITERIO_SELECCION, "FF") == 0
                    ? &proximo_hueco_first_fit
                    : &proximo_hueco_best_fit;
    */

    log_info(logger, "Archivo de configuracion cargado correctamente");

    config_destroy(cfg_file);

    return 1;
}


char** obtener_lista_de_archivos_swap(){

    char* substring = string_substring(cfg->ARCHIVOS_SWAP,1,string_length(cfg->ARCHIVOS_SWAP)-2);
    char** lista_paths = string_split(substring,",");

    free(substring);

    return lista_paths;

}

uint32_t cantidad_de_archivos_swap(){

    char** lista = obtener_lista_de_archivos_swap();

    uint32_t cantidad_archivos = 0;

    for(int i = 0 ; lista[i] != NULL ; i++){
        cantidad_archivos ++;
        free(lista[i]);
    }

    free(lista);

    return cantidad_archivos;
}



/* Crea los archivos de swap  */

bool cargar_swamp() {

    uint32_t cantidad_archivos = cantidad_de_archivos_swap();
    char** lista_paths = obtener_lista_de_archivos_swap();

    /* Chequear que este bien */
    areas_de_swap = list_create();
    tablas_de_frames_swap = list_create();

    for(int i = 0; i < cantidad_archivos ; i++){

        log_info(logger, "Creando archivos SWAP en <<%s>>", lista_paths[i]);
        int fd_swap = open(lista_paths[i], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        /* Tira error si no s epudo abrir crear el archivo swap */
        if (fd_swap == -1) {
            log_error(logger, "No se pudo crear el area de SWAP. (errno %i)", errno);
            return false;
        }

        /*  Tunca el archivo para que sea del tamanio de la swap */
        ftruncate(fd_swap, cfg->TAMANIO_SWAP);

        /*  Mapea el archivo y lo convierte en un array de bytes */
        void* area_swap = mmap(NULL, cfg->TAMANIO_SWAP, PROT_READ | PROT_WRITE, MAP_SHARED, fd_swap, 0);
        if (errno!=0) log_error(logger, "Error en mmap: errno %i", errno);

        memset(area_swap, '\0', cfg->TAMANIO_SWAP);

        list_add(areas_de_swap,area_swap);

        close(fd_swap);

        t_list* tabla_de_frames = list_create();
        list_add(tablas_de_frames_swap,tabla_de_frames);


    }

    for(int i = 0 ; i < cantidad_archivos ; i++){
        free(lista_paths[i]);
    }
    free(lista_paths);
    
    return true;
}


 void destruir_tabla(t_list* lista){
    list_destroy(lista);
}

void free_tabla_de_frames(){

    list_destroy_and_destroy_elements(tablas_de_frames_swap, (void*)destruir_tabla);

}

void cerrar_programa() {
    log_info(logger, "Finalizando programa...");

   
    log_destroy(logger);
    list_destroy(areas_de_swap);
    free(cfg->IP);
    free(cfg->ARCHIVOS_SWAP);
    free(cfg);
    free_tabla_de_frames();
    //finalizar_mutex();
}