/*
 * sbw.c
 *
 *  Created on: 26 de abr de 2022
 *      Author: pablo.jean
 */

#include "sbw.h"


/**
 * Privates
 */

//! \brief Array to store data for a memory write
word WriteData[50];
//! \brief Array to store data for a memory read
word ReadData[50];

//----------------------------------------------------------------------------
//! \brief Function for shifting a given 16-bit word into the JTAG data
//! register through TDI.
//! \param[in] word data (16-bit data, MSB first)
//! \return word (value is shifted out via TDO simultaneously)
static word DR_Shift16(sbw_data_link_t* sbw, word data)
{
    //TMSL_TDIL(sbw);
    // JTAG FSM state = Run-Test/Idle
    if (GetTCLK(sbw) & GetSBWDATO(sbw))
    {
        TMSH_TDIH(sbw);
    }
    else
    {
        TMSH_TDIL(sbw);
    }
    // JTAG FSM state = Select DR-Scan
    TMSL_TDIH(sbw);
    // JTAG FSM state = Capture-DR
    TMSL_TDIH(sbw);
    // JTAG FSM state = Shift-DR, Shift in TDI (16-bit)
    //TMSL_TDIH(sbw);

    return(AllShifts(sbw,  F_WORD, data));
    // JTAG FSM state = Run-Test/Idle
}

//----------------------------------------------------------------------------
//! \brief Function for shifting a given 20-bit address word into the
//! JTAG address register through TDI.
//! \param[in] unsigned long address (20-bit address word, MSB first)
//! \return unsigned long TDOvalue (is shifted out via TDO simultaneously)
static unsigned long DR_Shift20(sbw_data_link_t* sbw, unsigned long address)
{
    //TMSL_TDIL(sbw);
    // JTAG FSM state = Run-Test/Idle
    if (GetTCLK(sbw) & GetSBWDATO(sbw))
    {
        TMSH_TDIH(sbw);
    }
    else
    {
        TMSH_TDIL(sbw);
    }
    // JTAG FSM state = Select DR-Scan
    TMSL_TDIH(sbw);
    // JTAG FSM state = Capture-DR
    TMSL_TDIH(sbw);
    // JTAG FSM state = Shift-DR, Shift in TDI (16-bit)
    //TMSL_TDIH(sbw);

    return(AllShifts(sbw,  F_ADDR, address));
    // JTAG FSM state = Run-Test/Idle
}

//----------------------------------------------------------------------------
//! \brief Function for shifting a new instruction into the JTAG instruction
//! register through TDI (MSB first, but with interchanged MSB - LSB, to
//! simply use the same shifting function, Shift(), as used in DR_Shift16).
//! \param[in] byte Instruction (8bit JTAG instruction, MSB first)
//! \return word TDOword (value shifted out from TDO = JTAG ID)
static word IR_Shift(sbw_data_link_t* sbw, uint8_t instruction)
{
    //TMSL_TDIL(sbw);
    // JTAG FSM state = Run-Test/Idle
    if (GetTCLK(sbw) & GetSBWDATO(sbw))
    {
        TMSH_TDIH(sbw);
    }
    else
    {
        TMSH_TDIL(sbw);
    }
    // JTAG FSM state = Select DR-Scan
    TMSH_TDIH(sbw);

    // JTAG FSM state = Select IR-Scan
    TMSL_TDIH(sbw);
    // JTAG FSM state = Capture-IR
    TMSL_TDIH(sbw);

    // JTAG FSM state = Shift-IR, Shift in TDI (8-bit)
    return(AllShifts(sbw,  F_BYTE, instruction));
    // JTAG FSM state = Run-Test/Idle
}



//----------------------------------------------------------------------------
//! \brief Load a given address into the target CPU's program counter (PC).
//! \param[in] unsigned long Addr (destination address)
static void SetPC_430Xv2(sbw_data_link_t* sbw, unsigned long Addr)
{
    unsigned short Mova;
    unsigned long Pc_l;

    // to fix a bug to read the TLV, the address will shift in two
    // so, just return the desired address in two

    Mova  = 0x0080;
    Mova += (unsigned short)((Addr>>8) & 0x00000F00);
    Pc_l  = (unsigned short)((Addr & 0xFFFF));

    // Check Full-Emulation-State at the beginning
    IR_Shift(sbw, IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(sbw, 0) & 0x0301)
    {
        // MOVA #imm20, PC
        ClrTCLK(sbw);
        // take over bus control during clock LOW phase
        IR_Shift(sbw, IR_DATA_16BIT);
        SetTCLK(sbw);
        DR_Shift16(sbw, Mova);
        // insert on 24.03.2010 Florian
        ClrTCLK(sbw);
        // above is just for delay
        IR_Shift(sbw, IR_CNTRL_SIG_16BIT);
        DR_Shift16(sbw, 0x1400);
        IR_Shift(sbw, IR_DATA_16BIT);
        ClrTCLK(sbw);
        SetTCLK(sbw);
        DR_Shift16(sbw, Pc_l);
        ClrTCLK(sbw);
        SetTCLK(sbw);
        DR_Shift16(sbw, 0x4303);
        // fix a bug when try to read TLV
        // but, this will shift the PC in two
        ClrTCLK(sbw);
        ClrTCLK(sbw);
        ClrTCLK(sbw);
        IR_Shift(sbw, IR_ADDR_CAPTURE);
        DR_Shift20(sbw, 0x00000);
    }
}


