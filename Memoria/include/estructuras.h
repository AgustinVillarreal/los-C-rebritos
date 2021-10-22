#ifndef INCLUDE_ESTRUCTURAS_H_
#define INCLUDE_ESTRUCTURAS_H_


typedef struct {
    
} tlb_t

typedef union {
    uint64_t bytes;
    struct {
        unsigned libre          :  1;
        unsigned amedias        :  1;
        unsigned inicio_hueco   : 30;
        unsigned pid_ocupador   : 32;
    };
} frame_t;

#endif
