#ifndef PROTOCOLO_H_

#define PROTOCOLO_H_



#include <inttypes.h>

#include <sys/socket.h>

#include <unistd.h>

#include <stdio.h>

#include <stdbool.h>

#include <stdint.h>

#include <string.h>

#include <stdlib.h>

#include <commons/collections/list.h>

#include <commons/log.h>




#include "utils.h"

#include "frees.h"



///



typedef enum {

    DEBUG               = 69,

} op_code;


#endif