/*
 * hSpi.h
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HTIMER_H_
#define TI_HDRIVERS_HTIMER_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hBasics.h"
#ifdef HRTOS_IS_PRESENT
#include "hRtos.h"
#endif

/*
 * Macros
 */


/*
 * Enumerates
 */

typedef enum{
    HTIMER_MODE_ONE_SHOT,
    HTIMER_MODE_PERIODIC
} hTimer_mode_e;

/*
 * typedefs
 */

typedef void* hTimer_t;

/*
 * Weak Callbacks
 */

void hTimer_Callback(uint32_t index);


/*
 * Prototypes
 */

hTimer_t hTimer_Create(uint32_t TimerIndex, uint32_t Freq, hTimer_mode_e Mode);

void hTimer_setConfig(hTimer_t handle, uint16_t prescaler, uint32_t Freq);

void hTimer_Start(hTimer_t handle);

void hTimer_Stop(hTimer_t handle);

void hTimer_enableInterrupt(hTimer_t handle);

void hTimer_disableInterrupt(hTimer_t handle);

uint32_t hTimer_getValue(hTimer_t handle);

void hTimer_Delete(hTimer_t handle);

#endif
