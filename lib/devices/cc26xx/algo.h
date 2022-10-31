/*
 * algo.h
 *
 *  Created on: 21 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_DEVICES_CC26XX_ALGO_H_
#define LIB_DEVICES_CC26XX_ALGO_H_


#include <stdint.h>
#include <stddef.h>

/* Agama CC26x2/CC13x2 specific parameters */
#define CC26X2_MAX_SECTORS   128
#define CC26X2_SECTOR_LENGTH 0x2000
#define CC26X2_ALGO_BUFFER_0 0x20002000
#define CC26X2_ALGO_BUFFER_1 0x20004000
#define CC26X2_ALGO_PARAMS_0 0x20001fd8
#define CC26X2_ALGO_PARAMS_1 0x20001fec
#define CC26X2_WORKING_SIZE  (CC26X2_ALGO_BUFFER_1 + CC26X2_SECTOR_LENGTH - \
                             CC26XX_ALGO_BASE_ADDRESS)

/* CC26XX flash helper algorithm commands */
#define CC26XX_CMD_NO_ACTION                     0
#define CC26XX_CMD_ERASE_ALL                     1
#define CC26XX_CMD_PROGRAM                       2
#define CC26XX_CMD_ERASE_AND_PROGRAM             3
#define CC26XX_CMD_ERASE_AND_PROGRAM_WITH_RETAIN 4
#define CC26XX_CMD_ERASE_SECTORS                 5

/* CC26xx flash helper algorithm buffer flags */
#define CC26XX_BUFFER_EMPTY 0x00000000
#define CC26XX_BUFFER_FULL  0xffffffff

/* CC26xx and CC13xx device types */
#define CC26XX_NO_TYPE 0 /* Device type not determined yet */
#define CC26X0_TYPE    1 /* CC26x0 Chameleon device */
#define CC26X1_TYPE    2 /* CC26x1 Chameleon device */
#define CC26X2_TYPE    3 /* CC26x2 Agama device */
#define CC13X0_TYPE    4 /* CC13x0 Chameleon device */
#define CC13X2_TYPE    5 /* CC13x2 Agama device */

/* Flash helper algorithm parameter block struct */
#define CC26XX_STATUS_OFFSET 0x0c

/**
 * Typedefs
 */

typedef union{
    uint32_t _raw[4];
    struct{
        uint32_t address;
        uint32_t length;
        uint32_t command;
        uint32_t status;
    };
}cc26x2_lago_params_t;

/**
 * Externs
 */
extern uint8_t cc26x2_algo[];
extern uint32_t cc26x2_algo_len;


#endif /* LIB_DEVICES_CC26XX_ALGO_H_ */
