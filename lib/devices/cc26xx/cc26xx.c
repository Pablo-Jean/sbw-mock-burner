/*
 * cc26xx.c
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */

#include "cc26xx.h"


/**
 * Privates
 */

#define _bitMask_64  0xFFFFFFFFFFFFFFFF

char _tap_name[CC26XX_TAP_QTD][15] = { "TEST",
                                       "PBIST1",
                                       "PBIST2",
                                       "EFUSE",
                                       "reserved",
                                       "AON_WUC",
                                       "CM4F"};

static uint64_t _IrShift(cc26xx_t *chipset, uint64_t data, uint64_t len){
    cjtag_t *cjtag = chipset->cjtag;

    return cjtag_ir_shift(cjtag, data, len);
}

static uint64_t _DrShift(cc26xx_t *chipset, uint64_t data, uint64_t len, uint8_t endInPause){
    cjtag_t *cjtag = chipset->cjtag;

    return cjtag_dr_shift(cjtag, data, len, endInPause);
}

static uint64_t _IrShiftTap(cc26xx_t *chipset, cc26xx_tap_e tap, uint64_t data, uint64_t len){
    uint64_t d, l;
    uint8_t i;

    d = 0;
    l = 0;
    if (tap == CC26XX_TAP_ICEPICK){
         d |= data;
         l += len;
    }
    else{
        icepick_issueCMD(&chipset->icepick, ICEPICK_CMD_BYPASS, (uint32_t*)&d, (uint32_t*)&l);
    }
    for (i=0 ; i<CC26XX_TAP_QTD ; i++){
        if (chipset->taps[i].tapOn){
            if ((cc26xx_tap_e)i == tap){
                d <<= len;
                d |= (data);
                l += len;
            }
            else{
                // it's good to check if the TAP is in BYPASS
                if (1){
                    d <<= 4;
                    d |= (0xF);
                    l += 4;
                }
            }
        }
    }
    return _IrShift(chipset, d, l);
}

static uint64_t _DrShiftTap(cc26xx_t *chipset, cc26xx_tap_e tap, uint64_t data, uint64_t len, uint8_t endInPause){
    uint64_t d, l;
    int8_t i;

    d = 0;
    l = 0;
    if (tap == CC26XX_TAP_ICEPICK){
         d |= data;
         l += len;
    }
    else{
        d |= 1;
        l++;
    }
    for (i=0 ; i<CC26XX_TAP_QTD ; i++){
        if (chipset->taps[i].tapOn){
            if ((cc26xx_tap_e)i == tap){
                d <<= len;
                d |= (data);
                l += len;
            }
            else{
                // it's good to check if the TAP is in BYPASS
                if (1){
                    d <<= 1;
                    d |= (0x1);
                    l += 1;
                }
            }
        }
    }
    data = _DrShift(chipset, d, l, endInPause);
    if (tap == CC26XX_TAP_ICEPICK){
        data >>= (l-len);
    }
    else{
        for (i=CC26XX_TAP_QTD ; i>0 ; i--){
            if (chipset->taps[i].tapOn){
                if ((cc26xx_tap_e)i == tap){
                    data = data&(_bitMask_64>>(64-len));
                    break;
                }
                else{
                    data >>= 1;
                }
            }
        }
    }
    return data;
}

uint64_t __IrShift_CM4F(cc26xx_t *chipset, uint64_t data, uint8_t l){
    return _IrShiftTap(chipset, CC26XX_TAP_CM4F, data, l);
}

uint64_t __DrShift_CM4F(cc26xx_t *chipset, uint64_t data, uint8_t l){
    return _DrShiftTap(chipset, CC26XX_TAP_CM4F, data, l, 0);
}

uint32_t __get_algo_param_addr(cc26xx_t *chipset){
    if (chipset->algo.index == 0){
        return (CC26X2_ALGO_PARAMS_0);
    }
    else{
        return (CC26X2_ALGO_PARAMS_1);
    }
}

uint32_t __get_algo_buffer_addr(cc26xx_t *chipset){
    if (chipset->algo.index == 0){
        return (CC26X2_ALGO_BUFFER_0);
    }
    else{
        return (CC26X2_ALGO_BUFFER_1);
    }
}

void __algo_index_rollout(cc26xx_t *chipset){
    chipset->algo.index ^= 1;
}

cc26xx_err_e __algo_wait(cc26xx_t *chipset){
    uint32_t regsel, status;
    cc26x2_lago_params_t par;
    arm7_t *arm = &chipset->arm7;

    regsel = __get_algo_param_addr(chipset);
    regsel += CC26XX_STATUS_OFFSET;
    do{
        arm7_get_reg(arm, regsel, &status);
    } while (status != CC26XX_BUFFER_EMPTY);
}

