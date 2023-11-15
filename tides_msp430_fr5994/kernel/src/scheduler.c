/*
 * scheduler.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#include "scheduler.h"

__nv bool nvInited = 0;         // 0: not init.ed; 1: been init.ed
__nv uint16_t nvBgtRemained = 10;   //Budget remained to do a NVM ckp, replenish with a default value
__nv uint16_t nvCurrTaskID = 0;
__nv uint16_t nvCurrTaskIDShadow[2] = {0,0};

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
                tResult = __sc_verify(nvCurrTaskID);
                if(tResult == VERIFY_FAILED){
                    __ckp_restore_nvm();
                    tFlagPassVlid = 1;
                }
            }
            __ckp_restore_sram();
        }else{
            __ckp_init_bufs();
        }

        //NOTE: Execution
        tTaskID = (uint8_t)((taskfun_t)(_threads[0].task_array[nvCurrTaskID].fun_entry))(_threads[0].buffer.sram_bufs[svBufIdx._idx]);

        //NOTE: Checksum
        if(nvInited){
            __sc_checksum(nvCurrTaskID);
        }else{
            __sc_first_cksum();
            nvInited = 1;
        }

        //NOTE: sram checkpointing
        __ckp_commit_sram(tTaskID);

        //NOTE: trigger nvm checkpointing
        __ckp_check_cond_and_commit(tTaskID);
    }
}
