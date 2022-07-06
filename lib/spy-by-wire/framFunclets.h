/*
 * framFunclets.h
 *
 *  Created on: 9 de mai de 2022
 *      Author: pablo.jean
 */

#ifndef LIB_SPY_BY_WIRE_FRAMFUNCLETS_H_
#define LIB_SPY_BY_WIRE_FRAMFUNCLETS_H_


#include <stdint.h>

// ----- fram erase funclets externs
extern unsigned short FramErase_o[];
extern unsigned long FramErase_o_termination;
extern unsigned long FramErase_o_start;
extern unsigned long FramErase_o_finish;
extern unsigned long FramErase_o_length;

// ----- fram write funclets externs
extern unsigned short FramWrite_o[];
extern unsigned long FramWrite_o_termination;
extern unsigned long FramWrite_o_start;
extern unsigned long FramWrite_o_finish;
extern unsigned long FramWrite_o_length;


#endif /* LIB_SPY_BY_WIRE_FRAMFUNCLETS_H_ */
