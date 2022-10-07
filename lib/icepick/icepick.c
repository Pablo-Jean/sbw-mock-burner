/*
 * icepick.c
 *
 *  Created on: 28 de set de 2022
 *      Author: pablo.jean
 */


#include "icepick.h"


/**
 * Macros
 */

#define _WRITE_ENABLED                          1
#define _WRITE_DISABLED                         0


/**
 * Typedefs
 */

typedef union{
    uint32_t _raw;
    struct{
        uint32_t regValue :24;
        uint8_t regNumber :4;
        uint8_t blockSel  :3;
        uint8_t WrEn      :1;
    };
}_icepick_router_scan_t;

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
}_icepick_sdtr_t;

/**
 * Privates
 */

uint64_t _IrShift(icepick_t *ice, uint64_t data, uint64_t len){
    if (ice->IssuedCmd != data){
        ice->IssuedCmd = data;
        return ice->fxn.fxnIrShift(ice->linkHandle, data, len);
    }

    return 0x0;
}

uint64_t _DrShift(icepick_t *ice, uint64_t data, uint64_t len){
    return ice->fxn.fxnDrShift(ice->linkHandle, data, len);
}

uint32_t __icepick_set_router(icepick_t *ice, uint8_t block, uint8_t reg, uint32_t value){
    uint32_t ret;
    _icepick_router_scan_t router = {0};

    router.WrEn = _WRITE_ENABLED;
    router.blockSel = block;
    router.regNumber = reg;
    router.regValue = value;

    _IrShift(ice, ice->cmd.router, ice->cmd._size);
    ret = _DrShift(ice, router._raw, 32);

    return ret;
}

uint32_t __icepick_get_router(icepick_t *ice, uint8_t block, uint8_t reg){
    uint32_t ret;
    _icepick_router_scan_t router = {0};

    router.WrEn = _WRITE_DISABLED;
    router.blockSel = block;
    router.regNumber = reg;

    _IrShift(ice, ice->cmd.router, ice->cmd._size);
    ret = _DrShift(ice, router._raw, 32);

    return ret;
}

/**
 * Publics
 */


void icepick_init(icepick_t *ice){
    if (ice == NULL)
        return;
    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
        return;

    ice->fxn.fxnInit(ice->linkHandle, CJTAG_MODE_4PIN);

    _IrShift(ice, ice->cmd.idCode, ice->cmd._size);
    ice->info.DeviceId._raw = _DrShift(ice, 0x0, 32);

    _IrShift(ice, ice->cmd.icePickCode, ice->cmd._size);
    ice->info.IdCode._raw = _DrShift(ice, 0x0, 32);

    _IrShift(ice, ice->cmd.connect, ice->cmd._size);
    _DrShift(ice, 0x89, 8);
}

uint32_t icepick_router(icepick_t *ice){
    uint32_t ret;
    _icepick_sdtr_t sdtrSend, sdtrRec;

    if (ice == NULL)
        return 0;
    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
        return 0;

    sdtrSend._raw = 0x1000000;
    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);
    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);

    sdtrSend._raw = 0x21000;
    sdtrRec._raw = __icepick_set_router(ice, 0, 0x1, sdtrSend._raw);
    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);

    _IrShift(ice, ice->cmd.bypass, ice->cmd._size);

    sdtrSend._raw = 0x0;
    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);

    sdtrSend._raw = 0x12008;
    sdtrRec._raw = __icepick_set_router(ice, 2, 0x0, sdtrSend._raw );
    sdtrRec._raw = __icepick_set_router(ice, 2, 0x0, sdtrSend._raw );

    sdtrSend._raw = 0x0;
    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);


    sdtrSend._raw = 0x2108;
    sdtrRec._raw = __icepick_set_router(ice, 2, 0x00, sdtrSend._raw);

    sdtrSend._raw = 0x0;
    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);

    _IrShift(ice, ice->cmd.bypass, ice->cmd._size);

    _IrShift(ice, 0x01, 1);
    _DrShift(ice, 0x80, 8);

    _IrShift(ice, 0x030020, 8*3);
    ret = _DrShift(ice, 0x20000001, 8*3);
    ret = _DrShift(ice, 0x20000001, 33);

    return sdtrRec._raw;
}

uint32_t icepick_bypass(icepick_t *ice, uint32_t data, uint32_t len){
    uint32_t ret;
    if (ice == NULL)
        return 0;
    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
        return 0;

    _IrShift(ice, ice->cmd.bypass, ice->cmd._size);
    ret = (_DrShift(ice, data, len))>>1;

    return ret;
}
