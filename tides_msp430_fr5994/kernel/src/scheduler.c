/*
 * scheduler.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#include "scheduler.h"
#include "profile.h"

__nv bool     nvInited = 0;         // 0: not init.ed; 1: been init.ed
__nv uint16_t nvBgtRemained = 10;   //Budget remained to do a NVM ckp, replenish with a default value
__nv uint16_t nvCurrTaskID = 0;
__nv uint16_t nvCurrTaskIDShadow[2] = {0,0};

//__sv uint16_t svResetFlag = 0;
//__sv uint16_t svRoundCnter = 0;

__nv uint32_t nvTotalexecCnt = 0;
__nv uint32_t nvTotalverifySum = 0;
__nv uint32_t nvTotalrestoreSramSum = 0;
__nv uint32_t nvTotalrestoreNvmSum = 0;
__nv uint32_t nvTotalexecSum = 0;
__nv uint32_t nvTotalcksumSum = 0;
__nv uint32_t nvTotalckpSramSum = 0;
__nv uint32_t nvTotalckpNvmSum = 0;

__nv uint32_t nvTotalTaskCnt = 0;
//__nv uint16_t nvRoundTaskCnt = 0;
//__nv uint16_t nvRoundTaskRcd = 0;
extern uint16_t nvCnterNVM;

/*
 * [__scheduler_run]: 
 */
void __scheduler_run(){
    bool tFlagPassVlid = 0;         //0-Do not pass validation; 1-pass validation.
    bool tResult = 0;
    uint16_t tTaskID;

    svVerifiedBmp = 0;

    while(1){

        //NOTE: restore
        if(nvInited){
PRB_START(restoreNvm)
            __ckp_restore_INK();
PRB_END(restoreNvm)
        }

        //NOTE: Execution
PRB_START(exec)
        nvTotalTaskCnt++;
        tTaskID = (uint8_t)((taskfun_t)(_threads[0].task_array[nvCurrTaskID].fun_entry))(_threads[0].buffer.nvm_bufs[nvBufIdx._idx]);
PRB_END(exec)

        //NOTE: commit
PRB_START(ckpNvm)
        __ckp_commit_INK(tTaskID);
PRB_END(ckpNvm)
        if(!nvInited){
            nvInited = 1;
        }

        if(nvCurrTaskID==0){
            __bic_SR_register(GIE);  // 关闭全局中断

            P1OUT = 0b00000110;       // send signal to show we have done!
            __delay_cycles(16000000);
            pf_uartGpioInit();
            pf_uartInit();
            uart_printf("|--->|Total Cnt:%lu |APP num:%d.\r\n", nvTotalTaskCnt, execCnt);
            uart_printf("|--->|NVM Commit num:%d.\r\n",     nvCnterNVM);
            uart_printf("|VerifySum:%lu(100us)\r\n",        (uint32_t)(verifySum)/1600);
            uart_printf("|RestoreSramSum:%lu(100us)\r\n",   (uint32_t)(restoreSramSum)/1600);
            uart_printf("|RestoreNvmSum:%lu(100us)\r\n",    (uint32_t)(restoreNvmSum)/1600);
            uart_printf("|execSum:%lu(100us)\r\n",          (uint32_t)(execSum)/1600);
            uart_printf("|cksumSum:%lu(100us)\r\n",         (uint32_t)(cksumSum)/1600);
            uart_printf("|ckpSramSum:%lu(100us)\r\n",       (uint32_t)(ckpSramSum)/1600);
            uart_printf("|ckpNvmSum:%lu(100us)\r\n",        (uint32_t)(ckpNvmSum)/1600);
            uart_printf("|------>>>---->>>----.\r\n");
            while(1){
                P1OUT ^= BIT0;  // blink 1.0
                __delay_cycles(3200000);
            }

            __bis_SR_register(GIE);  // 开启全局中断
        }
    }
}
