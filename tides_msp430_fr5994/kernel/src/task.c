/*
 * task.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#include "task.h"
#include "memory_mapping.h"

__nv thread_t _threads[MAX_THREAD_NUM];
__nv uint16_t nvBufSize;

/** Assigns a slot to a thread. Should be called ONLY at the first system boot. */
void    __create_thread(uint8_t priority,
                        uint16_t size,
                        void *nvm_shadow,
                        void *nvm_persistent, 
                        void *sram_working, 
                        void *sram_backup)
{
    _threads[priority].priority = priority;
    _threads[priority].idx_of_first_empty_task = 0;

    _threads[priority].buffer.size          = size;
    _threads[priority].buffer.nvm_bufs[0]   = nvm_shadow;
    _threads[priority].buffer.nvm_bufs[1]   = nvm_persistent;
    _threads[priority].buffer.sram_bufs[0]  = sram_working;
    _threads[priority].buffer.sram_bufs[1]  = sram_backup;

    nvBufSize = size;
}


/** Init. a new TASK. */
// @para. addr_length: in word(2bytes)
void    __init_task(uint8_t priority, 
                    void *task_entry, 
                    uint16_t start_used_offset, 
                    uint16_t end_used_offset)
{
    uint16_t temp_index;

    temp_index = _threads[priority].idx_of_first_empty_task;
    _threads[priority].task_array[temp_index].fun_entry = task_entry;
    _threads[priority].task_array[temp_index].task_idx = temp_index;
    _threads[priority].task_array[temp_index].ck_set.start_used_offset = start_used_offset;
    _threads[priority].task_array[temp_index].ck_set.end_used_offset = end_used_offset;
    _threads[priority].idx_of_first_empty_task++;
}