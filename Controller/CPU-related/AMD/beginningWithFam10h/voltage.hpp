/*
 * from_K10.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef FROM_K10_VOLTAGE_H_
#define FROM_K10_VOLTAGE_H_

#include <hardware/CPU/fastest_data_type.h> //typedef fastestUnsignedDataType
#include <preprocessor_macros/bitmasks.h>
#include <stdint.h> // uint32_t
//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER, ...
#include "../configuration_space_addresses.h"

extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace;

  /** Use prefix "COFVID_STATUS_MSR" to be eindeutig (because there may
  * also be a field with the same meaning in e.g. another register, but at
  * another start address) */

  /** "48:42 [...]: minimum voltage. Read-only. Specifies the VID code
  * corresponding to the minimum voltage (highest VID code) that the processor
  * drives. 00h indicates that no minimum VID code is specified.
  * See section 2.4.1 [Processor Power Planes And Voltage Control]." */
  #define COFVID_STATUS_MSR_START_BIT_FOR_MIN_VOLTAGE_VID 42
  #define COFVID_STATUS_MSR_START_BIT_FOR_MAX_VID \
    COFVID_STATUS_MSR_START_BIT_FOR_MIN_VOLTAGE_VID
  #define COFVID_STATUS_MSR_START_BIT_FOR_MIN_VOLTAGE_VID_IN_EDX \
    (COFVID_STATUS_MSR_START_BIT_FOR_MIN_VOLTAGE_VID - 32)
  #define COFVID_STATUS_MSR_START_BIT_FOR_MAX_VID_IN_EDX \
    COFVID_STATUS_MSR_START_BIT_FOR_MIN_VOLTAGE_VID_IN_EDX

  #define COFVID_STATUS_MSR_START_BIT_FOR_MAX_VOLTAGE_VID 35
  #define COFVID_STATUS_MSR_START_BIT_FOR_MAX_VOLTAGE_VID_IN_EDX \
    COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE_VID - 32

  /** "MSRC001_0071 COFVID Status" : "41:35 MaxVid: maximum voltage."
   *  "VID code corresponding to the maximum voltage" */
  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID \
      COFVID_STATUS_MSR_START_BIT_FOR_MAX_VOLTAGE_VID

  //"MSRC001_0070 COFVID Control Register"
  #define COFVID_CONTROL_REGISTER_MSR_ADDRESS 0xC0010070
  /** see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
   * "MSRC001_0071 COFVID Status Register" */
  #define COFVID_STATUS_REGISTER_MSR_ADDRESS 0xC0010071

extern float g_fMaxMultiplier;

namespace AMD
{
  /** function for AMD CPUs beginning with K10 (including 15hex. */
  namespace fromK10
  {
		/** Applies to AMD family 10h,11h */
    inline float GetVoltageInVolt(const fastestUnsignedDataType voltageID)
    {
    /** BIOS and Kernel Developer’s Guide (BKDG)
        For AMD Family 10h Processors 31116 Rev 3.62 - January 11, 2013
        MSRC001_0071CO COFVID Status Register
      * 2.4.1.6.3 Serial VID (SVI) Encodings:  1.550V - 0.0125V * SviVid[6:0];

    /** BIOS and Kernel Developer’s Guide (BKDG)
        For AMD Family 10h Processors 31116 Rev 3.62 - January 11, 2013
      MSRC001_0071CO COFVID Status Register
    * 15:9 CurCpuVid: current core VID. Read-only. In dual-node processors, 
      CurCpuVid on internal node 0 is the voltage driven to VDD.
      CurCpuVid on internal node 1 is the voltage of the higest performance 
		*  P-state (lowest numbered P-state) requested by all cores on internal 
		 *  node 1. CurCpuVid on internal node 1 is greater than or equal to 
		 * CurCpuVid on internal node 0. */
		
    /** For AMD Turion X2 Ultra (family 11h) : 
		 *    VID 28 = 1.2 = 1.55 - 28 * 0.0125 ;
          VID 64 = 0.75 = 1.55 - 64 * 0.0125
          => 1.55 - voltageID * 0.0125   */
    /** 41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG
     *   chapter "2.4.1.2 Serial VID Interface" :
     *  "The processor includes an interface, intended to control external
     *  voltage regulators, called the serial VID inter-face (SVI)."
     *
     *  see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
     *    chapter "2.4.1.6.3 Serial VID (SVI) Encodings"
     *
     *  see Intersil "ISL6265 FN6599.0 Data Sheet May 7, 2008",
     *     "TABLE 3. SERIAL VID CODES"
     * 

		 *  For family 15h CPU: 
        inbetween VID "148" is shown when under load and on highest multiplier
     *  VID 11 corresponds to 1.428 V? in CPU-Z  1.55 - 11 * 0.0125 = 1,4125
     *  VID 14 corresponds to 1.296/ 1.284 V in CPU-Z
     *  VID 47 corresponds to 1.008 V shown in CPU-Z  1.55 - 47×0,0125=0,9625
     *
     *  1.296V-1.008V=0,288V  47-14=33
     *  1.428V- 1.008V=0,42V  */
      return 1.55f - voltageID*0.0125f;
    }

