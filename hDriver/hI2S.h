/*
 * hI2S.h
 *
 *  Created on: 10 de aug de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HI2S_H_
#define TI_HDRIVERS_HI2S_H_


#include <stdint.h>
#include <stdlib.h>

#include <hDriver.h>


/*
 * Enumerates
 */

typedef enum{
    HI2S_CHANNEL_MONO = 0x1,
    HI2S_CHANNEL_STEREO = 0x3
}hI2S_channel_mode_e;

/*
 * typedef
 */
typedef void *hI2S_t;

/*
 * FUnctions Prototyps
 */

// extern callback
void hI2S_Callback(uint32_t index);

// general
hI2S_t hI2S_Create(uint32_t I2SIndex, uint32_t SampleFreq, hI2S_channel_mode_e mode);

void hI2S_setRate(hI2S_t handle, uint32_t sampleRate);

void hI2S_Start(hI2S_t handle, void *buffer, uint16_t len);

void hI2S_Insert(hI2S_t handle, void *buffer, uint16_t len, uint8_t last);

void hI2S_Pause(hI2S_t handle);

void hI2S_Resume(hI2S_t handle);

void hI2S_Stop(hI2S_t handle);

void hI2S_Delete(hI2S_t handle);


#endif
