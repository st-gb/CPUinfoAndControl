/** File:   temperature.hpp
 * Author: s.gebauer
 * Created on 1. August 2017, 17:04 */

#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include "ReportedTemperatureControlRegister.h"
#include "../configuration_space_addresses.h"

namespace AMD
{
  /** function for AMD CPUs beginning with K10 (including 15hex. */
  namespace fromK10
  {
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
        CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER
        ,//) ((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
        F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER ,
        & dwValue
        );
      return dwValue;
    }
		
    /** Applies to AMD family 10h (K10), 11h, 15h */
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

    /** Applies to AMD family 10h (K10) and 11h */
    float //DLL_CALLING_CONVENTION 
      GetTemperatureInCelsius(WORD wCoreID)
    {
      DEBUGN("begin")
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
      DEBUGN("CurrentTemperatureRawValue:" << CurTmp)
      float fTempInDegCelsius = AMD::fromK10::GetTemperatureInDegCelsius(CurTmp);
      DEBUGN(" fTempInDegCelsius:" << fTempInDegCelsius)
      return fTempInDegCelsius;
    }
    
    inline fastestUnsignedDataType GetCurrentTemperatureSelect(
      const fastestUnsignedDataType reportedTemperatureControlValue)
    {
      fastestUnsignedDataType CurTmpTjSel = (reportedTemperatureControlValue >>
        AMD_FROM_K10_CURR_TEMP_TJ_SELECT_START_BIT) & BITMASK_FOR_LOWMOST_2BIT;
      DEBUGN( "CurTmpTjSel:" << CurTmpTjSel)
      return CurTmpTjSel;
    }
  }
}

#endif /* TEMPERATURE_HPP */
