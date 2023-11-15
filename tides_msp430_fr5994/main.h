/*
 * main.h
 *
 *  Created on: Nov 14, 2023
 *      Author: liusongran
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <msp430.h>

static void __cs_init(){
    CSCTL0_H    = CSKEY >> 8;                                   // Unlock CS registers
    CSCTL1      = DCOFSEL_0 | DCORSEL;                          // Set DCO to 1 MHz
    CSCTL2      = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;   // Set SMCLK = MCLK = DCO, ACLK = VLOCLK
    CSCTL3      = DIVA__1 | DIVS__1 | DIVM__1;                  // Set all dividers
    CSCTL0_H    = 0;                                            // Lock CS registers
}

void __mcu_init(){
    //Stop watchdog
    WDTCTL = WDTPW | WDTHOLD;
    
    //Clock system       
    __cs_init();                    

    //Zero FRAM wait states for 1 MHz operation
    FRCTL0 = FRCTLPW | NWAITS_0;
    
    //Disable the GPIO power-on default high-impedance mode.
    PM5CTL0 &= ~LOCKLPM5;

    //Set LED
    P1DIR = 0x3F;                   //0b-0011 1111
    P1OUT = 0x00;
    __delay_cycles(10);
    P1OUT = 0b010011;               //Set P1.4, Turn both LEDs on
}

#endif /* MAIN_H_ */
