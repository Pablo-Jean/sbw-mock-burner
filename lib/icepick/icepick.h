/*
 * icepick.h
 *
 *  Created on: 28 de set de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_ICEPICK_ICEPICK_H_
#define LIB_ICEPICK_ICEPICK_H_

/**
 * Includes
 */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "../cjtag/cjtag.h"

#include "icepick_defines.h"


/**
 * enumerates
 */

typedef enum{
    ICEPICK_DISCONNECTED,
    ICEPICK_CONNECTED
}icepick_connected_e;

typedef enum{
    MANUF_TEXAS_INST = 0x2F,

    MANUF_UNKNOWN = 0xFF
}icepick_manuf_id_e;

/**
 * Typedefs
 */


/**
 * Structs and Unions
 */

typedef union{
    uint32_t _raw;
    struct __attribute__ ((__packed__)){
        uint8_t Capabilities : 4;
        uint16_t ICEPickType : 12;
        uint8_t EMUTAPs      : 4;
        uint8_t TestTAPs     : 4;
        uint8_t Version;
    };
}icepick_idcode_t;

typedef union{
    uint32_t _raw;
    struct __attribute__ ((__packed__)){
        uint16_t Manufacturer : 12;
        uint16_t PartNumber;
        uint8_t version : 4;
    };
}icepick_device_id_t;

/*
 * Fxn structs
 */

typedef void(*_fxnInit)(void*, uint8_t);
typedef uint64_t(*_fxnDrShift)(void*, uint64_t, uint8_t);
typedef uint64_t(*_fxnIrShift)(void*, uint64_t, uint8_t);

typedef struct{
    _fxnInit fxnInit;
    _fxnDrShift fxnDrShift;
    _fxnIrShift fxnIrShift;
}icepick_fxn_t;

/*
 * General Struct
 */

typedef struct{
    icepick_fxn_t fxn;
    void *linkHandle;
    struct{
        icepick_device_id_t DeviceId;
        icepick_idcode_t IdCode;
    }info;
}icepick_t;


/**
 * Function prototypes
 */

void icepick_init(icepick_t *ice);

void icepick_router(icepick_t *ice);

#endif /* LIB_ICEPICK_ICEPICK_H_ */
