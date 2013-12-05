/*
 * from_K10.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef FROM_K10_H_
#define FROM_K10_H_

#include <fastest_data_type.h> //typedef fastestUnsignedDataType
#include <preprocessor_macros/bitmasks.h>
#include <stdint.h> // uint32_t
//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER, ...
#include "configuration_space_addresses.h"

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

  //for AMD family 10(K10)
  #define START_BIT_FOR_CPU_CORE_DIVISOR_ID 6

  //"MSRC001_0070 COFVID Control Register"
  #define COFVID_CONTROL_REGISTER_MSR_ADDRESS 0xC0010070
  /** see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
   * "MSRC001_0071 COFVID Status Register" */
  #define COFVID_STATUS_REGISTER_MSR_ADDRESS 0xC0010071

  #define F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER 0xA4
  /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh
   *  Processors" "31:21 CurTmp: current temperature" */
  #define AMD_FROM_K10_CURTMP_START_BIT 21
  /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh"
   *  "17:16 CurTmpTjSel:"
   *  "Tj" = "Junction Temperature" ? */
  #define AMD_FROM_K10_CURR_TEMP_TJ_SELECT_START_BIT 16

extern float g_fMaxMultiplier;

namespace AMD
{
  /** function for AMD CPUs beginning with K10 (including 15hex. */
  namespace fromK10
  {
    inline void GetDivisorID(fastestUnsignedDataType dwMSRlowmost,
        fastestUnsignedDataType & divisorID)
    {
      //Divisor ID. "8:6 CurCpuDid: current core divisor ID. Read-only."
      divisorID = //(BYTE)(
      //      (g_dwLowmostBits & 448//=111000000bin
      //      ) >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ; //<=>bits 6-8 shifted to     }
        ( dwMSRlowmost >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) &
        BITMASK_FOR_LOWMOST_3BIT ;
    }

    inline fastestUnsignedDataType GetFrequencyID(const /*DWORD*/
      fastestUnsignedDataType dwMSRlowmost)
    {
      //Frequency ID. "5:0 CurCpuFid: current core frequency ID. Read-only."
      return ( dwMSRlowmost & BITMASK_FOR_LOWMOST_6BIT ) ;
    }

    fastestUnsignedDataType GetMaxCPU_COF()
    {
      uint32_t dwEAXlowMostBits, dwEDXhighMostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1 );
      /** 54:49 MaxCpuCof */
      return (dwEDXhighMostBits >> 17) // bits "54:49" ^= 6 bit
        & BITMASK_FOR_LOWMOST_6BIT;
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

