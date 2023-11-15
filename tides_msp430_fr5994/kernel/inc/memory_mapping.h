/*
 * memory_mapping.h
 *
 *  Created on: Nov 6, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_MEMORY_MAPPING_H_
#define KERNEL_INC_MEMORY_MAPPING_H_
#include <msp430.h>

#define __nv            __attribute__((section(".nvm_vars")))       // variables on NVM
#define __nvm_buf       __attribute__((section(".nvm_bufs")))       // buffers on NVM
#define __sv            __attribute__((section(".sram_vars")))      // variables on SRAM
#define __sram_buf      __attribute__((section(".sram_bufs")))      // buffers on SRAM

#endif /* KERNEL_INC_MEMORY_MAPPING_H_ */
