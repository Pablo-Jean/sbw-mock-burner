/*
 * sbw.h
 *
 *  Created on: 26 de abr de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_SPY_BY_WIRE_SBW_H_
#define LIB_SPY_BY_WIRE_SBW_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "msp430_dev_codes.h"

#include "data_link.h"

/**
 * Macros
 */

/****************************************************************************/
/* Define section for constants                                             */
/****************************************************************************/

// Constants for flash erasing modes
//! \brief Constant for an erase of the entire flash main memory
#define ERASE_MAIN          0xA506
//! \brief Constants for an erase of a single flash memory bank
#define ERASE_BANK          0xA504
//! \brief Constant for an erase of the selected flash memory segment
#define ERASE_SEGMENT       0xA502
//! \brief Constant for an erase of the FRAM Main memory on FR5xx/FR6xx
#define MAIN_ERASE  0x1A1A
//! \brief Constant for an erase of the FRAM Main, INFO  & IP
//! protected memory on FR5xx/FR6xx
#define TOTAL_ERASE  0x1B1B
//! \brief Constant for an erase of the FRAM Main & INFO memory including JTAG
//! lock signature on FR4xx
#define USER_CODE_ERASE  0x1A1A
#define STOP_DEVICE 0xA55A

// Constants for runoff status
//! \brief return 0 = error
#define STATUS_ERROR     0      // false
//! \brief return 1 = no error
#define STATUS_OK        1      // true
//! \brief GetDevice returns this if the security fuse is blown
#define STATUS_FUSEBLOWN 2

//! \brief Replicator is active
#define STATUS_ACTIVE    2
//! \brief Replicator is idling
#define STATUS_IDLE      3

// dedicated addresses
//! \brief Triggers a regular reset on device release from JTAG control
#define V_RESET                    0xFFFE
//! \brief Triggers a "brown-out" reset on device release from JTAG control
#define V_BOR                      0x1B08

// Constants for JTAG mailbox data exchange
//! \brief JTAG mailbox constant -
#define OUT1RDY 0x0008
//! \brief JTAG mailbox constant -
#define IN0RDY  0x0001
//! \brief JTAG mailbox constant -
#define JMB32B  0x0010
//! \brief JTAG mailbox constant -
#define OUTREQ  0x0004
//! \brief JTAG mailbox constant -
#define INREQ   0x0001
//! \brief JTAG mailbox mode 32 bit -
#define MAIL_BOX_32BIT 0x10
//! \brief JTAG mailbox moede 16 bit -
#define MAIL_BOX_16BIT 0x00

// Constants for flash erasing modes
//! \brief Constant for an erase of the entire flash main memory
#define ERASE_MAIN          0xA506
//! \brief Constants for an erase of a single flash memory bank
#define ERASE_BANK          0xA504
//! \brief Constant for an erase of the selected flash memory segment
#define ERASE_SEGMENT       0xA502

//! \brief Set the target's Vcc level supplied by REP430F
//! \details data = 10*Vcc - range 2.1V to 3.6V or 0 (Vcc-OFF)
#define VCC_LEVEL  30

//! \brief Set start address of the main memory
#define MAIN_START_ADDRESS 0xC400

//! \brief Set length of main memory
//! \details Note that the memory length is counted in 16-bit WORDS!
#define MAIN_LENGTH 0x1000

//! \brief Set the start address of the device RAM
#define RAM_START_ADDRESS 0x2000

#define MSP430_SYS_BASE     0x140
#define MSP430_SYSBSLC      0x140 + 0x02
#define MSP430_SYSCFG0      0x140 + 0x20

#define MSP430_PMM_BASE     0x120
#define MSP430_PMMCTRL0     MSP430_PMM_BASE + 0x00
#define MSP430_PMMCTRL1     MSP430_PMM_BASE + 0x02
#define MSP430_PMMCTRL2     MSP430_PMM_BASE + 0x04
#define MSP430_PMMCTRLIF    MSP430_PMM_BASE + 0x0A
#define MSP430_PM5          MSP430_PMM_BASE + 0x10

#define MSP430_PORTA_BASE   0x200
#define MSP430_PORTA_IN     MSP430_PORTA_BASE + 0x00
#define MSP430_PORTA_OUT    MSP430_PORTA_BASE + 0x02
#define MSP430_PORTA_DIR    MSP430_PORTA_BASE + 0x04


/**
 * Enumerates
 */

typedef enum{
    CPU_OFFLINE,
    CPU_HALT,
    CPU_RELEASED,

    CPU_UNKNOWN_STATE = 0xFF
}sbw_cpu_state_e;

