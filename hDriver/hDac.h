/*
 * hDac.h
 *
 *  Created on: 17 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HDAC_H_
#define TI_HDRIVERS_HDAC_H_


#include <stdint.h>
#include <stdlib.h>

#include "hHeap.h"

/*
 * Enums
 */

typedef enum{
    DAC_OFF,
    DAC_IDLE,
    DAC_RUNNING,
    DAC_END_STREAM
}dac_state_e;


/*
 * Typedef
 */

typedef void* hDac_t;

/*
 * Functions Prototypes
 */

hDac_t hDac_init(uint32_t DACIndex, uint32_t TIMERIndex);
uint8_t hDac_setRate(hDac_t handle, uint32_t rate);
dac_state_e hDac_getStatus(hDac_t handle);
uint8_t hDac_Stream_Start(hDac_t handle, uint32_t *data, uint16_t size);
void hDac_Stream_Stop(hDac_t handle);
void hDac_Delete(hDac_t handle);

/* Callback */
void hDac_End_Callback(hDac_t handle);

#endif /* TI_HDRIVERS_HDAC_H_ */
