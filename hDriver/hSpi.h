/*
 * hSpi.h
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HSPI_H_
#define TI_HDRIVERS_HSPI_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef HRTOS_IS_PRESENT
#include "hRtos.h"
#endif
/*
 * Enums
 */

typedef enum{
    HSPI_CALLBACK_NONE,
    HSPI_CALLBACK_RX,
    HSPI_CALLBACK_TX,
    HSPI_CALLBACK_TXRX
}hSpi_cb_source_e;

typedef enum{
    HSPI_OK,
    HSPI_ERROR
}hSpi_error_e;

/*
 * Structs and Unions
 */

typedef void *hSpi_t;

/*
 * Prototypes
 */

hSpi_t hSpi_init(uint32_t SPIIndex, uint32_t bitRate);
void hSpi_Delete(hSpi_t handle);

void hSpi_BitRate(hSpi_t handle, uint32_t bitRate);

void hSpi_mtx_lock(hSpi_t handle, uint16_t hold);
void hSpi_mtx_unlock(hSpi_t handle);
uint16_t hSpi_mtx_whoHold(hSpi_t handle);

hSpi_error_e hSpi_transmit(hSpi_t handle, uint8_t *data, uint16_t len);
hSpi_error_e hSpi_receive(hSpi_t handle, uint8_t *data, uint16_t len);
hSpi_error_e hSpi_transmit_receive(hSpi_t handle, uint8_t *dataTx, uint8_t *dataRx, uint16_t len);

void hSpi_transmit_DMA(hSpi_t handle, uint8_t *data, uint16_t len);
void hSpi_receive_DMA(hSpi_t handle, uint8_t *data, uint16_t len);
void hSpi_transmit_receive_DMA(hSpi_t handle, uint8_t *dataTx, uint8_t *dataRx, uint16_t len);

/*
 * Callbacks
 */

void hSpi_Rx_Callback(hSpi_t handle, uint8_t *Rxdata, uint16_t len);

void hSpi_Tx_Callback(hSpi_t handle, uint8_t *Txdata, uint16_t len);

void hSpi_TxRx_Callback(hSpi_t handle, uint8_t *Txdata, uint8_t *Rxdata, uint16_t len);

#endif /* TI_HDRIVERS_HSPI_H_ */
