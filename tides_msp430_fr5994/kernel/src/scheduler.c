/*
 * scheduler.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#include "scheduler.h"
#include "profile.h"

__nv bool nvInited = 0;         // 0: not init.ed; 1: been init.ed
__nv uint16_t nvBgtRemained = 10;   //Budget remained to do a NVM ckp, replenish with a default value
__nv uint16_t nvCurrTaskID = 0;
__nv uint16_t nvCurrTaskIDShadow[2] = {0,0};

__sv uint16_t svResetFlag = 0;
__sv uint16_t svRoundCnter = 0;
/*
 * [__scheduler_run]: 
 */
void __scheduler_run(){
    bool tFlagPassVlid = 0; //0-Do not pass validation; 1-pass validation.
    bool tResult = 0;
    uint16_t tTaskID;

    svTrigOnRecd = 0;
    svVerifiedBmp = 0;
    while(1){
        //NOTE: Verify & Restore
        if(nvInited){ // 1: been init.ed
            if(!tFlagPassVlid){
PRB_START(verify)
                tResult = __sc_verify(nvCurrTaskID);
PRB_END(verify)
                if(tResult == VERIFY_FAILED){
PRB_START(restoreNvm)
                    __ckp_restore_nvm();
PRB_END(restoreNvm)
                    tFlagPassVlid = 1;
                }else{
PRB_START(restoreSram)
                    __ckp_restore_sram();
PRB_END(restoreSram)
                }
            }else{
PRB_START(restoreSram)
                __ckp_restore_sram();
PRB_END(restoreSram)
            }
        }
        
        if(!nvInited || svResetFlag){
            svResetFlag = 0;
            __ckp_init_bufs();
        }

        //NOTE: Execution
PRB_START(exec)
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
        __ckp_check_cond_and_commit(tTaskID);
PRB_END(ckpNvm)

        if(nvCurrTaskID==0){
            svResetFlag = 1;
            P1OUT = 0b100011;                               //set P1.5, clear P1.4
            svRoundCnter++;

            if(svRoundCnter > SIMU_ITERATION){
                while(1);
            }
            printf("|Round num:%d -->|APP num:%d.\r\n", svRoundCnter, execCnt);
            printf("|VerifySum:%lu(100us)\r\n",         (uint32_t)(verifySum)/1600);
            printf("|RestoreSramSum:%lu(100us)\r\n",    (uint32_t)(restoreSramSum)/1600);
            printf("|RestoreNvmSum:%lu(100us)\r\n",     (uint32_t)(restoreNvmSum)/1600);
            printf("|execSum:%lu(100us)\r\n",           (uint32_t)(execSum)/1600);
            printf("|cksumSum:%lu(100us)\r\n",          (uint32_t)(cksumSum)/1600);
            printf("|ckpSramSum:%lu(100us)\r\n",        (uint32_t)(ckpSramSum)/1600);
            printf("|ckpNvmSum:%lu(100us)\r\n",         (uint32_t)(ckpNvmSum)/1600);
            printf("|------>>>---->>>----.\r\n");
            pf_varReset();
        }
    }
}
