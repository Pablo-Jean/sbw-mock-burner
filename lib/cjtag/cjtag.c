/*
 * cjtag.c
 *
 *  Created on: 8 de set de 2022
 *      Author: pablo.jean
 */

#include "cjtag.h"


/**
 * Macros
 */

#define _DIR_OUTPUT             0
#define _DIR_INPUT              1

#define _HIGH                   1
#define _LOW                    0

#define _NOP()                  asm(" nop")

#define   NOPS                  { _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); }


#define _ir_shift_enter_pat     0x3
#define _ir_shift_update_pat    0x3

#define _dr_shift_enter_pat     0x1
#define _dr_shift_update_pat    0x3

//! \brief SBW macro: set TMS signal
//#define   TMS(x)    {cjtag->pinFxn.tmsSetIO(!x); NOPS cjtag->pinFxn.tckSetIO(_LOW); NOPS cjtag->pinFxn.tckSetIO(_HIGH); NOPS}

//! \brief SBW macro: clear TMS signal and immediately set it high again in
//! the SBWTCK low phase to enter the TDI slot with a high signal
//! \details Only used to clock TCLK (=TDI for SBW) in Run-Test/IDLE mode of
//! the JTAG FSM
//#define   TMSLDH  {link->fxn.sbwtdioW(LOW); NOPS cjtag->pinFxn.tckSetIO(LOW); NOPS link->fxn.sbwtdioW(HIGH); cjtag->pinFxn.tckSetIO(HIGH); NOPS}

//! \brief SBW macro: Set TDI = 1
#define   TDI(x)    { cjtag->pinFxn.tmsSetIO(!x); NOPS cjtag->pinFxn.tckSetIO(_LOW);  NOPS cjtag->pinFxn.tckSetIO(_HIGH); NOPS }

//! \brief SBW macro: TDO cycle without reading TDO
//#define   TDOnrd()  {cjtag->pinFxn.tmsSetDir(_DIR_INPUT); cjtag->pinFxn.tckSetIO(_LOW); NOPS cjtag->pinFxn.tckSetIO(_HIGH); cjtag->pinFxn.tmsSetDir(_DIR_OUTPUT); NOPS}

//! \brief SBW macro: TDO cycle with TDO read
#define   TDO_RD()  { cjtag->pinFxn.tmsSetDir(_DIR_INPUT); cjtag->pinFxn.tckSetIO(_LOW); NOPS cjtag->tdo_bit = cjtag->pinFxn.tmsGet(); cjtag->pinFxn.tckSetIO(_HIGH); cjtag->pinFxn.tmsSetDir(_DIR_OUTPUT); NOPS}

////! \brief SBW macro: RDY cycle without reading RDY
//#define   RDYnrd()  {cjtag->pinFxn.tmsSetDir(_DIR_INPUT); cjtag->pinFxn.tckSetIO(_LOW); NOPS cjtag->pinFxn.tckSetIO(_HIGH); cjtag->pinFxn.tmsSetDir(_DIR_OUTPUT); NOPS}

//! \brief SBW macro: RDY cycle with RDY read
#define   RDY_RD()  { cjtag->pinFxn.tmsSetDir(_DIR_INPUT); cjtag->pinFxn.tckSetIO(_LOW); NOPS; cjtag->rdy_bit = cjtag->pinFxn.tmsGet(); cjtag->pinFxn.tckSetIO(_HIGH); cjtag->pinFxn.tmsSetDir(_DIR_OUTPUT); NOPS}


/**
 * Privates
 */

void __cjtag_write_tck(cjtag_t *cjtag, uint8_t d){
    cjtag->pinFxn.tckSetIO(d);
}

void __cjtag_write_tms(cjtag_t *cjtag, uint8_t d){
    cjtag->pinFxn.tmsSetIO(d);
}

void __cjtag_set_tms_io(cjtag_t *cjtag, uint8_t InO){
    cjtag->pinFxn.tmsSetDir(InO);
}

