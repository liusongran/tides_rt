#include <apps.h>
#include <main.h> 
#include "scheduler.h"
/**
 * main.c
 */
int main(void){
    __mcu_init();

    while(1){
        if(!nvInited){
            _benchmark_sort_init();
        }
        __scheduler_run();          //kick-off run-time system.
    }
    return 0;
}
