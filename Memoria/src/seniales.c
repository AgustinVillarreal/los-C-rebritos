#include "../include/seniales.h"

extern t_log* logger;
extern uint32_t cant_hits_totales;
extern uint32_t cant_miss_totales;
extern t_list* CARPINCHOS_TABLE;
extern t_config_memoria* MEMORIA_CFG;

 

void imprimir_metricas_TLB() {
    log_info(logger, "Cantidad TLB Hits totales: %d", cant_hits_totales);
    log_info(logger, "Cantidad TLB Miss totales: %d", cant_miss_totales);
    void loggear_hits(void* tp_carpincho){
        tp_carpincho_t* tp = tp_carpincho;
        log_info(logger, "Carpincho: %d - Cantidad Hits: %d", tp->id_carpincho, tp->cant_hits);
    }

    void loggear_miss(void* tp_carpincho){
        tp_carpincho_t* tp = tp_carpincho;
        log_info(logger, "Carpincho: %d - Cantidad Miss: %d", tp->id_carpincho, tp->cant_miss);
    }
    list_iterate(CARPINCHOS_TABLE, loggear_hits);
    list_iterate(CARPINCHOS_TABLE, loggear_miss);    
}

void dumpear_TLB() {
    char* timestamp = temporal_get_string_time("%d-%m-%y %H:%M:%S");

    struct stat st = {0};

    if (stat(MEMORIA_CFG->PATH_DUMP_TLB, &st) == -1) {
        mkdir(MEMORIA_CFG->PATH_DUMP_TLB, 0700);
    }
    
    char* filename = string_from_format("%s/Dump_%s.tlb", MEMORIA_CFG->PATH_DUMP_TLB ,timestamp);
    FILE* dump_file = txt_open_for_append(filename);
    char* hr = string_repeat('-', 50);
    char* str_tlb = stringify_tlb();
    char* data = NULL;
    data = string_from_format(
            "\n%s\nDump: %s\n%s\n%s\n",
            hr, timestamp, str_tlb, hr
    ); 
    txt_write_in_file(dump_file, data);
    txt_close_file(dump_file);

    goto fallecer;
    fallecer:
        free(timestamp);
        free(filename);
        free(str_tlb);
        free(hr);
        free(data);
    return;
}

void borrar_entradas_TLB(){
    void destroy_tlb_entry(tlb_t* tlb_entry){
        free(tlb_entry);
    }
    list_clean_and_destroy_elements(TLB_TABLE,(void*)destroy_tlb_entry);
}