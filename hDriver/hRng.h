/*
 * hRng.h
 *
 *  Created on: 5 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef CC_HDRIVER_HRNG_H_
#define CC_HDRIVER_HRNG_H_


#include <stdint.h>
#include <stdlib.h>

/*
 * Enums
 */

typedef enum{
    hRNG_OK,
    HRNG_FAIL
}hRng_error_e;

/*
 * Function prototypes
 */

hRng_error_e hRng_random(uint8_t *value, uint16_t bytes);

#endif /* CC_HDRIVER_HRNG_H_ */
