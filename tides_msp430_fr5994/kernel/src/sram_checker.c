/*
 * sram_checker.c
 *
 *  Created on: Nov 7, 2023
 *      Author: liusongran
 */
#include "sram_checker.h"
#include "task.h"

__nv list_node_t    nvListNodePool[MAX_SUB_CKSUM_NUM];
__nv uint16_t       nvNodeBitmaps[2];   //backup & working
__nv sc_list_t      nvDualList[2];      //backup & working

__sv uint16_t       svVerifiedBmp;      //verified node bitmap of this power-on
__sv uint16_t       svRoundList[3];     //stack list of newly inserted nodes in this round search, cleared by each calling of cksum
__sv uint16_t       svRoundBmp;         //temp bitmap for this round search

uint16_t __sc_cksum_total(){
    uint16_t tTotalCksum = 0;
    uint16_t tNextNodeIdx = nvDualList[svBufIdx.idx].nextNode;
    while(tNextNodeIdx != EMPTY_NODE){
        tTotalCksum += nvListNodePool[tNextNodeIdx].subCksum;
        tNextNodeIdx = nvDualList[svBufIdx.idx].stElkList[tNextNodeIdx].nextNode;
    }
    return tTotalCksum;
}

/* ----------------
 * [_sc_find_empty_node]: done!!!
 * LOG: use to find the lowest empty node in nvListNodePool[].
 * 1. find the lowest empty node in nvListNodePool[]
 * 2. update bitmap `svRoundBmp` (new node this round) and `nvNodeBitmaps` (global used node)
 */
uint8_t _sc_find_empty_node(uint8_t bufIdx) {
    uint16_t tCnter=0;
    int16_t i;
    while(1){
        if((GET_BIT(nvNodeBitmaps[bufIdx],tCnter)) == 0){
            break;
        }
        tCnter++;
    }
    SET_BIT(svRoundBmp,tCnter);
    SET_BIT(nvNodeBitmaps[bufIdx],tCnter);

    for(i=0;i<3;i++){
        if(svRoundList[i] == EMPTY_NODE){
            svRoundList[i] = tCnter;
            break;
        }
    }

    return tCnter;
}

/* ---------------
 * [_sc_partial_crc_lower]: done!!!
 * LOG: `addrMiddle` is always an even number.
 */
void _sc_partial_crc_lower(uint8_t nodeIdx, uint8_t bufIdx, uint16_t addrMiddle){
    uint16_t tNodeIdx = 0;
    uint16_t addrStart = nvListNodePool[nodeIdx].intvlStart;
    uint16_t addrEnd = nvListNodePool[nodeIdx].intvlEnd;
    uint16_t addrHalf = (addrEnd-addrStart+1)>>1;
    list_node_t tListNode;

    tListNode.intvlStart = addrStart;
    tListNode.intvlEnd = addrMiddle-1;      // format to an Odd number
    tListNode.paddingNum = nvListNodePool[nodeIdx].paddingNum+addrEnd-addrMiddle+1;

    if((addrMiddle-addrStart) > (addrHalf+20)){ //FIXME: could add an alpha here.
        tListNode.subCksum =_sc_crc(addrMiddle,                             \
                                    addrEnd,                                \
                                    _threads[0].buffer.sram_bufs[bufIdx^1], \
                                    tListNode.paddingNum);
        tListNode.subCksum = nvListNodePool[nodeIdx].subCksum ^ tListNode.subCksum;
    }else{
        tListNode.subCksum =_sc_crc(addrStart,                              \
                                    addrMiddle-1,                           \
                                    _threads[0].buffer.sram_bufs[bufIdx],   \
                                    tListNode.paddingNum);
    }

    tNodeIdx = _sc_find_empty_node(bufIdx);
    nvListNodePool[tNodeIdx] = tListNode;
}

/* ---------------
 * [_sc_partial_crc_upper]: done!!!
 * LOG: `addrMiddle` is always an odd number.
 */
