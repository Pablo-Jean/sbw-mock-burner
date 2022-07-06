/*
 * hUsart.h
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef NRF_DRIVER_HUSART_H
#define NRF_DRIVER_HUSART_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/*
 * externs
 */

extern uint32_t HUSART_PARITY_NONE;
extern uint32_t HUSART_PARITY_ODD;
extern uint32_t HUSART_PARITY_EVEN;

// typedefs

typedef void *hUsart_t;

/*
 * Callbacks Weak
 */

void  hUsart_Receive_Callback(uint32_t handle, void *buf, size_t count);

void  hUsart_Transmit_Callback(uint32_t handle, void *buf, size_t count);

/*
 * Function prototypes
 */


hUsart_t hUsart_init(uint32_t UARTIndex, uint32_t baudRate, uint8_t parity);

void hUsart_Destroy(hUsart_t handler);

void hUsart_transmit(hUsart_t handler, uint8_t *buffer, size_t len);

void hUsart_receive(hUsart_t handler, uint8_t *buffer, size_t len, uint8_t timeout);

void hUsart_transmit_IT(hUsart_t handler, uint8_t *buffer, size_t len);

void hUsart_receive_IT(hUsart_t handler, uint8_t *buffer, size_t len);

void hUsart_receive_abort(hUsart_t handler);

#endif /* NRF_DRIVER_HUSART_H */
