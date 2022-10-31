/*
 * arm7.h
 *
 *  Created on: 26 de out de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_CORE_ARM7_H_
#define LIB_CORE_ARM7_H_


#include <stdint.h>
#include <stddef.h>

#include "../adiv5/jtag-dp.h"

/**
 * Macros
 */

#define ARM7_CORE_REG_WRITE_BIT      (1<<16)
#define ARM7_CORE_REG_READ_BIT       (0<<16)

#define ARM7_CPU_SCS_BASE               0xE000E000
#define ARM7_CPU_SCS_DHCSR              (ARM7_CPU_SCS_BASE | 0x0DF0)
#define ARM7_CPU_SCS_DCRSR              (ARM7_CPU_SCS_BASE | 0x0DF4)
#define ARM7_CPU_SCS_DCRDR              (ARM7_CPU_SCS_BASE | 0x0DF8)

#define ARM7_CPU_SCS_DHCSR_PASSWRD      (0xA05F << 16)

/**
 * Enumerates
 */

/* Cortex-M DCRSR.REGSEL selectors */
enum {
    ARMV7M_REGSEL_R0,
    ARMV7M_REGSEL_R1,
    ARMV7M_REGSEL_R2,
    ARMV7M_REGSEL_R3,

    ARMV7M_REGSEL_R4,
    ARMV7M_REGSEL_R5,
    ARMV7M_REGSEL_R6,
    ARMV7M_REGSEL_R7,

    ARMV7M_REGSEL_R8,
    ARMV7M_REGSEL_R9,
    ARMV7M_REGSEL_R10,
    ARMV7M_REGSEL_R11,

    ARMV7M_REGSEL_R12,
    ARMV7M_REGSEL_R13,
    ARMV7M_REGSEL_R14,
    ARMV7M_REGSEL_PC = 15,

    ARMV7M_REGSEL_xPSR = 16,
    ARMV7M_REGSEL_MSP,
    ARMV7M_REGSEL_PSP,

    ARMV8M_REGSEL_MSP_NS = 0x18,
    ARMV8M_REGSEL_PSP_NS,
    ARMV8M_REGSEL_MSP_S,
    ARMV8M_REGSEL_PSP_S,
    ARMV8M_REGSEL_MSPLIM_S,
    ARMV8M_REGSEL_PSPLIM_S,
    ARMV8M_REGSEL_MSPLIM_NS,
    ARMV8M_REGSEL_PSPLIM_NS,

    ARMV7M_REGSEL_PMSK_BPRI_FLTMSK_CTRL = 0x14,
    ARMV8M_REGSEL_PMSK_BPRI_FLTMSK_CTRL_S = 0x22,
    ARMV8M_REGSEL_PMSK_BPRI_FLTMSK_CTRL_NS = 0x23,
    ARMV7M_REGSEL_FPSCR = 0x21,

    /* 32bit Floating-point registers */
    ARMV7M_REGSEL_S0 = 0x40,
    ARMV7M_REGSEL_S1,
    ARMV7M_REGSEL_S2,
    ARMV7M_REGSEL_S3,
    ARMV7M_REGSEL_S4,
    ARMV7M_REGSEL_S5,
    ARMV7M_REGSEL_S6,
    ARMV7M_REGSEL_S7,
    ARMV7M_REGSEL_S8,
    ARMV7M_REGSEL_S9,
    ARMV7M_REGSEL_S10,
    ARMV7M_REGSEL_S11,
    ARMV7M_REGSEL_S12,
    ARMV7M_REGSEL_S13,
    ARMV7M_REGSEL_S14,
    ARMV7M_REGSEL_S15,
    ARMV7M_REGSEL_S16,
    ARMV7M_REGSEL_S17,
    ARMV7M_REGSEL_S18,
    ARMV7M_REGSEL_S19,
    ARMV7M_REGSEL_S20,
    ARMV7M_REGSEL_S21,
    ARMV7M_REGSEL_S22,
    ARMV7M_REGSEL_S23,
    ARMV7M_REGSEL_S24,
    ARMV7M_REGSEL_S25,
    ARMV7M_REGSEL_S26,
    ARMV7M_REGSEL_S27,
    ARMV7M_REGSEL_S28,
    ARMV7M_REGSEL_S29,
    ARMV7M_REGSEL_S30,
    ARMV7M_REGSEL_S31,
};

