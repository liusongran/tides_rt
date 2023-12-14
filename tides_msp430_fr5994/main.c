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
    __simulator_init();

    //__delay_cycles(100);
    //UART_transmitString("test\r\n");
    /*
    uart_printf("|pf_num_P35:%lu\r\n", pf_num_P35);
    uart_printf("|pf_num_P82:%lu\r\n", pf_num_P82);
    uart_printf("|pf_num_P83:%lu\r\n", pf_num_P83);
    uart_printf("|pf_very_start:%d\r\n", pf_very_start);
    P1DIR |= 0x02;
    P1OUT |= 0x02;
    __delay_cycles(10000000);
    P1OUT &= ~(0x02);
    */

    while(!pf_very_start);

    while(1){
        if(!nvInited){
            //uart_printf("_benchmark_sort_init()\r\n");
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
        //P1OUT |= 0x02;
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
        WDTCTL = 0;
    }

    if (P8IFG & BIT3) {
        // long power-off @P8.3
        pf_num_P83++;       // counter++
        pf_flag_long = 1;
        P8IFG &= ~BIT3;     // clear flag
        WDTCTL = 0;
    }
}

