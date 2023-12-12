/*
 * profile.c
 *
 *  Created on: Nov 15, 2023
 *      Author: liusongran
 */

#include <stdarg.h>
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


int fputc(int _c, register FILE *_fp){
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, (unsigned char) _c );
    return((unsigned char)_c);
}




int fputs(const char *_ptr, register FILE *_fp){
    unsigned int i, len;

    len = strlen(_ptr);

    for(i=0 ; i<len ; i++){
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, (unsigned char) _ptr[i]);
    }

    return len;
}


void pf_timerA1Init(){
    TA1CTL = TASSEL__SMCLK + MC_2 + TACLR + ID__4;
    TA1CCTL0 &= ~CCIE;
    TA1CCTL1 &= ~CCIE;
    TA1CCTL2 &= ~CCIE;
    TA1CTL &= ~TAIE;
}


/* Initializes Backchannel UART GPIO */
void pf_uartGpioInit()
{
    // Configure P2.0 - UCA0TXD and P2.1 - UCA0RXD
    GPIO_setOutputLowOnPin(UART_TXD_PORT, UART_TXD_PIN);
    GPIO_setAsOutputPin(UART_TXD_PORT, UART_TXD_PIN);
    /* Selecting UART functions for TXD and RXD */
    GPIO_setAsPeripheralModuleFunctionInputPin(
            UART_TXD_PORT,
            UART_TXD_PIN,
			UART_SELECT_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionInputPin(
            UART_RXD_PORT,
            UART_RXD_PIN,
			UART_SELECT_FUNCTION);
}

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
EUSCI_A_UART_initParam uartParam =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
		8,                                              // BRDIV = 4
        10,                                              // UCxBRF = 5
        247,                                             // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                         // No Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION   // Oversampling
};

void pf_uartInit() {
    /* Configuring UART Module */
    EUSCI_A_UART_init(EUSCI_A0_BASE, &uartParam);

    EUSCI_A_UART_disableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT);
    /* Enable UART module */
    EUSCI_A_UART_enable(EUSCI_A0_BASE);
}

/* Transmits String over UART */
void UART_transmitString( char *pStr )
{
	while( *pStr )
	{
		EUSCI_A_UART_transmitData(EUSCI_A0_BASE, *pStr );
		pStr++;
	}
}

int _write(int file, char *ptr, int len) {
    size_t i;

    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (i = 0; i < len; i++) {
            EUSCI_A_UART_transmitData(EUSCI_A0_BASE, ptr[i]);
        }
        return len;
    }
    return -1;
}


void uart_printf(const char *format, ...) {
    char buffer[512];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, 512, format, args);
    va_end(args);

    UART_transmitString(buffer);
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

