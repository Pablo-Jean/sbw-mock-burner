/*
 * hGpio.c
 *
 *  Created on: 16 de jun de 2021
 *      Author: pablo.jean
 */

#include "../hGpio.h"

#ifdef HDRIVER_MCU_CC2642

#include "ti_drivers_config.h"

#ifndef CONFIG_SYSCONFIG_PREVIEW
#error "SysConfig files not generated"
#endif

/* Drivers */
#include <ti/drivers/GPIO.h>



/*
 * Weak Callback
 */

void __attribute__((weak)) hGpio_Callback(uint32_t pin){

}

/*
 * Callback
 */

void gpioButtonFxn(uint_least8_t index){
    GPIO_clearInt(index);
    hGpio_Callback((uint32_t)index);
}

/*
 * Privates
 */

GPIO_PinConfig __select_interrupt(hGPIO_INTERRUPT_e e){
    GPIO_PinConfig cfg;

    switch (e){
    case hGPIO_INTERRUPT_ON_L_TO_H:
        cfg = GPIO_CFG_IN_INT_RISING;
        break;
    case hGPIO_INTERRUPT_ON_H_TO_L:
        cfg = GPIO_CFG_IN_INT_FALLING;
        break;
    default:
        cfg = GPIO_CFG_IN_INT_NONE;
        break;
    }

    return cfg;
}

/*
 * Publics
 */

void hGpio_enableInterrupt(uint32_t gpio, uint32_t pin){
    GPIO_setCallback(pin, gpioButtonFxn);
    GPIO_clearInt(pin);
    GPIO_enableInt(pin);
}

void hGpio_disableInterrupt(uint32_t gpio, uint32_t pin){
    GPIO_disableInt(pin);
    GPIO_clearInt(pin);
}

void hGpio_changeToInput(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e){
    GPIO_PinConfig cfg;

    cfg = __select_interrupt(e);
    cfg += GPIO_CFG_IN_NOPULL;
    GPIO_setConfig(pin, cfg);
}

void hGpio_changeToInput_withPullDown(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e){
    GPIO_PinConfig cfg;

    cfg = __select_interrupt(e);
    cfg += GPIO_CFG_IN_PD;
    GPIO_setConfig(pin, cfg);
}

void hGpio_changeToInput_withPullUp(uint32_t gpio, uint32_t pin, hGPIO_INTERRUPT_e e){
    GPIO_PinConfig cfg;

    cfg = __select_interrupt(e);
    cfg += GPIO_CFG_IN_PU;
    GPIO_setConfig(pin, cfg);
}

void hGpio_changeToOutput(uint32_t gpio, uint32_t pin){
    GPIO_PinConfig cfg;

    hGpio_disableInterrupt(gpio, pin);
    cfg = GPIO_CFG_OUTPUT;
    GPIO_setConfig(pin, cfg);
}

hGPIO_PIN_STATE_e hGpio_read(uint32_t gpio, uint32_t pin){
    return (hGPIO_PIN_STATE_e)GPIO_read((uint_least8_t) pin);
}

void hGpio_write(uint32_t gpio, uint32_t pin, hGPIO_PIN_STATE_e e){
    GPIO_write((uint_least8_t) pin, (unsigned int)e);
}

void hGpio_toggle(uint32_t gpio, uint32_t pin){
    GPIO_toggle(pin);
}

#endif