uint8_t __cjtag_get_tms(cjtag_t *cjtag){
    return cjtag->pinFxn.tmsGet();
}

void __update_tms(cjtag_t *cjtag, uint8_t tms){
    switch(cjtag->fsmState){
    case CJTAG_TEST_LOGIC_RESET:
        if (tms)
            cjtag->fsmState = CJTAG_TEST_LOGIC_RESET;
        else
            cjtag->fsmState = CJTAG_RUN_TEST_IDLE;
        break;
    case CJTAG_RUN_TEST_IDLE:
        if (tms)
            cjtag->fsmState = CJTAG_SELECT_DR_SCAN;
        else
            cjtag->fsmState = CJTAG_RUN_TEST_IDLE;
        break;
    case CJTAG_SELECT_DR_SCAN:
        if (tms)
            cjtag->fsmState = CJTAG_SELECT_IR_SCAN;
        else
            cjtag->fsmState = CJTAG_CAPTURE_DR;
        break;
    case CJTAG_CAPTURE_DR:
        if (tms)
            cjtag->fsmState = CJTAG_EXIT_1_DR;
        else
            cjtag->fsmState = CJTAG_SHIFT_DR;
        break;
    case CJTAG_SHIFT_DR:
        if (tms)
            cjtag->fsmState = CJTAG_EXIT_1_DR;
        else
            cjtag->fsmState = CJTAG_SHIFT_DR;
        break;
    case CJTAG_EXIT_1_DR:
        if (tms)
            cjtag->fsmState = CJTAG_UPDATE_DR;
        else
            cjtag->fsmState = CJTAG_PAUSE_DR;
        break;
    case CJTAG_PAUSE_DR:
        if (tms)
            cjtag->fsmState = CJTAG_EXIT_2_DR;
        else
            cjtag->fsmState = CJTAG_PAUSE_DR;
        break;
    case CJTAG_EXIT_2_DR:
        if (tms)
            cjtag->fsmState = CJTAG_UPDATE_DR;
        else
            cjtag->fsmState = CJTAG_SHIFT_DR;
        break;
    case CJTAG_UPDATE_DR:
        if(tms)
            cjtag->fsmState = CJTAG_SELECT_DR_SCAN;
        else
            cjtag->fsmState = CJTAG_RUN_TEST_IDLE;
        break;
    case CJTAG_SELECT_IR_SCAN:
        if (tms)
            cjtag->fsmState = CJTAG_TEST_LOGIC_RESET;
        else
            cjtag->fsmState = CJTAG_CAPTURE_IR;
        break;
    case CJTAG_CAPTURE_IR:
        if(tms)
            cjtag->fsmState = CJTAG_EXIT_1_IR;
        else
            cjtag->fsmState = CJTAG_SHIFT_IR;
        break;
    case CJTAG_SHIFT_IR:
        if (tms)
            cjtag->fsmState = CJTAG_EXIT_1_IR;
        else
            cjtag->fsmState = CJTAG_SHIFT_IR;
        break;
    case CJTAG_EXIT_1_IR:
        if (tms)
            cjtag->fsmState = CJTAG_UPDATE_IR;
        else
            cjtag->fsmState = CJTAG_PAUSE_IR;
        break;
    case CJTAG_PAUSE_IR:
        if (tms)
            cjtag->fsmState = CJTAG_EXIT_2_IR;
        else
            cjtag->fsmState = CJTAG_PAUSE_IR;
        break;
    case CJTAG_EXIT_2_IR:
        if (tms)
            cjtag->fsmState = CJTAG_SHIFT_IR;
        else
            cjtag->fsmState = CJTAG_UPDATE_IR;
        break;
    case CJTAG_UPDATE_IR:
        if (tms)
            cjtag->fsmState = CJTAG_SELECT_DR_SCAN;
        else
            cjtag->fsmState = CJTAG_RUN_TEST_IDLE;
        break;
    }
}

