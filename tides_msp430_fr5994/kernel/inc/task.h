/*
 * task.h
 *
 *  Created on: Nov 6, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_TASK_H_
#define KERNEL_INC_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "memory_mapping.h"
#include "buffer_manager.h"
#include "sram_checker.h"

/** The main structure for each Task. */
typedef struct {
    void *fun_entry;
    uint8_t task_idx;
    ck_set_t ck_set;
}task_t;

/** the main thread structure that holds all necessary info. */
typedef struct {
    uint8_t priority;                   //thread priority (unique)
    buffer_t buffer;                    //holds task shared persistent variables
    uint8_t idx_of_first_empty_task;    //index of the head of empty slots
    task_t task_array[MAX_TASK_NUM];    //task_t pool
}thread_t;


/**
 * ---------------------------
 * APIs for the usage of TIDES
 * ---------------------------
 */

/** Allocates dual-double buffer for the shared variables in the hybrid memory. */
#define __shared(...)                               \
        typedef struct {                            \
            __VA_ARGS__                             \
        } shared_t  __attribute__ ((aligned (2)));  \
        __nvm_buf   static shared_t __nvm_bufs[2];  \
        __sram_buf  static shared_t __sram_bufs[2]


/** Create a thread. */
void    __create_thread(uint8_t priority, 
                        uint16_t size,
                        void *nvm_shadow, 
                        void *nvm_persistent, 
                        void *sram_working, 
                        void *sram_backup);
#define __THREAD(priority)                                      \
        __create_thread(priority,                               \
                        sizeof(shared_t),                       \
                        (void *)&__nvm_bufs[0],                 \
                        (void *)&__nvm_bufs[1],                 \
                        (void *)&__sram_bufs[0],                \
                        (void *)&__sram_bufs[1]);               \
        memset((void *)&__nvm_bufs[0], 0, sizeof(shared_t));    \
        memset((void *)&__nvm_bufs[1], 0, sizeof(shared_t));    \
        memset((void *)&__sram_bufs[0], 0, sizeof(shared_t));   \
        memset((void *)&__sram_bufs[1], 0, sizeof(shared_t))


/** TASK structure init. */
void    __init_task(uint8_t priority, 
                    void *task_entry, 
                    uint16_t start_used_offset, 
                    uint16_t end_used_offset);
#define TASK_INIT(priority, name, start_offset, end_offset) \
        __init_task(priority,                               \
                    (void *)&name,                          \
                    start_offset,                           \
                    end_offset)


/** Declare a TASK. */
typedef uint8_t (*taskfun_t) (buffer_t *);  //The task definition (single C function)
#define TASK(name)  static uint8_t name(void *__buffer)

/** Reads the value from the original stack. */
#define __GET(x) ((shared_t *)__buffer)->x

/** Writes the value to the temporary stack. */
#define __SET(x) ((shared_t *)__buffer)->x

/** Point to next TASK's ID. */
#define NEXT(id)  return (uint16_t)id

extern thread_t _threads[MAX_THREAD_NUM];
extern uint16_t nvBufSize;
#endif /* KERNEL_INC_TASK_H_ */
