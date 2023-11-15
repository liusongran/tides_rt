#include <task.h>
#include <apps.h>
#include "./sort.h"

/*
 * 1. TASK declaration
 */
TASK(task_setup);                   //-(0)
TASK(task_init);                    //-(1)
TASK(task_inner_loop);              //-(2)
TASK(task_finish);                  //-(3)

/*
 * 2. Shared variable declaration (206 bytes)
 */
__shared(
    uint16_t iteration;             //-[1]:2
    uint16_t inner_idx;             //-[2]:2
    uint16_t outer_idx;             //-[3]:2
    uint16_t array[LENGTH];         //-[4]:200
);

uint16_t in_i, in_j, arr_i, arr_j;
/*
 * 3. TASK definition
 */
TASK(task_setup){ //-(0)
    __SET(iteration) = 0;
    NEXT(1);
}

TASK(task_init){ //-(1), R[4] || W[1,2,3,4]. NOTE: size-[0,205]
    __SET(outer_idx) = 0;
    __SET(inner_idx) = 1;
    ++__SET(iteration);

    const uint16_t* array_pt;

    if(__GET(iteration) & 0x01){
        array_pt = a1;
    }else{
        array_pt = a2;
    }

    uint16_t idx;
    for(idx = 0; idx<LENGTH; idx++){
        __SET(array[idx]) = array_pt[idx];
    }

    NEXT(2);
}

TASK(task_inner_loop){ //-(2), R[1-P,2,3] || W[1-P,3]. NOTE: size-[0,203]
    uint16_t i, j, x_i, x_j, temp;
    uint16_t x_k;

    for(x_k=0; x_k<500; x_k++){
        i = __GET(outer_idx);
        j = __GET(inner_idx);

        x_i = __GET(array[i]);
        x_j = __GET(array[j]);

        if(x_i > x_j){
            temp = x_j;
            x_j = x_i;
            x_i = temp;
        }
        __SET(array[i]) = x_i;
        __SET(array[j]) = x_j;
        ++__SET(inner_idx);
        if (__GET(inner_idx) >= LENGTH){
            ++__SET(outer_idx);
            __SET(inner_idx) = __GET(outer_idx) + 1;
            if (__GET(outer_idx) >= LENGTH - 1){
                NEXT(3);
            }
        }
    }
    NEXT(2);
}

TASK(task_finish){ //-(3)
    if(__GET(iteration)>400){
        NEXT(0);
    }else{
        NEXT(1);
    }
}

/*
 * 0. Benchmark main goes here, interval in byte, format always like [even, odd]
 */
void _benchmark_sort_init(){
    __THREAD(0);
    TASK_INIT(0, task_setup,        0,      9);
    TASK_INIT(0, task_init,         0,      99);   //0 - [0,205]
    TASK_INIT(0, task_inner_loop,   50,     149);   //1 - [0,203]
    TASK_INIT(0, task_finish,       0,      205);     //3 - [0,0]
}
