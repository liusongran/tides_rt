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
#include "profile.h"


static void __cs_init(){
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);      //Set DCO frequency to 16MHz
    /**
     * Configure one FRAM waitstate as required by the device datasheet for MCLK
     * operation beyond 8MHz _before_ configuring the clock system.
     */
    FRCTL0 = FRCTLPW | AUTO_1;

    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK,CS_LFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);
}

void __mcu_init(){
    WDTCTL = WDTPW | WDTHOLD;       //Stop watchdog.
    PM5CTL0 &= ~LOCKLPM5;           //Disable the GPIO power-on default high-impedance mode.

    P1DIR = 0x3F;                   //0b-0011 1111
    P1OUT = 0x00;
    __delay_cycles(10);
    P1OUT = 0b010011;               //Set P1.4, Turn both LEDs on

    __cs_init();                    //Clock system
}

#endif /* MAIN_H_ */
