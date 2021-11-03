#include "../include/manejo_memoria.h"
#define MIN(A,B) ((A)<(B)?(A):(B))


extern uint32_t memoria_disponible;
extern void* memoria_principal;
extern t_config_memoria* MEMORIA_CFG;
extern t_log* logger;


bool entra_en_mp(size_t tamanio) {
    size_t rem;
    return cant_frames_libres() >= cant_paginas(tamanio, &rem);
}

bool esta_en_tlb(unsigned long id_carpincho){
    return false;
}

uint32_t alloc_carpincho_en_mp(size_t size, unsigned long pid, t_list* tp_carp, bool* nuevapag){

    //AGUANTE TP_CARP RIVER PLATE PASION <3

    log_info(logger, "Agregando data (size %zu) a carpincho id# %lu en memoria", size, pid);
    uint32_t t_pag = MEMORIA_CFG->TAMANIO_PAGINA;

    // Data de la primera pag libre, para saber si esta por la mitad o que
    //log_info(logger, "Buscando frame libre... (size %zu)", size);
    //print_framo(true);     // debug
    //print_tppatotas(true); // debug

    uint32_t offset = 0;
    uint32_t frame_de_pag_fragmentada = primer_frame_libre_framo(pid, &offset, tp_carp);
    //log_info(logger, "Encontre el %" PRIu32, frame_de_pag_fragmentada);
    if (frame_de_pag_fragmentada == 0xFFFF) {
        //TODO: aca se lo paso a pitu
    }
    *nuevapag = offset==0; // SIGNIFICA QUE INAUGURA UNA NUEVA PAGINA

    size_t rem = 0;
    size_t size_ajustado; // sin el "cachito que sobra porque entra en la pag fragmentada"
    if (offset) {
        if (t_pag - offset > size)  size_ajustado = 0;
        else                        size_ajustado = size - (t_pag - offset);
    }
    else                            size_ajustado = size;

    uint32_t n_pags = cant_paginas(size_ajustado, &rem);  // iteraciones sin offset (sin el "cachito")
    if (offset) n_pags++;                                 // iteraciones ajustadas  (el "cachito")

    //log_info(logger, "\nVou inserir um size %zu com rem %zu e offset %" PRIu32 " fazendo %" PRIu32 " iteracoes",
    //   size, rem, offset, n_pags);

    // Itera de a una pagina y las mete en MP
    uint32_t n_iteraciones = n_pags;                    // +0.5 profes de PDP

    for (uint32_t i=0; i<n_iteraciones; i++) {
        size_t size_chunk = i==0
            ? MIN(t_pag - offset, size)                 // Primera pagina, posible offset
            : rem && (i==n_iteraciones-1)? rem : t_pag; // Otras paginas, si es la ultima es tamanio rem si hay rem

        //log_info(logger, "Chunk inserido: %zu", size_chunk);
        // buf = malloc(size_chunk);
        // if (i == 0)      memcpy(buf, data,                          size_chunk); // primera
        // else if (offset) memcpy(buf, data+t_pag-offset+(i-1)*t_pag, size_chunk); // completas intermedias o final (hubo offset)
        // else             memcpy(buf, data+i*t_pag,                  size_chunk); // completas intermedias o final (no hubo offset)

        //log_info(logger, "Antes de meter_pagina_en_mp");
        if (!meter_pagina_en_mp(size_chunk, pid, i, !!offset)) {
            // free(buf);
            return 0xFFFF;
        }
        //log_info(logger, "Despues de meter_pagina_en_mp");
        // free(buf);
    }

    return offset;
}

// La funcion que de verdad mete data en una pagina y actualiza estructuras admin. y MP
// Si se llama, es porque hay espacio en MP o en SWAP
static bool meter_pagina_en_mp(size_t size, unsigned long pid, uint32_t iter, bool offset) {
    // iter es la pagina numero i que esta metiendo en esta vuelta
    // offset indica si se empezo a cargar en una pag. que estaba por la mitad

    uint32_t inicio;
    uint32_t frame_libre = primer_frame_libre_framo(pid, &inicio);
    //log_info(logger, "Iter (%" PRIu32 "), hubo offset? (%d)", iter, offset);
    //log_info(logger, "Primer frame libre: %" PRIu32 ", inicio %" PRIu32, frame_libre, inicio);
    
    bool en_mp = true;
    if (frame_libre == 0xFFFF) {
        // LA CARGA EN SWAP DIRECTAMENTE
        //TODO: Tengo que buscar hueco victima, etc etc
        en_mp = false;
    }

    uint32_t nro_frame = frame_libre;

    //log_info(logger, "meter_pagina_en_mp: creo que paso todo. Vamos al final");

    // if (en_mp) ocupar_frame_framo(nro_frame, size, pid);                  // admin.
    // if (en_mp) memcpy_pagina_en_frame_mp(nro_frame, inicio, data, size);  // MP
    list_add_page_frame_tppatotas(pid, nro_frame, size, en_mp);           // admin.

    //print_swap(true);

    /*log_info(logger,
        "Ocupe el frame %" PRIu32 " desde el inicio %" PRIu32 " con data de size %zu\n",
        nro_frame, inicio, size
    );*/
    return true;
}
