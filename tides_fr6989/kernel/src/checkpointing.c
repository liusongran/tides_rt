/*
 * checkpointing.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */
#include "checkpointing.h"
#include "sram_checker.h"
#include "task.h"

__nv buffer_idx_t   nvBufIdx = {0,1}; 
__sv buffer_idx_t   svBufIdx = {0,1};
__nv uint16_t       nvTrigCond;
__sv uint16_t       svTrigOnRecd;
__nv uint16_t       nvTotalCksum;
extern sc_list_t    nvDualList[2];          //backup & working
extern uint16_t     nvNodeBitmaps[2];       //backup & working
extern uint16_t     nvCurrTaskID;
extern uint16_t     nvCurrTaskIDShadow[2];
extern list_node_t  nvListNodePool[MAX_SUB_CKSUM_NUM];

// size should be in words
void _dma_word_copy(unsigned int from, unsigned int to, unsigned short size) {
    DMA0CTL &= ~DMAEN;
    // Configure DMA channel 0
    __data16_write_addr((unsigned int) &DMA0SA,(unsigned long) from);   // Source block address
    __data16_write_addr((unsigned int) &DMA0DA,(unsigned long) to);     // Destination single address

    DMA0SZ = size;                                                      // Block size
    DMA0CTL = DMADT_5 | DMASRCINCR_3 | DMADSTINCR_3;                    // Rpt, inc
    DMA0CTL |= DMAEN;                                                   // Enable DMA0
    DMA0CTL |= DMAREQ;                                                  // Trigger block transfer
}

/* ----------------
 * [__ckp_restore_sram]: done!!!
 * LOG: use to prepare working buffer by restoring states from SRAM buffer (aka, backup buffer) to working buffer.
 * 1. shared data
 * 2. checksum lists
 * 3. checksum node pool bitmaps
 */
void __ckp_restore_sram(){
    //global data.  backup-->working
    buffer_t *buffer = &_threads[0].buffer;
    _dma_word_copy( (unsigned int)buffer->sram_bufs[svBufIdx.idx],  \
                    (unsigned int)buffer->sram_bufs[svBufIdx._idx], \
                    nvBufSize>>1);
    //Dual-list.    backup-->working
    _dma_word_copy( (unsigned int)&nvDualList[svBufIdx.idx],        \
                    (unsigned int)&nvDualList[svBufIdx._idx],       \
                    (unsigned short)sizeof(sc_list_t)>>1);
    //List nodeBp.  backup-->working
    nvNodeBitmaps[svBufIdx._idx] = nvNodeBitmaps[svBufIdx.idx];
}

/* ----------------
 * [__ckp_restore_nvm]: done!!!
 * LOG: use to prepare working buffer by restoring states from NVM buffer (aka, persistent buffer) to working buffer.
 * 1. restore working buffer
 * 2. copy data to shadow buffer for future commitment
 * 3. restore state of `nvCurrTaskID` from `nvCurrTaskIDShadow`
 * 4. restore state of `nvDualList[working_buffer]` and `nvNodeBitmaps[working_buffer]`
 */
void __ckp_restore_nvm(){
    //global data.  persistent-->working
    buffer_t *buffer = &_threads[0].buffer;
    _dma_word_copy( (unsigned int)buffer->nvm_bufs[nvBufIdx.idx],   \
                    (unsigned int)buffer->sram_bufs[svBufIdx._idx], \
                    nvBufSize>>1);
    //global data.  persistent-->shadow
    _dma_word_copy( (unsigned int)buffer->nvm_bufs[nvBufIdx.idx],   \
                    (unsigned int)buffer->nvm_bufs[nvBufIdx._idx],  \
                    nvBufSize>>1);
    nvCurrTaskIDShadow[nvBufIdx._idx] = nvCurrTaskIDShadow[nvBufIdx.idx];
    nvCurrTaskID = nvCurrTaskIDShadow[nvBufIdx.idx];

    _sc_listInit(&nvDualList[svBufIdx._idx]);

    nvNodeBitmaps[svBufIdx._idx] = 0b0001;                     //use node 0 in nvListNodePool[]
    _sc_listFirstAdd(&nvDualList[svBufIdx._idx], 0);
    nvListNodePool[0].intvlStart = 0;                           //in **byte**
    nvListNodePool[0].intvlEnd = nvBufSize-1;     //in **byte**
    nvListNodePool[0].paddingNum = 0;
    nvListNodePool[0].subCksum = nvTotalCksum;
    svVerifiedBmp = 0b0001;
}

/* ----------------
 * [__ckp_commit_sram]: done!!!
 * LOG: use to commit changes to backup buffer.
 */
void __ckp_commit_sram(uint8_t tempTaskID){
    //FIXME: disable interrupt
    svBufIdx._idx   = svBufIdx._idx ^ 1;
    svBufIdx.idx    = svBufIdx.idx ^ 1;
    nvCurrTaskID    = tempTaskID;
    //FIXME: enable interrupt
}

/* ----------------
 * [_ckp_commit_nvm]: done!!!
 * LOG: use to commit changes to persistent buffer.
 */
inline void _ckp_commit_nvm(uint8_t tempTaskID){
    //FIXME: disable interrupt
    // backup --> shadow
    buffer_t *buffer    = &_threads[0].buffer;
    _dma_word_copy( (unsigned int)buffer->sram_bufs[svBufIdx.idx],  \
                    (unsigned int)buffer->nvm_bufs[nvBufIdx._idx],  \
                    nvBufSize>>1);
    nvBufIdx._idx = nvBufIdx._idx ^ 1;
    nvBufIdx.idx  = nvBufIdx.idx ^ 1;
    nvTrigCond    = 0;
    nvTotalCksum  = __sc_cksum_total();
    nvCurrTaskIDShadow[nvBufIdx._idx] = tempTaskID;
    
    //FIXME: enable interrupt
}

/* ----------------
 * [__ckp_init_bufs]:
 * LOG: use to init all buffers to '0'.
 */
void __ckp_init_bufs(){
    buffer_t *buffer = &_threads[0].buffer;
    memset(buffer->sram_bufs[0], 0, nvBufSize);
    memset(buffer->sram_bufs[1], 0, nvBufSize);
    memset(buffer->nvm_bufs[0], 0, nvBufSize);
    memset(buffer->nvm_bufs[1], 0, nvBufSize);
}

/* ----------------FIXME:
 * [__ckp_check_cond_and_commit]: 
 * LOG: use to init all buffers to '0'.
 */
void __ckp_check_cond_and_commit(uint8_t tempTaskID){
    svTrigOnRecd ++;
    if(svTrigOnRecd == ON_TASK_CNTER){
        nvTrigCond ++;
        if(nvTrigCond == OFF_POWER_CNTER){
            _ckp_commit_nvm(tempTaskID);
        }
    }
}