void _sc_partial_crc_upper(uint8_t nodeIdx, uint8_t bufIdx, uint16_t addrMiddle){
    uint16_t tNodeIdx = 0;
    uint16_t addrStart = nvListNodePool[nodeIdx].intvlStart;
    uint16_t addrEnd = nvListNodePool[nodeIdx].intvlEnd;
    uint16_t addrHalf = (addrEnd-addrStart+1)>>1;
    list_node_t tListNode;

    tListNode.intvlStart = addrMiddle+1;
    tListNode.intvlEnd = addrEnd;
    tListNode.paddingNum = nvListNodePool[nodeIdx].paddingNum;

    if((addrMiddle-addrStart) < (addrHalf-20)){ //FIXME: could add an alpha here.
        tListNode.subCksum =_sc_crc(addrStart,                              \
                                    addrMiddle,                             \
                                    _threads[0].buffer.sram_bufs[bufIdx^1], \
                                    tListNode.paddingNum);
        tListNode.subCksum = nvListNodePool[nodeIdx].subCksum ^ tListNode.subCksum;
    }else{
        tListNode.subCksum =_sc_crc(addrMiddle+1,                           \
                                    addrEnd,                                \
                                    _threads[0].buffer.sram_bufs[bufIdx],   \
                                    tListNode.paddingNum);
    }

    tNodeIdx = _sc_find_empty_node(bufIdx);
    nvListNodePool[tNodeIdx] = tListNode;
}

/* ---------------
 * [__sc_verify]: done!!!
 * LOG: Always verify content in backup buffer.
 * 1. find sub-interval index of target sub-interval
 * 2. verify each sub-interval
 * 3. update verified-flag
 * 4. return results
 */
bool __sc_verify(uint16_t taskID){
    bool tFlagSearched = 0;
    bool tResult = 0;
    uint8_t tBufIdxBackup = svBufIdx.idx;   // working on backup buffer
    uint16_t tCksum = 0;
    uint16_t idxIntvlStart = 0;             //target start interval index.
    uint16_t idxIntvlEnd = 0;               //target end interval index.
    uint16_t tNextNodeIdx = 0;
    ck_set_t tInterval = _threads[0].task_array[taskID].ck_set;

    if(tInterval.end_used_offset==0){
        return VERIFY_PASSED;
    }

    tNextNodeIdx = nvDualList[tBufIdxBackup].nextNode;
    while(1){
        if((!tFlagSearched) && (tInterval.start_used_offset < nvListNodePool[tNextNodeIdx].intvlEnd)){
            idxIntvlStart = tNextNodeIdx;
            tFlagSearched = 1;
        }
        if((tFlagSearched) && (tInterval.end_used_offset <= nvListNodePool[tNextNodeIdx].intvlEnd)){
            idxIntvlEnd = tNextNodeIdx;
            break;
        }
        tNextNodeIdx = nvDualList[tBufIdxBackup].stElkList[tNextNodeIdx].nextNode;
    }

    tNextNodeIdx = idxIntvlStart;
    while(1){
        if(GET_BIT(svVerifiedBmp,tNextNodeIdx)==0){
            tCksum =_sc_crc(nvListNodePool[tNextNodeIdx].intvlStart,    \
                            nvListNodePool[tNextNodeIdx].intvlEnd,      \
                            _threads[0].buffer.sram_bufs[tBufIdxBackup],\
                            nvListNodePool[tNextNodeIdx].paddingNum);
            if(tCksum != nvListNodePool[tNextNodeIdx].subCksum){
                tResult = VERIFY_PASSED;               //FIXME: should be failed
                SET_BIT(svVerifiedBmp,tNextNodeIdx);
            }else{
                SET_BIT(svVerifiedBmp,tNextNodeIdx);
            }
        }
        if(tNextNodeIdx==idxIntvlEnd){
            tResult = VERIFY_PASSED;
            break;
        }else{
            tNextNodeIdx = nvDualList[tBufIdxBackup].stElkList[tNextNodeIdx].nextNode;
        }
    }
    return tResult;
}

