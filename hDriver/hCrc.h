/*
 * hCrc.h
 *
 *  Created on: 5 de jul de 2021
 *      Author: pablo.jean
 */

#ifndef CC_HDRIVER_HCRC_H_
#define CC_HDRIVER_HCRC_H_


#include <stddef.h>
#include <stdint.h>
#include <math.h>
/*
 * CRC Calculate Lib: CRC8-SAE-J1850
 * *==========================*
 * | polynomial  | 0x1D       |
 * |-------------|------------|
 * | init value  | 0xFF       |
 * |-------------|------------|
 * | final xor   | 0xFF       |
 * |-------------|------------|
 * | reflec in   | false      |
 * |-------------|------------|
 * | reclec out  | false      |
 * |-------------|------------|
 * | magic check | 0xB3       |
 * *==========================*
 */

#define MAGIC_CHECK     0x3B
#define INITIAL_VALUE           0xFF
#define FINAL_XOR       0xFF


/**
 * @brief           This function is used to calculate the CRC-8 of an array
 *
 * @param[in]       data: data we want to generate the crc value
 * @param[in]       len: len of the array
 *
 * @returns         crc value
 */
uint8_t crc8(uint8_t *data, uint8_t len);


/**
 * @brief           This function is used to calculate the CRC-32 of an array
 *                  with Polynomial = 0X4C11DB7, Initial = 0xFFFFFFFF and
 *                  Final Xor = 0xFFFFFFFF
 *
 * @param[in]       data: data we want to generate the crc value
 * @param[in]       len: len of the array
 *
 * @returns         crc value
 */
uint32_t crc32(uint32_t initial, uint8_t *data, uint16_t len);

#endif /* CC_HDRIVER_HCRC_H_ */
