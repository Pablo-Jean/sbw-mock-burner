/*
 * hECDSA.h
 *
 *  Created on: 30 de set de 2021
 *      Author: pablo.jean
 */

#ifndef CC_HDRIVER_HSHA_H_
#define CC_HDRIVER_HSHA_H_


#include <stdint.h>

#include "hDriver.h"


/*
 * Macros
 */

/*
 * Enumerates
 */

typedef enum{
    hSHA_OK,
    hSHA_FAIL,
    hSHA_NULL = 0xFF
}hSha_error_e;

typedef enum{
    hSHA_HASH_TYPE_224,
    hSHA_HASH_TYPE_256,
    hSHA_HASH_TYPE_384,
    hSHA_HASH_TYPE_512
}hSha_mode_e;

/*
 * Typedefs
 */

typedef void* hSha_t;

/*
 * Function Prototypes
 */

hSha_t hSHA_Init(uint32_t index, hSha_mode_e curve);

hSha_error_e hSHA_Update(hSha_t handle, uint8_t *data, uint16_t len);

hSha_error_e hSHA_Finalize(hSha_t handle, uint8_t *digest);

void hSHA_Destroy(hSha_t handle);


#endif /* CC_HDRIVER_HAES_H_ */
