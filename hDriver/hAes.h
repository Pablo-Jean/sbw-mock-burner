/*
 * hAes.h
 *
 *  Created on: 5 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef CC_HDRIVER_HAES_H_
#define CC_HDRIVER_HAES_H_


#include <stdint.h>
#include <stddef.h>

#include "pkc7.h"

/*
 * Macros
 */
#define AES_MODE_128_BLOCK      16

/*
 * Enumerates
 */

typedef enum{
    hAES_OK,
    hAES_FAIL,
    hAES_NULL = 0xFF
}hAes_error_e;

/*
 * Function Prototypes
 */

hAes_error_e hAes_Encrypt_128(uint8_t *IV, uint8_t *Key, uint8_t *inputData, uint16_t inputLen, uint8_t *outputData, uint16_t *outputLen);
hAes_error_e hAes_Decrypt_128(uint8_t *IV, uint8_t *Key, uint8_t *inputData, uint16_t inputLen, uint8_t *outputData, uint16_t *outputLen);

hAes_error_e hAes_Encrypt_128_PKCS7(uint8_t *IV, uint8_t *Key, uint8_t *inputData, uint16_t inputLen, uint8_t *outputData, uint16_t *outputLen);
hAes_error_e hAes_Decrypt_128_PKCS7(uint8_t *IV, uint8_t *Key, uint8_t *inputData, uint16_t inputLen, uint8_t *outputData, uint16_t *outputLen);


#endif /* CC_HDRIVER_HAES_H_ */
