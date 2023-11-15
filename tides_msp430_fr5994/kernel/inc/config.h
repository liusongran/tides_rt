/*
 * config.h
 *
 *  Created on: Nov 6, 2023
 *      Author: liusongran
 */

#ifndef KERNEL_INC_CONFIG_H_
#define KERNEL_INC_CONFIG_H_
#include "apps.h"

#define P_TRUE              1
#define P_FALSE             0              
#define PROFILE_ENABLED     P_TRUE

#define PRIORITY_LEV        1
#define MAX_THREAD_NUM      PRIORITY_LEV
#define APP_REGION_NUM      4
#define MAX_SUB_CKSUM_NUM   5
#define MAX_CKSUM_TAB_NUM   128

#define SIMU_ITERATION      20

#endif /* KERNEL_INC_CONFIG_H_ */
