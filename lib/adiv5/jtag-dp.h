/*
 * adiv5.h
 *
 *  Created on: 7 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_ADIV5_JTAG_DP_H_
#define LIB_ADIV5_JTAG_DP_H_


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/typedefs.h"

#include "adiv5_define.h"

/**
 * Defines
 */

/* JEP106 ID for ARM */
#define ARM_ID 0x23B

/* three-bit ACK values for SWD access (sent LSB first) */
#define ADIV5_ACK_OK_FAULT    0x2
#define ADIV5_ACK_WAIT        0x1

#define DPAP_WRITE      0
#define DPAP_READ       1

#define BANK_REG(bank, reg) (((bank) << 4) | (reg))

/* A[3:0] for DP registers; A[1:0] are always zero.
 * - JTAG accesses all of these via JTAG_DP_DPACC, except for
 *   IDCODE (JTAG_DP_IDCODE) and ABORT (JTAG_DP_ABORT).
 * - SWD accesses these directly, sometimes needing SELECT.DPBANKSEL
 */
#define DP_DPIDR        BANK_REG(0x0, 0x0) /* DPv1+: ro */
#define DP_ABORT        BANK_REG(0x0, 0x0) /* DPv1+: SWD: wo */
#define DP_DPIDR1       BANK_REG(0x1, 0x0) /* DPv3: ro */
#define DP_BASEPTR0     BANK_REG(0x2, 0x0) /* DPv3: ro */
#define DP_BASEPTR1     BANK_REG(0x3, 0x0) /* DPv3: ro */
#define DP_CTRL_STAT    BANK_REG(0x0, 0x4) /* DPv0+: rw */
#define DP_DLCR         BANK_REG(0x1, 0x4) /* DPv1+: SWD: rw */
#define DP_TARGETID     BANK_REG(0x2, 0x4) /* DPv2: ro */
#define DP_DLPIDR       BANK_REG(0x3, 0x4) /* DPv2: ro */
#define DP_EVENTSTAT    BANK_REG(0x4, 0x4) /* DPv2: ro */
#define DP_SELECT1      BANK_REG(0x5, 0x4) /* DPv3: ro */
#define DP_RESEND       BANK_REG(0x0, 0x8) /* DPv1+: SWD: ro */
#define DP_SELECT       BANK_REG(0x0, 0x8) /* DPv0+: JTAG: rw; SWD: wo */
#define DP_RDBUFF       BANK_REG(0x0, 0xC) /* DPv0+: ro */
#define DP_TARGETSEL    BANK_REG(0x0, 0xC) /* DPv2: SWD: wo */

#define DLCR_TO_TRN(dlcr) ((uint32_t)(1 + ((3 & (dlcr)) >> 8))) /* 1..4 clocks */

/* Fields of DP_DPIDR register */
#define DP_DPIDR_VERSION_SHIFT  12
#define DP_DPIDR_VERSION_MASK   (0xFUL << DP_DPIDR_VERSION_SHIFT)

/* Fields of the DP's CTRL/STAT register */
#define CORUNDETECT     (1UL << 0)
#define SSTICKYORUN     (1UL << 1)
/* 3:2 - transaction mode (e.g. pushed compare) */
#define SSTICKYCMP      (1UL << 4)
#define SSTICKYERR      (1UL << 5)
//#define READOK          (1UL << 6) /* SWD-only */
//#define WDATAERR        (1UL << 7) /* SWD-only */

/* 11:8 - mask lanes for pushed compare or verify ops */
/* 21:12 - transaction counter */
#define CDBGRSTREQ      (1UL << 26)
#define CDBGRSTACK      (1UL << 27)
#define CDBGPWRUPREQ    (1UL << 28)
#define CDBGPWRUPACK    (1UL << 29)
#define CSYSPWRUPREQ    (1UL << 30)
#define CSYSPWRUPACK    (1UL << 31)

#define DP_DLPIDR_PROTVSN   1u

