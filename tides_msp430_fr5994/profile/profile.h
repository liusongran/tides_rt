/*
 * profile.h
 *
 *  Created on: Nov 15, 2023
 *      Author: liusongran
 */

#ifndef PROFILE_PROFILE_H_
#define PROFILE_PROFILE_H_

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cs.h>
#include <eusci_a_uart.h>
#include "config.h"
#include "memory_mapping.h"


extern uint16_t verifyStart;
extern uint16_t verifyEnd;
extern uint16_t verifyCnt;
extern uint64_t verifySum;

extern uint16_t restoreSramStart;
extern uint16_t restoreSramEnd;
extern uint16_t restoreSramCnt;
extern uint64_t restoreSramSum;

extern uint16_t restoreNvmStart;
extern uint16_t restoreNvmEnd;
extern uint16_t restoreNvmCnt;
extern uint64_t restoreNvmSum;

extern uint16_t execStart;
extern uint16_t execEnd;
extern uint16_t execCnt;
extern uint64_t execSum;

extern uint16_t cksumStart;
extern uint16_t cksumEnd;
extern uint16_t cksumCnt;
extern uint64_t cksumSum;

extern uint16_t ckpSramStart;
extern uint16_t ckpSramEnd;
extern uint16_t ckpSramCnt;
extern uint64_t ckpSramSum;

extern uint16_t ckpNvmStart;
extern uint16_t ckpNvmEnd;
extern uint16_t ckpNvmCnt;
extern uint64_t ckpNvmSum;

#define UART_TXD_PORT        2
#define UART_TXD_PIN         (0x0001)

#define UART_RXD_PORT        2
#define UART_RXD_PIN         (0x0002)

void pf_timerA1Init();
void pf_varReset();
void pf_uartInit();

int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);

/*
 * [timerA1Start]: 
 */
inline uint16_t timerA1Start() {
    TA1CTL |= MC_2;         // 设置计时器 A1 为连续模式以开始计数
    return TA1R;            // 返回当前的计数值
}
/*
 * [timerA1Stop]: 
 */
inline uint16_t timerA1Stop() {
    uint16_t count = TA1R;  // 获取当前计数值
    TA1CTL |= MC_0;         // 停止计时器 A1
    TA1CTL |= TACLR;        // 清除计时器 A1，计数值归零
    return count;           // 返回停止时的计数值
}


#if (PROFILE_ENABLED == P_TRUE)
#define PRB_START(var)  var##Start = timerA1Start();
#else
#define PRB_START(var)
#endif


#if (PROFILE_ENABLED == P_TRUE)
#define PRB_END(var)                        \
        var##End = timerA1Stop();           \
        var##Cnt ++ ;                       \
        var##Sum += (var##End-var##Start);
#else
#define PRB_END(var)
#endif


#endif /* PROFILE_PROFILE_H_ */
