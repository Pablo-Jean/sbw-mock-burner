/*
 * cjtag.h
 *
 *  Created on: 6 de set de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_CJTAG_CJTAG_H_
#define LIB_CJTAG_CJTAG_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <ti/sysbios/knl/Task.h>

#include "../common/typedefs.h"
#include "cjtag_defines.h"

/**
 * Enumerates
 */



/**
 * Structs and Unions
 */


typedef struct{
    GpioSetIO tmsSetDir;
    GpioOut tmsSetIO;
    GpioIn tmsGet;
    GpioOut tckSetIO;
    GpioOut resetSetIO;
}cjtag_gpio_t;

typedef struct{
    cjtag_scan_format_e scanFormat;
    cjtag_fsm_state_e fsmState;
    cjtag_gpio_t pinFxn;
    uint8_t tdo_bit;
    uint8_t rdy_bit;
}cjtag_t;


/**
 * Public Functions
 */

void cjtag_init(cjtag_t *cjtag);

void cjtag_TmsPattent(cjtag_t *cjtag, uint64_t pattern, uint8_t bits);

uint64_t cjtag_writeOscan(cjtag_t *cjtag, uint64_t tms, uint64_t tdi, uint8_t bits);

uint64_t cjtag_ir_shift(cjtag_t *cjtag, uint64_t d, uint8_t l, uint8_t endInIdle);

uint64_t cjtag_dr_shift(cjtag_t *cjtag, uint64_t d, uint8_t l, uint8_t endInIdle);

void cjtag_reset(cjtag_t *cjtag);

void cjtag_release(cjtag_t *cjtag);


#endif /* LIB_CJTAG_CJTAG_H_ */
