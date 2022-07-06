/*
 * hRtc.h
 *
 *  Created on: 30 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HRTC_H_
#define TI_HDRIVERS_HRTC_H_

#include <stdlib.h>
#include <stdint.h>


// Callback
void hRtc_Callback();

void hRtc_init();

void hRtc_stop();

void hRtc_setSeconds(uint32_t sec);

uint32_t hRtc_getSeconds();

void hRtc_enableInterrupt(uint32_t timeMs);

void hRtc_disableInterrupt();

void hRtc_changeMs(uint32_t timeMs);

#endif
