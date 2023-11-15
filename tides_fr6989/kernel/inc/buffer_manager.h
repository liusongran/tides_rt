/*
 * buffer_manager.h
 *
 *  Created on: Nov 6, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_BUFFER_MANAGER_H_
#define KERNEL_INC_BUFFER_MANAGER_H_

#include <stdint.h>

/** meta data */

typedef struct {
    volatile uint8_t idx;
    volatile uint8_t _idx;
}buffer_idx_t;


/** Each thread will hold a buffer for the variables shared by the tasks it is encapsulating. */
typedef struct {
    void *nvm_bufs[2];       //hold pointers to global data on nvm buffers (shadow and persistent)
    void *sram_bufs[2];      //hold pointers to global data on sram buffers (working and backup)
    uint16_t size;          //size of each buffer
}buffer_t;

extern buffer_idx_t nvBufIdx;   // index for dual-buffer on NVM
extern buffer_idx_t svBufIdx;   // index for dual-buffer on SRAM

#endif /* KERNEL_INC_BUFFER_MANAGER_H_ */
