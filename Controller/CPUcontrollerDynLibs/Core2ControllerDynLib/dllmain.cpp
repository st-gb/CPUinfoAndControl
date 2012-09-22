/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2012.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//This file is intellectual property of Trilobyte SE GmbH, Berlin, Germany.
//Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany.
//It must not be used commercially without the permission of Trilobyte
//SE GmbH, Berlin, Germany.
//It may be used for educational / academic purposes for free.
//It may be used for personal use for free.
//If you want to publish (parts) this source code or generated binaries for other
// purposes than for a DLL for x86Info&Control you have to ask Trilobyte.
//If you use (parts) of this source code then this license text must be contained.
#ifdef _MSC_VER //MicroSoft compiler
#include "stdafx.h"
#endif

#ifdef INSERT_DEFAULT_P_STATES
  #include <ModelData/ModelData.hpp>
  //A I_CPUaccess pointer is passed as parameter in Init(...)
  #include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#endif //#ifdef INSERT_DEFAULT_P_STATES
//#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
//  #include <Controller/CPU-related/Intel/Intel_registers.h>
#include <Controller/CPU-related/Intel/Core/Core.hpp>
//  #include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//  #include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
#ifdef _DEBUG
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
//#include <Windows/GetNumberOfLogicalCPUs.h>
//  #include <preprocessor_helper_macros.h>  //for BITMASK_FOR_LOWMOST_5BIT
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for LOGFONTA etc. for winuser.h
#endif
//#include <winuser.h> //::MessageBox(...)
#include <windef.h> //for BYTE; WORD
//  #include <sstream> //std::stringstream
//  #include <tchar.h> //_T()

//  extern ReadMSR_func_type g_pfnreadmsr ;
//  extern WriteMSR_func_type g_pfn_write_msr ;
//  extern float g_fReferenceClockInMHz ;
//float g_fReferenceClockInMHz ;

//BYTE g_byValue1 ;

#ifdef _DEBUG
//Logger g_logger ;
#endif

//  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

#ifndef APIENTRY
  #define APIENTRY
#endif

#ifdef COMPILE_WITH_LOG
//g_logger, DEBUGN()
#include <preprocessor_macros/logging_preprocessor_macros.h>
void OpenLogFile()
{
  std::string strExeFileNameWithoutDirs = //GetExeFileNameWithoutDirs() ;
    "Intel_Core_controller_log.txt" ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
      ("Core2ControllerDLL_log.txt") ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
//  LPSTR lpstrModuleName ;
//  CHAR ar_strModuleName[100] ;
//  DWORD dwChars =
//     GetModuleFileName(
//     //HINSTANCE
//     //NULL
//     GetModuleHandle(NULL)
////     ,NULL //LPSTR
//     , ar_strModuleName
//     ,99 //DWORD
//     ) ;
  DEBUGN("chars for module name needed:" //<< dwChars //<< ar_strModuleName
      << strExeFileNameWithoutDirs )
//  LPSTR = new STR[dwChars] ;
//  DEBUGN()
}
#endif

bool Init()
{
  #ifdef COMPILE_WITH_LOG
  OpenLogFile() ;
  #endif
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  DEBUGN( FULL_FUNC_NAME <<  "ReadMSR fct. pointer :" << (void *) g_pfnreadmsr
      << " WriteMSR fct. pointer :" << (void *) g_pfn_write_msr
      )
  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
  {
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
    ::MessageBox(
      NULL ,
      "Pointers could not be assigned to the executables export functions\n"
      "Does the executable that loads this DLL have ReadMSR and WriteMSR"
      "export functions at all?(analyze this with a tool)"
      //Title
      ,"error"
      , MB_OK) ;
#endif
    return FALSE ;
  }
  GetReferenceClockFromMSR_FSB_FREQ() ;
  DEBUGN( FULL_FUNC_NAME << "ref. clock in MHz:" << g_fReferenceClockInMHz)

  //      //Force the cond. "< min. time diff" to become true.
  //      g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
  //      g_dwPreviousTickCountInMilliseconds
  //        //->time diff gets > max. time diff, so it calcs a ref clock.
  //        -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );

  //      //The reference clock is needed for setting the current frequency. So it
  //      //must be determined prior to any call of this function.
  //      GetCurrentReferenceClock(12.0, 100 , MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
  return true ;
}

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//For exporting this function with the same name as here in the source file.
//Especially for MinGW this line is needed in order to be called automatically
//for DLL attach / detach etc. actions.
EXPORT
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
           )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    //return Init() ;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
#endif //#ifdef _WIN32

//#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
#define NEHALEM_DLL_CALLING_CONVENTION

EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  NEHALEM_DLL_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNumberOfArrayElements
    )
{
  //see "Intel(R) 64 and IA-32 Architectures Software Developer's Manual,
  // Volume 3B: System Programming Guide, Part 2"
  //  B-38Vol. 3
  // "Table B-3.  MSRs in Processors Based on Intel Core Microarchitecture"
  // "17H 23 MSR_PLATFORM_I
  //  D"
  // "12:8 Maximum Qualified Ratio. (R)
  //  The maximum allowed bus ratio."

  //see "Intel(R) 64 and IA-32 Architectures Software Developer's Manual,
  // Volume 3B: System Programming Guide, Part 2"
  //  B-38Vol. 3
  // "Table B-3.  MSRs in Processors Based on Intel Core Microarchitecture"
  //198H 408 MSR_PERF_STATUS
  //"44:40" "Maximum Bus Ratio (R/O)"
  //"Indicates maximum bus ratio configured for
  // the processor."

//  * p_wNumberOfArrayElements = 0 ;
//  return NULL ;
  return GetAvailableMultipliersIntelCore( * p_wNumberOfArrayElements ) ;
}

EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  NEHALEM_DLL_CALLING_CONVENTION
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
  //See "Intel® Core™2 Duo Mobile Processor, Intel® Core™2 Solo Mobile
  //Processor and Intel® Core™2 Extreme Mobile Processor on 45-nm Process
  //Datasheet Doc#320120-004.32012001
  //For platforms based on Mobile Intel® 4 Series Express Chipset Family
  //March 2009",
  //chapter "3.10Processor DC Specification"

//  * p_wNum = 0 ;
//  return NULL ;
  return GetAvailableVoltagesInVoltIntelCore( * p_wNum ) ;
}

EXPORT
  BYTE
  NEHALEM_DLL_CALLING_CONVENTION
  GetCurrentVoltageAndFrequency(
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  return GetCurrentVoltageAndFrequencyIntelCore(
    p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , p_fMultiplier
    , p_fReferenceClockInMHz
    , wCoreID
    ) ;
//    return byRet ;
}

EXPORT
float
  NEHALEM_DLL_CALLING_CONVENTION
  GetTemperatureInCelsius ( WORD wCoreID
  )
{
  return //(float) byTempInDegCelsius ;
    GetTemperatureInDegCelsiusIntel( wCoreID ) ;
}

void InsertDefaultPstates(I_CPUaccess * pi_cpuaccess)
{
  float fVoltageForMiddleMulti ;
  float fMiddleMulti ;
  float fVoltageForHighestMulti ;
  float fHighestMulti ;

  if( GetDefaultPstates(
      fVoltageForMiddleMulti,
      fMiddleMulti,
      fVoltageForHighestMulti,
      fHighestMulti
      )
    )
  {
    DEBUGN("adding default voltage " << fVoltageForMiddleMulti << " for "
      << (WORD) fMiddleMulti * g_fReferenceClockInMHz << "MHz" )
    pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
      AddDefaultVoltageForFreq_inline( fVoltageForMiddleMulti,
        (WORD) ( fMiddleMulti * g_fReferenceClockInMHz )
        ) ;
    DEBUGN("adding default voltage " << fVoltageForHighestMulti << " for "
      << (WORD) fHighestMulti * g_fReferenceClockInMHz << "MHz" )
    pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
      AddDefaultVoltageForFreq_inline( fVoltageForHighestMulti,
        (WORD) ( fHighestMulti * g_fReferenceClockInMHz)
        ) ;
    //Diff may be for either multi with or without IDA (e.g. 9.0 or 9.5 for P8600)
    //e.g. for multi 9.5 at 1.2375V: 1.2375V - 1 = 0.2375V
    const float VoltageDiff = fVoltageForHighestMulti - fVoltageForMiddleMulti;
    const float MultiDiff = fHighestMulti - fMiddleMulti;
    const float minSuperLFMmulti = fMiddleMulti / 2.0f;

    //e.g. 1.0V - 0.1125 / 3 * 3 = 0.875 or 1.0V - 0.2375 / 3.5 * 3 = 0.875
    const float VoltageForMinMulti = fVoltageForMiddleMulti -
      VoltageDiff / MultiDiff * //(fMiddleMulti - 3.0f);
      minSuperLFMmulti;

    const float minFrequencyInMHz = //3.0f
      minSuperLFMmulti * g_fReferenceClockInMHz;
    //Core 2 P8600's MSR 0x15F has "00 20 86 0D" data: 86 hex=134 dec = multi 3,
    // 0D=13 dec= VID for 0.875 V
    DEBUGN("adding default voltage " << VoltageForMinMulti << " for "
      << (WORD) minFrequencyInMHz << "MHz" )
    pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
      AddDefaultVoltageForFreq_inline( VoltageForMinMulti ,
        (WORD) minFrequencyInMHz
        ) ;
  }
}

EXPORT
void
  NEHALEM_DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
  /** CPUaccess object inside the exe.*/
  I_CPUaccess * pi_cpuaccess
//  void * p_v
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
  Init() ;
#ifdef INSERT_DEFAULT_P_STATES
  InsertDefaultPstates(pi_cpuaccess);
#endif
}

EXPORT
  BYTE
  NEHALEM_DLL_CALLING_CONVENTION //can be omitted.
  SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
//    g_byValue1 = SetCurrentVoltageAndMultiplierIntelCore(
//      fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
  return //g_byValue1 ;
    SetCurrentVoltageAndMultiplierIntelCore(
      fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
}
