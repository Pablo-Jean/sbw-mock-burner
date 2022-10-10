/*
 * cc26xx.h
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_DEVICES_CC26XX_CC26XX_H_
#define LIB_DEVICES_CC26XX_CC26XX_H_

#include <stdint.h>
#include <stddef.h>

#include "icepick.h"
#include "cjtag.h"
#include "adiv5.h"

/**
 * Macros
 */



/**
 * Enumerates
 */

typedef enum{
    CC26XX_OK,
    CC26XX_NOT_FOUND,
    CC26XX_ERR_VAR,

    CC26XX_ERR = 0xFF
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
    CC26XX_TAP_AON_WIC,
    CC26XX_TAP_CM4F
}cc26xx_tap_names_e;

/**
 * Typedefs
 */

typedef uint64_t(*_fxnDrShift)(void* handle, uint64_t data, uint8_t l);
typedef uint64_t(*_fxnIrShift)(void* handle, uint64_t data, uint8_t l);


typedef struct{
    uint8_t tapOn;
    uint32_t cmd;
    adiv5_t adi;
}cc26xx_taps_t;

typedef struct{
    _fxnDrShift DrShift;
    _fxnIrShift IrShift;
}cc26xx_fxn_t;

typedef struct{
    cc26xx_fxn_t fxn;
    cjtag_t *cjtag;
    icepick_t icepick;
    cc26xx_taps_t taps[7];
}cc26xx_t;

/**
 * Prototypes
 */

cc26xx_err_e cc26xx_init(cc26xx_t *cc26);

cc26xx_err_e cc26xx_get_id(cc26xx_t *cc26, cc26xx_id_source_e e, uint32_t *id);

cc26xx_err_e cc26xx_read_data(cc26xx_t *cc26, uint8_t *data, uint32_t len);

cc26xx_err_e cc26xx_write_data(cc26xx_t *cc26, uint8_t *data, uint32_t len);

#endif /* LIB_DEVICES_CC26XX_CC26XX_H_ */
