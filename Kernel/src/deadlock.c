#include "../include/deadlock.h"

extern t_config_kernel* KERNEL_CFG;
extern t_log* logger;


void listener_deadlock(){
    while(1){
        usleep(KERNEL_CFG->TIEMPO_DEADLOCK * 1000);
        // TODO VER QUE ONDA
    }
}