    inline fastestUnsignedDataType GetMaximumVoltageID()
    {
    //    return 64 ;
      uint32_t dwEAXlowMostBits, dwEDXhighMostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1 ) ;
      fastestUnsignedDataType highestVID = ( dwEDXhighMostBits >>
          ( COFVID_STATUS_MSR_START_BIT_FOR_MAX_VID - 32 )
          ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("highest VID:" << highestVID)
      return highestVID ;
    }

    inline fastestUnsignedDataType GetMinimumVoltageID()
    {
    //    return 36 ;
      uint32_t dwEAXlowMostBits, dwEDXhighMostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1
        ) ;
      const fastestUnsignedDataType lowestVID = ( dwEDXhighMostBits >>
          ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
          ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("lowest VID:" << lowestVID)
      return lowestVID ;
    }

		/** Applies to AMD family 10h, 11h, 15h */
    inline void GetMinAndMaxVoltageID(
      fastestUnsignedDataType & voltageIDforLowestVoltage,
      fastestUnsignedDataType & voltageIDforHighestVoltage)
    {
      static uint32_t lowmost32bit, highmost32bit;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & lowmost32bit,
        & highmost32bit,
        1 ) ;
			/** -"42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models"
			 *    00h-0Fh Processors",
			 *  -"31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG" 
			 *  -"41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG":
			 *   "MSRC001_0071 COFVID Status:"
			 *   "48:42 MinVid: minimum voltage."  "41:35 MaxVid: maximum voltage." */
      voltageIDforLowestVoltage = ( highmost32bit >>
        ( COFVID_STATUS_MSR_START_BIT_FOR_MAX_VID_IN_EDX )
        ) & BITMASK_FOR_LOWMOST_7BIT;
			/** -31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *   "MSRC001_0071 COFVID Status Register",
			 *   "48:42 MinVid: minimum voltage" :
			 *   "00h indicates that no minimum VID code is specified." */
			if( voltageIDforLowestVoltage == 0 )
				 voltageIDforLowestVoltage = 0b1010100;
				//Table 8: SVI and internal VID codes: 101_0100b   0.5000 V
      DEBUGN("voltage ID for minimum voltage::" << voltageIDforLowestVoltage )
      voltageIDforHighestVoltage = ( highmost32bit >>
	    /** -31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *   "MSRC001_0071 COFVID Status Register",
			 *   "41:35 MaxVid: maximum voltages. Read-only. Specifies the VID code" :
			 *   "00h indicates that no maximum VID code is specified." */
      /** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG : 
			 *    MSRC001_0071 COFVID Status Register :
			 *   "41:35 [...]: maximum voltage. Read-only. Specifies the VID code
       *   corresponding to the maximum voltage (lowest VID code) that the processor
       *   drives. 00h indicates that no maximum VID code is specified.
       *   See section 2.4.1 [Processor Power Planes And Voltage Control].*/
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
        ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("lowest VID:" << voltageIDforHighestVoltage )
    }

    inline void GetCurrentVoltageIDfromCOFVIDstatusRegisterBits(
      const DWORD dwMSRlowmost,
      BYTE & byVoltageID)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG 
			 *    "15:9 CurCpuVid: current core VID. Read-only." */
      byVoltageID = //(BYTE) (
      //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
        ( dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
    }

    /** works for family 15 model 0-F, but not for family 15 model F-1F?! */
    inline fastestUnsignedDataType GetCurrentVoltageIDfromCOFVIDstatusRegisterBits(
      const DWORD dwMSRlowmost)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG 
			 *    "15:9 CurCpuVid: current core VID. Read-only." */
      return //(BYTE) (
      //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
        ( dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
    }

  /*inline void GetAvailableVoltagesInVolt(VIDforLowestVoltage, minVID)
	{
	}*/
  }
}

#endif /* FROM_K10_H_ */