/* offsets into armv7m core register cache */
typedef enum {
    /* for convenience, the first set of indices match
     * the Cortex-M DCRSR.REGSEL selectors
     */
    ARMV7M_R0 = ARMV7M_REGSEL_R0,
    ARMV7M_R1 = ARMV7M_REGSEL_R1,
    ARMV7M_R2 = ARMV7M_REGSEL_R2,
    ARMV7M_R3 = ARMV7M_REGSEL_R3,

    ARMV7M_R4 = ARMV7M_REGSEL_R4,
    ARMV7M_R5 = ARMV7M_REGSEL_R5,
    ARMV7M_R6 = ARMV7M_REGSEL_R6,
    ARMV7M_R7 = ARMV7M_REGSEL_R7,

    ARMV7M_R8 = ARMV7M_REGSEL_R8,
    ARMV7M_R9 = ARMV7M_REGSEL_R9,
    ARMV7M_R10 = ARMV7M_REGSEL_R10,
    ARMV7M_R11 = ARMV7M_REGSEL_R11,

    ARMV7M_R12 = ARMV7M_REGSEL_R12,
    ARMV7M_R13 = ARMV7M_REGSEL_R13,
    ARMV7M_R14 = ARMV7M_REGSEL_R14,
    ARMV7M_PC = ARMV7M_REGSEL_PC,

    ARMV7M_xPSR = ARMV7M_REGSEL_xPSR,
    ARMV7M_MSP = ARMV7M_REGSEL_MSP,
    ARMV7M_PSP = ARMV7M_REGSEL_PSP,

    /* following indices are arbitrary, do not match DCRSR.REGSEL selectors */

    /* A block of container and contained registers follows:
     * THE ORDER IS IMPORTANT to the end of the block ! */
    /* working register for packing/unpacking special regs, hidden from gdb */
    ARMV7M_PMSK_BPRI_FLTMSK_CTRL,

    /* WARNING: If you use armv7m_write_core_reg() on one of 4 following
     * special registers, the new data go to ARMV7M_PMSK_BPRI_FLTMSK_CTRL
     * cache only and are not flushed to CPU HW register.
     * To trigger write to CPU HW register, add
     *      armv7m_write_core_reg(,,ARMV7M_PMSK_BPRI_FLTMSK_CTRL,);
     */
    ARMV7M_PRIMASK,
    ARMV7M_BASEPRI,
    ARMV7M_FAULTMASK,
    ARMV7M_CONTROL,
    /* The end of block of container and contained registers */

    /* ARMv8-M specific registers */
    ARMV8M_MSP_NS,
    ARMV8M_PSP_NS,
    ARMV8M_MSP_S,
    ARMV8M_PSP_S,
    ARMV8M_MSPLIM_S,
    ARMV8M_PSPLIM_S,
    ARMV8M_MSPLIM_NS,
    ARMV8M_PSPLIM_NS,

    /* A block of container and contained registers follows:
     * THE ORDER IS IMPORTANT to the end of the block ! */
    ARMV8M_PMSK_BPRI_FLTMSK_CTRL_S,
    ARMV8M_PRIMASK_S,
    ARMV8M_BASEPRI_S,
    ARMV8M_FAULTMASK_S,
    ARMV8M_CONTROL_S,
    /* The end of block of container and contained registers */

    /* A block of container and contained registers follows:
     * THE ORDER IS IMPORTANT to the end of the block ! */
    ARMV8M_PMSK_BPRI_FLTMSK_CTRL_NS,
    ARMV8M_PRIMASK_NS,
    ARMV8M_BASEPRI_NS,
    ARMV8M_FAULTMASK_NS,
    ARMV8M_CONTROL_NS,
    /* The end of block of container and contained registers */

    /* 64bit Floating-point registers */
    ARMV7M_D0,
    ARMV7M_D1,
    ARMV7M_D2,
    ARMV7M_D3,
    ARMV7M_D4,
    ARMV7M_D5,
    ARMV7M_D6,
    ARMV7M_D7,
    ARMV7M_D8,
    ARMV7M_D9,
    ARMV7M_D10,
    ARMV7M_D11,
    ARMV7M_D12,
    ARMV7M_D13,
    ARMV7M_D14,
    ARMV7M_D15,

    /* Floating-point status register */
    ARMV7M_FPSCR,

    /* for convenience add registers' block delimiters */
    ARMV7M_LAST_REG,
    ARMV7M_CORE_FIRST_REG = ARMV7M_R0,
    ARMV7M_CORE_LAST_REG = ARMV7M_xPSR,
    ARMV7M_FPU_FIRST_REG = ARMV7M_D0,
    ARMV7M_FPU_LAST_REG = ARMV7M_FPSCR,
    ARMV8M_FIRST_REG = ARMV8M_MSP_NS,
    ARMV8M_LAST_REG = ARMV8M_CONTROL_NS,
}arm7_core_reg_e;

