/*
 * dap.c
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */


#include <jtag-dp.h>

/**
 * Macros
 */

static uint64_t _IrShift(adiv5_t *dap, uint64_t data, uint8_t len){
    if (dap->fxn.IrShift == NULL)
        return 0;
    return dap->fxn.IrShift(dap->target, data, len);
}

static uint64_t _DrShift(adiv5_t *dap, uint64_t data, uint8_t len){
    if (dap->fxn.DrShift == NULL)
        return 0;
    return dap->fxn.DrShift(dap->target, data, len);
}

/**
 * Privates
 */

adiv5_err_e __adiv5_jtag_dp_scan_out(adiv5_t *dap, adiv5_cmd_params_t *params, uint32_t *data, uint8_t *ack){
    uint32_t sel = (params->regAddr >> 4) & 0xf;
    uint64_t dr, s, ov;

    if (params->cmd == ADIV5_CMD_DPACC && params->regAddr == DP_SELECT && params->regAddr == DP_RDBUFF && sel != dap->ap_select){
        dr = DPAP_WRITE | ((DP_SELECT>>1) & 0x6) | (sel << 3);
        _IrShift(dap, ADIV5_CMD_DPACC, 4);
        _DrShift(dap, dr, 35);
        dap->ap_select = sel;
    }

    ov = params->outVal; //needed because of the overflow of the 32bits of uint32_t type
    dr = (uint64_t)(params->rnw) | (uint64_t)((params->regAddr>>1) & 0x6) | (uint64_t)(ov << 3);
    if (dap->IssueCmd != params->cmd){
        _IrShift(dap, params->cmd, 4);
    }
    s = _DrShift(dap, dr, 35);
    if (params->rnw == DPAP_READ && dap->regAddr != MEM_AP_REG_DRW)
        s = _DrShift(dap, dr, 35);

    if (ack)
        *ack = (s&0x7);
    if (data)
        *data = (s>>3);
    dap->IssueCmd = params->cmd;
    dap->regAddr = params->regAddr;

    return ADIV5_OK;
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

adiv5_err_e adiv5_idcode(adiv5_t *dap, uint32_t *id){
    if (dap == NULL)
        return ADIV5_FAIL;

    return ADIV5_OK;
}

adiv5_err_e adiv5_ap_read(adiv5_t *dap, uint32_t reg, uint32_t *data){
    uint32_t d;
    uint8_t ack;
    adiv5_cmd_params_t cmdPar;
    if (dap == NULL)
        return ADIV5_FAIL;

    cmdPar.cmd = ADIV5_CMD_APACC;
    cmdPar.regAddr = reg;
    cmdPar.rnw = DPAP_READ;
    cmdPar.outVal = 0x00000000;
    __adiv5_jtag_dp_scan_out(dap, &cmdPar, &d, &ack);

    if (data != NULL)
        *data = d;
    if ( ack == ADIV5_ACK_OK_FAULT )
        return ADIV5_OK;
    return ADIV5_FAIL;
}
//
adiv5_err_e adiv5_ap_write(adiv5_t *dap, uint32_t reg, uint32_t data){
    uint32_t d;
    uint8_t ack;
    adiv5_cmd_params_t cmdPar;
    if (dap == NULL)
        return ADIV5_FAIL;

    cmdPar.cmd = ADIV5_CMD_APACC;
    cmdPar.regAddr = reg;
    cmdPar.rnw = DPAP_WRITE;
    cmdPar.outVal = data;
    __adiv5_jtag_dp_scan_out(dap, &cmdPar, &d, &ack);

    if ( ack == ADIV5_ACK_OK_FAULT ){
        if (reg == MEM_AP_REG_CSW)
            dap->ap_scw._raw = data;
        return ADIV5_OK;
    }
    return ADIV5_FAIL;
}
//
adiv5_err_e adiv5_dp_read(adiv5_t *dap, uint32_t reg, uint32_t *data){
    uint32_t d;
    uint8_t ack;
    adiv5_cmd_params_t cmdPar;
    if (dap == NULL)
        return ADIV5_FAIL;

    cmdPar.cmd = ADIV5_CMD_DPACC;
    cmdPar.regAddr = reg;
    cmdPar.rnw = DPAP_READ;
    cmdPar.outVal = 0x00000000;
    __adiv5_jtag_dp_scan_out(dap, &cmdPar, &d, &ack);

    if (data != NULL)
        *data = d;
    if ( ack == ADIV5_ACK_OK_FAULT )
        return ADIV5_OK;
    return ADIV5_FAIL;
}
//
adiv5_err_e adiv5_dp_write(adiv5_t *dap, uint32_t reg, uint32_t data){
    uint32_t d;
    uint8_t ack;
    adiv5_cmd_params_t cmdPar;
    if (dap == NULL)
        return ADIV5_FAIL;

    cmdPar.cmd = ADIV5_CMD_DPACC;
    cmdPar.regAddr = reg;
    cmdPar.rnw = DPAP_WRITE;
    cmdPar.outVal = data;
    __adiv5_jtag_dp_scan_out(dap, &cmdPar, &d, &ack);

    if ( ack == ADIV5_ACK_OK_FAULT )
        return ADIV5_OK;
    return ADIV5_FAIL;
}

adiv5_err_e cc26xx_cm4f_bankselect(adiv5_t *dap, uint32_t reg){
    uint64_t sel;

    if (dap == NULL)
        return ADIV5_FAIL;

    sel = (dap->ap_num << 24) | (reg&0xF0);
    if (sel == dap->ap_select){
        return ADIV5_OK;
    }
    dap->ap_num = sel;
    adiv5_dp_write(dap, DP_SELECT, sel);

    return ADIV5_OK;
}
