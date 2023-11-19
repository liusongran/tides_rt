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
        //NOTE: restore
        if(nvInited){
PRB_START(restoreNvm)
            __ckp_restore_INK();
PRB_END(restoreNvm)
        }

        if(!nvInited || svResetFlag){
            svResetFlag = 0;
            __ckp_init_bufs();
        }

        //NOTE: Execution
PRB_START(exec)
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