void TMS(cjtag_t *cjtag, uint8_t x){
    cjtag->pinFxn.tmsSetIO(x);
    NOPS;
    cjtag->pinFxn.tckSetIO(_LOW);
    NOPS;
    cjtag->pinFxn.tckSetIO(_HIGH);
    NOPS;

    __update_tms(cjtag, x);
}

// Scan formats
void __cjtag_oscan0(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *rdy, uint8_t *tdo){
    TDI(tdi);
    TMS(cjtag, tms);
    RDY_RD();
    TDO_RD();
    if (rdy)
        *rdy = cjtag->rdy_bit;
    if (tdo)
        *tdo = cjtag->tdo_bit;
}
void __cjtag_oscan1(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    TDI(tdi);
    TMS(cjtag, tms);
    TDO_RD();
//    if (tdo)
//        *tdo = cjtag->tdo_bit;
}

void __cjtag_oscan2(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi);
            TMS(cjtag, tms);
            TDO_RD();
//            if (tdo)
//                *tdo = cjtag->tdo_bit;
            break;
        default:
            TMS(cjtag, tms);
            break;
    }
}

void __cjtag_oscan3(cjtag_t *cjtag, uint8_t tms, uint8_t tdi){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi);
            TMS(cjtag, tms);
            break;
        default:
            TMS(cjtag, tms);
            break;
    }
}

void __cjtag_oscan4(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *rdy, uint8_t *tdo){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi);
            RDY_RD();
            TDO_RD();
            if (tdo)
                *tdo = cjtag->tdo_bit;
            if (rdy)
                *rdy = cjtag->rdy_bit;
            break;
        default:
            TDI(tdi);
            TMS(cjtag, tms);
            RDY_RD();
            TDO_RD();
            if (tdo)
                *tdo = cjtag->tdo_bit;
            if (rdy)
                *rdy = cjtag->rdy_bit;
            break;
    }
}

void __cjtag_oscan5(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi);
            TDO_RD();
            if (tdo)
                *tdo = cjtag->tdo_bit;
            break;
        default:
            TDI(tdi);
            TMS(cjtag, tms);
            TDO_RD();
            if (tdo)
                *tdo = cjtag->tdo_bit;
            break;
    }
}

void __cjtag_oscan6(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi);
            TMS(cjtag, tms);
            TDO_RD();
            if (tdo)
                *tdo = cjtag->tdo_bit;
            break;
        default:
            TMS(cjtag, tms);
            break;
    }
}

void __cjtag_oscan7(cjtag_t *cjtag, uint8_t tms, uint8_t tdi){
    switch (cjtag->fsmState) {
        case CJTAG_SHIFT_IR:
        case CJTAG_SHIFT_DR:
            TDI(tdi)
            break;
        default:
            TMS(cjtag, tms);
            break;
    }
}

void __cjtag_jscan0(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    cjtag->pinFxn.tdiSetIo(tdi);
    cjtag->pinFxn.tmsSetIO(tms);
    NOPS;
    cjtag->pinFxn.tckSetIO(_HIGH);
    NOPS;
    cjtag->pinFxn.tckSetIO(_LOW);
    NOPS;
    cjtag->tdo_bit = cjtag->pinFxn.tdoGet();
    if (tdo != NULL)
        *tdo = cjtag->tdo_bit;
    NOPS;
    __update_tms(cjtag, tms);
}

void __cjtag_write(cjtag_t *cjtag, uint8_t tms, uint8_t tdi, uint8_t *tdo){
    switch(cjtag->scanFormat){
    case CJTAG_JSCAN_0:
        __cjtag_jscan0(cjtag, tms, tdi, tdo);
        break;
    case CJTAG_OSCAN_0:
        __cjtag_oscan0(cjtag, tms, tdi, NULL, tdo);
        break;
    case CJTAG_OSCAN_1:
        __cjtag_oscan1(cjtag, tms, tdi, tdo);
        break;
    case CJTAG_OSCAN_2:
        __cjtag_oscan2(cjtag, tms, tdi, tdo);
        break;
    case CJTAG_OSCAN_3:
        __cjtag_oscan3(cjtag, tms, tdi);
        break;
    case CJTAG_OSCAN_4:
        __cjtag_oscan4(cjtag, tms, tdi, NULL, tdo);
        break;
    case CJTAG_OSCAN_5:
        __cjtag_oscan5(cjtag, tms, tdi, tdo);
        break;
    case CJTAG_OSCAN_6:
        __cjtag_oscan6(cjtag, tms, tdi, tdo);
        break;
    case CJTAG_OSCAN_7:
        __cjtag_oscan7(cjtag, tms, tdi);
        break;
    default:
        // works only on TMS pin
        TMS(cjtag, tms);

        break;
    }
}

