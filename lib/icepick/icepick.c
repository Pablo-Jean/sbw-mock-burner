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

#define _DrShift(handle, data, len)            ice->fxn.fxnDrShift(handle, data, len)
#define _IrShift(handle, data, len)            ice->fxn.fxnIrShift(handle, data, len)


/**
 * Privates
 */



/**
 * Publics
 */


void icepick_init(icepick_t *ice){
    if (ice == NULL)
        return;
    if (ice->fxn.fxnInit == NULL || ice->fxn.fxnDrShift == NULL || ice->fxn.fxnIrShift == NULL)
        return;

    ice->fxn.fxnInit(ice->linkHandle, CJTAG_MODE_4PIN);

    _IrShift(ice->linkHandle, 0x4, 6);
    ice->info.DeviceId._raw = _DrShift(ice->linkHandle, 0x0, 32);

    _IrShift(ice->linkHandle, 0x5, 6);
    ice->info.IdCode._raw = _DrShift(ice->linkHandle, 0x0, 32);
}
