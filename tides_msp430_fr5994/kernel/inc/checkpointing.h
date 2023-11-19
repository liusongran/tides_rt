/*
 * checkpointing.h
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_CHECKPOINTING_H_
#define KERNEL_INC_CHECKPOINTING_H_
#include <stdint.h>
#include <msp430.h>
#include "config.h"
#include "memory_mapping.h"
#include "buffer_manager.h"


#define ON_TASK_CNTER   5       //FIXME: put in config.h, record finished TASK number in each power-on round 
#define OFF_POWER_CNTER 1       //FIXME: this value should change with power traces, trigger when to NVM ckp

extern uint16_t nvTrigCond;
extern uint16_t svTrigOnRecd;


void __ckp_restore_sram();
void __ckp_restore_nvm();
void __ckp_restore_INK();
void __ckp_commit_sram(uint8_t tempTaskID);
void __ckp_commit_INK(uint8_t tempTaskID);
void __ckp_init_bufs();
void __ckp_check_cond_and_commit(uint8_t tTaskID);

#endif /* KERNEL_INC_CHECKPOINTING_H_ */
