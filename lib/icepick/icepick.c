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

#define _CMD_LEN                                6

#define _CONNECT_CODE                           0x89


/**
 * Typedefs
 */


/**
 * Privates
 */

static uint64_t _IrShift(icepick_t *ice, uint64_t data, uint64_t len){
    if (ice->fxn.fxnIrShift == NULL)
        return 0x00;

    if (ice->IssuedCmd != data){
        ice->IssuedCmd = data;
        return ice->fxn.fxnIrShift(ice->linkHandle, data, len);
    }

    return 0x0;
}

static uint64_t _DrShift(icepick_t *ice, uint64_t data, uint64_t len){
    if (ice->fxn.fxnDrShift == NULL)
        return 0x00;

    return ice->fxn.fxnDrShift(ice->linkHandle, data, len);
}

//uint32_t __icepick_set_router(icepick_t *ice, uint8_t block, uint8_t reg, uint32_t value){
//    uint32_t ret;
//    _icepick_router_scan_t router = {0};
//
//    router.WrEn = _WRITE_ENABLED;
//    router.blockSel = block;
//    router.regNumber = reg;
//    router.regValue = value;
//
//    _IrShift(ice, ICEPICK_CMD_ROUTER, _CMD_LEN);
//    ret = _DrShift(ice, router._raw, 32);
//
//    return ret;
//}
//
//uint32_t __icepick_get_router(icepick_t *ice, uint8_t block, uint8_t reg){
//    uint32_t ret;
//    _icepick_router_scan_t router = {0};
//
//    router.WrEn = _WRITE_DISABLED;
//    router.blockSel = block;
//    router.regNumber = reg;
//
//    _IrShift(ice, ICEPICK_CMD_ROUTER, _CMD_LEN);
//    ret = _DrShift(ice, router._raw, 32);
//
//    return ret;
//}

/**
 * Publics
 */


void icepick_init(icepick_t *ice){
    if (ice == NULL)
        return;

//    _IrShift(ice, ice->cmd.idCode, _CMD_LEN);
//    ice->info.DeviceId._raw = _DrShift(ice, 0x0, 32);
//
//    _IrShift(ice, ice->cmd.icePickCode, _CMD_LEN);
//    ice->info.IdCode._raw = _DrShift(ice, 0x0, 32);
//
//    _IrShift(ice, ICEPICK_CMD_CONNECT, _CMD_LEN);
//    _DrShift(ice, _CONNECT_CODE, 8);
}

//uint32_t icepick_router(icepick_t *ice){
//    uint32_t ret;
//    _icepick_sdtr_t sdtrSend, sdtrRec;
//
//    if (ice == NULL)
//        return 0;
//    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
//        return 0;
//
//    sdtrSend._raw = 0x1000000;
//    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);
//    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);
//
//    sdtrSend._raw = 0x21000;
//    sdtrRec._raw = __icepick_set_router(ice, 0, 0x1, sdtrSend._raw);
//    sdtrRec._raw = __icepick_get_router(ice, 0, 0x1);
//
//    _IrShift(ice, ICEPICK_CMD_BYPASS, _CMD_LEN);
//
//    sdtrSend._raw = 0x0;
//    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
//    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
//
//    sdtrSend._raw = 0x12008;
//    sdtrRec._raw = __icepick_set_router(ice, 2, 0x0, sdtrSend._raw );
//    sdtrRec._raw = __icepick_set_router(ice, 2, 0x0, sdtrSend._raw );
//
//    sdtrSend._raw = 0x0;
//    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
//    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
//
//
//    sdtrSend._raw = 0x2108;
//    sdtrRec._raw = __icepick_set_router(ice, 2, 0x00, sdtrSend._raw);
//
//    sdtrSend._raw = 0x0;
//    sdtrRec._raw = __icepick_get_router(ice, 2, 0x00);
//
//    _IrShift(ice, ICEPICK_CMD_BYPASS, _CMD_LEN);
//
//    _IrShift(ice, 0x01, 1);
//    _DrShift(ice, 0x80, 8);
//
//    _IrShift(ice, 0x030020, 8*3);
//    ret = _DrShift(ice, 0x20000001, 8*3);
//    ret = _DrShift(ice, 0x20000001, 33);
//
//    return sdtrRec._raw;
//}
//
//uint32_t icepick_bypass(icepick_t *ice, uint32_t data, uint32_t len){
//    uint32_t ret;
//    if (ice == NULL)
//        return 0;
//    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
//        return 0;
//
//    _IrShift(ice, ICEPICK_CMD_BYPASS, _CMD_LEN);
//    ret = (_DrShift(ice, data, len))>>1;
//
//    return ret;
//}

uint32_t icepick_issueCMD(icepick_t *ice, icepick_cmd_e cmd, uint32_t *dataO, uint32_t *lenO){
    if (ice == NULL)
        return 1;

    if (dataO != NULL)
        *dataO = cmd;
    if (lenO != NULL)
        *lenO = _CMD_LEN;
    return _IrShift(ice, ICEPICK_CMD_BYPASS, _CMD_LEN);
}

uint32_t icepick_router(icepick_t *ice, icepick_router_scan_t params, uint32_t *dataO, uint32_t *lenO){
    if (ice == NULL)
        return 1;

    if (dataO != NULL)
        *dataO = params._raw;
    if (lenO != NULL)
        *lenO = 32;
    return _DrShift(ice, params._raw, 32);
}

uint32_t icepick_connect(icepick_t *ice, uint32_t *dataO, uint32_t *lenO){
    if (ice == NULL)
        return 1;

    if (dataO != NULL)
        *dataO = _CONNECT_CODE;
    if (lenO != NULL)
        *lenO = 8;
    return _DrShift(ice, _CONNECT_CODE, 8);
}

uint32_t icepick_bypass(icepick_t *ice, uint32_t data, uint32_t len){
    if (ice == NULL)
        return 1;

    return _DrShift(ice, data, len);
}

uint32_t icepick_idcode(icepick_t *ice, uint32_t *idcode){
    uint32_t d;

    if (ice == NULL)
        return 1;

    d = _DrShift(ice, 0x00, 32);
    if (idcode != NULL)
        *idcode = d;

    return d;
}

uint32_t icepick_icecode(icepick_t *ice, uint32_t *idcode){
    uint32_t d;

    if (ice == NULL)
        return 1;

    d = _DrShift(ice, 0x0, 32);
    if (idcode != NULL)
        *idcode = d;

    return d;
}
