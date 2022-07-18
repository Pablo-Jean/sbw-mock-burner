/*
 * data_link.c
 *
 *  Created on: 25 de abr de 2022
 *      Author: pablo.jean
 */

#include "data_link.h"

/*
 * Macros
 */

#define US_REG_DELAY    4

/**
 * MACROS
 */

#define   l1NOPS  { _NOP(); _NOP(); }
#define   l2NOPS   { l1NOPS; }
#define   l3NOPS   { l1NOPS; }

//#define   SBWDATO   link->fxn.sbwtdioR()
#define   SBWDATO   0x1
#define   SBWDATI   link->fxn.sbwtdioR()

//! \brief SBW macro: set TMS signal
#define   TMSH    {link->fxn.sbwtdioW(HIGH); l3NOPS link->fxn.sbwtckW(LOW); l3NOPS link->fxn.sbwtckW(HIGH); l2NOPS}

//! \brief SBW macro: clear TMS signal
#define   TMSL    {link->fxn.sbwtdioW(LOW); l3NOPS link->fxn.sbwtckW(LOW); l3NOPS link->fxn.sbwtckW(HIGH); l2NOPS}

//! \brief SBW macro: clear TMS signal and immediately set it high again in
//! the SBWTCK low phase to enter the TDI slot with a high signal
//! \details Only used to clock TCLK (=TDI for SBW) in Run-Test/IDLE mode of
//! the JTAG FSM
#define   TMSLDH  {link->fxn.sbwtdioW(LOW); l3NOPS link->fxn.sbwtckW(LOW); l3NOPS link->fxn.sbwtdioW(HIGH); link->fxn.sbwtckW(HIGH); l2NOPS}

//! \brief SBW macro: Set TDI = 1
#define   TDIH    {link->fxn.sbwtdioW(HIGH); l3NOPS link->fxn.sbwtckW(LOW); l3NOPS link->fxn.sbwtckW(HIGH); l2NOPS}

//! \brief SBW macro: clear TDI signal
#define   TDIL    {link->fxn.sbwtdioW(LOW); l3NOPS link->fxn.sbwtckW(LOW); l3NOPS link->fxn.sbwtckW(HIGH); l2NOPS}

//! \brief SBW macro: TDO cycle without reading TDO
#define   TDOsbw  {link->fxn.sbwtdioSet(INPUT); l3NOPS link->fxn.sbwtckW(LOW); l2NOPS link->fxn.sbwtckW(HIGH); link->fxn.sbwtdioSet(OUTPUT); l2NOPS}

//! \brief SBW macro: TDO cycle with TDO read
#define   TDO_RD  {link->fxn.sbwtdioSet(INPUT); link->fxn.sbwtckW(LOW); l3NOPS tdo_bit = link->fxn.sbwtdioR(); link->fxn.sbwtckW(HIGH); link->fxn.sbwtdioSet(OUTPUT); l2NOPS}

//! \brief SBW macro: set TCK signal
#define   SetSBWTCK(link)     {link->fxn.sbwtckW(HIGH); }
//! \brief SBW macro: clear TCK signal
#define   ClrSBWTCK(link)     {link->fxn.sbwtckW(LOW); }
//! \brief SBW macro: set TDIO signal
#define   SetSBWTDIO(link)    {link->fxn.sbwtdioW(HIGH); }
//! \brief SBW macro: clear TDIO signal
#define   ClrSBWTDIO(link)    {link->fxn.sbwtdioW(LOW); }




/**
 * Privates
 */

/****************************************************************************/
/* GLOBAL VARIABLES                                                         */
/****************************************************************************/

//! \brief Holds the value of TDO-bit
uint8_t tdo_bit;
//! \brief Holds the last value of TCLK before entering a JTAG sequence
uint8_t TCLK_saved = 1;


/* ============== DELAY AND WAIT  ================== */
void __us_delay(sbw_data_link_t *link, uint32_t us){
    link->fxn.delayUs(us);
}
void __ms_delay(sbw_data_link_t *link, uint32_t ms){
    link->fxn.delayMs(ms);
}