    inline void GetMinAndMaxVoltageID(
      fastestUnsignedDataType & maxVID,
      fastestUnsignedDataType & minVID)
    {
      static uint32_t lowmost32bit, highmost32bit;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & lowmost32bit,
        & highmost32bit,
        1 ) ;
      maxVID = ( highmost32bit >>
        ( COFVID_STATUS_MSR_START_BIT_FOR_MAX_VID_IN_EDX )
        ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("highest Voltage ID:" << maxVID )
      minVID = ( highmost32bit >>
      //"41:35 [...]: maximum voltage. Read-only. Specifies the VID code
      //corresponding to the maximum voltage (lowest VID code) that the processor
      //drives. 00h indicates that no maximum VID code is specified.
      //See section 2.4.1 [Processor Power Planes And Voltage Control].
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
        ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("lowest VID:" << minVID )
    }

    inline fastestUnsignedDataType GetCurrentTemperatureRawValue()
    {
      DWORD dwValue;
      g_pfnReadPCIconfigSpace(
        0, //bus number
        //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
  //      g_byValue1
        CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER
        ,//) ((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
        F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER ,
        & dwValue
        );
      return dwValue;
    }

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    inline void GetSameReferenceClockForAllCPUcores(
      float * p_fReferenceClockInMHz)
    {
      //TODO "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
      //  "MSRC001_0015 Hardware Configuration Register (HWCR)":
      //"TscFreqSel: TSC frequency select. Read-write. BIOS: 1.
      // 0= The TSC increments at the rate of the REFCLK frequency.
      // 1=The TSC increments at the rate of the core P-state 0 COF specified by
      //   MSRC001_0064."

      //TODO "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
      // "CPUID Fn8000_0007 Advanced Power Management Information" :
      // Register EDX; bit # 8, value = 1: "TscInvariant"
      //  : "The TSC rate is ensured to be invariant across all P-states, C-States,
      //  and stop-grant transitions (such as STPCLK Throttling)."

      //#ifdef _DEBUG
      //This call sets g_fReferenceClockInMHz to the current reference clock.
      //This update of the value would be senseful for setting the CPU core
      //via "frequency" as parameter value the next time.
      GetCurrentReferenceClock(
        //TODO what is the reference clock/ max multiplier?
        // http://en.wikipedia.org/wiki/List_of_AMD_Turion_microprocessors#.22Lion.22_.2865_nm.29
        // says: max multi = ( "Performance State 0's Frequency ID" + 8 ) / 2,
        //http://en.wikipedia.org/wiki/List_of_AMD_Turion_microprocessors#fn_1:
        //  "The multiplier here applies to the 200 MHz system clock frequency"

        // "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
        // "CLKIN. The reference clock provided to the processor, nominally 200Mhz."
        g_fMaxMultiplier , //divide TSC diff by this divisor
        * p_fReferenceClockInMHz ,
      //    fReferenceClockInMHz,
        1000 , //min. timespan in ms
        10000  //max. timespan in ms
        ) ;

      DEBUGN(/*FULL_FUNC_NAME <<*/ "calculated reference clock:"
        << * p_fReferenceClockInMHz)
      //  if( * p_fReferenceClockInMHz == 0.0f )
      //    * p_fReferenceClockInMHz =
      //      //Default reference clock in MHz for Griffin CPUs.
      //      100.0f;

      //Time span too high or too low.
      if( * p_fReferenceClockInMHz == 0.0f )
        * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
      else
        //Save the current reference clock as last retrieved reference clock.
        g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;
    }
    #endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF

    inline void GetReferenceClock(float * p_fReferenceClockInMHz, unsigned coreID)
    {
      //TODO better use APIC Timer Operation?
      /** BIOS and Kernel Developer’s Guide (BKDG) For AMD Family 11h Processors:
      * "The pro-cessor bus clock is divided by the value in APIC3E0[Div] to
      * obtain a time base for the timer." */

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
      //Only get ref. clock if 1st core (else may be too small because the time
      //diff between core 0 and 1 may be taken)
      if( coreID == 0)
        GetSameReferenceClockForAllCPUcores(p_fReferenceClockInMHz);
      else
        * p_fReferenceClockInMHz = g_fReferenceClockInMHz;
    #else //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
      #ifdef MAX_MULTI_IS_MAIN_PLL_OP_FREQ_ID_MAX_PLUS_8
      * p_fReferenceClockInMHz = 100.0f ;
      #else
      * p_fReferenceClockInMHz = 200.0f ;
      #endif
    #endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    }

    inline float GetTemperatureInDegCelsius(const fastestUnsignedDataType CurTmp)
    {
      /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models"
       *  "00h-0Fh Processors" "D18F3xA4 Reported Temperature Control"
       * "<CurTmp*0.125>" */
      return (float) CurTmp * 0.125f;
    }

    inline void GetVoltageIDfromCOFVIDstatusRegisterBits(
      const DWORD dwMSRlowmost,
      BYTE & byVoltageID)
    {
      //voltage ID "15:9 CurCpuVid: current core VID. Read-only."
      byVoltageID = //(BYTE) (
      //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
        ( dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
    }

    /** works for family 15 model 0-F, but not for family 15 model F-1F?! */
    inline fastestUnsignedDataType GetVoltageIDfromCOFVIDstatusRegisterBits(
      const DWORD dwMSRlowmost)
    {
      //voltage ID "15:9 CurCpuVid: current core VID. Read-only."
      return //(BYTE) (
      //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
        ( dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
    }
  }
}

#endif /* FROM_K10_H_ */