cc26xx_err_e __algo_write(cc26xx_t *chipset, uint32_t address, uint32_t *data, uint32_t len){
    cc26x2_lago_params_t par;
    uint32_t regsel, count, size, i;
    uint32_t secLen_32 = CC26X2_SECTOR_LENGTH/4;
    arm7_t *arm = &chipset->arm7;

    par.command = CC26XX_CMD_PROGRAM;

    count = len/4;
    i = 0;
    while(count > 0){
        if (count > secLen_32)
            size = secLen_32;
        else
            size = count;
        regsel = __get_algo_buffer_addr(chipset);
        arm7_set_reg_buffered(arm, regsel, &(data[i]), size);

        regsel = __get_algo_param_addr(chipset);
        par.address = address;
        par.length = (size*4);
        par.status = CC26XX_BUFFER_FULL;
        arm7_set_reg_buffered(arm, regsel, par._raw, 4);
        __algo_wait(chipset);

        __algo_index_rollout(chipset);

        count -= size;
        address += (size*4);
        i += size;
    }

    return CC26XX_OK;
}

cc26xx_err_e _algo_erase_all(cc26xx_t *chipset){
    cc26x2_lago_params_t par;
    uint32_t regsel, i;
    arm7_t *arm;

//    for (i=0 ; i<5 ; i++){
        arm = &chipset->arm7;
        regsel = __get_algo_param_addr(chipset);
        par.address = 0x0;
        par.length = 0;
        par.command = CC26XX_CMD_ERASE_ALL;
        par.status = CC26XX_BUFFER_FULL;
        arm7_set_reg_buffered(arm, regsel, par._raw, 4);
        __algo_wait(chipset);
        __algo_index_rollout(chipset);
//    }

    return CC26XX_OK;
}

/*
 * Initialize fxns
 */
void __jtag_init(cc26xx_t *chipset){
    cjtag_init(chipset->cjtag, CJTAG_MODE_4PIN);
}

void __icepick_init(cc26xx_t *chipset){
    icepick_t *ice = &chipset->icepick;
    uint32_t data, len;

    ice->linkHandle = (void*)chipset->cjtag;
    icepick_init(ice);

    icepick_issueCMD(ice, ICEPICK_CMD_IDCODE, &data, &len);
    _IrShift(chipset, data, len);
    chipset->IDs.deviceId._raw = _DrShift(chipset, 0x0, 32, 0);

    icepick_issueCMD(ice, ICEPICK_CMD_ICECODE, &data, &len);
    _IrShift(chipset, data, len);
    chipset->IDs.iceCode._raw = _DrShift(chipset, 0x0, 32, 0);

    icepick_issueCMD(ice, ICEPICK_CMD_CONNECT, &data, &len);
    _IrShift(chipset, data, len);
    icepick_connect(ice, &data, &len);
    _DrShift(chipset, data, len, 0);
}

void __taps_initialize(cc26xx_t *chipset){
    uint8_t i;

    for (i=0 ; i<CC26XX_TAP_QTD ; i++){
        strcpy(chipset->taps[i].name, _tap_name[i]);
        chipset->taps[i].tapOn = 0;
        chipset->taps[i].cmd = (uint32_t*)&chipset->taps[i].adi.IssueCmd;
        adiv5_init(&chipset->taps[i].adi);
    }
}

cc26xx_err_e __tap_cfg(cc26xx_t *chipset, cc26xx_tap_e e, icepick_sdtr_t *param){
    icepick_router_scan_t router;
    icepick_sdtr_t resp;
    icepick_t *ice;
    uint32_t data, len;

    if (chipset == NULL || param == NULL)
        return CC26XX_ERR_VAR;
    if (e >= CC26XX_TAP_QTD)
        return CC26XX_NOT_FOUND;

    ice = &chipset->icepick;
    icepick_issueCMD(ice, ICEPICK_CMD_ROUTER, &data, &len);
    _IrShiftTap(chipset, CC26XX_TAP_ICEPICK, data, len);

    if (e == CC26XX_TAP_CM4F){
        router.blockSel = 2;
        router.regNumber = 0;
    }
    else{
        router.blockSel = 1;
        router.regNumber = (uint8_t)e;
    }
    resp._raw = _DrShiftTap(chipset, CC26XX_TAP_ICEPICK, router._raw, 32, 0);
    resp._raw = _DrShiftTap(chipset, CC26XX_TAP_ICEPICK, router._raw, 32, 0);

    router.WrEn = 1;
    router.regValue = param->_raw;
    resp._raw = _DrShiftTap(chipset, CC26XX_TAP_ICEPICK, router._raw, 32, 0);

//    _IrShift(chipset, 0xFFFFFFFF, 32);

    if (param->SelectTap){
        chipset->taps[e].tapOn = 1;
    }
    else{
        chipset->taps[e].tapOn = 0;
    }

    icepick_issueCMD(ice, ICEPICK_CMD_ROUTER, &data, &len);
    _IrShiftTap(chipset, CC26XX_TAP_ICEPICK, data, len);
    router.WrEn = 0;
    resp._raw = _DrShiftTap(chipset, CC26XX_TAP_ICEPICK, router._raw, 32, 0);
    resp._raw = _DrShiftTap(chipset, CC26XX_TAP_ICEPICK, router._raw, 32, 0);

    // check if the Tap exists and is on scan path
    if (resp.TapAccessible && resp.Visibletap){
        chipset->taps[e].tapOn = 1;
    }
    else{
        chipset->taps[e].tapOn = 0;
    }

    icepick_issueCMD(ice, ICEPICK_CMD_BYPASS, &data, &len);
    _IrShiftTap(chipset, CC26XX_TAP_ICEPICK, data, len);
    if (param->SelectTap == resp.Visibletap){
        return CC26XX_OK;
    }

    return CC26XX_FAILED;
}