#define DP_SELECT_APSEL 0xFF000000
#define DP_SELECT_APBANK 0x000000F0
#define DP_SELECT_DPBANK 0x0000000F
#define DP_SELECT_INVALID 0x00FFFF00 /* Reserved bits one */

#define DP_APSEL_MAX        (255) /* for ADIv5 only */
#define DP_APSEL_INVALID    0xF00 /* more than DP_APSEL_MAX and not ADIv6 aligned 4k */

#define DP_TARGETSEL_INVALID 0xFFFFFFFFU
#define DP_TARGETSEL_DPID_MASK 0x0FFFFFFFU
#define DP_TARGETSEL_INSTANCEID_MASK 0xF0000000U
#define DP_TARGETSEL_INSTANCEID_SHIFT 28

/* FIX V6 */
#define is_adiv6(dap)           0


/* MEM-AP register addresses */
#define ADIV5_MEM_AP_REG_CSW    (0x00)
#define ADIV5_MEM_AP_REG_TAR    (0x04)
#define ADIV5_MEM_AP_REG_TAR64  (0x08)      /* RW: Large Physical Address Extension */
#define ADIV5_MEM_AP_REG_DRW    (0x0C)      /* RW: Data Read/Write register */
#define ADIV5_MEM_AP_REG_BD0    (0x10)      /* RW: Banked Data register 0-3 */
#define ADIV5_MEM_AP_REG_BD1    (0x14)
#define ADIV5_MEM_AP_REG_BD2    (0x18)
#define ADIV5_MEM_AP_REG_BD3    (0x1C)
#define ADIV5_MEM_AP_REG_MBT    (0x20)      /* --: Memory Barrier Transfer register */
#define ADIV5_MEM_AP_REG_BASE64 (0xF0)      /* RO: Debug Base Address (LA) register */
#define ADIV5_MEM_AP_REG_CFG    (0xF4)      /* RO: Configuration register */
#define ADIV5_MEM_AP_REG_BASE   (0xF8)      /* RO: Debug Base Address register */

#define MEM_AP_REG_CSW          ADIV5_MEM_AP_REG_CSW
#define MEM_AP_REG_TAR          ADIV5_MEM_AP_REG_TAR
#define MEM_AP_REG_TAR64        ADIV5_MEM_AP_REG_TAR64
#define MEM_AP_REG_DRW          ADIV5_MEM_AP_REG_DRW
#define MEM_AP_REG_BD0          ADIV5_MEM_AP_REG_BD0
#define MEM_AP_REG_BD1          ADIV5_MEM_AP_REG_BD1
#define MEM_AP_REG_BD2          ADIV5_MEM_AP_REG_BD2
#define MEM_AP_REG_BD3          ADIV5_MEM_AP_REG_BD3
#define MEM_AP_REG_MBT          ADIV5_MEM_AP_REG_MBT
#define MEM_AP_REG_BASE64       ADIV5_MEM_AP_REG_BASE64
#define MEM_AP_REG_CFG          ADIV5_MEM_AP_REG_CFG
#define MEM_AP_REG_BASE         ADIV5_MEM_AP_REG_BASE

/* Generic AP register address */
#define ADIV5_AP_REG_IDR        (0xFC)      /* RO: Identification Register */
#define ADIV6_AP_REG_IDR        (0xD00 + ADIV5_AP_REG_IDR)
#define AP_REG_IDR(dap)         (is_adiv6(dap) ? ADIV6_AP_REG_IDR : ADIV5_AP_REG_IDR)

/* Fields of the MEM-AP's CSW register */
#define CSW_SIZE_MASK       7
#define CSW_8BIT        0
#define CSW_16BIT       1
#define CSW_32BIT       2
#define CSW_ADDRINC_MASK    (3 << 4)
#define CSW_ADDRINC_OFF     0
#define CSW_ADDRINC_SINGLE  1
#define CSW_ADDRINC_PACKED  2
#define CSW_DEVICE_EN       1
#define CSW_TRIN_PROG       1

/**
 * Enumerates
 */

typedef enum{
    ADIV5_OK,
    ADIV5_FAIL
}adiv5_err_e;

