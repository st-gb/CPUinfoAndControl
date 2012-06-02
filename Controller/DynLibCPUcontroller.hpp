/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //Include guard.

#include "CPUcontrollerDynLib_function_definitions.h"
//#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
//#include <Controller/exported_functions.h>
//#include <windef.h> //BYTE et.c

class DynLibCPUcontroller
{
protected:
  pfn_GetCurrentVoltageAndFrequency_type m_pfnGetCurrentVoltageAndFrequency ;
  dll_GetNumberOfCPUcores_type m_pfnGetNumberOfCPUcores ;
//  dll_SetCurrentPstate_type m_pfnsetcurrentpstate ;
  pfnSetCurrentVoltageAndMultiplier_type m_pfnSetCurrentVoltageAndMultiplier ;
  pfnGetAvailableMultipliers_type m_pfnGetAvailableMultipliers ;
  pfnGetAvailableMultipliers_type m_pfnGetAvailableVoltages ;
//  dll_GetMaximumFrequencyInMHz_type m_pfnGetMaximumFrequencyInMHz ;
//  dll_GetMaximumVoltageID_type m_pfnGetMaximumVoltageID ;
//  dll_GetMinimumVoltageID_type m_pfnGetMinimumVoltageID ;
//  dll_GetMaximumFrequencyInMHz_type m_pfnGetMinimumFrequencyInMHz ;
  dll_GetTemperatureInCelsius_type m_pfngettemperatureincelsius ;
  dll_PrepareForNextPerformanceCounting
    m_pfn_preparefornextperformancecounting ;
  dll_GetVoltageInVolt_type m_pfn_GetVoltageInVolt ;
  dll_GetVoltageID_type m_pfn_GetVoltageID ;
//  dynlib_SetPstateFromFreq m_pfn_set_pstate_from_freq ;

  //Writing to MSR can be done inside the CPU controller dyn lib in a safer way
  //because inside the
  //CPU controller DLL is the knowledge that knows writing to which MSR registers
  //may be harmful-
  dll_WriteMSR_type m_pfn_write_msr ;

};