void __cjtag_zbs(cjtag_t *cjtag){
    if (cjtag == NULL)
        return;

    if (cjtag->fsmState == CJTAG_RUN_TEST_IDLE){
        cjtag_TmsPattent(cjtag, 0x5, 4); // DRSELECT -> DRCAPTURE -> DREXIT1 -> DRPAUSE
    }
    cjtag_TmsPattent(cjtag, 0x3, 3); // DREXIT2 -> DRUPDATE -> IDLE
    cjtag_TmsPattent(cjtag, 0x5, 4); // DRSELECT -> DRCAPTURE -> DREXIT1 -> DRPAUSE
}

void __cjtag_escapeSeq(cjtag_t *cjtag, uint8_t set){
    uint8_t i;

    for (i=0 ; i<set ; i++){
        if (cjtag->pinFxn.tmsGet()){
            cjtag->pinFxn.tmsSetIO(_LOW);
        }
        else{
            cjtag->pinFxn.tmsSetIO(_HIGH);
        }
    }
}

void __cjtag_strobeTck(cjtag_t *cjtag, uint32_t amount){
    uint32_t i;

    for (i=0 ; i<amount ; i++){
        if (i%2){
            cjtag->pinFxn.tckSetIO(_LOW);
        }
        else{
            cjtag->pinFxn.tckSetIO(_HIGH);
        }
    }
}

void __cjtag_CmdLevel(cjtag_t *cjtag, uint8_t level){
    uint8_t i;

    // Open control level 2 by doing 2 zero bit scans, then a 1 bit DR shift
    for (i=0 ; i<level ; i++){
    // zero bit scan
        __cjtag_zbs(cjtag);
    }
    // Shift in 1 bit to lock control level
    cjtag_TmsPattent(cjtag, 0xAD, 9); //
    //cjtag_Tm-sPattent(cjtag, 0x3, 5); //
}

void __cjtag_IssueCmd1Par(cjtag_t *cjtag, uint8_t cmd, uint8_t par){
    uint8_t i, d[2];

    d[0] = cmd;
    d[1] = par;
    for (i=0 ; i<2 ; i++){
        if (cjtag->fsmState == CJTAG_PAUSE_DR){
            // go to
            cjtag_writeOscan(cjtag, 0x1, 0, 2); // DREXIT2
        }
        else if (cjtag->fsmState == CJTAG_RUN_TEST_IDLE){
            cjtag_writeOscan(cjtag, 0x5, 0, 4); // DRSELECT -> DRCAPTURE -> EXIT1 -> PAUSE
            cjtag_writeOscan(cjtag, 0x1, 0, 2); // DRSELECT -> DRCAPTURE -> DRSHIFT
        }
        // i == 0 : CMD
        // i == 1 : PARAMETER1
        if (d[i] > 0){
            // issue the number of bits in shift state
            cjtag_writeOscan(cjtag, 0x0, 0, d[i]-1); // SHIFT(n)
        }
        if (i < 1){
            cjtag_writeOscan(cjtag, 0x3, 0, 3); // DREXIT1 -> DRUPDATE -> IDLE
            cjtag_writeOscan(cjtag, 0x5, 0, 4); // DRSELECT -> DRCAPTURE -> DREXIT1 -> DRPAUSE
        }
    }
    if (cmd == OPCODE_STFMT){
        cjtag_writeOscan(cjtag, 0x3, 0, 3); // IDLE

        cjtag->scanFormat = CJTAG_OSCAN_2;

        cjtag_writeOscan(cjtag, 0x5, 0, 4); // DRSELECT -> DRCAPTURE -> DREXIT1 -> DRPAUSE
    }
    else{
        cjtag_writeOscan(cjtag, 0x3, 0, 3); // DREXIT1 -> DRUPDATE -> IDLE
        cjtag_writeOscan(cjtag, 0x5, 0, 4); // DRSELECT -> DRCAPTURE -> DREXIT1 -> DRPAUSE
    }

}

