/*
 * hGpio.h
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#ifndef TI_HDRIVERS_HGPIO_H_
#define TI_HDRIVERS_HGPIO_H_


#include <stdint.h>
#include <stdlib.h>

/*
 * Enums
 */

typedef enum {
    hGPIO_PIN_LOW,
    hGPIO_PIN_HIGH
}hGPIO_PIN_STATE_e;

typedef enum {
    hGPIO_INTERRUPT_OFF,
    hGPIO_INTERRUPT_ON_L_TO_H,
    hGPIO_INTERRUPT_ON_H_TO_L,
    hGPIO_INTERRUPT_HIGH,
    hGPIO_INTERRUPT_LOW,
    hGPIO_INTERRUPT_ON_BOTH,
}hGPIO_INTERRUPT_e;

typedef enum {
    hGPIO_EDGE_L_TO_H,
    hGPIO_EDGE_H_TO_L
}hGPIO_EDGE_INT_e;

/*
 * Functions prototypes
 */

void hGpio_enableInterrupt(uint32_t gpio, uint32_t pin);

void hGpio_disableInterrupt(uint32_t gpio, uint32_t pin);

void hGpio_changeToInput(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e);

void hGpio_changeToInput_withPullDown(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e);

void hGpio_changeToInput_withPullUp(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e);

void hGpio_changeToOutput(uint32_t gpio, uint32_t pin);

void hGpio_write(uint32_t gpio, uint32_t pin, hGPIO_PIN_STATE_e e);

void hGpio_toggle(uint32_t gpio, uint32_t pin);

hGPIO_PIN_STATE_e hGpio_read(uint32_t gpio, uint32_t pin);

//callback
void hGpio_Callback(uint32_t pin);


#endif /* TI_HDRIVERS_HGPIO_H_ */