//----------------------------------------------------------------------------
//! \brief This function reads one byte/word from a given address in memory
//! \param[in] word Format (F_BYTE or F_WORD)
//! \param[in] word Addr (address of memory)
//! \return word (content of the addressed memory location)
word ReadMem_430Xv2(sbw_t* sbw, word Format, unsigned long Addr)
{
    word TDOword = 0;
    // Check Init State at the beginning
    data_link_delay_ms(sbw->link, 1);
    IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(sbw->link, 0) & 0x0301)
    {
        // Read Memory
        ClrTCLK(sbw->link);
        IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
        if  (Format == F_WORD)
        {
            DR_Shift16(sbw->link, 0x0501);             // Set word read
        }
        else
        {
            DR_Shift16(sbw->link, 0x0511);             // Set byte read
        }
        IR_Shift(sbw->link, IR_ADDR_16BIT);
        DR_Shift20(sbw->link, Addr);                   // Set address

        IR_Shift(sbw->link, IR_DATA_TO_ADDR);
        SetTCLK(sbw->link);
        ClrTCLK(sbw->link);
        TDOword = DR_Shift16(sbw->link, 0x0000);       // Shift out 16 bits

        SetTCLK(sbw->link);
        // one or more cycle, so CPU is driving correct MAB
        ClrTCLK(sbw->link);
        SetTCLK(sbw->link);
        // Processor is now again in Init State
    }
    return TDOword;
}

//----------------------------------------------------------------------------
//! \brief This function reads an array of words from the memory.
//! \param[in] word StartAddr (Start address of memory to be read)
//! \param[in] word Length (Number of words to be read)
//! \param[out] word *DataArray (Pointer to array for the data)
word ReadMemQuick_430Xv2(sbw_t* sbw, unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    unsigned long i, lPc = 0;

    // Set PC to 'safe' address
    if((IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE) == JTAG_ID99) || (IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE) == JTAG_ID98))
    {
        lPc = 0x00000004;
    }

    SetPC_430Xv2(sbw->link, StartAddr);
    SetTCLK(sbw->link);
    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x0501);
    IR_Shift(sbw->link, IR_ADDR_CAPTURE);

    IR_Shift(sbw->link, IR_DATA_QUICK);
    for (i = 0; i < Length; i++)
    {
        SetTCLK(sbw->link);
        ClrTCLK(sbw->link);
        *DataArray++   = (word)DR_Shift16(sbw->link, 0x0);  // Read data from memory.
    }

    if(lPc)
    {
        SetPC_430Xv2(sbw->link, lPc);
    }
    SetTCLK(sbw->link);

    return STATUS_OK;
}

//----------------------------------------------------------------------------
//! \brief Reset target JTAG interface and perform fuse-HW check.
static void ResetTAP(sbw_data_link_t* sbw)
{
    word i;
    // Reset JTAG FSM
    for (i = 6; i > 0; i--)
    {
        TMSH_TDIH(sbw);
    }
    // JTAG FSM is now in Test-Logic-Reset
    TMSL_TDIH(sbw);                 // now in Run/Test Idle
}


//----------------------------------------------------------------------------
//! \brief This function writes one byte/word at a given address ( <0xA00)
//! \param[in] word Format (F_BYTE or F_WORD)
//! \param[in] word Addr (Address of data to be written)
//! \param[in] word Data (shifted data)
void WriteMem_430Xv2(sbw_t* sbw, word Format, unsigned long Addr, word Data)
{
    word state;
    // Check Init State at the beginning
    IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE);
    state = DR_Shift16(sbw->link, 0);
    if(state & 0x0301)
    {
        ClrTCLK(sbw->link);
        IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
        if  (Format == F_WORD)
        {
            DR_Shift16(sbw->link, 0x0500);
        }
        else
        {
            DR_Shift16(sbw->link, 0x0510);
        }

        IR_Shift(sbw->link, IR_ADDR_16BIT);
        DR_Shift20(sbw->link, Addr);

        SetTCLK(sbw->link);
        // New style: Only apply data during clock high phase
        IR_Shift(sbw->link, IR_DATA_TO_ADDR);
        DR_Shift16(sbw->link, Data);           // Shift in 16 bits

        ClrTCLK(sbw->link);

        IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
        DR_Shift16(sbw->link, 0x0501);
        SetTCLK(sbw->link);

        // one or more cycle, so CPU is driving correct MAB
        ClrTCLK(sbw->link);
        SetTCLK(sbw->link);
        // Processor is now again in Init State

    }
}

