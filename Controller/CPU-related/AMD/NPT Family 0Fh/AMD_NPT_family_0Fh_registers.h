/*
 * AMD_NPT_family_0Fh_registers.h
 *
 *  Created on: 11.06.2012
 *      Author: Stefan
 */

#ifndef AMD_NPT_FAMILY_0FH_REGISTERS_H_
#define AMD_NPT_FAMILY_0FH_REGISTERS_H_

#include "AMD_NPT_family_0Fh_FIDVID_STATUS_register.h"
#include <Controller/CPU-related/AMD/AMD_MSR_adresses.h>

//10.5 Processor Performance States:
//MSR C001_0041h (FIDVID_CTL) and MSR C001_0042h (FIDVID_STATUS) are used for
//initiating P-state transitions and verifying that they are complete.

// "EventSelect 07Fh [L2 Fill/Writeback]"
#define EVENT_SELECT 07Fh //[L2 Fill/Writeback],
// "CPUID Fn8000_0008 Address Size And Physical Core Count Information"

//Thermtrip Status Register Function 3: Offset E4h
#define CPU_TEMPERATURE_OFFSET 0xE4

//#define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 3
//Device number 24, Function 3 is "CPU Miscellaneous Control"
// ( 24 << 3 ) | 3 = 192 | 3 = 192 + 3 = 195
#define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 195

//"23-14 CurTmp Current Temperature"
// 4.6 Function 3—Miscellaneous Control
#define MISCELLANEOUS_CONTROL_FUNCTION_NUMBER 3
//4.1 Configuration Space Accesses : "[...]Configuration accesses to Bus 0[...]"
#define CONFIGURATION_ACCESS_BUS_NUMBER 0
//see Table 74. VID Code Voltages
#define MAXIMUM_VOLTAGE_ID 63


#endif /* AMD_NPT_FAMILY_0FH_REGISTERS_H_ */
