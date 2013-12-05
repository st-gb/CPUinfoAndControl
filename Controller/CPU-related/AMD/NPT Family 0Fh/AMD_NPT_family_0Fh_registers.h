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
//CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER, ...
#include "../configuration_space_addresses.h"

//10.5 Processor Performance States:
//MSR C001_0041h (FIDVID_CTL) and MSR C001_0042h (FIDVID_STATUS) are used for
//initiating P-state transitions and verifying that they are complete.

// "EventSelect 07Fh [L2 Fill/Writeback]"
#define EVENT_SELECT 07Fh //[L2 Fill/Writeback],
// "CPUID Fn8000_0008 Address Size And Physical Core Count Information"

/** "BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors"
 *  "32559 Rev. 3.16 November 2009" :
 *  "Thermtrip Status Register Function 3: Offset E4h" */
#define AMD_FAMILY_F_HEX_CPU_TEMPERATURE_OFFSET 0xE4

//#define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 3

//see Table 74. VID Code Voltages
#define MAXIMUM_VOLTAGE_ID 63


#endif /* AMD_NPT_FAMILY_0FH_REGISTERS_H_ */