/**
 * Publics
 */

void cjtag_TmsPattent(cjtag_t *cjtag, uint64_t pattern, uint8_t bits){
    uint8_t i;

    for (i=0 ; i<bits ; i++){
//        __cjtag_write(cjtag, (pattern>>i)&0x1, _LOW, NULL);
        TMS(cjtag, (pattern>>i)&0x1);
    }
}

void cjtag_init(cjtag_t *cjtag, cjtag_mode_init_e mode){
    if (cjtag == NULL)
        return;
    if (cjtag->pinFxn.tckSetIO == NULL || cjtag->pinFxn.tmsGet == NULL || cjtag->pinFxn.tmsSetDir == NULL || cjtag->pinFxn.tmsSetIO == NULL)
        return;

    cjtag->scanFormat = CJTAG_UNKNOWN;
    cjtag->pinFxn.tmsSetDir(_DIR_OUTPUT);
    cjtag->pinFxn.tckSetIO(_HIGH);
    cjtag->pinFxn.tmsSetIO(_HIGH);
    if (cjtag->pinFxn.tdiSetIo != NULL){
        cjtag->pinFxn.tdiSetIo(_HIGH);
    }

    cjtag_reset(cjtag);

    __cjtag_strobeTck(cjtag, 1000);
    Task_sleep(10);

    cjtag_release(cjtag);

    __cjtag_strobeTck(cjtag, 40E3);
    // wait chip to reset
    Task_sleep(10);
//
    __cjtag_strobeTck(cjtag, 40E3);

//    cjtag->pinFxn.tmsSetIO(_LOW);
//
//    Task_sleep(10);

    // Escape sequence "Reset": >= 8 TMS line state changes while TCK == HIGH
    // cJTAG TAP7 is in JScan0 mode now (TCK + TMS are transmitted)
    // Put device on IDLE (via TLR)
    cjtag_TmsPattent(cjtag, CJTAG_PATTERN_TLR, 16);
    cjtag_TmsPattent(cjtag, CJTAG_PATTERN_IDLE, 1);

    // Execute a BYPASS IR to start at control level 0
     cjtag_TmsPattent(cjtag, 0x03, 3); // DRSELECT -> IRSELECT -> IRCAPTURE
     cjtag_TmsPattent(cjtag, 0x00, 64); // Shift in 64 bits to load bypass instruction
     cjtag_TmsPattent(cjtag, 0x03, 3); // IREXIT1 -> IRUPDATE -> IDLE

    // Execute a BYPASS IR to start at control level 0
     cjtag_TmsPattent(cjtag, 0x01, 2); // DRSELECT -> DRCAPTURE
     cjtag_TmsPattent(cjtag, 0x00, 32); // Shift in 64 bits to load Data
     cjtag_TmsPattent(cjtag, 0x03, 3); // DREXIT1 -> DRUPDATE -> IDLE

     // Execute a BYPASS IR to start at control level 0
      cjtag_TmsPattent(cjtag, 0x01, 2); // DRSELECT -> DRCAPTURE
      cjtag_TmsPattent(cjtag, 0x00, 32); // Shift in 64 bits to load Data
      cjtag_TmsPattent(cjtag, 0x03, 3); // DREXIT1 -> DRUPDATE -> IDLE

      // Execute a BYPASS IR to start at control level 0
       cjtag_TmsPattent(cjtag, 0x03, 3); // DRSELECT -> IRSELECT -> IRCAPTURE
       cjtag_TmsPattent(cjtag, 0x00, 64); // Shift in 64 bits to load bypass instruction
       cjtag_TmsPattent(cjtag, 0x03, 3); // IREXIT1 -> IRUPDATE -> IDLE

    __cjtag_CmdLevel(cjtag, 2);

//    __cjtag_IssueCmd1Par(cjtag, OPCODE_STC1, _CMD_STC1_SEDGE_RISE_EDGE);
//    cjtag_writeOscan(cjtag, 0x0, 0, 4);
//    cjtag_TmsPattent(cjtag, 0x3, 4);

    if (mode == CJTAG_MODE_2PIN){
        __cjtag_IssueCmd1Par(cjtag, OPCODE_STFMT, _CMD_STFMT_OSCAN2);
//        cjtag_writeOscan(cjtag, 0x3, 0, 4);

//        cjtag_TmsPattent(cjtag, CJTAG_PATTERN_TLR, 16);
//        cjtag_TmsPattent(cjtag, CJTAG_PATTERN_IDLE, 1);

//        __cjtag_IssueCmd1Par(cjtag, OPCODE_STMC, 1);
//        cjtag_writeOscan(cjtag, 0x3, 0, 4);
    }
    else{
        __cjtag_IssueCmd1Par(cjtag, OPCODE_STC2, 9);
//        cjtag_writeOscan(cjtag, 0x3, 0, 4);

        cjtag->scanFormat = CJTAG_JSCAN_0;
//        cjtag_TmsPattent(cjtag, CJTAG_PATTERN_TLR, 16);
//        cjtag_TmsPattent(cjtag, CJTAG_PATTERN_IDLE, 1);

        __cjtag_IssueCmd1Par(cjtag, OPCODE_STMC, 1);

//        cjtag_writeOscan(cjtag, 0x3, 0, 4);
    }

//     Execute a BYPASS IR to start at control level 0
    cjtag_writeOscan(cjtag, 0x01, 0xFF, 2); // EXIT2 -> DRSHIDR
    cjtag_writeOscan(cjtag, 0x00, ~0, 64); // Shift in 64 bits to load bypass instruction
    cjtag_writeOscan(cjtag, 0x03, 0xFF, 3); // IREXIT1 -> IRUPDATE -> IDLE


    // Execute a BYPASS IR to start at control level 0
//     cjtag_TmsPattent(cjtag, 0x03, 4); // DRSELECT -> IRSELECT -> IRCAPTURE
//     cjtag_TmsPattent(cjtag, 0x00, 63); // Shift in 64 bits to load bypass instruction
//     cjtag_TmsPattent(cjtag, 0x03, 3); // IREXIT2 -> IRUPDATE -> IDLE

     // Set ICEPick IDCODE
//      cjtag_TmsPattent(cjtag, 0x03, 4); // DRSELECT -> IRSELECT -> IRCAPTURE
//      cjtag_TmsPattent(cjtag, 0x00, 3); // Shift in 64 bits to load bypass instruction
//      cjtag_TmsPattent(cjtag, 0x03, 3); // IREXIT2 -> IRUPDATE -> IDLE

//    cjtag_writeOscan(cjtag, 0x3, 0, 3);
//    cjtag_writeOscan(cjtag, 0x0, 0, 8);
}

