#include "../include/monitor_tablas.h"

extern t_log* logger;

uint32_t static_pid;
uint32_t static_nro_frame;

extern uint32_t cant_hits_totales;
extern uint32_t cant_miss_totales;
extern t_config_memoria* MEMORIA_CFG;
extern uint32_t global_TUR;  
extern uint32_t global_TUR_TLB;  
extern t_list* CARPINCHOS_TABLE;


void agregar_tabla_a_tp_carpinchos(void* tp_carpincho) {
    pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
    list_add(CARPINCHOS_TABLE, tp_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);
    return;
}

void quitar_paginas_a_carpinchos(unsigned long  id_carpincho, uint32_t cantidad_paginas) {
    tp_carpincho_t* carpincho = find_tp_carpincho(id_carpincho);
    for(uint32_t i = 0; i < cantidad_paginas; i++   ){
        pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
        list_remove (carpincho -> paginas,list_size(carpincho-> paginas)-1);        
        pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);
        //SACAR DE TLB Y SWAPP
    }
    return;
}


bool tabla_vacia(unsigned long id){
    
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);

    uint32_t is_empty;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    is_empty = list_is_empty(tabla_carpincho->paginas);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);
    
    return is_empty;
}

tp_carpincho_t* find_tp_carpincho(unsigned long id){
    bool es_tabla_carpincho(void* tabla){
        return ((tp_carpincho_t *) tabla)->id_carpincho == id;
    }
    
    pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
    tp_carpincho_t* tabla_carpincho =  (tp_carpincho_t*) list_find(CARPINCHOS_TABLE, es_tabla_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);

    return tabla_carpincho;
}

entrada_tp_t* list_get_pagina(tp_carpincho_t* tabla_carpincho ,uint32_t index){   
    entrada_tp_t* entrada_tp;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    entrada_tp = list_get(tabla_carpincho->paginas, index);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);    
    return entrada_tp;
}

bool buscar_en_TLB(unsigned long id_carpincho, uint32_t nro_pagina, entrada_tp_t** entrada_buscada){
    //TODO: busca en tlb 
    tlb_t* entrada_tlb = tlb_get_entrada_tlb(id_carpincho, nro_pagina);
    //Ya estaría en la tlb
    if(entrada_tlb != NULL){
        *entrada_buscada = entrada_tlb->entrada_tp;
        actualizar_hits(id_carpincho);
      
        if(MEMORIA_CFG->LRU_TLB){
            actualizarTUR(entrada_tlb);
        }
        usleep(MEMORIA_CFG->RETARDO_ACIERTO_TLB * 1000);
        return true;
    }
    actualizar_miss(id_carpincho);
    //No esta en la Tlb
    usleep(MEMORIA_CFG->RETARDO_FALLO_TLB * 1000);
    return false;
}

void actualizar_hits(unsigned long id_carpincho){
    pthread_mutex_lock(&MUTEX_TOTAL_HITS);
    cant_hits_totales++;
    pthread_mutex_unlock(&MUTEX_TOTAL_HITS);  
    tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
    pthread_mutex_lock(&tp_carpincho->mutex_hits);
    tp_carpincho->cant_hits++;
    pthread_mutex_unlock(&tp_carpincho->mutex_hits);  
}

void actualizar_miss(unsigned long id_carpincho){
    pthread_mutex_lock(&MUTEX_TOTAL_MISS);
    cant_miss_totales++;
    pthread_mutex_unlock(&MUTEX_TOTAL_MISS);     
    tp_carpincho_t* tp_carpincho = find_tp_carpincho(id_carpincho);
    pthread_mutex_lock(&tp_carpincho->mutex_miss);
    tp_carpincho->cant_miss++;
    pthread_mutex_unlock(&tp_carpincho->mutex_miss);  
}

void actualizarTUR(tlb_t* entrada_tlb){
    pthread_mutex_lock(&MUTEX_GLOBAL_TUR_TLB);
    entrada_tlb->TUR = global_TUR_TLB++;
    pthread_mutex_unlock(&MUTEX_GLOBAL_TUR_TLB);       
}

void crear_en_TLB(unsigned long id_carpincho, entrada_tp_t* entrada){
    tlb_t* entrada_tlb = malloc(sizeof(tlb_t));
    entrada_tlb->entrada_tp = entrada;
    entrada_tlb->id_carpincho = id_carpincho;
    actualizarTUR(entrada_tlb);
    if(get_tlb_size() < MEMORIA_CFG->CANTIDAD_ENTRADAS_TLB){
        list_add_tlb(entrada_tlb);
    } else {
        correr_algoritmo_tlb(entrada_tlb);
    }
}