/* --------------------
 * [__elk_first_cksum]: done!!!
 * LOG: the very first checksum of global variables as a whole to list node nvListNodePool[0].
 * 1. clear nodeBitmap set;
 * 2. update nodeBitmap[working_buffer];
 * 3. calculate the whole cksum;
 */
void __sc_first_cksum(){
    uint8_t tBufIdxWorking = svBufIdx._idx;                     //working on the working buffer
    nvNodeBitmaps[tBufIdxWorking] = 0b0001;                     //use node 0 in nvListNodePool[]
    _sc_listFirstAdd(&nvDualList[tBufIdxWorking], 0);
    nvListNodePool[0].intvlStart = 0;                           //in **byte**
    nvListNodePool[0].intvlEnd = _threads[0].buffer.size-1;     //in **byte**
    nvListNodePool[0].paddingNum = 0;
    nvListNodePool[0].subCksum =_sc_crc(0,                                              \
                                        nvListNodePool[0].intvlEnd,                     \
                                        _threads[0].buffer.sram_bufs[tBufIdxWorking],   \
                                        0);
    svVerifiedBmp = 0b0001;
}

/* ---------------------
 * [_sc_normal_cksum]: done!!!
 * LOG: checksum in normal mode.
 * 1. find out the start&end interval index of target-interval.
 * 2. clear elkNodeBitmaps[2].
 * 3. go across multiple intervals or split a single interval into two/three.
 */