uint64_t cjtag_writeOscan(cjtag_t *cjtag, uint64_t tms, uint64_t tdi, uint8_t bits){
    uint64_t tdo;
    uint8_t i;

    if (cjtag == NULL)
        return 0;
    if (cjtag->pinFxn.tckSetIO == NULL || cjtag->pinFxn.tmsGet == NULL || cjtag->pinFxn.tmsSetDir == NULL || cjtag->pinFxn.tmsSetIO == NULL)
        return 0;

    tdo = 0;
    for (i=0 ; i<bits ; i++){
        __cjtag_write(cjtag, (tms>>i)&0x1, (tdi>>i)&0x1, NULL);
        tdo |= (cjtag->tdo_bit<<i);
    }

    return tdo;
}

uint64_t cjtag_ir_shift(cjtag_t *cjtag, uint64_t d, uint8_t l){
    uint8_t i;
    uint64_t ret;

    if (cjtag == NULL)
        return 0;
    if (cjtag->pinFxn.tckSetIO == NULL || cjtag->pinFxn.tmsGet == NULL || cjtag->pinFxn.tmsSetDir == NULL || cjtag->pinFxn.tmsSetIO == NULL)
        return 0;

    // enter on IR_SHIFT state
    switch (cjtag->fsmState){
    case CJTAG_RUN_TEST_IDLE:
        cjtag_writeOscan(cjtag, _ir_shift_enter_pat, _LOW, 4);
        break;
    case CJTAG_PAUSE_DR:
    case CJTAG_PAUSE_IR:
        cjtag_writeOscan(cjtag, 0x07, _LOW, 6);
        break;
    }

    ret = cjtag->tdo_bit;
    for (i=0 ; i<l ; i++){
        if (i<(l-1))
            ret |= cjtag_writeOscan(cjtag, _LOW, (d>>i)&0x1, 1)<<(i+1);
        else
            cjtag_writeOscan(cjtag, _HIGH, (d>>i)&0x1, 1);
    }

    if (1){
        // update IR and go to IDLE state
        cjtag_writeOscan(cjtag, 0x1, _LOW, 2);
    }
    else{
        // Go to Pause IR
//        cjtag_writeOscan(cjtag, 0x17, _LOW, 6);
        cjtag_writeOscan(cjtag, 0x1, _LOW, 2);
    }

    return ret;
}

