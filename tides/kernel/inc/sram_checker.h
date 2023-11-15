/*
 * sram_checker.h
 *
 *  Created on: Nov 6, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_SRAM_CHECKER_H_
#define KERNEL_INC_SRAM_CHECKER_H_

#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>
#include "config.h"
#include "memory_mapping.h"
#include "buffer_manager.h"

#define EMPTY_NODE              3535
#define VERIFY_PASSED           0
#define VERIFY_FAILED           1
#define SET_BIT(num,offset)     num |= ((0x01)<<offset)
#define GET_BIT(num,offset)     ((num>>offset) & (0x01))

extern uint16_t svVerifiedBmp;

typedef struct {
    uint16_t nvCksumTemp;
    uint16_t svCksumTemp;
}cksum_temp_t;

typedef struct {
    uint8_t itvalStartIdx;
    uint8_t itvalEndIdx;
}itval_idx_t;

/** Var info for each Task. */
typedef struct {
    uint16_t    start_used_offset;      // in byte
    uint16_t    end_used_offset;        // in byte
}ck_set_t;

/** checksum interval management */
typedef struct {
    uint16_t    intvlStart;
    uint16_t    intvlEnd;
    uint16_t    subCksum;
    uint16_t    paddingNum;         //number of padding-zeros to end
}list_node_t;

typedef struct {
    uint16_t    prevNode;
    uint16_t    nextNode;
}dlist_t;

typedef struct {
    uint16_t    prevNode;
    uint16_t    nextNode;
    dlist_t     stElkList[MAX_SUB_CKSUM_NUM];
}sc_list_t;

inline void _sc_listInit(sc_list_t *list){
    list->prevNode = EMPTY_NODE;
    list->nextNode = EMPTY_NODE;
}

inline void _sc_listFirstAdd(sc_list_t *list, uint16_t nodeIdx){
    list->stElkList[nodeIdx].nextNode = EMPTY_NODE;
    list->stElkList[nodeIdx].prevNode = EMPTY_NODE;

    list->nextNode = nodeIdx;
    list->prevNode = nodeIdx;
}

//FIXME:
inline uint16_t _sc_crc(uint16_t start_offset, uint16_t end_offset, void *bufAddr, uint16_t paddings){
    uint16_t tempSize = (end_offset-start_offset+1);     //size in word
    uint16_t tempI;
    uint16_t tempCksum;

    CRCINIRES = 0xFFFF;
    for(tempI=start_offset; tempI<tempSize; tempI=tempI+2){
        CRCDI = *(uint16_t *)((uint16_t)bufAddr+tempI);
    }
    tempCksum = CRCINIRES;

    if(paddings){
        tempCksum ^= 0x0011;
    }
    return tempCksum;
}

bool __sc_verify(uint16_t taskID);
void __sc_first_cksum();
void __sc_checksum(uint8_t taskID);
uint16_t __sc_cksum_total();

#endif /* KERNEL_INC_SRAM_CHECKER_H_ */
