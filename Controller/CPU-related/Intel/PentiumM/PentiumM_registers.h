/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * PentiumM_registers.h
 *
 *  Created on: Jun 5, 2010
 *      Author: Stefan
 */

#ifndef PENTIUMM_REGISTERS_H_
#define PENTIUMM_REGISTERS_H_

//#define MIN_AND_MAX_FID 0x19D
  //for Pentium Ms the perf counter bit width seems to be 40:
  //                                    <8 bit > <8 bit > <8 bit > <8 bit > <8 bit >
  //previous value: 5497519814387   100 11111111 11111101 10110111 00110110 11110011
  //current value:  4398069455221   100 00000000 00000001 01011110 00011001 01110101
  //#infdef _MSC_VER
  //suffix "ULL" is not needed by MS compiler, but by g++
  //10xF = 10x 4 bit =40 bit
  #define MAXIMUM_PERFORMANCE_COUNTER_VALUE 0xFFFFFFFFFFULL

#endif /* PENTIUMM_REGISTERS_H_ */
