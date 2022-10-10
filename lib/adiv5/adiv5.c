/*
 * dap.c
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */


#include "dap.h"

/**
 * Privates
 */

static void __IrShift(adiv5_t *dap, uint64_t data, uint8_t l){
    dap->fxn.IrShift(data, l);
}

static uint64_t __DrShift(adiv5_t *dap, uint64_t data, uint8_t l){
    return dap->fxn.DrShift(data, l);
}

void __bank_select(adiv5_t *dap, uint32_t reg){
    uint32_t select_ap_bank;

    select_ap_bank = reg & 0x000000F0;
    if (dap->ap_bank_vale == select_ap_bank)
        return;
    dap->ap_bank_vale = select_ap_bank;
    select_ap_bank |= dap->ap_current;

    //__IrShift(dap, ADIV5_CMD_DPACC, 4);
    //__DrShift(dap, select_ap_bank, 32);
}


/**
 * Publics
 */

void adiv5_init(adiv5_t *dap){
    // what can i DO?????
    if (dap == NULL)
        return;

    dap->IssueCmd = ADIV5_CMD_BYPASS;
}

uint64_t adiv5_ap_read(adiv5_t *dap, uint64_t reg, uint64_t *data){
    uint32_t select_ap_bank;

    if (dap == NULL || dap->fxn.IrShift == NULL || dap->fxn.DrShift == NULL)
        return;

    __bank_select(dap, reg);

    //__IrShift(dap, ADIV5_CMD_APACC, 4);
    //__DrShift(dap, reg, 32);
}

void adiv5_ap_write(adiv5_t *dap, uint64_t addr, uint64_t data){
    if (dap == NULL || dap->fxn.IrShift == NULL || dap->fxn.DrShift == NULL)
        return;
}

uint64_t adiv5_dp_read(adiv5_t *dap, uint64_t addr, uint64_t *data){
    if (dap == NULL || dap->fxn.IrShift == NULL || dap->fxn.DrShift == NULL)
        return;
}

void adiv5_dp_write(adiv5_t *dap, uint64_t addr, uint64_t data){
    if (dap == NULL || dap->fxn.IrShift == NULL || dap->fxn.DrShift == NULL)
        return;
}
