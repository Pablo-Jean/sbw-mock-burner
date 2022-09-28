/*
 * cjtag_defines.h
 *
 *  Created on: 8 de set de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_CJTAG_CJTAG_DEFINES_H_
#define LIB_CJTAG_CJTAG_DEFINES_H_


#include <stdint.h>

/*
 * Macros
 */

// STMC Store Miscellaneous Control Commands
#define _CMD_STMC_NOP                   (0)
#define _CMD_STMC_ECL                   (1)
#define _CMD_STMC_SUSPEND               (2)
#define _CMD_STMC_ZBSINH                (3)

#define _CMD_STMC_SGC_OFF               ((1 << 2) | (0))
#define _CMD_STMC_SGC_ON                ((1 << 2) | (1))
#define _CMD_STMC_CGM_OFF               ((1 << 2) | (0))
#define _CMD_STMC_CGM_ON                ((1 << 2) | (1))

// With a scan format other than the MScan Scan Format, the number of logic 1 RDY bits preceding
// the last bit of the SP payload is xy + 1, where xy is the bit1 and bit0, respectively
#define _CMD_STMC_RDYC                  ((2 << 2) | (0))

#define _CMD_STMC_DLYC_OFF              ((3 << 2) | (0))
#define _CMD_STMC_DLYC_ONE_TCKC         ((3 << 2) | (1))
#define _CMD_STMC_DLYC_TWO_TCKC         ((3 << 2) | (2))
#define _CMD_STMC_DLYC_VAR_TCKC         ((3 << 2) | (3))

// STC1 Store Conditional 1 bit
#define _CMD_STC1_SEDGE_FALL_EDGE       (0)
#define _CMD_STC1_SEDGE_RISE_EDGE       (1)
#define _CMD_STC1_SEDGE_FALL_IF_CGM     ((1 << 4) | (0))
#define _CMD_STC1_SEDGE_RISE_IF_CGM     ((1 << 4) | (1))

// STC2 Store Conditional 2 bit
#define _CMD_STC2_APFC_DEF_PIN          ((2 << 2) | (0))
#define _CMD_STC2_APFC_STD_PIN          ((2 << 2) | (1))
#define _CMD_STC2_APFC_AUX_PIN          ((2 << 2) | (2))
#define _CMD_STC2_APFC_DEF_PIN_IF_CGM   ((1 << 4) | (2 << 2) | (0))
#define _CMD_STC2_APFC_STD_PIN_IF_CGM   ((1 << 4) | (2 << 2) | (1))
#define _CMD_STC2_APFC_AUX_PIN_IF_CGM   ((1 << 4) | (2 << 2) | (2))

// STFMT Store Scan Format
#define _CMD_STFMT_JSCAN0               (0)
#define _CMD_STFMT_JSCAN1               (1)
#define _CMD_STFMT_JSCAN2               (2)
#define _CMD_STFMT_JSCAN3               (3)
#define _CMD_STFMT_OSCAN0               (8)
#define _CMD_STFMT_OSCAN1               (9)
#define _CMD_STFMT_OSCAN2               (10)
#define _CMD_STFMT_OSCAN3               (11)
#define _CMD_STFMT_OSCAN4               (12)
#define _CMD_STFMT_OSCAN5               (13)
#define _CMD_STFMT_OSCAN6               (14)
#define _CMD_STFMT_OSCAN7               (15)


// MSS Make Scan Selection
#define _CMD_MSS_SGC_CLRD               (0)
#define _CMD_MSS_SGC_NOT_AFFECTED       ((1 << 4))

// MSS Make Scan Selection
#define _CMD_CCE_CGM_CLRD               (0)
#define _CMD_CCE_CGM_NOT_AFFECTED       ((1 << 4))

// SCNB Scan Bit
#define _CMD_SCNB_SGC_WR                (0)
#define _CMD_SCNB_CGM_WR                (1)
#define _CMD_SCNB_CNFG_RD_0             (2)
#define _CMD_SCNB_CNFG_RD_1             (3)
#define _CMD_SCNB_CNFG_RD_2             (4)
#define _CMD_SCNB_CNFG_RD_3             (5)

// PATTERNS
#define CJTAG_PATTERN_IDLE              0x00
#define CJTAG_PATTERN_TLR               0xFFFFFFFF


/*
 * Typedefs
 */

typedef enum{
    CJTAG_MODE_2PIN,
    CJTAG_MODE_4PIN
}cjtag_mode_init_e;

typedef enum{
    CJTAG_TEST_LOGIC_RESET,
    CJTAG_RUN_TEST_IDLE,
    CJTAG_SELECT_DR_SCAN,
    CJTAG_CAPTURE_DR,
    CJTAG_SHIFT_DR,
    CJTAG_EXIT_1_DR,
    CJTAG_PAUSE_DR,
    CJTAG_EXIT_2_DR,
    CJTAG_UPDATE_DR,
    CJTAG_SELECT_IR_SCAN,
    CJTAG_CAPTURE_IR,
    CJTAG_SHIFT_IR,
    CJTAG_EXIT_1_IR,
    CJTAG_PAUSE_IR,
    CJTAG_EXIT_2_IR,
    CJTAG_UPDATE_IR
}cjtag_fsm_state_e;

typedef enum{
    CJTAG_JSCAN_0,
    CJTAG_JSCAN_1,
    CJTAG_JSCAN_2,
    CJTAG_JSCAN_3,
    CJTAG_MSCAN,
    CJTAG_OSCAN_0,
    CJTAG_OSCAN_1,
    CJTAG_OSCAN_2,
    CJTAG_OSCAN_3,
    CJTAG_OSCAN_4,
    CJTAG_OSCAN_5,
    CJTAG_OSCAN_6,
    CJTAG_OSCAN_7,
    CJTAG_SSCAN_0,
    CJTAG_SSCAN_1,
    CJTAG_SSCAN_2,
    CJTAG_SSCAN_3,

    CJTAG_UNKNOWN = 0xFF
}cjtag_scan_format_e;

typedef enum{
    OPCODE_STMC = 0x00,
    OPCODE_STC1 = 0x01,
    OPCODE_STC2 = 0x02,
    OPCODE_STFMT = 0x03,
    OPCODE_MSS = 0x04,
    OPCODE_CCE = 0x07,
    OPCODE_SCNB = 0x08
}cjtag_opcodes_e;




#endif /* LIB_CJTAG_CJTAG_DEFINES_H_ */