//----------------------------------------------------------------------------
//! \brief This function writes an array of words into the target memory.
//! \param[in] word StartAddr (Start address of target memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteMemQuick_430Xv2(sbw_t* sbw, unsigned long StartAddr, unsigned long Length, unsigned short const *DataArray)
{
    unsigned long i;

    for (i = 0; i < Length; i++)
    {
        WriteMem_430Xv2(sbw, F_WORD, StartAddr, DataArray[i]);
        StartAddr += 2;
    }
}


void ExecuteBOR(sbw_t *sbw){
    SetTST(sbw->link);
    data_link_delay_ms(sbw->link, 20);
    ClrTST(sbw->link);

    SetRST(sbw->link);
    data_link_delay_ms(sbw->link, 20);
    ClrRST(sbw->link);
    data_link_delay_ms(sbw->link, 20);
}

//----------------------------------------------------------------------------
//! \brief Function to execute a Power-On Reset (POR) using JTAG CNTRL SIG
//! register
//! \return word (STATUS_OK if target is in Full-Emulation-State afterwards,
//! STATUS_ERROR otherwise)
static word ExecutePOR_430Xv2(sbw_t* sbw)
{
    // provide one clock cycle to empty the pipe
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);

    // prepare access to the JTAG CNTRL SIG register
    IR_Shift(sbw->link,  IR_CNTRL_SIG_16BIT);
    // release CPUSUSP signal and apply POR signal
    DR_Shift16(sbw->link,  0x0C01);
    // release POR signal again
    DR_Shift16(sbw->link,  0x0401);
    // set PC to safe memory location
    IR_Shift(sbw->link, IR_DATA_16BIT);
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);
    DR_Shift16(sbw->link, 0x4);

    //Drive safe address into PC
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);
    IR_Shift(sbw->link, IR_DATA_CAPTURE);

    // two more to release CPU internal POR delay signals
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);

    // now set CPUSUSP signal again
    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x0501);
    // and provide one more clock
    ClrTCLK(sbw->link);
    SetTCLK(sbw->link);
    // the CPU is now in 'Full-Emulation-State'

    // disable Watchdog Timer on target device now by setting the HOLD signal
    // in the WDT_CNTRL register
    WriteMem_430Xv2(sbw, F_WORD, 0x01CC, 0x5A80);

    WriteMem_430Xv2(sbw, F_WORD, 0x06, 0x3FFF);
    WriteMem_430Xv2(sbw, F_WORD, 0x08, 0x3FFF);

    // Check if device is in Full-Emulation-State again and return status
    IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(sbw->link, 0) & 0x0301)
    {
      return(STATUS_OK);
    }

    return(STATUS_ERROR);
}


//----------------------------------------------------------------------------
//! \brief Read a 32bit value from the JTAG mailbox.
//! \return unsigned long (32bit value from JTAG mailbox)
unsigned long i_ReadJmbOut(sbw_t* sbw)
{
    unsigned short sJMBINCTL;
    unsigned long  lJMBOUT = 0;
    unsigned short sJMBOUT0, sJMBOUT1;

    sJMBINCTL = 0;

    IR_Shift(sbw->link, IR_JMB_EXCHANGE);// start exchange
    lJMBOUT = DR_Shift16(sbw->link, sJMBINCTL);

    if(lJMBOUT & OUT1RDY)// check if new data available
    {
        sJMBINCTL |= JMB32B + OUTREQ;
        lJMBOUT  = DR_Shift16(sbw->link, sJMBINCTL);
        sJMBOUT0 = (unsigned short)DR_Shift16(sbw->link, 0);
        sJMBOUT1 = (unsigned short)DR_Shift16(sbw->link, 0);
        lJMBOUT = ((unsigned long)sJMBOUT1<<16) + sJMBOUT0;
    }
    return lJMBOUT;
}

//----------------------------------------------------------------------------
//! \brief Write a 16bit value into the JTAG mailbox system.
//! The function timeouts if the mailbox is not empty after a certain number
//! of retries.
//! \param[in] word dataX (data to be shifted into mailbox)
short i_WriteJmbIn16(sbw_t* sbw, word dataX)
{
    unsigned short sJMBINCTL;
    unsigned short sJMBIN0;
    unsigned long Timeout = 0;
    sJMBIN0 = (unsigned short)(dataX & 0x0000FFFF);
    sJMBINCTL = INREQ;

    IR_Shift(sbw->link, IR_JMB_EXCHANGE);
    do
    {
        Timeout++;
        if(Timeout >= 3000)
        {
            return STATUS_ERROR;
        }
    }
    while(!(DR_Shift16(sbw->link, 0x0000) & IN0RDY) && Timeout < 3000);
    if(Timeout < 3000)
    {
        DR_Shift16(sbw->link, sJMBINCTL);
        DR_Shift16(sbw->link, sJMBIN0);
    }
    return STATUS_OK;
}

