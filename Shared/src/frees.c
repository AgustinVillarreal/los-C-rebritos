#include "../shared/frees.h"

void config_free_array_value(char*** arr) {
    char** a = *arr;
    int i = 0;

    while(a[i] != NULL) {
        free(a[i]);
        i++;
    }

    free(a);
}