//  combinations of sbw-cycles (TMS, TDI, TDO)
//---------------------------------
void TMSL_TDIL(sbw_data_link_t* link)
{
    TMSL  TDIL  TDOsbw
}
//---------------------------------
void TMSH_TDIL(sbw_data_link_t* link)
{
    TMSH  TDIL  TDOsbw
}
//------------------------------------
void TMSL_TDIH(sbw_data_link_t* link)
{
    TMSL  TDIH  TDOsbw
}
//-------------------------------------
void TMSH_TDIH(sbw_data_link_t* link)
{
    TMSH  TDIH  TDOsbw
}
//------------------------------------
void TMSL_TDIH_TDOrd(sbw_data_link_t* link)
{
    TMSL  TDIH  TDO_RD
}
//------------------------------------
void TMSL_TDIL_TDOrd(sbw_data_link_t* link)
{
    TMSL  TDIL  TDO_RD
}
//------------------------------------
void TMSH_TDIH_TDOrd(sbw_data_link_t* link)
{
    TMSH  TDIH  TDO_RD
}
//------------------------------------
void TMSH_TDIL_TDOrd(sbw_data_link_t* link)
{
    TMSH  TDIL  TDO_RD
}

//----------------------------------------------------------------------------
//! \brief Clear TCLK in Spy-Bi-Wire mode
//! \details enters with TCLK_saved and exits with TCLK = 0
void ClrTCLK(sbw_data_link_t* link)
{
    if (TCLK_saved & SBWDATO)
    {
        TMSLDH
    }
    else
    {
        TMSL
    }

    ClrSBWTDIO(link);

    TDIL TDOsbw    //ExitTCLK
    TCLK_saved = (~SBWDATO);
}

//----------------------------------------------------------------------------
//! \brief Set TCLK in Spy-Bi-Wire mode
//! \details enters with TCLK_saved and exits with TCLK = 1
void SetTCLK(sbw_data_link_t* link)
{
   if (TCLK_saved & SBWDATO)
   {
        TMSLDH
   }
   else
   {
        TMSL
   }

   SetSBWTDIO(link);

   TDIH TDOsbw    //ExitTCLK
   TCLK_saved = SBWDATO;
}

//----------------------------------------------------------------------------
//! \brief This function generates Amount strobes with the Flash Timing Generator
//! \details Frequency fFTG = 257..476kHz (t = 3.9..2.1us).
//! Used freq. in procedure - 400 kHz\n
//! User knows target frequency, instruction cycles, C implementation.\n
//! No. of MCKL cycles - 18MHz/400 kHz = 45 or 12MHz/400 kHz = 30
//! \param Amount (number of strobes to be generated)
void TCLKstrobes(sbw_data_link_t* link, word Amount)
{
    volatile word i;

    // This implementation has 45 (MCLK=18MHz)
    // or 30 (MCLK 12MHz) body cycles! -> 400kHz
    // DO NOT MODIFY IT !

    for (i = Amount; i > 0; i--)
    {
        SetSBWTCK(link);
        l3NOPS;
        l3NOPS;
        l3NOPS;
        l3NOPS;
        ClrSBWTCK(link);
        l3NOPS;
    }
}


//----------------------------------------------------------------------------
//! \brief Shift a value into TDI (MSB first) and simultaneously shift out a
//! value from TDO (MSB first).
//! \param Format (number of bits shifted, 8 (F_BYTE), 16 (F_WORD),
//! 20 (F_ADDR) or 32 (F_LONG))
//! \param Data (data to be shifted into TDI)
//! \return unsigned long (scanned TDO value)
unsigned long AllShifts(sbw_data_link_t* link, word Format, uint32_t Data)
{
   uint32_t TDOword = 0x00000000;
   uint32_t MSB = 0x00000000;
   word i;

   switch(Format)
   {
   case F_BYTE: MSB = 0x00000080;
     break;
   case F_WORD: MSB = 0x00008000;
     break;
   case F_ADDR: MSB = 0x00080000;
     break;
   case F_LONG: MSB = 0x80000000;
     break;
   default: // this is an unsupported format, function will just return 0
     return TDOword;
   }
   // shift in bits
   for (i = Format; i > 0; i--)
   {
        if (i == 1)                     // last bit requires TMS=1; TDO one bit before TDI
        {
          ((Data & MSB) == 0) ? TMSH_TDIL_TDOrd(link) : TMSH_TDIH_TDOrd(link);
        }
        else
        {
          ((Data & MSB) == 0) ? TMSL_TDIL_TDOrd(link) : TMSL_TDIH_TDOrd(link);
        }
        Data <<= 1;
        if (tdo_bit)
            TDOword++;
        if (i > 1)
            TDOword <<= 1;               // TDO could be any port pin
   }
   TMSH_TDIH(link);                         // update IR
   if (TCLK_saved & SBWDATO)
   {
        TMSL_TDIH(link);
   }
   else
   {
        TMSL_TDIL(link);
   }

   // de-scramble bits on a 20bit shift
   if(Format == F_ADDR)
   {
     TDOword = ((TDOword << 16) + (TDOword >> 4)) & 0x000FFFFF;
   }
   return(TDOword);
}

