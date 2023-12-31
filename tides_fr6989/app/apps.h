/*
 * app.h
 *
 *  Created on: Nov 14, 2023
 *      Author: liusongran
 */

#ifndef APP_APPS_H_
#define APP_APPS_H_
#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* -----------------
 * RUNTIME SELECTION:
 */
#define TIDES           1
#define INK             2
#define TOTALRECALL     3
#define RUNTIME         TIDES


/* -----------------------
 * APP SELECTION FOR USER:
 */
#define SRT
//#define AR
//#define DIJ
//#define BC
//#define RSA
//#define CK
//#define CEM
//#define CRC
//#define BC
//#define FFT
//#define ADPCM


/* ----------------------
 * APP SELECTION FOR ELK:
 */
#define APP_REGION_NUM      4           // protected memory region number

#ifdef SRT
#define APP_TASK_NUM        5
void _benchmark_sort_init();
#endif

#ifdef AR
#define APP_TASK_NUM        8
void _benchmark_ar_init();
#endif

#ifdef DIJ
#define APP_TASK_NUM        5
void _benchmark_dijkstra_init();
#endif

#ifdef BC
#define APP_TASK_NUM        11
void _benchmark_bc_init();
#endif


#ifdef CK
#define APP_TASK_NUM        8
void _benchmark_cuckoo_init();
#endif

#ifdef RSA
#define APP_TASK_NUM        8
void _benchmark_rsa_init();
#endif

#ifdef CRC
#define APP_TASK_NUM        4
void _benchmark_crc_init();
#endif

#ifdef CEM
#define APP_TASK_NUM        11
void _benchmark_cem_init();
#endif

#ifdef FFT
#define APP_TASK_NUM        6
void _benchmark_fft_init();
#endif

#ifdef ADPCM
#define APP_TASK_NUM        8
void _benchmark_adpcm_init();
#endif

#define MAX_TASK_NUM        APP_TASK_NUM
#endif /* APP_APPS_H_ */