uint32_t get_tlb_size(){
    uint32_t size;
    pthread_mutex_lock(&MUTEX_TLB_BUSY);
    size = list_size(TLB_TABLE);
    pthread_mutex_unlock(&MUTEX_TLB_BUSY);   
    return size;
}

//Tambien se hace un free de la entrada que le pasamos 
void remove_entrada_tlb(tlb_t* entrada_tlb){
    bool tlb_entrada_carpincho(void* entrada){
        return ((tlb_t *) entrada)->id_carpincho == entrada_tlb->id_carpincho && 
                ((tlb_t *) entrada)->entrada_tp->nro_pagina == entrada_tlb->entrada_tp->nro_pagina;
    }
    pthread_mutex_lock(&MUTEX_TLB_BUSY);
    list_remove_by_condition(TLB_TABLE, tlb_entrada_carpincho);
    pthread_mutex_unlock(&MUTEX_TLB_BUSY); 
    free(entrada_tlb); 
}

void list_add_tlb(tlb_t* entrada_tlb){
    pthread_mutex_lock(&MUTEX_TLB_BUSY);
    list_add(TLB_TABLE, entrada_tlb);
    pthread_mutex_unlock(&MUTEX_TLB_BUSY);   
}

tlb_t* tlb_get_entrada_tlb(unsigned long id_carpincho, uint32_t nro_pagina){
    bool tlb_entrada_carpincho(void* entrada_tlb){
        return ((tlb_t *) entrada_tlb)->id_carpincho == id_carpincho && 
                ((tlb_t *) entrada_tlb)->entrada_tp->nro_pagina == nro_pagina;
    }
    tlb_t* entrada_buscada;
    pthread_mutex_lock(&MUTEX_TLB_BUSY);
    entrada_buscada = list_find(TLB_TABLE, tlb_entrada_carpincho);
    pthread_mutex_unlock(&MUTEX_TLB_BUSY);   
    
    return entrada_buscada;
}

void mutex_init_tabla(){
    pthread_mutex_init(&MUTEX_TP_CARPINCHOS, NULL); 
    pthread_mutex_init(&MUTEX_TLB_BUSY, NULL);  
    pthread_mutex_init(&MUTEX_GLOBAL_TUR, NULL);   
    pthread_mutex_init(&MUTEX_GLOBAL_TUR_TLB, NULL); 
    pthread_mutex_init(&MUTEX_TOTAL_HITS, NULL);   
    pthread_mutex_init(&MUTEX_TOTAL_MISS, NULL);   
}

uint32_t table_size(unsigned long id){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
    uint32_t size;
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    size = list_size(tabla_carpincho->paginas);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);   
    return size;
}

void append_frame_tp(unsigned long id, uint32_t nro_pagina, uint32_t nro_frame){
    tp_carpincho_t* tabla_carpincho = find_tp_carpincho(id);
    entrada_tp_t* entrada_tp = malloc(sizeof(entrada_tp_t));
    pthread_mutex_init(&(entrada_tp->mutex_bits), NULL); 
    entrada_tp->nro_pagina = nro_pagina;
    entrada_tp->nro_frame = nro_frame;
    entrada_tp->bit_P = 1;
    //TODO: Ver mi TUR
    if(!MEMORIA_CFG->LRU_MMU){
        entrada_tp->algoritmo.clock_m = malloc(sizeof(clock_m_t));
    }
    pthread_mutex_lock(&tabla_carpincho->mutex_paginas);
    tabla_carpincho->pages++;
    list_add(tabla_carpincho->paginas, entrada_tp);
    pthread_mutex_unlock(&tabla_carpincho->mutex_paginas);
    crear_en_TLB(id, entrada_tp); 
    return;
}

void actualizar_bits(entrada_tp_t* entrada_tp, bool se_modifica){
    pthread_mutex_lock(&(entrada_tp->mutex_bits));
    if(MEMORIA_CFG->LRU_MMU){
        pthread_mutex_lock(&MUTEX_GLOBAL_TUR);      
        entrada_tp->algoritmo.TUR = global_TUR++;
        pthread_mutex_unlock(&MUTEX_GLOBAL_TUR);
    } else {
        entrada_tp->algoritmo.clock_m->bit_U = 1;
        if(se_modifica){
            entrada_tp->bit_M = 1; 
        }
    }
    pthread_mutex_unlock(&(entrada_tp->mutex_bits));
}