inline void _sc_normal_cksum(uint16_t tgtIntvlStart, uint16_t tgtIntvlEnd){
    // Working on working-buffer
    uint8_t tBufIdxWorking = svBufIdx._idx;         
    uint8_t tFlagSearch = 0;

    uint16_t tStartNodeIdx = 0;     // node index of the start of super-interval
    uint16_t tEndNodeIdx = 0;       // node index of the end of super-interval
    uint16_t tNodeIdx = 0;
    uint16_t tRmBmp = 0;            // Bitmap that records nodes to be removed

    // First, find the super-interval of the given interval and return with node index `tStartNodeIdx` and `tEndNodeIdx`
    tNodeIdx = nvDualList[tBufIdxWorking].nextNode;
    svRoundBmp = 0;
    while(1){
        if((!tFlagSearch) && (tgtIntvlStart < nvListNodePool[tNodeIdx].intvlEnd)){
            SET_BIT(tRmBmp, tNodeIdx);
            tStartNodeIdx = tNodeIdx;
            tFlagSearch = 1;
        }
        if((tFlagSearch) && (tgtIntvlEnd <= nvListNodePool[tNodeIdx].intvlEnd)){
            tEndNodeIdx = tNodeIdx;
            break;
        }
        tNodeIdx = nvDualList[tBufIdxWorking].stElkList[tNodeIdx].nextNode;
        if(tFlagSearch){
            SET_BIT(tRmBmp, tNodeIdx);
        }
        if(tNodeIdx==EMPTY_NODE){
            // MARK: something is wrong!!! check `tStartNodeIdx` and `tEndNodeIdx`
            while(1);
        }
    }


    // clear stack-list used to record newly created node
    svRoundList[0] = EMPTY_NODE;
    svRoundList[1] = EMPTY_NODE;
    svRoundList[2] = EMPTY_NODE;

    //  ---||:seg-1 
    if(nvListNodePool[tStartNodeIdx].intvlStart < tgtIntvlStart){
        _sc_partial_crc_lower(tStartNodeIdx, tBufIdxWorking, tgtIntvlStart);
    }
    //  ---||:seg-2
    tNodeIdx = _sc_find_empty_node(tBufIdxWorking);
    nvListNodePool[tNodeIdx].paddingNum = nvListNodePool[tEndNodeIdx].paddingNum+nvListNodePool[tEndNodeIdx].intvlEnd-tgtIntvlEnd+1;
    nvListNodePool[tNodeIdx].subCksum   = _sc_crc(  tgtIntvlStart,                                  \
                                                    tgtIntvlEnd,                                    \
                                                    _threads[0].buffer.sram_bufs[tBufIdxWorking],   \
                                                    nvListNodePool[tNodeIdx].paddingNum);
    nvListNodePool[tNodeIdx].intvlStart = tgtIntvlStart;
    nvListNodePool[tNodeIdx].intvlEnd   = tgtIntvlEnd;
    //  ---||:seg-3
    if(nvListNodePool[tEndNodeIdx].intvlEnd > tgtIntvlEnd){
        _sc_partial_crc_upper(tEndNodeIdx, tBufIdxWorking, tgtIntvlEnd);
    }

    nvNodeBitmaps[tBufIdxWorking] = nvNodeBitmaps[tBufIdxWorking] - tRmBmp;


    // Modify `nvDualList`
    uint16_t tStartModNodeIdx, tEndModNodeIdx;
    tStartModNodeIdx = nvDualList[tBufIdxWorking].stElkList[tStartNodeIdx].prevNode;
    tEndModNodeIdx = nvDualList[tBufIdxWorking].stElkList[tEndNodeIdx].nextNode;
    if(tStartModNodeIdx==EMPTY_NODE){
        nvDualList[tBufIdxWorking].nextNode = svRoundList[0];
    }else{
        nvDualList[tBufIdxWorking].stElkList[tStartModNodeIdx].nextNode = svRoundList[0];
    }
    nvDualList[tBufIdxWorking].stElkList[svRoundList[0]].prevNode = tStartModNodeIdx;

    if(svRoundList[1] != EMPTY_NODE){
        nvDualList[tBufIdxWorking].stElkList[svRoundList[0]].nextNode = svRoundList[1];
        nvDualList[tBufIdxWorking].stElkList[svRoundList[1]].prevNode = svRoundList[0];
        if(svRoundList[2] != EMPTY_NODE){
            nvDualList[tBufIdxWorking].stElkList[svRoundList[1]].nextNode = svRoundList[2];
            nvDualList[tBufIdxWorking].stElkList[svRoundList[2]].prevNode = svRoundList[1];
            nvDualList[tBufIdxWorking].stElkList[svRoundList[2]].nextNode = tEndModNodeIdx;
            if(tEndModNodeIdx==EMPTY_NODE){
                nvDualList[tBufIdxWorking].prevNode = svRoundList[2];
            }else{
                nvDualList[tBufIdxWorking].stElkList[tEndModNodeIdx].prevNode = svRoundList[2];
            }
        }else{
            nvDualList[tBufIdxWorking].stElkList[svRoundList[1]].nextNode = tEndModNodeIdx;
            if(tEndModNodeIdx==EMPTY_NODE){
                nvDualList[tBufIdxWorking].prevNode = svRoundList[1];
            }else{
                nvDualList[tBufIdxWorking].stElkList[tEndModNodeIdx].prevNode = svRoundList[1];
            }
        }
    }else{
        nvDualList[tBufIdxWorking].stElkList[svRoundList[0]].nextNode = tEndModNodeIdx;
        if(tEndModNodeIdx==EMPTY_NODE){
            nvDualList[tBufIdxWorking].prevNode = svRoundList[0];
        }else{
            nvDualList[tBufIdxWorking].stElkList[tEndModNodeIdx].prevNode = svRoundList[0];
        }
    }
    // Update `svVerifiedBmp`, note that this value is no need to be exact
    svVerifiedBmp = svVerifiedBmp + svRoundBmp;
}

/* ---------------------
 * [__sc_checksum]: done!!!
 * LOG: checksum in normal mode, external API.
 */
void __sc_checksum(uint8_t taskID) {
    ck_set_t tCkSet = _threads[0].task_array[taskID].ck_set;
    if(tCkSet.end_used_offset){
        _sc_normal_cksum(tCkSet.start_used_offset, tCkSet.end_used_offset);
    }
}