//! \brief Write a 32bit value into the JTAG mailbox system.
//! The function timeouts if the mailbox is not empty after a certain number
//! of retries.
//! \param[in] word dataX (data to be shifted into mailbox)
//! \param[in] word dataY (data to be shifted into mailbox)
short i_WriteJmbIn32(sbw_t* sbw, unsigned short dataX,unsigned short dataY)
{
    unsigned short sJMBINCTL;
    unsigned short sJMBIN0,sJMBIN1;
    unsigned long Timeout = 0;

    sJMBIN0 = (unsigned short)(dataX & 0x0000FFFF);
    sJMBIN1 = (unsigned short)(dataY & 0x0000FFFF);
    sJMBINCTL =  JMB32B | INREQ;

    IR_Shift(sbw->link, IR_JMB_EXCHANGE);
    do
    {
        Timeout++;
        if(Timeout >= 3000)
        {
            return STATUS_ERROR;
        }
    }
    while(!(DR_Shift16(sbw->link, 0x0000) & IN0RDY) && Timeout < 3000);

    if(Timeout < 3000)
    {
        sJMBINCTL = 0x11;
        DR_Shift16(sbw->link, sJMBINCTL) ;
        DR_Shift16(sbw->link, sJMBIN0);
        DR_Shift16(sbw->link, sJMBIN1);
    }
    return STATUS_OK;
}

//------------------------------------------------------------------------
//! \brief This function disables the Memory Protection Unit (FRAM devices only)
//! \return word (STATUS_OK if MPU was disabled successfully, STATUS_ERROR
//! otherwise)
word DisableMpu_430Xv2(sbw_t* sbw)
{
    word val;
    word newRegisterVal;

    if(IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE) == JTAG_ID98)
    {
        newRegisterVal = ReadMem_430Xv2(sbw, F_WORD, MSP430_SYSCFG0);
        newRegisterVal &= ~0xFF03;
        newRegisterVal |= 0xA500;
        // unlock MPU for FR4xx/FR2xx
        WriteMem_430Xv2(sbw, F_WORD, MSP430_SYSCFG0, newRegisterVal);
        val = ReadMem_430Xv2(sbw, F_WORD, MSP430_SYSCFG0);
        if((val & 0x3) == 0x0)
        {
            return STATUS_OK;
        }
    }

    return STATUS_ERROR;
}

word DisableWDT_MSP430Xv2(sbw_t* sbw){
    word FR2433_WDTCTL = 0x01CC;
    uint8_t WDTHOLD = 0x80;
    uint16_t WDTPW = 0x5A00;
    word val;
    unsigned short newRegisterVal;

    if(IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE) == JTAG_ID98)
    {
        newRegisterVal = ReadMem_430Xv2(sbw, F_WORD, FR2433_WDTCTL);
        newRegisterVal = WDTPW; // write the password
        newRegisterVal |= (WDTHOLD); // disable WDT0 and value

        WriteMem_430Xv2(sbw, F_WORD, FR2433_WDTCTL, newRegisterVal);
        val = ReadMem_430Xv2(sbw, F_WORD, FR2433_WDTCTL);
        if((val&0xFF) == (newRegisterVal&0xFF))
        {
            return STATUS_OK;
        }
    }
    return STATUS_ERROR;
}

word UnlockBSL_MSP430Xv2(sbw_t* sbw){
    word val;
    unsigned short newRegisterVal;

    if(IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE) == JTAG_ID98)
    {
        newRegisterVal = ReadMem_430Xv2(sbw, F_WORD, MSP430_SYSBSLC);
        newRegisterVal |= (0x8000); // disable WDT0
        // unlock MPU for FR4xx/FR2xx
        WriteMem_430Xv2(sbw, F_WORD, MSP430_SYSBSLC, newRegisterVal);
        val = ReadMem_430Xv2(sbw, F_WORD, MSP430_SYSBSLC);
        if((val&0x8000) == 0)
        {
            return STATUS_OK;
        }
    }
    return STATUS_ERROR;
}

