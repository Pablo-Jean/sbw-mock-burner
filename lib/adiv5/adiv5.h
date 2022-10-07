/*
 * adiv5.h
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_ADIV5_ADIV5_H_
#define LIB_ADIV5_ADIV5_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/typedefs.h"

#include "adiv5_define.h"

/**
 * Enumerates
 */

typedef enum{
    ADIV5_CMD_ABORT     = 0x8,
    ADIV5_CMD_DPACC     = 0xA,
    ADIV5_CMD_APACC     = 0xB,
    ADIV5_CMD_IDCODE    = 0xE,
    ADIV5_CMD_BYPASS    = 0xF,
    ADIV5_CMD_EXTEST    = 0x0,
    ADIV5_CMD_SAMPLE    = 0x1,
    ADIV5_CMD_PRELOAD   = 0x2,
    ADIV5_CMD_INTEST    = 0x4,
    ADIV5_CMD_CLAMP     = 0x5,
    ADIV5_CMD_HIGHZ     = 0x6,
    ADIV5_CMD_CLAMPZ    = 0x7
}adiv5_cmd_e;

/**
 * Typedefs
 */

typedef uint64_t(*_fxnTransfer)(uint64_t data, uint8_t l);

typedef struct{
    uint32_t abort;
    uint32_t dpacc;
    uint32_t apacc;
    uint32_t idcode;
    uint32_t bypass;
    uint32_t extest;
    uint32_t sample;
    uint32_t preload;
    uint32_t intest;
    uint32_t clamp;
    uint32_t highz;
    uint32_t clampz;
}adiv5_cmd_t;

typedef struct{
    _fxnTransfer IrShift;
    _fxnTransfer DrShift;
}adiv5_fxn_t;


typedef struct{
    adiv5_cmd_e IssueCmd;
    adiv5_fxn_t fxn;
    uint32_t ap_bank_vale;
    uint32_t ap_current;
}adiv5_t;

/**
 * Public Functions
 */

void adiv5_init(adiv5_t *adiv5);

void adiv5_ap_read(adiv5_t *adiv5, uint32_t reg, uint32_t *data);

void adiv5_ap_write(adiv5_t *adiv5, uint32_t addr, uint32_t data);

void adiv5_dp_read(adiv5_t *adiv5, uint32_t addr, uint32_t *data);

void adiv5_dp_write(adiv5_t *adiv5, uint32_t addr, uint32_t data);


#endif /* LIB_ADIV5_ADIV5_H_ */
