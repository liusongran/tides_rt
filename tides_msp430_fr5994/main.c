#include <main.h> 
#include "scheduler.h"

__nv bool pf_very_start = 0;
__nv bool pf_flag_long = 0;
__nv uint32_t pf_num_P35 = 0;
__nv uint32_t pf_num_P82 = 0;
__nv uint32_t pf_num_P83 = 0;

/**
 * main.c
 */
int main(void){
    __mcu_init();
    pf_timerA1Init();
    //pf_uartGpioInit();
    //pf_uartInit();
    //__simulator_init();

    //while(!pf_very_start);

    while(1){
        if(!nvInited){
            _benchmark_sort_init();
        }
        //kick-off run-time system.
        __scheduler_run();
    }
    return 0;
}

/**
 * P3.5 - very_start
 */
__attribute__((interrupt(PORT3_VECTOR)))
void Port_3(void) {
    if (P3IFG & BIT5) {
        // very start @P3.5
        pf_num_P35 ++;      // counter++
        pf_very_start = 1;  // setup flag to show benchmark program starts to run
        P1OUT |= 0x02;
        P3IFG &= ~BIT5;     // clear flag
    }
}


/**
 * P8.2-short_off; P8.3-long_off
 */
__attribute__((interrupt(PORT8_VECTOR)))
void Port_8(void) {
    if (P8IFG & BIT2) {
        // short power-off @P8.2
        pf_num_P82++;       // counter++
        P8IFG &= ~BIT2;     // clear flag
        __sc_checksum_total();
        WDTCTL = 0;
    }

    if (P8IFG & BIT3) {
        // long power-off @P8.3
        pf_num_P83++;       // counter++
        __sc_checksum_total();
        pf_flag_long = 1;
        P8IFG &= ~BIT3;     // clear flag
        WDTCTL = 0;
    }
}

