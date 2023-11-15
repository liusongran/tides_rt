/*
 * scheduler.h
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_SCHEDULER_H_
#define KERNEL_INC_SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>
#include "task.h"
#include "checkpointing.h"

extern bool nvInited;
void __scheduler_run();

#endif /* KERNEL_INC_SCHEDULER_H_ */
