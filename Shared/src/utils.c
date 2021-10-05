#include "../shared/utils.h"

bool config_has_all_properties(t_config* cfg, char** properties) {
    for(uint8_t i = 0; properties[i] != NULL; i++) {
        if(!config_has_property(cfg, properties[i]))
            return false;
    }

    return true;
}

// El array tiene que estar terminado en NULL
t_list* extraer_dispositivos(char** str_dispositivos) {
    t_list* lista = list_create();
    int i = 0;

    while(str_dispositivos[i] != NULL) {
        list_add(lista, str_dispositivos[i]);
        i++;
    }
    return lista;
}

t_list* extraer_duraciones(char** str_duraciones) {
	 t_list* lista = list_create();
	 int i = 0;

	 while(str_duraciones[i] != NULL) {
	   list_add(lista, str_duraciones[i]);
	   i++;
	 }
	 return lista;
}


