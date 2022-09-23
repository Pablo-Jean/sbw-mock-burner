/*
 * data_link.h
 *
 *  Created on: 25 de abr de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_SPY_BY_WIRE_DATA_LINK_H_
#define LIB_SPY_BY_WIRE_DATA_LINK_H_

#include <stdint.h>
#include <stddef.h>

#include "../common/typedefs.h"

/**
 * Macros
 */

#define HIGH            1
#define LOW             0

#define INPUT           HIGH
#define OUTPUT          LOW


#define F_BYTE                     8
#define F_WORD                     16
#define F_ADDR                     20
#define F_LONG                     32

#define _NOP()                    asm(" nop")

// JTAG identification values for all existing Flash-based MSP430 devices
//! \brief JTAG identification value for 430X architecture devices
#define JTAG_ID                    0x89
//! \brief JTAG identification value for 430Xv2 architecture devices
#define JTAG_ID91                  0x91
//! \brief JTAG identification value for 430Xv2 architecture FR4XX/FR2xx devices
#define JTAG_ID98                  0x98
//! \brief JTAG identification value for 430Xv2 architecture FR59XX devices
#define JTAG_ID99                  0x99
// Additional instructions for JTAG_ID91 architectures
//! \brief Instruction to determine device's CoreIP
#define IR_COREIP_ID               0xE8   // original value: 0x17
//! \brief Instruction to determine device's DeviceID
#define IR_DEVICE_ID               0xE1   // original value: 0x87


//! \brief SBW macro: set RST signal
#define SetRST(link)   {link->fxn.sbwtdioW(HIGH);}
//! \brief SBW macro: clear RST signal
#define ClrRST(link)   {link->fxn.sbwtdioW(LOW);}
//! \brief SBW macro: release RST signal (pro forma)
#define ReleaseRST(link) ()
//! \brief SBW macro: set TEST pin signal
#define SetTST(link)   {link->fxn.sbwtckW(HIGH); }
//! \brief SBW macro: clear TEST pin signal
#define ClrTST(link)   {link->fxn.sbwtckW(LOW); }

/**
 * Enumerates
 */

typedef enum{
    TMS_LOW,
    TMS_HIGH
}tms_e;

typedef enum{
    TDI_LOW,
    TDI_HIGH
}tdi_e;

typedef enum{
    TDO_LOW,
    TDO_HIGH
}tdo_e;

typedef enum{
    LATCH_ST_LOW,
    LATCH_ST_HIGH
}latch_st_e;

/**
 * typedefs
 */

/**
 * Structs and Unions
 */

typedef struct{
    struct{
        GpioSetIO sbwtdioSet;
        GpioOut sbwtdioW;
        GpioIn sbwtdioR;
        GpioOut sbwtckW;
        DelayUs delayUs;
        DelayMs delayMs;
    }fxn;
}sbw_data_link_t;

/**
 * Function prototypes
 */

void data_link_start(sbw_data_link_t* link);

uint8_t GetTDO(sbw_data_link_t* link);
uint8_t GetTCLK(sbw_data_link_t* link);
uint8_t GetSBWDATO(sbw_data_link_t* link);

void TMSL_TDIL(sbw_data_link_t* link);
void TMSH_TDIL(sbw_data_link_t* link);
void TMSL_TDIH(sbw_data_link_t* link);
void TMSH_TDIH(sbw_data_link_t* link);
void TMSL_TDIH_TDOrd(sbw_data_link_t* link);
void TMSL_TDIL_TDOrd(sbw_data_link_t* link);
void TMSH_TDIH_TDOrd(sbw_data_link_t* link);
void TMSH_TDIL_TDOrd(sbw_data_link_t* link);
void ClrTCLK(sbw_data_link_t* link);
void SetTCLK(sbw_data_link_t* link);
void TCLKstrobes(sbw_data_link_t* link, word Amount);
unsigned long AllShifts(sbw_data_link_t* link, word Format, uint32_t Data);

void data_link_delay_us(sbw_data_link_t* link, uint32_t us);

void data_link_delay_ms(sbw_data_link_t* link, uint32_t ms);

void EntrySequences_RstHigh_SBW(sbw_data_link_t* link);
void EntrySequences_RstLow_SBW(sbw_data_link_t* link);


#endif /* LIB_SPY_BY_WIRE_DATA_LINK_H_ */
