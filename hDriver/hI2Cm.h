/*
 * hI2Cm.h
 *
 *  Created on: 30 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HI2CM_H_
#define TI_HDRIVERS_HI2CM_H_

#include <stdlib.h>
#include <stdint.h>

/*
 * externs
 */

/*
 * Enuemrates
 */

typedef enum{
    HI2C_CALLBACK_RX,
    HI2C_CALLBACK_TX,
    HI2C_CALLBACK_TXRX
}hI2C_cb_source_e;

// typedefs

typedef void *hI2Cm_t;

/*
 * Callbacks Weak
 */

void hI2Cm_Tx_Callback(uint32_t handle, void *buf, size_t count);

void hI2Cm_Rx_Callback(uint32_t handle, void *buf, size_t count);

void hI2Cm_TxRx_Callback(uint32_t handle, void *txbuf, void *rxbuf, size_t count);

/*
 * Function prototypes
 */

hI2Cm_t hI2Cm_init(uint32_t I2CIndex);

void hI2Cm_Stop(hI2Cm_t handler);

void hI2Cm_Destroy(hI2Cm_t handler);

void hI2Cm_mtx_lock(hI2Cm_t handler, uint32_t index);

void hI2Cm_mtx_unlock(hI2Cm_t handler);

void hI2Cm_transmit(hI2Cm_t handler, uint8_t slaveAddress, uint8_t *txBuffer, size_t len);

void hI2Cm_receive(hI2Cm_t handler, uint8_t slaveAddress, uint8_t *rxBuffer, size_t len);

void hI2Cm_transmit_receive(hI2Cm_t handler, uint8_t slaveAddress, uint8_t *txBuffer, uint8_t *rxBuffer, size_t len);

void hI2Cm_transmit_IT(hI2Cm_t handler, uint8_t slaveAddress, uint8_t *txBuffer, size_t len);

void hI2Cm_receive_IT(hI2Cm_t handler, uint8_t slaveAddress, uint8_t *rxBuffer, size_t len);

#endif
