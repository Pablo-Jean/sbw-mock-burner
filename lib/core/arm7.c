/*
 * arm7.c
 *
 *  Created on: 26 de out de 2022
 *      Author: pablo.jean
 */

#include "arm7.h"


/**
 * Macros
 */


/**
 * Enumerates
 */


/**
 * Typedefs
 */


/**
 * Privates
 */

uint32_t __armv7m_map_id_to_regsel(uint32_t arm_reg_id){
    switch (arm_reg_id) {
    case ARMV7M_R0 ... ARMV7M_R14:
    case ARMV7M_PC:
    case ARMV7M_xPSR:
    case ARMV7M_MSP:
    case ARMV7M_PSP:
        /* NOTE:  we "know" here that the register identifiers
         * match the Cortex-M DCRSR.REGSEL selectors values
         * for R0..R14, PC, xPSR, MSP, and PSP.
         */
        return arm_reg_id;

    case ARMV7M_PMSK_BPRI_FLTMSK_CTRL:
        return ARMV7M_REGSEL_PMSK_BPRI_FLTMSK_CTRL;

    case ARMV8M_MSP_NS...ARMV8M_PSPLIM_NS:
        return arm_reg_id - ARMV8M_MSP_NS + ARMV8M_REGSEL_MSP_NS;

    case ARMV8M_PMSK_BPRI_FLTMSK_CTRL_S:
        return ARMV8M_REGSEL_PMSK_BPRI_FLTMSK_CTRL_S;

    case ARMV8M_PMSK_BPRI_FLTMSK_CTRL_NS:
        return ARMV8M_REGSEL_PMSK_BPRI_FLTMSK_CTRL_NS;

    case ARMV7M_FPSCR:
        return ARMV7M_REGSEL_FPSCR;

    case ARMV7M_D0 ... ARMV7M_D15:
        return ARMV7M_REGSEL_S0 + 2 * (arm_reg_id - ARMV7M_D0);

    default:
        return arm_reg_id;
    }
}

void _read_ctrl_stat(adiv5_t *dap){
    uint32_t info;

    adiv5_dp_read(dap, DP_CTRL_STAT, &info);
    adiv5_dp_read(dap, DP_RDBUFF, &info);
    dap->dp_ctrl_stat._raw = info;
}

void _read_select(adiv5_t *dap, uint8_t bnk){
    adiv5_dp_write(dap, DP_SELECT, bnk);
}

void _upd_tarAddr(arm7_t *arm){
    adiv5_t *dap = (adiv5_t*)(arm->link);

    if (dap->ap_scw.AddrInc != CSW_ADDRINC_OFF){
        switch (dap->ap_scw.Size){
        case CSW_8BIT:
            arm->tarAddr += 1;
            break;
        case CSW_16BIT:
            arm->tarAddr += 2;
            break;
        case CSW_32BIT:
            arm->tarAddr += 4;
            break;
        }
    }
}

void _set_tar(arm7_t *arm, uint32_t tar){
    adiv5_t* dap;

//    if (tar != arm->tarAddr){
        switch (arm->linkType){
            case ARM7_LINK_JTAGDP:
                dap = (adiv5_t*)(arm->link);
                break;
            case ARM7_LINK_SWD:
                // NOT IMPLEMENTED
                break;
        }
        adiv5_ap_write(dap, MEM_AP_REG_TAR, tar);
        arm->tarAddr = tar;
//    }
}

void _set_drw(arm7_t *arm, uint32_t drw){
    adiv5_t* dap;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }
    adiv5_ap_write(dap, MEM_AP_REG_DRW, drw);
    _upd_tarAddr(arm);
}

void _get_drw(arm7_t *arm, uint32_t *drw){
    adiv5_t* dap;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }
    adiv5_ap_read(dap, MEM_AP_REG_DRW, drw);
    _upd_tarAddr(arm);
}

void _write_drw(arm7_t *arm, uint32_t tar, uint32_t drw){
    _set_tar(arm, tar);
    _set_drw(arm, drw);
}

void _read_drw(arm7_t *arm, uint32_t tar, uint32_t *drw){
    uint32_t d;

    _set_tar(arm, tar);
    _get_drw(arm, &d);
    if (drw != NULL)
        *drw = d;
}


/**
 * Publics
 */

arm7_err_e arm7_init(arm7_t *arm, uint8_t prot){
    adiv5_t *dap;
    uint32_t status;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }
    if (dap->target == NULL)
        return ARM7_INVALID_ARGS;

    adiv5_idcode(dap, &arm->idcode);

    dap->dp_ctrl_stat._raw = 0;
    dap->dp_ctrl_stat._CDBGPWRUPREQ = 1;
    dap->dp_ctrl_stat._CSYSPWRUPREQ = 1;
    adiv5_dp_write(dap, DP_CTRL_STAT, dap->dp_ctrl_stat._raw);
    // wait for debug and sys power ACK signal
    do{
        adiv5_dp_read(dap, DP_CTRL_STAT, &status);
        dap->dp_ctrl_stat._raw = status;
    }while((status & (CDBGPWRUPACK | CSYSPWRUPACK)) == 0);

    dap->ap_scw._raw = 0x40;
    dap->ap_scw.Prot = prot;
    dap->ap_scw.AddrInc = CSW_ADDRINC_SINGLE;
    dap->ap_scw.Size = CSW_32BIT;
    adiv5_ap_write(dap, MEM_AP_REG_CSW, dap->ap_scw._raw);

    return ARM7_OK;
}