typedef enum{
    SBW_EXECUTE_POR,
    SBW_GET_DEVICE_ID,
    SBW_HALT_DEVICE,
    SBW_RELEASE_DEVICE,
    SBW_WRITE_DATA,
    SBW_READ_DATA,
    SBW_WRITE_WITH_CHECK,
    SBW_CHECK_ACCESS,
    SBW_DISABLE_MPU,
    SBW_ERASE_MAIN,
    SBW_ERASE_CHECK,
    SBW_TEST_RAM_WRITE,
    SBW_UNLOCK_BSL,
    SBW_DISABLE_WDT,
    SBW_JTAG_PASSWORD_WRITE,
    SBW_JTAG_PASSWORD_REMOVE
}sbw_cmd_e;

typedef enum{
 IR_CNTRL_SIG_16BIT =        0xC8,   // original value: 0x13
//! \brief Read out the JTAG control signal register
 IR_CNTRL_SIG_CAPTURE =      0x28,   // original value: 0x14
//! \brief Release the CPU from JTAG control
 IR_CNTRL_SIG_RELEASE =      0xA8,   // original value: 0x15

// Instructions for the JTAG fuse
//! \brief Prepare for JTAG fuse blow
 IR_PREPARE_BLOW =           0x44,   // original value: 0x22
//! \brief Perform JTAG fuse blow
 IR_EX_BLOW =                0x24,   // original value: 0x24

// Instructions for the JTAG data register
//! \brief Set the MSP430 MDB to a specific 16-bit value with the next
//! 16-bit data access
 IR_DATA_16BIT =             0x82,   // original value: 0x41
//! \brief Set the MSP430 MDB to a specific 16-bit value (RAM only)
 IR_DATA_QUICK =             0xC2,   // original value: 0x43

// Instructions for the JTAG PSA mode
//! \brief Switch JTAG data register to PSA mode
 IR_DATA_PSA =               0x22,   // original value: 0x44
//! \brief Shift out the PSA pattern generated by IR_DATA_PSA
 IR_SHIFT_OUT_PSA =          0x62,   // original value: 0x46

// Instructions for the JTAG address register
//! \brief Set the MSP430 MAB to a specific 16-bit value
//! \details Use the 20-bit macro for 430X and 430Xv2 architectures
 IR_ADDR_16BIT =             0xC1,   // original value: 0x83
//! \brief Read out the MAB data on the next 16/20-bit data access
 IR_ADDR_CAPTURE =           0x21,   // original value: 0x84
//! \brief Set the MSP430 MDB with a specific 16-bit value and write
//! it to the memory address which is currently on the MAB
 IR_DATA_TO_ADDR =           0xA1,   // original value: 0x85
//! \brief Bypass instruction - TDI input is shifted to TDO as an output
 IR_BYPASS =                 0xFF,   // original value: 0xFF
 IR_DATA_CAPTURE =           0x42,
    //! \brief Request a JTAG mailbox exchange
 IR_JMB_EXCHANGE =           0x86,   // original value: 0x61
//! \brief Instruction for test register in 5xx
 IR_TEST_REG =               0x54,   // original value: 0x2A
//! \brief Instruction for 3 volt test register in 5xx
 IR_TEST_3V_REG =             0xF4   // original value: 0x2F
}jtag_mai_e;


/**
 * Structs and Unions
 */

typedef union{
    struct __attribute__((packed)){
        struct __attribute__((packed)){
            uint8_t info_len;
            uint8_t crc_len;
            uint16_t crc;
            uint16_t deviceID;
            uint8_t hardware_rev;
            uint8_t firmware_rev;
        }information;
        struct __attribute__((packed)){
            uint8_t die_record_tag;
            uint8_t die_record_length;
            uint32_t lot_wafer_id;
            uint16_t dieX;
            uint16_t dieY;
            uint16_t test_result;
        }die_record;
        struct __attribute__((packed)){
            uint8_t calib_tag;
            uint8_t adc_calib_len;
            uint8_t adc_gain_factor;
            uint8_t adc_offset;
            uint8_t adc_1_5_ref_30;
            uint8_t adc_1_5_ref_85;
        }adc_calib;
        struct __attribute__((packed)){
            uint8_t calib_tag;
            uint8_t calib_len;
            uint8_t ref_factor_1_5;
            uint8_t dco_tap_30;
        }reference_dco;
    };
    uint16_t _raw[(0x24/2)];
}tlv_t;

typedef struct{
    uint16_t *data;
    uint32_t len;
    uint32_t stAddr;
}sbw_buf_t;

typedef struct{
    sbw_data_link_t *link;
    tlv_t TLV;
    msp430_model_address_t *memoryMap;
    struct{
        msp430_ids_e Device;
        msp430_jtag_id_e Jtag;
        uint16_t CoreID;
        msp430_fuse_e Efuse;
        sbw_cpu_state_e Cpu;
    }info;
    struct{
        sbw_buf_t *wr;
        sbw_buf_t *rd;
    }buf;
}sbw_t;


/**
 * Function prototypes
 */

int8_t sbw_init(sbw_t *obj, sbw_data_link_t *link);

uint32_t sbw_cmd(sbw_t *obj, sbw_cmd_e cmd, void *bVal);


#endif /* LIB_SPY_BY_WIRE_SBW_H_ */
