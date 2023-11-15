/*
 * profile.c
 *
 *  Created on: Nov 15, 2023
 *      Author: liusongran
 */

#include "profile.h"

__nv uint16_t verifyStart       = 0;
__nv uint16_t verifyEnd         = 0;
__nv uint16_t verifyCnt         = 0;
__nv uint64_t verifySum         = 0;

__nv uint16_t restoreSramStart  = 0;
__nv uint16_t restoreSramEnd    = 0;
__nv uint16_t restoreSramCnt    = 0;
__nv uint64_t restoreSramSum    = 0;

__nv uint16_t restoreNvmStart   = 0;
__nv uint16_t restoreNvmEnd     = 0;
__nv uint16_t restoreNvmCnt     = 0;
__nv uint64_t restoreNvmSum     = 0;

__nv uint16_t execStart         = 0;
__nv uint16_t execEnd           = 0;
__nv uint16_t execCnt           = 0;
__nv uint64_t execSum           = 0;

__nv uint16_t cksumStart        = 0;
__nv uint16_t cksumEnd          = 0;
__nv uint16_t cksumCnt          = 0;
__nv uint64_t cksumSum          = 0;

__nv uint16_t ckpSramStart      = 0;
__nv uint16_t ckpSramEnd        = 0;
__nv uint16_t ckpSramCnt        = 0;
__nv uint64_t ckpSramSum        = 0;

__nv uint16_t ckpNvmStart       = 0;
__nv uint16_t ckpNvmEnd         = 0;
__nv uint16_t ckpNvmCnt         = 0;
__nv uint64_t ckpNvmSum         = 0;


void pf_timerA1Init(){
    TA1CTL = TASSEL__SMCLK + MC_2 + TACLR + ID__4;
}

void pf_uartInit() {
    // 关闭 UART 功能，准备进行配置
    UCA1CTLW0 |= UCSWRST;
    
    // 设置 SMCLK 为 UART 时钟源
    UCA1CTLW0 |= UCSSEL__SMCLK;
    
    // 设置波特率为 115200，SMCLK = 16MHz
    // 这些值来自于 MSP430 16MHz 时钟波特率配置表
    UCA1BRW = 8;                              // 16MHz 115200
    UCA1MCTLW |= UCOS16 | UCBRF_10 | 0xF700;  // 波特率调制控制设置

    // 配置 Tx 和 Rx 引脚功能选择
    P2SEL0 &= ~(UART_TXD_PIN | UART_RXD_PIN);
    P2SEL1 |= (UART_TXD_PIN | UART_RXD_PIN);

    // 启动 UART 功能
    UCA1CTLW0 &= ~UCSWRST;
}

int uart_putchar(int c) {
    while (!(UCA1IFG & UCTXIFG));              // 等待发送缓冲区准备好
    UCA1TXBUF = (unsigned char) c;             // 发送字符
    return c;
}

int _write(int fd, const void *buf, size_t count) {
    size_t i;
    for (i = 0; i < count; i++) {
        uart_putchar(((const char *)buf)[i]);
    }
    return count;
}


void pf_varReset(){
    verifySum = 0;
    verifyCnt = 0;
    restoreSramSum = 0;
    restoreSramCnt = 0;
    restoreNvmSum = 0;
    restoreNvmCnt = 0;
    execSum = 0;
    execCnt = 0;
    cksumSum = 0;
    cksumCnt = 0;
    ckpSramSum = 0;
    ckpSramCnt = 0;
    ckpNvmSum = 0;
    ckpNvmCnt = 0;
}

