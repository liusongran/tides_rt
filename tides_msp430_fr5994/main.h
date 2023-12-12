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
    //FRCTL0 = FRCTLPW | AUTO_1;
    FRCTL0 = FRCTLPW | AUTO_0 | NWAITS_13;
    //FRCTL0 = FRCTLPW | AUTO_0;

    CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK,CS_LFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);
}

void __mcu_init(){
    WDTCTL = WDTPW | WDTHOLD;       //Stop watchdog.
    PM5CTL0 &= ~LOCKLPM5;           //Disable the GPIO power-on default high-impedance mode.

    P1DIR = 0x07;                   //0b-0000 0111
    P1OUT = 0x01;                   //Turn RED LED on

    __cs_init();                    //Clock system
}

void __simulator_init(){
    // 初始化P8.2 和 P8.3为输入
    P8DIR &= ~(BIT2 | BIT3);
    P8REN |= BIT2 | BIT3;        // 启用内部上拉/下拉电阻
    P8OUT |= BIT2 | BIT3;        // 选择上拉电阻
    P8IES &= ~(BIT2 | BIT3);
    P8IFG &= ~(BIT2 | BIT3);     // 清除中断标志
    P8IE |= BIT2 | BIT3;         // 为P1.4和P1.5启用中断

    P3DIR &= ~(BIT5);
    P3REN |= BIT5;        // 启用内部上拉/下拉电阻
    P3OUT |= BIT5;        // 选择上拉电阻
    P3IES &= ~(BIT5);
    P3IFG &= ~(BIT5);     // 清除中断标志
    P3IE |= BIT5;         // 为P1.4和P1.5启用中断

    __bis_SR_register(GIE);             // 开启全局中断
}

#endif /* MAIN_H_ */
