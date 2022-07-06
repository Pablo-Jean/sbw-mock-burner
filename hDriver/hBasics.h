/*
 * hBasics.h
 *
 *  Created on: 17 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HBASICS_H_
#define TI_HDRIVERS_HBASICS_H_

#include <stdint.h>
#include <stdlib.h>

#ifndef HRTOS_IS_PRESENT
void* hHeap_Malloc(size_t size);
void hHeap_Free(void* p);
#endif

void System_Reset();
void System_NOP();
void hDelay_ms(uint32_t ms);
void hDelay_us(uint32_t us);
void hCpuDelay(uint32_t ticks);

uint32_t hGet_Tick();

void hWatchdog_start();

void hWatchdog_clear();

void hWatchdog_setMs(uint32_t ms);

#endif /* TI_HDRIVERS_HBASICS_H_ */
