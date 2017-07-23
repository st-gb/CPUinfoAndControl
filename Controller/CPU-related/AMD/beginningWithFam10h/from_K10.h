/*
 * from_K10.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef FROM_K10_H_
#define FROM_K10_H_

#include <hardware/CPU/fastest_data_type.h> //typedef fastestUnsignedDataType
#include <preprocessor_macros/bitmasks.h>
#include <stdint.h> // uint32_t
#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
  #include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
  float g_fMaxMultiplier;
#endif
/** ReadMSR(...)*/
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER, ...
#include "../configuration_space_addresses.h"
#include "voltage.hpp"
#include "GetMultiplier.hpp"

extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace;

	/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
	 *    MSRC001_0071 COFVID Status Register 
	 *    "8:6 CurCpuDid: current core divisor ID. Read-only." 
	 *  42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 
	 *    00h-0Fh Processors :
	 *    MSRC001_0071 COFVID Status :
	 *    "8:6 CurCpuDid: current core divisor ID."  */
  #define START_BIT_FOR_CPU_CORE_DIVISOR_ID 6
  #define NUM_BITS_FOR_CPU_CORE_DIVISOR_ID 3

  //"MSRC001_0070 COFVID Control Register"
  #define COFVID_CONTROL_REGISTER_MSR_ADDRESS 0xC0010070
  /** see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
   * "MSRC001_0071 COFVID Status Register" */
  #define COFVID_STATUS_REGISTER_MSR_ADDRESS 0xC0010071

  #define F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER 0xA4
  /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
	 *    F3xA4 Reported Temperature Control Register :
	 *    "31:21 CurTmp: current temperature"
	 * 
	 *  41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG 
	 *    F3xA4 Reported Temperature Control Register  
	 *    "31:21 CurTmp[10:0]: current temperature"
	 * 
   *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh
   *     Processors" 
	 *    "31:21 CurTmp: current temperature"  */
  #define AMD_FROM_K10_CURTMP_START_BIT 21
  /** 31116 Rev 3.62 - January 11, 2013  AMD Family 10h Processor BKDG
	 *   F3xA4 Reported Temperature Control Register
	 *   "17:16 CurTmpSel."
	 * 
	 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh"
   *   "17:16 CurTmpTjSel:"
   *   "Tj" = "Junction Temperature" ?
	 */
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
      divisorID = //(BYTE)(
      //      (g_dwLowmostBits & 448//=111000000bin
      //      ) >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ; //<=>bits 6-8 shifted to     }
        ( dwMSRlowmost >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) &
        BITMASK_FOR_LOWMOST_3BIT ;
    }

    inline fastestUnsignedDataType GetFrequencyID(const /*DWORD*/
      fastestUnsignedDataType dwMSRlowmost)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *    MSRC001_0071 COFVID Status Register 
			 *   "5:0 CurCpuFid: current core frequency ID. Read-only."  */
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
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *   MSRC001_0071 COFVID Status Register :
			 *   "54:49 MaxCpuCof" */
      return (dwEDXhighMostBits >> 17) // bits "54:49" ^= 6 bit
        & BITMASK_FOR_LOWMOST_6BIT;
    }

		/** Applies to AMD family 10h (K10), 11h (,15h?) */
    inline fastestUnsignedDataType GetReportedTemperatureControlValue()
    {
      DWORD dwValue;
			/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			       F3xA4 Reported Temperature Control Register
			 *  41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG
			       F3xA4 Reported Temperature Control Register */
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
		
		/** Applies to AMD family 10h (K10) and 11h */
		float //DLL_CALLING_CONVENTION 
			GetTemperatureInCelsius( WORD wCoreID
			)
		{
			DEBUGN( "begin")
			const fastestUnsignedDataType reportedTemperatureControlValue = 
				AMD::fromK10::GetReportedTemperatureControlValue();
			/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
				   F3xA4 Reported Temperature Control Register  
			  *  "31:21 CurTmp: current temperature." */
			/** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG 
			 *    F3xA4 Reported Temperature Control Register
			 *    "31:21 CurTmp[10:0]: current temperature." */
			const fastestUnsignedDataType CurTmp = reportedTemperatureControlValue >>
				AMD_FROM_K10_CURTMP_START_BIT;
			DEBUGN( "CurrentTemperatureRawValue:" << CurTmp)
//			fastestUnsignedDataType CurTmpTjSel = (reportedTemperatureControlValue >>
//				AMD_FROM_K10_CURR_TEMP_TJ_SELECT_START_BIT) & BITMASK_FOR_LOWMOST_2BIT;
//			DEBUGN( "CurTmpTjSel:" << CurTmpTjSel)
			float fTempInDegCelsius = AMD::fromK10::GetTemperatureInDegCelsius(CurTmp );
			DEBUGN( " fTempInDegCelsius:" << fTempInDegCelsius)
			return fTempInDegCelsius;
		}

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    inline void GetSameReferenceClockForAllCPUcores(
      float * p_fReferenceClockInMHz)
    {
      //TODO 
			/**"41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
          "MSRC001_0015 Hardware Configuration Register (HWCR)":
          "TscFreqSel: TSC frequency select. Read-write. BIOS: 1.
           0= The TSC increments at the rate of the REFCLK frequency.
           1=The TSC increments at the rate of the core P-state 0 COF specified 
           by MSRC001_0064." */
			
			//"NCLK. The main Northbridge clock. The NCLK frequency is the NB COF."

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
			{
				//TODO ReadMSR(MSRC001_0015 Hardware Configuration Register)
				// if ( TscFreqSel == 1 )
          GetSameReferenceClockForAllCPUcores(p_fReferenceClockInMHz);
			}
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
			/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *    F3xA4 Reported Temperature Control Register 
			 *     "This is encoded as value = 1/8th degree * Tctl"
			 *  41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG
			 *    , F3xA4 Reported Temperature Control Register :
			 *    "This is encoded as value = 1/8th degree * Tctl" */
      /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models"
       *    "00h-0Fh Processors" "D18F3xA4 Reported Temperature Control"
       *    "<CurTmp*0.125>" */
      return (float) CurTmp * 0.125f;
    }

		/** Applies to AMD family 10h, 11h */
		inline float * GetAvailableVoltagesInVolt(
		  WORD wCoreID
			, WORD * p_wNum )
		{
			fastestUnsignedDataType voltageIDforHighestVoltage, voltageIDforLowestVoltage;
			GetMinAndMaxVoltageID(voltageIDforLowestVoltage, voltageIDforHighestVoltage);
			const fastestUnsignedDataType numAvailableVoltages =  
				voltageIDforLowestVoltage - voltageIDforHighestVoltage + 1;
			float * ar_f = new float[numAvailableVoltages];
			if( ar_f)
			{
				fastestUnsignedDataType arrayIndex = 0;
				/** see 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG 
				 *  , "Table 8: SVI and internal VID codes" :
				 *   Higher voltage IDs mean lower voltages */
			  for( fastestUnsignedDataType voltageID = voltageIDforLowestVoltage; 
					voltageID >= voltageIDforHighestVoltage; voltageID --)
				{
					ar_f[arrayIndex ++] = GetVoltageInVolt(voltageID);
				}
				* p_wNum = numAvailableVoltages;
			}
			else
				* p_wNum = 0;
		  return ar_f;
		}

		/** The calculation for the multiplier applies to AMD family 10h and 15h */
    inline BYTE GetCurrentVoltageAndFrequency(
        float * p_fVoltageInVolt
        //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
        , float * p_fMultiplier
        , float * p_fReferenceClockInMHz
        , fastestUnsignedDataType wCoreID
        )
    {
      static uint32_t EAX, EDX;
      static fastestUnsignedDataType frequencyID, divisorID;
      static BYTE by ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin core ID:" << wCoreID)
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG: 
			 *     MSRC001_0071 COFVID Status Register :
			 *    "8:6 CurCpuDid: current core divisor ID"
       *    "5:0 CurCpuFid: current core frequency ID.". 
			 * 
			 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 
			 *    00h-0Fh Processors" : 
			 *    MSRC001_0071 COFVID Status :
			 *    "8:6 CurCpuDid: current core divisor ID"  
			 *    "5:0 CurCpuFid" 
			 * 
			 *  => read register contents of MSRC001_0071 COFVID Status */
      by = ::ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & EAX,
        & EDX,
        1 << wCoreID
        );

      AMD::fromK10::GetDivisorID(EAX, divisorID);
      frequencyID = AMD::fromK10::GetFrequencyID(EAX);
      * p_fMultiplier = AMD::fromK10::GetMultiplier(frequencyID, divisorID);

      const fastestUnsignedDataType voltageID = AMD::fromK10::
        GetCurrentVoltageIDfromCOFVIDstatusRegisterBits(EAX);
      * p_fVoltageInVolt = AMD::fromK10::GetVoltageInVolt(voltageID);
//      * p_fReferenceClockInMHz = 0.0f;
      AMD::fromK10::GetReferenceClock(p_fReferenceClockInMHz, wCoreID);
      return by;
    }
  }
}

#endif /* FROM_K10_H_ */