uint8_t GetTDO(sbw_data_link_t* link){
    if (link != NULL){
        return tdo_bit;
    }
    return 0;
}

uint8_t GetTCLK(sbw_data_link_t* link){
    if (link != NULL){
        return TCLK_saved;
    }
    return 0;
}

uint8_t GetSBWDATO(sbw_data_link_t* link){
    if (link != NULL){
        return SBWDATO;
    }
    return 0;
}

/**
 * Publics
 */

void data_link_start(sbw_data_link_t* link){
    if (link != NULL){
//        __tck_set(link, HIGH);
//        __tdo(link);
    }
}


//uint8_t data_link_write(sbw_data_link_t* link, tms_e TMS, tdi_e TDI, tdo_e *TDO){
//    tdo_e TDOhold;
//
//    if (link != NULL){
//        TDOhold = __SLOT(link, TMS, TDI);
//        if (TDO != NULL){
//            *TDO = TDOhold;
//        }
//        return 1;
//    }
//    return 0;
//}
//
//void data_link_tclk_clr(sbw_data_link_t* link, latch_st_e latch){
//    if (link != NULL){
//        __ClrTCLK(link, latch);
//    }
//}
//
//void data_link_tclk_set(sbw_data_link_t* link, latch_st_e latch){
//    if (link != NULL){
//        __SetTCLK(link, latch);
//    }
//}
//
//void data_link_strobes(sbw_data_link_t* link, latch_st_e latch, uint8_t n){
//    if (link != NULL){
//        __TCLKstrobes(link, latch, n);
//    }
//}
//
//void data_link_enter_jtag(sbw_data_link_t* link){
//    if (link != NULL){
//        __link_enter_jtag(link);
//    }
//}

void data_link_delay_us(sbw_data_link_t* link, uint32_t us){
    if (link != NULL){
        __us_delay(link, us);
    }
}

void data_link_delay_ms(sbw_data_link_t* link, uint32_t ms){
    if (link != NULL){
        __ms_delay(link, ms);
    }
}

//----------------------------------------------------------------------------
//! \brief Function to start the JTAG communication - RST line high - device starts
//! code execution
void EntrySequences_RstHigh_SBW(sbw_data_link_t* link)
{
    ClrTST(link);    //1
    data_link_delay_ms(link, 4); // reset TEST logic

    SetRST(link);    //2

    SetTST(link);    //3
    data_link_delay_ms(link, 20); // activate TEST logic

    // phase 1
    SetRST(link);    //4
    data_link_delay_us(link, 60);

    // phase 2 -> TEST pin to 0, no change on RST pin
    // for Spy-Bi-Wire
    ClrTST(link);

    // phase 3
    data_link_delay_us(link, 1);
    // phase 4 -> TEST pin to 1, no change on RST pin
    // for Spy-Bi-Wire
    SetTST(link);

    //_EINT_FET();
    data_link_delay_us(link, 60);

    // phase 5
    data_link_delay_ms(link, 5);
}

//! \brief Function to start the SBW communication - RST line low - device do not
//! start code execution
void EntrySequences_RstLow_SBW(sbw_data_link_t* link)
{
    ClrTST(link);                //1
    data_link_delay_ms(link, 1);       // reset TEST logic

    ClrRST(link);                //2
    data_link_delay_ms(link, 50);

    SetTST(link);                //3
    data_link_delay_ms(link, 100);    // activate TEST logic

    // phase 1
    SetRST(link);                //4
    data_link_delay_us(link, 40);

    // phase 2 -> TEST pin to 0, no change on RST pin
    // for Spy-Bi-Wire
    ClrTST(link);                  //5

    data_link_delay_us(link, 1);

    // phase 4 -> TEST pin to 1, no change on RST pin
    // for Spy-Bi-Wire
    SetTST(link);      //7
    data_link_delay_us(link, 40);
    data_link_delay_ms(link, 5);
}