typedef enum{
    ARM7_OK,
    ARM7_FAIL,
    ARM7_INVALID_ARGS
}arm7_err_e;

typedef enum{
    ARM7_RUNNING,
    ARM7_HALT
}arm7_halt_e;

typedef enum{
    ARM7_LINK_JTAGDP,
    ARM7_LINK_SWD
}arm7_link_e;


/**
 * Typedefs
 */

typedef struct{
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    uint32_t R12;
    uint32_t SP;
    uint32_t LR;
    uint32_t PC;
    uint32_t xPSR;
    uint32_t MSP;
    uint32_t PSP;
    uint32_t control_mask;
}arm7_core_reg_t;

typedef union{
    struct __attribute__((packed)){
        uint8_t C_DEBUGEN       :1;
        uint8_t C_HALTA         :1;
        uint8_t C_STEP          :1;
        uint8_t C_MASKINTS      :1;
        uint8_t                 :1;
        uint8_t C_SNAPSTALL     :1;
        uint8_t                 :2;
        uint8_t                 :8;
        uint8_t S_REGRDY        :1;
        uint8_t S_HALT          :1;
        uint8_t S_SLEEP         :1;
        uint8_t S_LOCKUP        :1;
        uint8_t                 :4;
        uint8_t S_RETIRE_ST     :1;
        uint8_t S_RESET_ST      :1;
        uint8_t                 :6;
    };
    uint32_t _raw;
}arm7_dhcsr_t;

typedef struct{
    void *link;
    arm7_link_e linkType;
    uint32_t idcode;
    uint32_t core_reg_addr;
    uint32_t tarAddr;
    arm7_dhcsr_t dhcsr_reg;
    arm7_core_reg_t core_reg;
}arm7_t;

/**
 * Publics
 */

arm7_err_e arm7_init(arm7_t *arm, uint8_t prot);

arm7_err_e arm7_set_reg(arm7_t *arm, uint32_t reg, uint32_t data);

arm7_err_e arm7_get_reg(arm7_t *arm, uint32_t reg, uint32_t *data);

arm7_err_e arm7_update_core_regs(arm7_t *arm);

arm7_err_e arm7_get_core_reg(arm7_t *arm, arm7_core_reg_e reg, uint32_t *data);

arm7_err_e arm7_set_core_reg(arm7_t *arm, arm7_core_reg_e reg, uint32_t data);

arm7_err_e arm7_set_reg_buffered(arm7_t *arm, uint32_t reg, uint32_t *data, uint32_t len);

arm7_err_e arm7_get_reg_buffered(arm7_t *arm, uint32_t reg, uint32_t *data, uint32_t len);



#endif /* LIB_CORE_ARM7_H_ */
