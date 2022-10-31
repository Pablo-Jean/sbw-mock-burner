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

typedef enum{
    ICEPICK_CMD_ROUTER        = 0x02,
    ICEPICK_CMD_IDCODE        = 0x04,
    ICEPICK_CMD_ICECODE       = 0x05,
    ICEPICK_CMD_CONNECT       = 0x07,
    ICEPICK_CMD_BYPASS        = 0x3f
}icepick_cmd_e;

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

// abandoned
//typedef struct{
//    uint8_t _size;
//    uint32_t connect;
//    uint32_t idCode;
//    uint32_t icePickCode;
//    uint32_t bypass;
//    uint32_t router;
//}icepick_cmd_t;

/*
 * Fxn structs
 */

typedef uint64_t(*_fxnDrShift)(void* handle, uint64_t data, uint8_t l);
typedef uint64_t(*_fxnIrShift)(void* handle, uint64_t data, uint8_t l);

typedef struct{
    _fxnDrShift fxnDrShift;
    _fxnIrShift fxnIrShift;
}icepick_fxn_t;

/*
 * General Struct
 */

typedef union{
    struct{
        uint8_t TapPresent      :1;
        uint8_t TapAccessible   :1;
        uint8_t _reserved1      :1;
        uint8_t ForceActive     :1;
        uint8_t _reserved2      :4;
        uint8_t SelectTap       :1;
        uint8_t Visibletap      :1;
        uint8_t _reserved3      :4;
        uint8_t ResetControl    :3;
        uint8_t InReset_RelWIR  :1;
        uint8_t _reserved4      :2;
        uint8_t InhibitSleep    :1;
        uint8_t _reserved5      :3;
    };
    uint32_t _raw;
}icepick_sdtr_t;



typedef union{
    uint32_t _raw;
    struct{
        uint32_t regValue :24;
        uint8_t regNumber :4;
        uint8_t blockSel  :3;
        uint8_t WrEn      :1;
    };
}icepick_router_scan_t;

typedef struct{
    icepick_fxn_t fxn;
    void *linkHandle;
    struct{
        icepick_device_id_t DeviceId;
        icepick_idcode_t IdCode;
    }info;
    icepick_cmd_e IssuedCmd;
}icepick_t;


/**
 * Function prototypes
 */

void icepick_init(icepick_t *ice);

uint32_t icepick_issueCMD(icepick_t *ice, icepick_cmd_e cmd, uint32_t *dataO, uint32_t *lenO);

uint32_t icepick_router(icepick_t *ice, icepick_router_scan_t params, uint32_t *dataO, uint32_t *lenO);

uint32_t icepick_connect(icepick_t *ice, uint32_t *dataO, uint32_t *lenO);

uint32_t icepick_bypass(icepick_t *ice, uint32_t data, uint32_t len);

uint32_t icepick_idcode(icepick_t *ice, uint32_t *idcode);

uint32_t icepick_icecode(icepick_t *ice, uint32_t *idcode);

#endif /* LIB_ICEPICK_ICEPICK_H_ */