uint64_t cjtag_dr_shift(cjtag_t *cjtag, uint64_t d, uint8_t l, uint8_t endInPause){
    uint8_t i;
    uint64_t ret;

    if (cjtag == NULL)
        return 0;
    if (cjtag->pinFxn.tckSetIO == NULL || cjtag->pinFxn.tmsGet == NULL || cjtag->pinFxn.tmsSetDir == NULL || cjtag->pinFxn.tmsSetIO == NULL)
        return 0;

    // enter on DR_SHIFT state
    switch (cjtag->fsmState){
    case CJTAG_RUN_TEST_IDLE:
        cjtag_writeOscan(cjtag, _dr_shift_enter_pat, _LOW, 3);
        break;
    case CJTAG_PAUSE_DR:
    case CJTAG_PAUSE_IR:
        cjtag_writeOscan(cjtag, 0x01, _LOW, 2);
        break;
    }

    ret = cjtag->tdo_bit;
    for (i=0 ; i<l ; i++){
        if (i<(l-1))
            ret |= cjtag_writeOscan(cjtag, _LOW, (d>>i)&0x1, 1)<<(i+1);
        else{
            // last bit doens't have a valid TDO
            cjtag_writeOscan(cjtag, _HIGH, (d>>i)&0x1, 1);
        }
    }

    if (1){
        // update DR and go to IDLE state
        cjtag_writeOscan(cjtag, 0x1, _LOW, 2);
    }
    else{
        // Go to Pause DR
//        cjtag_writeOscan(cjtag, 0x0B, _LOW, 5);
        cjtag_writeOscan(cjtag, 0x0, _LOW, 2);
    }

    return ret;
}

void cjtag_reset(cjtag_t *cjtag){
    if (cjtag == NULL || cjtag->pinFxn.resetSetIO == NULL)
        return;

    cjtag->pinFxn.resetSetIO(_LOW);
}

void cjtag_release(cjtag_t *cjtag){
    if (cjtag == NULL || cjtag->pinFxn.resetSetIO == NULL)
        return;

    cjtag->pinFxn.resetSetIO(_HIGH);
}

void cjtag_open_cmd_window(cjtag_t *cjtag){

}


