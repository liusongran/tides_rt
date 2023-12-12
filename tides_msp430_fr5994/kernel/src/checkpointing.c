/*
 * checkpointing.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */
#include "checkpointing.h"
#include "sram_checker.h"
#include "task.h"
#include "profile.h"

__nv buffer_idx_t   nvBufIdx = {0,1}; 
__sv buffer_idx_t   svBufIdx = {0,1};
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
    __bic_SR_register(GIE);         // disable
    svBufIdx._idx   = svBufIdx._idx ^ 1;
    svBufIdx.idx    = svBufIdx.idx ^ 1;
    nvCurrTaskID    = tempTaskID;
    __bis_SR_register(GIE);         // enable
}

/* ----------------
 * [__ckp_commit_INK]: done!!!
 * LOG: use to commit changes to backup buffer.
 */
void __ckp_commit_INK(uint8_t tempTaskID){
    __bic_SR_register(GIE);         // disable
    nvBufIdx._idx   = nvBufIdx._idx ^ 1;
    nvBufIdx.idx    = nvBufIdx.idx ^ 1;
    nvCurrTaskID    = tempTaskID;
    __bis_SR_register(GIE);         // enable
}

/* ----------------
 * [_ckp_commit_nvm]: done!!!
 * LOG: use to commit changes to persistent buffer.
 */
__nv uint16_t nvCnterNVM = 0;
void _ckp_commit_nvm(uint8_t tempTaskID){
    __bic_SR_register(GIE);         // disable

    // backup --> shadow
    buffer_t *buffer    = &_threads[0].buffer;
    _dma_word_copy( (unsigned int)buffer->sram_bufs[svBufIdx.idx],  \
                    (unsigned int)buffer->nvm_bufs[nvBufIdx._idx],  \
                    nvBufSize>>1);
    nvCurrTaskIDShadow[nvBufIdx._idx] = tempTaskID;
    nvBufIdx._idx = nvBufIdx._idx ^ 1;
    nvBufIdx.idx  = nvBufIdx.idx ^ 1;
    nvTotalCksum  = __sc_cksum_total();
    nvCnterNVM ++;
    __bis_SR_register(GIE);         // enable
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
__nv uint8_t nvRandomArray[200] = {
    13, 21, 49, 97, 38, 28, 43, 76, 91, 18,
    21, 76, 6, 67, 43, 49, 86, 22, 68, 71,
    58, 91, 3, 27, 100, 57, 85, 63, 22, 5,
    50, 99, 92, 57, 95, 73, 18, 6, 58, 29,
    46, 43, 69, 57, 46, 31, 54, 96, 75, 4,
    57, 39, 68, 84, 52, 17, 1, 73, 51, 84,
    36, 18, 31, 62, 50, 78, 62, 17, 21, 67,
    94, 74, 53, 60, 74, 57, 65, 68, 98, 4,
    72, 54, 35, 71, 25, 9, 45, 99, 3, 50,
    68, 81, 70, 32, 16, 45, 66, 16, 74, 17,
    25, 77, 38, 1, 81, 23, 94, 56, 40, 17,
    12, 84, 69, 27, 46, 41, 26, 34, 21, 78,
    42, 56, 28, 4, 96, 10, 41, 99, 74, 49,
    58, 20, 51, 36, 15, 64, 84, 87, 85, 58,
    83, 88, 95, 48, 76, 80, 100, 41, 45, 74,
    19, 94, 29, 13, 51, 75, 96, 60, 19, 59,
    22, 68, 50, 44, 42, 1, 10, 18, 96, 59,
    92, 69, 82, 40, 26, 88, 1, 65, 85, 11,
    95, 24, 92, 76, 49, 48, 13, 83, 45, 11,
    48, 41, 41, 15, 87, 42, 46, 56, 56, 63
};

extern uint32_t nvTotalTaskCnt;
extern uint16_t nvRoundTaskRcd;
void __ckp_check_cond_and_commit(uint8_t tempTaskID){
    uint8_t tRdmVar;
    svTrigOnRecd ++;
    if(svTrigOnRecd == ON_TASK_CNTER){
        tRdmVar = nvRandomArray[nvTotalTaskCnt%200];
        if (tRdmVar<10) {
            _ckp_commit_nvm(tempTaskID);
        }
    }
}

/* ----------------
 * [__ckp_restore_INK]: done!!!
 * LOG: use to prepare working buffer by restoring states from SRAM buffer (aka, backup buffer) to working buffer.
 * 1. shared data
 * 2. checksum lists
 * 3. checksum node pool bitmaps
 */
void __ckp_restore_INK(){
    //global data.  backup-->working
    buffer_t *buffer = &_threads[0].buffer;
    _dma_word_copy( (unsigned int)buffer->nvm_bufs[nvBufIdx.idx],  \
                    (unsigned int)buffer->nvm_bufs[nvBufIdx._idx], \
                    nvBufSize>>1);
}


/**
void __ckp_check_cond_and_commit(uint8_t tempTaskID){
    uint16_t tRdmVar;
    uint16_t tRdmStart=0;
    uint16_t tRdmEnd;
    svTrigOnRecd ++;
    tRdmEnd = timerA1Stop();
    //uart_printf("|svTrigOnRecd:%d\r\n", svTrigOnRecd);
    if(svTrigOnRecd == ON_TASK_CNTER){
        srand(tRdmEnd-tRdmStart);
        tRdmVar = rand()%100;
        uart_printf("|tRdmVar:%d\r\n", tRdmVar);
        if (tRdmVar<10) {
            _ckp_commit_nvm(tempTaskID);
        }
    }
    tRdmStart = timerA1Start();
}
*/