//
//cc26xx_err_e __adiv5_jtag_dp_scan_out(cc26xx_t *chipset, adiv5_cmd_params_t *params, uint32_t *data, uint8_t *ack){
//    uint32_t sel = (params->regAddr >> 4) & 0xf;
//    uint64_t dr, s, ov;
//
////    cc26xx_cm4f_bankselect(chipset, chipset->adiv5.regAddr);
//    if (params->cmd == ADIV5_CMD_DPACC && params->regAddr == DP_SELECT && params->regAddr == DP_RDBUFF && sel != chipset->adiv5.ap_select){
//        dr = DPAP_WRITE | ((DP_SELECT>>1) & 0x6) | (sel << 3);
//        _IrShiftTap(chipset, CC26XX_TAP_CM4F, ADIV5_CMD_DPACC, 4);
//        _DrShiftTap(chipset, CC26XX_TAP_CM4F, dr, 35, 0);
//        chipset->adiv5.ap_select = sel;
//    }
//
//    ov = params->outVal; //needed because of the overflow of the 32bits of uint32_t type
//    dr = (uint64_t)(params->rnw) | (uint64_t)((params->regAddr>>1) & 0x6) | (uint64_t)(ov << 3);
//    if (chipset->adiv5.IssueCmd != params->cmd){
//        _IrShiftTap(chipset, CC26XX_TAP_CM4F, params->cmd, 4);
//    }
//    s = _DrShiftTap(chipset, CC26XX_TAP_CM4F, dr, 35, 0);
//    if (params->rnw == DPAP_READ && chipset->adiv5.regAddr != MEM_AP_REG_DRW)
//        s = _DrShiftTap(chipset, CC26XX_TAP_CM4F, dr, 35, 0);
//
//    if (ack)
//        *ack = (s&0x7);
//    if (data)
//        *data = (s>>3);
//    chipset->adiv5.IssueCmd = params->cmd;
//    chipset->adiv5.regAddr = params->regAddr;
//
//    return CC26XX_OK;
//}

/*
 * Command and response process
 */

/**
 * Publics
 */

