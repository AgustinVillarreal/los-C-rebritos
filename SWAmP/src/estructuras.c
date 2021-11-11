#include "../include/estructuras.h"


void destroy_frame(frame_swap_t* frame){
    free(frame);
}