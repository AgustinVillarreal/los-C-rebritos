#ifndef MATELIB_H_INCLUDED
#define MATELIB_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <commons/config.h>
#include <shared/sockets.h>
#include <shared/protocolo.h>
#include "utils.h"
#include <commons/log.h>

//-------------------Type Definitions----------------------/
typedef struct mate_instance
{
    void *group_info;
} mate_instance;

typedef struct mate_inner_structure
{
	unsigned long id;
    bool kernel_connected;
    t_log* logger;
    int servidor_fd;
    char* IP;
	char* PUERTO;
} mate_inner_structure;

typedef char *mate_io_resource;

typedef char *mate_sem_name;

typedef int32_t mate_pointer;

enum mate_errors {
    MATE_FREE_FAULT = -5,
    MATE_READ_FAULT = -6,
    MATE_WRITE_FAULT = -7
};

// TODO: Docstrings

//------------------General Functions---------------------/
int mate_init(mate_instance *lib_ref, char *config);

int mate_close(mate_instance *lib_ref);

//-----------------Semaphore Functions---------------------/
int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value);

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem);

// //--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg);

// //--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size);
int mate_memfree(mate_instance *lib_ref, mate_pointer addr);
int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size);
int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size);

#endif
