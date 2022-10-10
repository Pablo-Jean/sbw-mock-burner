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




void __jtag_init(cc26xx_t *cc26){

}

void __icepick_init(cc26xx_t *cc26){

}

void __taps_initialize(cc26xx_t *cc26){

}

/**
 * Publics
 */

cc26xx_err_e cc26xx_init(cc26xx_t *cc26){
    if (cc26 == NULL || cc26->cjtag == NULL)
        return CC26XX_ERR_VAR;

    cc26->icepick.icepick.
}

cc26xx_err_e cc26xx_get_id(cc26xx_t *cc26, cc26xx_id_source_e e, uint32_t *id){

}

cc26xx_err_e cc26xx_read_data(cc26xx_t *cc26, uint8_t *data, uint32_t len){

}

cc26xx_err_e cc26xx_write_data(cc26xx_t *cc26, uint8_t *data, uint32_t len){

}
