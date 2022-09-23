/*
 * gpio.h
 *
 *  Created on: 6 de set de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_COMMON_GPIO_H_
#define LIB_COMMON_GPIO_H_

#include <stdio.h>


/**
 * Typedefs
 */

    /**
     * GPIO TYPEDEFS
     */
    // 1 for Input, 0 for Output
    typedef void(*GpioSetIO)(uint8_t InO);
    // 0 for LOW, 1 for HIGH
    typedef void(*GpioOut)(uint8_t state);
    // 0 fir LOW, 1 for HIGH
    typedef uint8_t(*GpioIn)(void);

    /**
     * DELAY TYPEDEFS
     */
    typedef void(*DelayUs)(uint32_t us);
    typedef void(*DelayMs)(uint32_t ms);

    /**
     * Types
     */
    typedef uint16_t word;



/* */
#endif