word magicPattern(sbw_t *sbw){
    word deviceJtagID = 0;

    // entry low reset sequence
    EntrySequences_RstLow_SBW(sbw->link);
    // reset TAP state machine -> Run-Test/Idle
    ResetTAP(sbw->link);
    // feed JTAG mailbox with magic pattern
    if(i_WriteJmbIn16(sbw,  STOP_DEVICE) == STATUS_OK)
    {
        // Apply again 4wire/SBW entry Sequence.
        EntrySequences_RstHigh_SBW(sbw->link);

        ResetTAP(sbw->link);  // reset TAP state machine -> Run-Test/Idle

        deviceJtagID = (word)IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE);

        return deviceJtagID;
    }

    return 1;  // return 1 as an invalid JTAG ID
}


//----------------------------------------------------------------------------
//! \brief Function to determine & compare core identification info
//! \return word (STATUS_OK if correct JTAG ID was returned, STATUS_ERROR
//! otherwise)
static word GetCoreID (sbw_t* sbw)
{
    word i;
    word JtagId = 0;  //initialize JtagId with an invalid value

    for (i = 0; i < 5; i++)
    {
        EntrySequences_RstHigh_SBW(sbw->link);
        // reset TAP state machine -> Run-Test/Idle
        ResetTAP(sbw->link);
        // shift out JTAG ID
        JtagId = (word)IR_Shift(sbw->link, IR_CNTRL_SIG_CAPTURE);

        // break if a valid JTAG ID is being returned
        if((JtagId == JTAG_ID91) || (JtagId == JTAG_ID99))                     //****************************
        {
            sbw->info.Jtag = (msp430_jtag_id_e)JtagId;
            return(STATUS_OK);
        }
        else if (JtagId == JTAG_ID98){
            break;
        }
    }
    for (i=0 ; i<5 ; i++){
        JtagId = magicPattern(sbw);
        if((JtagId == JTAG_ID91) || (JtagId == JTAG_ID99) || (JtagId == JTAG_ID98))                     //****************************
        {
            sbw->info.Jtag = (msp430_jtag_id_e)JtagId;
            return(STATUS_OK);
        }
    }
    sbw->info.Jtag = JTAG_INVALID;

    return(STATUS_ERROR);
}

//----------------------------------------------------------------------------
//! \brief Function to determine & compare core identification info (Xv2)
//! \return word (STATUS_OK if correct JTAG ID was returned, STATUS_ERROR
//! otherwise)
uint32_t GetCoreipIdXv2(sbw_t* sbw)
{
    uint32_t pDeviceId;

    IR_Shift(sbw->link, IR_COREIP_ID);
    sbw->info.CoreID = DR_Shift16(sbw->link, 0);
    if(sbw->info.CoreID == 0)
    {
        return(STATUS_ERROR);
    }
    IR_Shift(sbw->link, IR_DEVICE_ID);
    pDeviceId = DR_Shift20(sbw->link, 0);

    // The ID pointer is an un-scrambled 20bit value
    // so we need to correct
    pDeviceId = ((pDeviceId & 0xFFFF) << 4) + (pDeviceId >> 16);

    return(pDeviceId);
}

//------------------------------------------------------------------------
//! \brief This function checks if the JTAG lock key is programmed.
//! \return word (STATUS_OK if fuse is blown, STATUS_ERROR otherwise)
word IsLockKeyProgrammed(sbw_data_link_t* sbw)
{
    word i;

    for (i = 3; i > 0; i--)     //  First trial could be wrong
    {
        IR_Shift(sbw, IR_CNTRL_SIG_CAPTURE);
        if (DR_Shift16(sbw, 0xAAAA) == 0x5555)
        {
            return(STATUS_OK);  // Fuse is blown
        }
    }
    return(STATUS_ERROR);       // Fuse is not blown
}

//----------------------------------------------------------------------------
//! \brief Function to resync the JTAG connection and execute a Power-On-Reset
//! \return word (STATUS_OK if operation was successful, STATUS_ERROR
//! otherwise)
static word SyncJtag_AssertPor (sbw_t* sbw)
{
    uint8_t i;

    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x1501);                  // Set device into JTAG mode + read

    // wait for sync
    i = 0;
    while(!(DR_Shift16(sbw->link, 0) & 0x0200) && i<50)
    {
        i++;
    }
    if (i >= 50){
        return(STATUS_ERROR);
    }

    // execute a Power-On-Reset
    if(ExecutePOR_430Xv2(sbw) != STATUS_OK)
    {
        return(STATUS_ERROR);
    }
    sbw->info.Cpu = CPU_HALT;

    return(STATUS_OK);
}

word HaltCPU_430Xv2(sbw_t* sbw){
    // "JMP $" instruction to keep CPU from changing the state
    IR_Shift(sbw->link, IR_DATA_16BIT);
    DR_Shift16(sbw->link, 0x3FFF);
    ClrTCLK(sbw->link);

    // HALT_JTAG bit
    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x2409);
    SetTCLK(sbw->link);
    sbw->info.Cpu = CPU_HALT;

    return STATUS_OK;
}

