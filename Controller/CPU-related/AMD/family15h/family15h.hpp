#pragma once

#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include "../beginningWithFam10h/from_K10.h"

namespace AMD
{
  namespace family15h /** Bulldozer*/
  {
		float GetTemperatureInCelsius(WORD wCoreID)
		{
			DEBUGN( "begin")
			const fastestUnsignedDataType reportedTemperatureControlValue = 
				AMD::fromK10::GetReportedTemperatureControlValue();
			const fastestUnsignedDataType CurTmp = reportedTemperatureControlValue >>
				AMD_FROM_K10_CURTMP_START_BIT;
			DEBUGN( "CurrentTemperatureRawValue:" << CurTmp)
			const fastestUnsignedDataType CurTmpTjSel = 
				(reportedTemperatureControlValue >>
				AMD_FROM_K10_CURR_TEMP_TJ_SELECT_START_BIT) & BITMASK_FOR_LOWMOST_2BIT;
			DEBUGN( "CurTmpTjSel:" << CurTmpTjSel)
			if( CurTmpTjSel == 3 )
			{
		#ifdef _DEBUG
				DEBUGN( "CurTmp >> 2:" << (CurTmp >> 2) )
		#else
		#endif
				/** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models
				 *   00h-0Fh Processors", 
				 *   chapter "D18F3xA4 Reported Temperature Control"
				 *   "<(CurTmp[10:2]*0.5)-49>" */
				float fTempInDegCelsius = (float) ( (CurTmp >> 2) &
					BITMASK_FOR_LOWMOST_9BIT) * 0.5f - 49.0f;
				DEBUGN( " fTempInDegCelsius:" << fTempInDegCelsius)
				return fTempInDegCelsius;
			}
			else
			{
				/** 42301  Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 
				 *    00h-0Fh Processors 
				 *   "D18F3xA4 Reported Temperature Control" :
				 *    "IF (D18F3xA4[CurTmpTjSel]!=11b) THEN" 
				 *    "<CurTmp*0.125>"  */
				float fTempInDegCelsius = AMD::fromK10::GetTemperatureInDegCelsius(CurTmp );
				DEBUGN( " fTempInDegCelsius:" << fTempInDegCelsius)
				return fTempInDegCelsius;
			}			
		}
  }
}
