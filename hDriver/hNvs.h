/*
 * hNVS.h
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HNVS_H_
#define TI_HDRIVERS_HNVS_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hDriver.h"
#include "hBasics.h"
#include "hSpi.h"

/*
 * Enumerates
 */

typedef enum{
    HNVS_OK,
    HNVS_FAIL,
    HNVS_MEMFULL,
    HNVS_KEYS_ALREADY_GENERATED,
    HNVS_ENCRIPTION_NOT_ENABLED,
    HNVS_ADDRESS_BLOCKED
}hNvs_state_e;

typedef enum{
    HNVS_SECURE_DISABLED,
    HNVS_SECURE_ENABLED
}hnvs_secure_e;

/*
 * Function Prototypes
 */

uint8_t hNvs_init(hSpi_t SPIHandle);
uint8_t hNvs_init_secured(hSpi_t SPIHandle);
hNvs_state_e hNvs_Format();
hNvs_state_e hNvs_Write(uint32_t address, uint8_t *data, uint16_t len);
hNvs_state_e hNvs_Read(uint32_t address, uint8_t *data, uint16_t len);
hNvs_state_e hNvs_Delete(uint32_t address);

#endif