arm7_err_e arm7_get_idcode(arm7_t *arm, uint32_t *idcode){
    adiv5_t *dap;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }
    if (dap->target == NULL)
        return ARM7_INVALID_ARGS;

    adiv5_idcode(dap, idcode);

    return ARM7_OK;
}

arm7_err_e arm7_set_reg(arm7_t *arm, uint32_t reg, uint32_t data){
    adiv5_t *dap;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }

    _read_select(dap, 0x0);

    _write_drw(arm, reg, data);

    _read_ctrl_stat(dap);

    return ARM7_OK;
}

arm7_err_e arm7_get_reg(arm7_t *arm, uint32_t reg, uint32_t *data){
    adiv5_t *dap;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }

    _read_select(dap, 0x0);

    _read_drw(arm, reg, data);

    _read_ctrl_stat(dap);

    return ARM7_OK;
}

arm7_err_e arm7_set_reg_buffered(arm7_t *arm, uint32_t reg, uint32_t *data, uint32_t len){
    adiv5_t *dap;
    uint32_t i, j;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }

    _read_select(dap, 0x0);
    _set_tar(arm, reg);
    j=0;
    for (i=0 ; i<len ; i++){
        if (j > 3){
            _set_tar(arm, reg);
            j = 0;
        }
//        _write_drw(arm, reg, data[i]);
        _set_drw(arm, data[i]);
        reg += 4;
        j++;
    }

    _read_ctrl_stat(dap);

    return ARM7_OK;
}

arm7_err_e arm7_get_reg_buffered(arm7_t *arm, uint32_t reg, uint32_t *data, uint32_t len){
    adiv5_t *dap;
    uint32_t i, d;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    switch (arm->linkType){
        case ARM7_LINK_JTAGDP:
            dap = (adiv5_t*)(arm->link);
            break;
        case ARM7_LINK_SWD:
            // NOT IMPLEMENTED
            break;
    }

    _read_select(dap, 0x0);
//    _set_tar(arm, reg);
    for (i=0 ; i<len ; i++){
        _read_drw(arm, reg, &d);
//        _get_drw(arm, &d);
        data[i] = d;
        reg += 4;
    }

    _read_ctrl_stat(dap);

    return ARM7_OK;
}

arm7_err_e arm7_update_core_regs(arm7_t *arm){
    uint16_t k;
    uint32_t get;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    for (k=ARMV7M_R0 ; k<ARMV7M_PRIMASK ; k++){
        arm7_get_core_reg(arm, (arm7_core_reg_e)k, &get);
        *((uint32_t*)(&arm->core_reg) + (k*4)) = get;
    }
}

arm7_err_e arm7_get_core_reg(arm7_t *arm, arm7_core_reg_e reg, uint32_t *data){
    uint32_t regsel, get;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    regsel = (ARM7_CORE_REG_READ_BIT | __armv7m_map_id_to_regsel(reg));
    arm7_set_reg(arm, ARM7_CPU_SCS_DCRSR, regsel);
    do{
        arm7_get_reg(arm, ARM7_CPU_SCS_DHCSR, &get);
        arm->dhcsr_reg._raw = get;
    } while(arm->dhcsr_reg.S_REGRDY == 0);
    arm7_get_reg(arm, ARM7_CPU_SCS_DCRDR, data);

    return ARM7_OK;
}

arm7_err_e arm7_set_core_reg(arm7_t *arm, arm7_core_reg_e reg, uint32_t data){
    uint32_t regsel, get;

    if (arm == NULL || arm->link == NULL)
        return ARM7_INVALID_ARGS;

    regsel = (ARM7_CORE_REG_WRITE_BIT | __armv7m_map_id_to_regsel(reg));
    arm7_set_reg(arm, ARM7_CPU_SCS_DCRDR, data);
    arm7_set_reg(arm, ARM7_CPU_SCS_DCRSR, regsel);
    do{
        arm7_get_reg(arm, ARM7_CPU_SCS_DHCSR, &get);
        arm->dhcsr_reg._raw = get;
    } while(arm->dhcsr_reg.S_REGRDY == 0);

    return ARM7_OK;
}

//arm7_err_e arm7_start_algorithm(arm7_t *arm){
//
//}
//
//arm7_err_e arm7_wait_algorithm(arm7_t *arm){
//
//}
//
//arm7_err_e arm7_stop_algorithm(arm7_t *arm){
//
//}