word ReleaseCPU_430Xv2(sbw_t* sbw){
    ClrTCLK(sbw->link);
    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x2401);
    IR_Shift(sbw->link, IR_ADDR_CAPTURE);
    SetTCLK(sbw->link);

    return STATUS_OK;
}


//----------------------------------------------------------------------------
//! \brief This function compares the computed PSA (Pseudo Signature Analysis)
//! value to the PSA value shifted out from the target device.
//! It is used for very fast data block write or erasure verification.
//! \param[in] unsigned long StartAddr (Start address of data block to be checked)
//! \param[in] unsigned long Length (Number of words within data block)
//! \param[in] word *DataArray (Pointer to array with the data, 0 for Erase Check)
//! \return word (STATUS_OK if comparison was successful, STATUS_ERROR otherwise)
word VerifyPSA_430Xv2(sbw_t *sbw, unsigned long StartAddr, unsigned long Length, unsigned short const *DataArray)
{
    word TDOword;
    unsigned long i;
    const word POLY = 0x0805;             // Polynom value for PSA calculation
    word PSA_CRC = (word)(StartAddr-2);   // Start value for PSA calculation

    ExecutePOR_430Xv2(sbw);

    SetPC_430Xv2(sbw->link, StartAddr);

    SetTCLK(sbw->link);

    IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
    DR_Shift16(sbw->link, 0x0501);

    IR_Shift(sbw->link, IR_DATA_16BIT);
    DR_Shift16(sbw->link, PSA_CRC);

    IR_Shift(sbw->link, IR_DATA_PSA);

    for (i = 0; i < Length; i++)
    {
        // Calculate the PSA (Pseudo Signature Analysis) value
        if ((PSA_CRC & 0x8000) == 0x8000)
        {
            PSA_CRC ^= POLY;
            PSA_CRC <<= 1;
            PSA_CRC |= 0x0001;
        }
        else
        {
            PSA_CRC <<= 1;
        }
        // if pointer is 0 then use erase check mask, otherwise data
        &DataArray[0] == 0 ? (PSA_CRC ^= 0xFFFF) : (PSA_CRC ^= DataArray[i]);
        ClrTCLK(sbw->link);

        TMSH_TDIH(sbw->link);
        TMSL_TDIH(sbw->link);
        TMSL_TDIH(sbw->link);
        TMSH_TDIH(sbw->link);
        TMSH_TDIH(sbw->link);
        TMSL_TDIH(sbw->link);

        SetTCLK(sbw->link);
    }

    IR_Shift(sbw->link, IR_SHIFT_OUT_PSA);
    TDOword = DR_Shift16(sbw->link, 0x0000);     // Read out the PSA value

    ExecutePOR_430Xv2(sbw);

    return((TDOword == PSA_CRC) ? STATUS_OK : STATUS_ERROR);
}

//----------------------------------------------------------------------------
//! \brief Function to release the target device from JTAG control
//! \param[in] word Addr (0xFFFE: Perform Reset, means Load Reset Vector into
//! PC, otherwise: Load Addr into PC)
word ReleaseDevice_430Xv2(sbw_t* sbw, unsigned long Addr)
{
    switch(Addr)
    {
        case V_BOR:

        // perform a BOR via JTAG - we loose control of the device then...
        IR_Shift(sbw->link, IR_TEST_REG);
        DR_Shift16(sbw->link, 0x0200);
        data_link_delay_ms(sbw->link, 5);     // wait some time before doing any other action
        // JTAG control is lost now - GetDevice() needs to be called again to gain control.
        break;

        case V_RESET:

        IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
        DR_Shift16(sbw->link, 0x0C01);                 // Perform a reset
        DR_Shift16(sbw->link, 0x0401);
        IR_Shift(sbw->link, IR_CNTRL_SIG_RELEASE);
        break;

        default:

        SetPC_430Xv2(sbw->link, Addr);                 // Set target CPU's PC
        // prepare release & release
        SetTCLK(sbw->link);
        IR_Shift(sbw->link, IR_CNTRL_SIG_16BIT);
        DR_Shift16(sbw->link, 0x0401);
        IR_Shift(sbw->link, IR_ADDR_CAPTURE);
        IR_Shift(sbw->link, IR_CNTRL_SIG_RELEASE);

    }
    sbw->info.Cpu = CPU_RELEASED;

    return STATUS_OK;
}


