/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard; http://en.wikipedia.org/wiki/Pragma_once

#ifndef AMD_FAMILY_17_H_ //see http://en.wikipedia.org/wiki/Include_guard
  #define AMD_FAMILY_17_H_

  //# of MSR addresses where different FID and VID combinations are stored.
  #define NUMBER_OF_PSTATES 8

  //AMD: "if MainPllOpFreqIdMax = 00h, then there is no frequency limit"
  #define NO_CPU_CORE_FREQUENCY_LIMIT 0x00

  #define BITMASK_FOR_HIGHMOST_32_BIT_FOR_MAX_VID 1016 //=1111111000bin
  #define BITMASK_FOR_HIGHMOST_32_BIT_FOR_MIN_VID 130048 //=11111110000000000bin
  #define BITMASK_FOR_STABILIZATION_TIME 7 //=111bin

  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MAIN_PLL_OP_FREQ_ID_MAX 49

  #define START_BIT_FOR_CURRENT_DIVISOR_ID 40

  //=1111111000000000bin <= VID value is from 10th to 16th(=7 bit)
  //(9:15 beginning from bit 0) bit
  #define BITMASK_FOR_CPU_CORE_VOLTAGE_ID 0xFE00
  #define BITMASK_FOR_CPU_FREQ_ID 63 //=111111bin <= FID value is from 1st to 6th (0:5 beginning from bit 0) bit
  #define BITMASK_FOR_CPU_DIV_ID 448 //=111000000bin <= DivID value is from 7st to 9th (6:8 beginning from bit 0) bit

  #define BITMASK_FOR_CURRENT_DIVISOR 768 //=1100000000bin <= value from 40:41 (beginning from bit 0) bit
  #define BITMASK_FOR_CURRENT_VALUE 255 //=11111111bin <= value from 32:39 (beginning from bit 0) bit
  #define BITMASK_FOR_PCI_CONFIG_ALTVID 127 //=1111111bin <= AMD: "6:0 AltVid: alternate VID."

  #define HARDWARE_CONFIGURATION_REGISTER 0xC001_0015

  #define MINIMUM_VID_FOR_ZM82 28
  #define MAXIMUM_VID_FOR_ZM82 64

  #define MSR_TIME_STAMP_COUNTER_REGISTER 0x10

  //for PCI configuration space:
  #define F3xA0_POWER_CONTROL_MISCELLANEOUS_REGISTER 0xA0
  //Device number 24, Function 3 is "CPU Miscellaneous Control"
  // ( 24 << 3 ) | 3 = 192 | 3 = 192 + 3 = 195
  #define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 195

  #define F3xDC_CLOCK_POWER_TIMING_CONTROL_2_REGISTER_ADDRESS 0xDC
  #define F3xD4_CLOCK_POWER_TIMING_CONTROL_0_REGISTER 0xD4

  #define FIRST_RESERVED_DIVISOR_ID 0x4
  //above 3 the core divisor IDs are reserved
  #define HIGHEST_EFFECTIVE_DIVISOR_ID 3

  //(see AMDs "BIOS and Kernel Dev Guide")
  #define P_STATE_CURRENT_LIMIT_REGISTER 0xC0010061 //"MSRC001_0061 P-state Current Limit Register"
  #define P_STATE_CONTROL_REGISTER 0xC0010062 //"MSRC001_0062 P-state Control Register"
  #define P_STATE_STATUS_REGISTER 0xC0010063 //"MSRC001_0063 P-state Status Register"

  //2147483648 dec= 10000000 00000000 00000000 00000000 bin
  //                --8 bit- --8 bit- --8 bit- --8 bit-
  #define SET_P_STATE_TO_VALID(highDWORD) ( highDWORD |= 2147483648UL )
  //#define SET_P_STATE_TO_VALID(dword) ( dword = ( 1 << 31) )
  #define MAX_VALUE_FOR_DID 7//=7 hex//=111 bin; has bits 8:6 = 3 bits

  #define MAX_VALUE_FOR_VID 127//=0x7F hex//=1111111 bin; has bits 15:9 = 7 bits

  #define MAX_NUMERIC_POSSIBLE_MULTI ( MAX_VALUE_FOR_FID + 8 )
  #define MAX_MULTI_DIV2 ( MAX_VALUE_FOR_FID + 8 ) / 2



#endif //AMD_FAMILY_17_H_
