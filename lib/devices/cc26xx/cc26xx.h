/*
 * cc26xx.h
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_DEVICES_CC26XX_CC26XX_H_
#define LIB_DEVICES_CC26XX_CC26XX_H_

#include <jtag-dp.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "algo.h"

#include "../core/arm7.h"
#include "../icepick/icepick.h"
#include "../cjtag/cjtag.h"

/**
 * Macros
 */

#define CC26XX_CSW_PROT_DATA        0x63

#define CC26XX_ALGO_ADDRESS         0x20000000

/**
 * Enumerates
 */

typedef enum{
    CC26XX_OK,
    CC26XX_FAILED,
    CC26XX_NOT_FOUND,
    CC26XX_TAP_DISABLED,
    CC26XX_ERR_VAR,

    CC26XX_ERR_GENERIC = 0xFF
}cc26xx_err_e;

typedef enum{
    CC26XX_ID_SOURCE_ICEPICK,
    CC26XX_ID_SOURCE_CPU
}cc26xx_id_source_e;

typedef enum{
    CC26XXX_TAP_TEST,
    CC26XX_TAP_PBIST1,
    CC26XX_TAP_PBIST2,
    CC26XX_TAP_EFUSE,
    CC26XX_TAP_reserved,
    CC26XX_TAP_AON_WUC,
    CC26XX_TAP_CM4F,

    CC26XX_TAP_QTD,

    CC26XX_TAP_ICEPICK = 0xFF
}cc26xx_tap_e;

/**
 * Typedefs
 */


typedef struct{
    char name[15];
    uint8_t tapOn;
    uint32_t *cmd;
    adiv5_t adi;
}cc26xx_taps_t;


typedef struct{
    cjtag_t *cjtag;
    icepick_t icepick;
    cc26xx_taps_t taps[7];
    struct{
        icepick_device_id_t deviceId;
        icepick_idcode_t iceCode;
    }IDs;
    struct{
        uint8_t started;
        uint8_t index;
        cc26x2_lago_params_t par[2];
    }algo;
    adiv5_t adiv5;
    arm7_t arm7;
}cc26xx_t;

/**
 * Prototypes
 */

// Generic

cc26xx_err_e cc26xx_init(cc26xx_t *chipset);

cc26xx_err_e cc26xx_get_id(cc26xx_t *chipset, cc26xx_id_source_e e, uint32_t *id);

cc26xx_err_e cc26xx_cfg_tap(cc26xx_t *chipset, cc26xx_tap_e e, icepick_sdtr_t *param);

/**
 * CM4F
 */
cc26xx_err_e cc26xx_cm4f_idcode(cc26xx_t *chipset, uint32_t *idcode);
cc26xx_err_e cc26xx_cm4f_route(cc26xx_t *chipset);

//cc26xx_err_e cc26xx_cm4f_bankselect(cc26xx_t *chipset, uint32_t reg);
//cc26xx_err_e cc26xx_cm4f_ap_write(cc26xx_t *chipset, uint64_t reg, uint32_t data);
//cc26xx_err_e cc26xx_cm4f_ap_read(cc26xx_t *chipset, uint64_t reg, uint32_t *data);
//cc26xx_err_e cc26xx_cm4f_dp_write(cc26xx_t *chipset, uint64_t reg, uint32_t data);
//cc26xx_err_e cc26xx_cm4f_dp_read(cc26xx_t *chipset, uint64_t reg, uint32_t *data);
cc26xx_err_e cc26xx_cm4f_reg_write(cc26xx_t *chipset, uint32_t reg, uint32_t data);
cc26xx_err_e cc26xx_cm4f_reg_read(cc26xx_t *chipset, uint32_t reg, uint32_t *data);
cc26xx_err_e cc26xx_cm4f_start_script(cc26xx_t *chipset);
cc26xx_err_e cc26xx_cm4f_write(cc26xx_t *chipset, uint32_t stAddress, uint32_t *data, uint32_t len);
cc26xx_err_e cc26xx_cm4f_read(cc26xx_t *chipset, uint32_t stAddress, uint32_t *data, uint32_t len);
cc26xx_err_e cc26xx_cm4f_erase_all(cc26xx_t *chipset);



cc26xx_err_e cc26xx_wuc_write(cc26xx_t *chipset, uint8_t ir, uint8_t dr);

//cc26xx_err_e cc26xx_read_data(cc26xx_t *chipset, uint8_t *data, uint32_t len);
//
//cc26xx_err_e cc26xx_write_data(cc26xx_t *chipset, uint8_t *data, uint32_t len);

#endif /* LIB_DEVICES_CC26XX_CC26XX_H_ */