//! \brief This function performs a Erase of FRxx devices using the JTAG mailbox
//! \param[in] word mailBoxMode 32Bit 16Bit mode
//! \param[in] word data1 mailbox data - first 16BIT
//! \param[in] word data2 mailbox data - second 16BIT
//! \return word (STATUS_OK if erase was successful, STATUS_ERROR
//! otherwise)
word EraseFRAMViaBootCode_430Xv2(sbw_t* sbw, word mailBoxMode, word data1, word data2)
{
    short mailBoxError =  0;
    // restart device
    ClrTST(sbw->link);
    ClrRST(sbw->link);
    data_link_delay_ms(sbw->link, 200);

    EntrySequences_RstLow_SBW(sbw->link);

    ResetTAP(sbw->link);

    if(mailBoxMode == MAIL_BOX_32BIT)// 32Bit Mode
    {
        mailBoxError = i_WriteJmbIn32(sbw, data1,data2);
    }
    else // 16 Bit Mode
    {
        mailBoxError = i_WriteJmbIn16(sbw, data1);
    }
    // restart device
    ClrTST(sbw->link);
    SetRST(sbw->link);
    data_link_delay_ms(sbw->link, 200);

    EntrySequences_RstHigh_SBW(sbw->link);

    ResetTAP(sbw->link);
    // wait until erase is done
    data_link_delay_ms(sbw->link, 60);

    if(SyncJtag_AssertPor(sbw) != STATUS_OK)
    {
        return(STATUS_ERROR);
    }

    // check if mailbox input was ok
    if(mailBoxError == STATUS_ERROR)
    {
        return(STATUS_ERROR);
    }
    return STATUS_OK;
}

//----------------------------------------------------------------------------
//! \brief This function performs an erase of a user defined FRAM memory section.
//! For FRAM devices the erase equals a write operation of 0xFFFF to the
//! respective memory section. (Could be extended with erase check via PSA)
//! This function should be used for "segment erases" only. For a "mass erase",
//! consider using EraseFRAMViaBootCode_430Xv2 instead.
//! \param[in] word StartAddr (start address for the erase)
//! \param[in] word Length (length of the memory section in WORDS)
word EraseFRAM_430Xv2(sbw_t *sbw, word EraseMode, unsigned long EraseAddr)
{
    EraseFRAMViaBootCode_430Xv2(sbw, MAIL_BOX_32BIT, STOP_DEVICE, USER_CODE_ERASE);

    return STATUS_OK;
}

word EraseCheck_430Xv2(sbw_t *sbw, uint32_t StartAddr, uint32_t Length){
    return VerifyPSA_430Xv2(sbw, StartAddr, Length, NULL);
}


/**
 * Abstration for Publics
 */

msp430_model_address_t *getDeviceMemOrganization(msp430_ids_e id){
    uint32_t qtd, i;

    qtd = sizeof(msp430_model_address)/sizeof(msp430_model_address_t);
    for (i=0 ; i<qtd ; i++){
        if (id == msp430_model_address[i].ID){
            return &msp430_model_address[i];
        }
    }

    return NULL;
}

word GetDevice_430Xv2(sbw_t *obj){
    uint32_t pDeviceID;

    obj->info.Cpu = CPU_UNKNOWN_STATE;
    if (GetCoreID(obj) != STATUS_OK){
        return STATUS_ERROR;
    }
    if (IsLockKeyProgrammed(obj->link)){
        // my fuse is blown :(
        obj->info.Efuse = MSP430_FUSE_BLOWN;
        return STATUS_FUSEBLOWN;
    }
    obj->info.Cpu = CPU_RELEASED;
    obj->info.Efuse = MSP430_FUSE_OK;
    pDeviceID = GetCoreipIdXv2(obj);
    if (pDeviceID == STATUS_ERROR){
        return STATUS_ERROR;
    }

    if (SyncJtag_AssertPor(obj) != STATUS_OK){
        return STATUS_ERROR;
    }

    ReadMemQuick_430Xv2(obj, pDeviceID, sizeof(tlv_t)/sizeof(uint16_t), obj->TLV._raw);
    obj->info.Device = (msp430_ids_e)obj->TLV.information.deviceID;
    obj->memoryMap = getDeviceMemOrganization(obj->info.Device);

    return STATUS_OK;
}


//----------------------------------------------------------------------------
//! \brief This function programs/verifies an array of words into the FLASH
//! memory by using the FLASH controller.
//! \param[in] word StartAddr (Start address of FLASH memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
word WriteFLASH_430Xv2(sbw_t *sbw, unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    DisableWDT_MSP430Xv2(sbw);
    DisableMpu_430Xv2(sbw);

    if (DataArray != NULL){
        WriteMemQuick_430Xv2(sbw, StartAddr, Length, DataArray);
    }


    return STATUS_OK;
}

word WriteFLASHwChk_430Xv2(sbw_t *sbw, unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word *chkData;
    word ret;

    ret = STATUS_ERROR;
    if (DataArray != NULL){
        WriteFLASH_430Xv2(sbw, StartAddr, Length, DataArray);
    }
    chkData = malloc(Length);
    ReadMemQuick_430Xv2(sbw, StartAddr, Length, chkData);
    if (memcmp(DataArray, chkData, Length) == 0){
        ret = STATUS_OK;
    }
    free(chkData);


    return ret;
}


