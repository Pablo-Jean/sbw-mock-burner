/*
 * pkc7.h
 *
 *  Created on: 5 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef API_PKC7_H_
#define API_PKC7_H_

#include <stdint.h>
#include <stddef.h>

uint8_t PKCS7_padding_add(uint8_t block, uint8_t *bff, uint16_t inLen, uint16_t *outLen);

uint8_t PKCS7_padding_remove(uint8_t block, uint8_t *bff, uint16_t inLen, uint16_t *outLen);

#endif /* API_PKC7_H_ */
