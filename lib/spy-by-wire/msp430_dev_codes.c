/*
 * msp430_dev_codes.c
 *
 *  Created on: 19 de jul de 2022
 *      Author: pablo.jean
 */

#include "msp430_dev_codes.h"

msp430_model_address_t msp430_model_address[2] = {
    {
        .ID = MSP430FR2433,
        .ram.start = 0x2000,
        .ram.size = 4096,
        .main.start = 0xC400,
        .main.size = 15359,
        .info.start = 0x1800,
        .info.size = 512,
        .peripherals.start = 0x0000,
        .peripherals.size = 4096
    },
    {
        .ID = MSP430FR2533,
        .ram.start = 0x2000,
        .ram.size = 2048,
        .main.start = 0xC400,
        .main.size = 15359,
        .info.start = 0x1800,
        .info.size = 512,
        .peripherals.start = 0x0000,
        .peripherals.size = 4096
    }
};

