#include <main.h> 
#include "scheduler.h"

/**
 * main.c
 */
int main(void){
    __mcu_init();
    pf_timerA1Init();

    printf("teststssssttt.\r\n");
    while(1){
        if(!nvInited){
            _benchmark_sort_init();
        }

        //kick-off run-time system.
        __scheduler_run();
    }
    return 0;
}
