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

    svTrigOnRecd = 0;
    svVerifiedBmp = 0;
    //nvRoundTaskRcd = nvRoundTaskCnt;
    //nvRoundTaskCnt = 0;
    //uart_printf("__scheduler_run()\r\n");
    while(1){
        //NOTE: Verify & Restore
        if(nvInited){ // 1: been init.ed
            if(!tFlagPassVlid){
PRB_START(verify)
                tResult = __sc_verify(nvCurrTaskID);
PRB_END(verify)
                if(tResult == VERIFY_FAILED){
PRB_START(restoreNvm)
                    //uart_printf("__ckp_restore_nvm()\r\n");
                    __ckp_restore_nvm();
PRB_END(restoreNvm)
                    tFlagPassVlid = 1;
                }else{
PRB_START(restoreSram)
                    //uart_printf("__ckp_restore_sram()\r\n");
                    __ckp_restore_sram();
PRB_END(restoreSram)
                }
            }else{
PRB_START(restoreSram)
                //uart_printf("__ckp_restore_sram()\r\n");
                __ckp_restore_sram();
PRB_END(restoreSram)
            }
        }

        /*
        if(!nvInited || svResetFlag){
            svResetFlag = 0;
            __ckp_init_bufs();
        }
        */

        //NOTE: Execution
        //uart_printf("exe(%d)\r\n",nvCurrTaskID);
PRB_START(exec)
        nvTotalTaskCnt++;
        //nvRoundTaskCnt++;
        tTaskID = (uint8_t)((taskfun_t)(_threads[0].task_array[nvCurrTaskID].fun_entry))(_threads[0].buffer.sram_bufs[svBufIdx._idx]);
PRB_END(exec)

        //NOTE: Checksum
        if(nvInited){
PRB_START(cksum)
            __sc_checksum(nvCurrTaskID);
PRB_END(cksum)
        }else{
PRB_START(cksum)
            __sc_first_cksum();
PRB_END(cksum)
            nvInited = 1;
        }

        //NOTE: sram checkpointing
PRB_START(ckpSram)
        __ckp_commit_sram(tTaskID);
PRB_END(ckpSram)

        //NOTE: trigger nvm checkpointing
PRB_START(ckpNvm)
        //__ckp_check_cond_and_commit(tTaskID);
PRB_END(ckpNvm)

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

            svTrigOnRecd = 0;
            __bis_SR_register(GIE);  // 开启全局中断
            /*
            svResetFlag = 1;
            svRoundCnter++;
            if(svRoundCnter > SIMU_ITERATION){
                P1OUT = 0b00000110;       // send signal to show we have done!
                __delay_cycles(16000000);
                pf_uartGpioInit();
                pf_uartInit();
                uart_printf("|Round num:%d -->|APP num:%lu.\r\n", svRoundCnter-1, nvTotalexecCnt);
                uart_printf("|VerifySum:%lu(100us)\r\n",         nvTotalverifySum);
                uart_printf("|RestoreSramSum:%lu(100us)\r\n",    nvTotalrestoreSramSum);
                uart_printf("|RestoreNvmSum:%lu(100us)\r\n",     nvTotalrestoreNvmSum);
                uart_printf("|execSum:%lu(100us)\r\n",           nvTotalexecSum);
                uart_printf("|cksumSum:%lu(100us)\r\n",          nvTotalcksumSum);
                uart_printf("|ckpSramSum:%lu(100us)\r\n",        nvTotalckpSramSum);
                uart_printf("|ckpNvmSum:%lu(100us)\r\n",         nvTotalckpNvmSum);
                uart_printf("|------>>>---->>>----.\r\n");
                while(1){
                    P1OUT ^= BIT0;  // blink 1.0
                    __delay_cycles(500000);
                }
            }
            

            nvTotalexecCnt += execCnt;
            nvTotalverifySum += (uint32_t)(verifySum)/1600;
            nvTotalrestoreSramSum += (uint32_t)(restoreSramSum)/1600;
            nvTotalrestoreNvmSum += (uint32_t)(restoreNvmSum)/1600;
            nvTotalexecSum += (uint32_t)(execSum)/1600;
            nvTotalcksumSum += (uint32_t)(cksumSum)/1600;
            nvTotalckpSramSum += (uint32_t)(ckpSramSum)/1600;
            nvTotalckpNvmSum += (uint32_t)(ckpNvmSum)/1600;

            pf_varReset();
            */
        }
    }
}