bool esta_presente(void* pagina){
    return ((entrada_tp_t*)pagina)->bit_P;
}

t_list* paginas_presentes_local(tp_carpincho_t* tabla_carpincho){
    t_list* frames_presentes = list_create();
    pthread_mutex_lock(&(tabla_carpincho->mutex_paginas));
    t_list_iterator* i_paginas = list_iterator_create(tabla_carpincho->paginas);
    while (list_iterator_has_next(i_paginas)) {
        entrada_tp_t* pagina = list_iterator_next(i_paginas);    
        if (pagina->bit_P==0) {
            continue;
        }
        algoritmo_t* pagina_carpincho = malloc(sizeof(algoritmo_t));
        pagina_carpincho->entrada_tp = pagina;
        pagina_carpincho->id_carpincho = tabla_carpincho->id_carpincho;
        list_add(frames_presentes, (void*) pagina_carpincho);
    }
    list_iterator_destroy(i_paginas);
    pthread_mutex_unlock(&(tabla_carpincho->mutex_paginas));

    
    return frames_presentes;
}

t_list* paginas_presentes_dinamica(){
    t_list* frames_presentes = list_create();    
    pthread_mutex_lock(&MUTEX_TP_CARPINCHOS);
    t_list_iterator* i_tp_carpincho = list_iterator_create(CARPINCHOS_TABLE);
    while (list_iterator_has_next(i_tp_carpincho)) {
        tp_carpincho_t* tabla_carpincho = list_iterator_next(i_tp_carpincho);
        pthread_mutex_lock(&(tabla_carpincho->mutex_paginas));
        t_list_iterator* i_paginas = list_iterator_create(tabla_carpincho->paginas);        
        while (list_iterator_has_next(i_paginas)) {
            entrada_tp_t* pagina = list_iterator_next(i_paginas);
            if (pagina->bit_P==0) continue;
            algoritmo_t* pagina_carpincho = malloc(sizeof(algoritmo_t));
            pagina_carpincho->entrada_tp = pagina;
            pagina_carpincho->id_carpincho = tabla_carpincho->id_carpincho;
            list_add(frames_presentes, (void*) pagina_carpincho);
        }
        list_iterator_destroy(i_paginas);
        pthread_mutex_unlock(&(tabla_carpincho->mutex_paginas));        
    }
    list_iterator_destroy(i_tp_carpincho);
    pthread_mutex_unlock(&MUTEX_TP_CARPINCHOS);
    return frames_presentes;
}


char* stringify_tlb() {
    const uint32_t size_line = 70; // sin el \0
    int i = 0;
    pthread_mutex_lock(&MUTEX_TLB_BUSY);
    uint32_t cant_entradas_tlb_actual = list_size(TLB_TABLE);
    const size_t size_str = MEMORIA_CFG->CANTIDAD_ENTRADAS_TLB * size_line + 1; // rows * size_row + \0
    char* str = malloc(size_str);
    memset(str, 0, size_str);
    
    t_list_iterator* i_tlb_entrada = list_iterator_create(TLB_TABLE);
    for ( ; list_iterator_has_next(i_tlb_entrada); i++) {
        tlb_t* entrada = list_iterator_next(i_tlb_entrada);
        char* line = entrada_tlb_a_string_de_dump(entrada, i, true);
        memcpy(str+strlen(line)*i, line, strlen(line)); // sin el \0
        free(line);
    }
    list_iterator_destroy(i_tlb_entrada);
    pthread_mutex_unlock(&MUTEX_TLB_BUSY);
    for (; i < MEMORIA_CFG->CANTIDAD_ENTRADAS_TLB ; i++) {
        char* line = entrada_tlb_a_string_de_dump(NULL, i, false);
        memcpy(str+strlen(line)*i, line, strlen(line)); // sin el \0
        free(line);
    }
    return str;
}

char* entrada_tlb_a_string_de_dump(tlb_t* entrada, int nro_entrada, bool ocupado){
    char* str;
    if(ocupado){
        str = string_from_format(
            "Entrada:%d	Estado:Ocupado	Carpincho: %lu	Pagina: %d	Marco: %d\n",
            nro_entrada, entrada->id_carpincho, entrada->entrada_tp->nro_pagina, entrada->entrada_tp->nro_frame 
        );
    } else {
        str = string_from_format(
            "Entrada:%d	Estado:Libre	Carpincho: -	Pagina: -	Marco: -\n",
            nro_entrada
        );        
    }
    return str;
}