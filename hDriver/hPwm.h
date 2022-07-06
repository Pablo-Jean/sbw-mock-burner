/*
 * hPwm.h
 *
 *  Created on: 23 de nov de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HPWM_H_
#define TI_HDRIVERS_HPWM_H_

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "hHeap.h"

/*
 * externs
 */



// typedefs

typedef void *hPwm_t;

/*
 * Function prototypes
 */


hPwm_t hPwm_init(uint32_t PWMIndex, uint32_t frequency, uint32_t resolution, uint8_t idleLevel);

void hPwm_Destroy(hPwm_t handler);

void hPwm_Start(hPwm_t handler);

void hPwm_SetDuty(hPwm_t handler, uint32_t Duty);

void hPwm_Stop(hPwm_t handler);

#endif /* TI_HDRIVERS_HPWM_H_ */