cc26xx_err_e cc26xx_init(cc26xx_t *chipset){
    icepick_sdtr_t sdtr;

    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;

    chipset->adiv5.ap_num = 0x0;
    chipset->adiv5.regAddr = 0xFF;
    chipset->adiv5.fxn.DrShift = __DrShift_CM4F;
    chipset->adiv5.fxn.IrShift = __IrShift_CM4F;
    chipset->adiv5.target = (void*)(chipset);

    chipset->arm7.link = (void*)(&chipset->adiv5);
    chipset->arm7.linkType = ARM7_LINK_JTAGDP;

    chipset->algo.started = 0;
    chipset->algo.index = 0;

    __jtag_init(chipset);
    __icepick_init(chipset);
    __taps_initialize(chipset);

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_get_id(cc26xx_t *chipset, cc26xx_id_source_e e, uint32_t *id){
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;

//    _IrShift(chipset, ADIV5_CMD_DPACC, 4);
    //_DrShift(chipset, data, len)
}

cc26xx_err_e cc26xx_cfg_tap(cc26xx_t *chipset, cc26xx_tap_e e, icepick_sdtr_t *param){
    if (chipset == NULL || chipset->cjtag == NULL || param == NULL)
        return CC26XX_ERR_VAR;

    return __tap_cfg(chipset, e, param);
}

cc26xx_err_e cc26xx_cm4f_idcode(cc26xx_t *chipset, uint32_t *idcode){
    uint64_t d;
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;

    _IrShiftTap(chipset, CC26XX_TAP_CM4F, ADIV5_CMD_IDCODE, 4);
    d = _DrShiftTap(chipset, CC26XX_TAP_CM4F, 0x0, 32, 0);

    if (idcode != NULL)
        *idcode = d;

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_cm4f_route(cc26xx_t *chipset){
    icepick_sdtr_t sdtr;
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;

    sdtr._raw = 0x28233F;
    cc26xx_cfg_tap(chipset, CC26XX_TAP_CM4F, &sdtr);

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_cm4f_reg_write(cc26xx_t *chipset, uint32_t reg, uint32_t data){
    arm7_t *arm;
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;

    arm = &chipset->arm7;
//    cc26xx_cm4f_dp_write(chipset, DP_SELECT, 0x00);
//
//    cc26xx_cm4f_ap_write(chipset, MEM_AP_REG_TAR, reg);
//    cc26xx_cm4f_ap_write(chipset, MEM_AP_REG_DRW, data);
//
//    cc26xx_cm4f_dp_read(chipset, DP_CTRL_STAT, &d);
//    cc26xx_cm4f_dp_read(chipset, DP_RDBUFF, &d);
    arm7_set_reg(arm, reg, data);

    return CC26XX_OK;
}


cc26xx_err_e cc26xx_cm4f_reg_read(cc26xx_t *chipset, uint32_t reg, uint32_t *data){
    arm7_t *arm;
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;

    arm = &chipset->arm7;
//    cc26xx_cm4f_dp_write(chipset, DP_SELECT, 0x00);

//    cc26xx_cm4f_ap_write(chipset, MEM_AP_REG_TAR, reg);
//    cc26xx_cm4f_ap_read(chipset, MEM_AP_REG_DRW, &d);
//    if (data != NULL)
//        *data = d;
//
//    cc26xx_cm4f_dp_read(chipset, DP_CTRL_STAT, &d);
//    cc26xx_cm4f_dp_read(chipset, DP_RDBUFF, &d);
    arm7_get_reg(arm, reg, data);

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_cm4f_start_script(cc26xx_t *chipset){
    arm7_t *arm = &chipset->arm7;
    uint32_t *algo32 = (uint32_t*)cc26x2_algo;
    uint32_t addr = CC26XX_ALGO_ADDRESS;
    uint32_t size = cc26x2_algo_len/4;
    uint32_t i, test[10], check;

    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;

    for (i = 0 ; i < size ; i++){
       arm7_set_reg(arm, addr, algo32[i]);
       addr +=4 ;
    }
//
    addr = CC26XX_ALGO_ADDRESS;
    for (i = 0 ; i < 10 ; i++){
        arm7_get_reg(arm, addr, &test[i]);
        addr += 4;
    }

    // enable DBG and HALT core
    arm7_set_reg(arm, ARM7_CPU_SCS_DHCSR, ARM7_CPU_SCS_DHCSR_PASSWRD | (0x3 << 0));

//
    arm7_set_core_reg(arm, ARMV7M_xPSR, 0x01000000);
    arm7_set_core_reg(arm, ARMV7M_PC, CC26XX_ALGO_ADDRESS);

    arm7_get_core_reg(arm, ARMV7M_xPSR, &check);
    arm7_get_core_reg(arm, ARMV7M_PC, &check);
//
    // unhalt target, but keep debug online
    arm7_set_reg(arm, ARM7_CPU_SCS_DHCSR, ARM7_CPU_SCS_DHCSR_PASSWRD | (0x1 << 0));
    chipset->algo.index = 0;
    chipset->algo.started = 1;

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_cm4f_write(cc26xx_t *chipset, uint32_t stAddress, uint32_t *data, uint32_t len){
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;
    // the write operation uses the helper algorithm
    if (chipset->algo.started == 0)
        return CC26XX_FAILED;

    return __algo_write(chipset, stAddress, data, len);
}

cc26xx_err_e cc26xx_cm4f_read(cc26xx_t *chipset, uint32_t stAddress, uint32_t *data, uint32_t len){
    arm7_t *arm = &chipset->arm7;

    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;

    arm7_get_reg_buffered(arm, stAddress, data, (len/4));

    return CC26XX_OK;
}

cc26xx_err_e cc26xx_cm4f_erase_all(cc26xx_t *chipset){
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_CM4F].tapOn)
        return CC26XX_TAP_DISABLED;
    if (chipset->algo.started == 0){
        return CC26XX_FAILED;
    }

    return _algo_erase_all(chipset);
}

cc26xx_err_e cc26xx_wuc_write(cc26xx_t *chipset, uint8_t ir, uint8_t dr){
    if (chipset == NULL || chipset->cjtag == NULL)
        return CC26XX_ERR_VAR;
    if (!chipset->taps[CC26XX_TAP_AON_WUC].tapOn)
        return CC26XX_TAP_DISABLED;

    _IrShiftTap(chipset, CC26XX_TAP_AON_WUC, ir, 8);
    _DrShiftTap(chipset, CC26XX_TAP_AON_WUC, dr, 8, 0);

    return CC26XX_OK;
}