/**
 * Publics
 */

int8_t sbw_init(sbw_t *obj, sbw_data_link_t *link){
    int8_t valid;

    valid = 0;
    if (obj != NULL && link != NULL){
        if (link->fxn.delayMs == NULL){
            valid = -1;
        }
        if (link->fxn.delayUs == NULL){
            valid = -1;
        }
        if (link->fxn.sbwtckW == NULL){
            valid = -1;
        }
        if (link->fxn.sbwtdioR == NULL){
            valid = -1;
        }
        if (link->fxn.sbwtdioSet == NULL){
            valid = -1;
        }
        if (link->fxn.sbwtdioW == NULL){
            valid = -1;
        }
        obj->link = link;
    }

    return valid;
}

word TestRAM_Write (sbw_t *sbw){
    uint8_t Data[] = "THIS IS SOME MESSAGE FOR A RAM TEST ON THE DEVICE@!!";
    uint8_t Check[64];

    WriteMemQuick_430Xv2(sbw, sbw->memoryMap->ram.start, sizeof(Data)/2, (word*)Data);
    ReadMemQuick_430Xv2(sbw, sbw->memoryMap->ram.start, sizeof(Data)/2, (word*)Check);
    if (memcmp(Check, Data, sizeof(Data)/2) == 0){
        return STATUS_OK;
    }

    return STATUS_ERROR;
}

word ProgramLockKey(sbw_t *sbw){
    word LockKey[2] = { 0x5555, 0x5555 };

    // write JTAG lock key
    WriteMemQuick_430Xv2(sbw, 0xff80, 2, LockKey);

    // now perform a BOR via JTAG - we loose control of the device then...
    IR_Shift(sbw->link, IR_TEST_REG);
    DR_Shift16(sbw->link, 0x0200);
    data_link_delay_ms(sbw->link, 5);     // wait some time until Bootcode is executed

    // -> get it under JTAG control again
    // and return result of "is fuse blown?"
    return(GetDevice_430Xv2(sbw) == STATUS_FUSEBLOWN);
}


uint32_t sbw_cmd(sbw_t *obj, sbw_cmd_e cmd, void *bVal){
    word ret;

    ret = STATUS_ERROR;
    if (obj != NULL){

        switch (cmd){
        case SBW_GET_DEVICE_ID:
            ret = (uint32_t)GetDevice_430Xv2(obj);
            break;

        case SBW_HALT_DEVICE:
            ret = HaltCPU_430Xv2(obj);
            break;

        case SBW_RELEASE_DEVICE:
            ret = ReleaseDevice_430Xv2(obj, V_RESET);
            break;

        case SBW_CHECK_ACCESS:

            break;

        case SBW_ERASE_MAIN:
            ret = EraseFRAM_430Xv2(obj, ERASE_MAIN, MAIN_START_ADDRESS);
            break;

        case SBW_WRITE_DATA:
            if (bVal != NULL){
                obj->buf.wr = bVal;
                ret = WriteFLASH_430Xv2(obj, obj->buf.wr->stAddr, obj->buf.wr->len, obj->buf.wr->data);
            }
            break;

        case SBW_READ_DATA:
            if (bVal != NULL){
                obj->buf.rd = bVal;
                ret = ReadMemQuick_430Xv2(obj, obj->buf.rd->stAddr, obj->buf.rd->len, obj->buf.rd->data);
            }
            break;

        case SBW_WRITE_WITH_CHECK:
            if (bVal != NULL){
                obj->buf.wr = bVal;
                ret = WriteFLASHwChk_430Xv2(obj, obj->buf.wr->stAddr, obj->buf.wr->len, obj->buf.wr->data);
            }

            break;

        case SBW_DISABLE_MPU:
            ret = DisableMpu_430Xv2(obj);
            break;

        case SBW_ERASE_CHECK:

            break;

        case SBW_TEST_RAM_WRITE:
            ret = TestRAM_Write(obj);
            break;

        case SBW_UNLOCK_BSL:
            ret = UnlockBSL_MSP430Xv2(obj);
            break;

        case SBW_DISABLE_WDT:
            ret = DisableWDT_MSP430Xv2(obj);
            break;

        case SBW_JTAG_PASSWORD_WRITE:
            ProgramLockKey(obj);
            break;

        case SBW_JTAG_PASSWORD_REMOVE:
            EraseFRAMViaBootCode_430Xv2(obj, MAIL_BOX_32BIT, STOP_DEVICE, USER_CODE_ERASE);
            break;

        default:

            break;
        }
    }

    return ret;
}