typedef enum{
    ADIV5_CMD_ABORT     = 0x8,
    ADIV5_CMD_DPACC     = 0xA,
    ADIV5_CMD_APACC     = 0xB,
    ADIV5_CMD_IDCODE    = 0xE,
    ADIV5_CMD_BYPASS    = 0xF,
    ADIV5_CMD_EXTEST    = 0x0,
    ADIV5_CMD_SAMPLE    = 0x1,
    ADIV5_CMD_PRELOAD   = 0x2,
    ADIV5_CMD_INTEST    = 0x4,
    ADIV5_CMD_CLAMP     = 0x5,
    ADIV5_CMD_HIGHZ     = 0x6,
    ADIV5_CMD_CLAMPZ    = 0x7
}adiv5_cmd_e;

/**
 * Typedefs
 */

typedef uint64_t(*_fxnTransfer)(void *target, uint64_t data, uint8_t l);

typedef union{
    struct __attribute__((packed)){
        //byte 1L
        uint8_t _ORUNDETECT     : 1;
        uint8_t _STICKYORUN     : 1;
        uint8_t _TRNMODE        : 2;
        //byte 1H
        uint8_t _STICKYCMP      : 1;
        uint8_t _STICKYERR      : 1;
        uint8_t _READOK         : 1;
        uint8_t _WDATAERR       : 1;
        //byte 2L
        uint8_t _MASKLANE       : 4;
        //byte 2H
        uint16_t _TRNCNT        : 10;
        //byte 3L
        uint8_t _none           : 2;
        uint8_t _rsv1           : 2;
        //byte 3H
        uint8_t _CDBGRSTREQ     : 1;
        uint8_t _CDBGRSTACK     : 1;
        uint8_t _CDBGPWRUPREQ   : 1;
        uint8_t _CDBGPWRUPACK   : 1;
        uint8_t _CSYSPWRUPREQ   : 1;
        uint8_t _CSYSPWRUPACK   : 1;
    };
    uint32_t _raw;
}adiv5_ctrl_stat_t;

typedef union{
    struct __attribute__((packed)){
        uint8_t Size        :3;
        uint8_t             :1;
        uint8_t AddrInc     :2;
        uint8_t DeviceEin   :1;
        uint8_t TrInProg    :1;
        uint8_t Mode        :4;
        uint16_t            :11;
        uint8_t SPIDEN      :1;
        uint8_t Prot        :7;
        uint8_t DbgSwEnable :1;
    };
    uint32_t _raw;
}adiv5_scw_t;

typedef struct{
    adiv5_cmd_e cmd;
    uint16_t regAddr;
    uint8_t rnw;
    uint32_t outVal;
}adiv5_cmd_params_t;

typedef struct{
    _fxnTransfer IrShift;
    _fxnTransfer DrShift;
}adiv5_fxn_t;

typedef struct{
    void *target;
    adiv5_cmd_e IssueCmd;
    adiv5_fxn_t fxn;
    uint32_t ap_select;
    uint32_t ap_current;
    uint32_t ap_num;
    uint16_t regAddr;
    uint8_t packed_transfers;
    adiv5_ctrl_stat_t dp_ctrl_stat;
    adiv5_scw_t ap_scw;
}adiv5_t;

/**
 * Public Functions
 */

void adiv5_init(adiv5_t *dap);

adiv5_err_e adiv5_idcode(adiv5_t *dap, uint32_t *id);

adiv5_err_e adiv5_ap_read(adiv5_t *dap, uint32_t reg, uint32_t *data);
//
adiv5_err_e adiv5_ap_write(adiv5_t *dap, uint32_t reg, uint32_t data);
//
adiv5_err_e adiv5_dp_read(adiv5_t *dap, uint32_t reg, uint32_t *data);
//
adiv5_err_e adiv5_dp_write(adiv5_t *dap, uint32_t reg, uint32_t data);

adiv5_err_e cc26xx_cm4f_bankselect(adiv5_t *dap, uint32_t reg);


#endif /* LIB_ADIV5_JTAG_DP_H_ */
