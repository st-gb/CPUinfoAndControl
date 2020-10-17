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
#include <Controller/CPU-related/Intel/Core2/Core2.hpp>
// Intel::Pentium_M::GetAvailableMultipliers(...)
#include <Controller/CPU-related/Intel/Core/CoreAndCore2.hpp>
//  #include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//  #include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#include <Controller/CPU-related/Intel/Core/Core2_GetConfigurableMultipliers.hpp>
#include <Controller/CPU-related/Intel/HyperThreading.hpp> //Intel::IsHyperThreaded()
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
std::ofstream g_std_ofstream;
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
  #include <Windows/Process/GetDLLfileName.hpp> //GetDLLfileName(...)
  static HMODULE g_hModuleThisDLL;
#endif

//  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

#ifndef APIENTRY
  #define APIENTRY
#endif

float g_fReferenceClockInMHz;
float g_fReferenceClockMultiplier;
BYTE g_byValue1;
ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;
//This flag is for ODCM.
bool g_IsHyperThreaded = false;

#ifdef COMPILE_WITH_LOG
//g_logger, DEBUGN()
#include <preprocessor_macros/logging_preprocessor_macros.h>
void OpenLogFile()
{
#ifdef _WIN32 //win 32 or 64 bit
  std::tstring stdtstrDLLfileName  = GetDLLfileName(g_hModuleThisDLL);
#endif //#ifdef _WIN32
  std::string strCurrentProcessFileNameWithoutDirs //= GetStdString(
    = CurrentProcess::GetExeFileNameWithoutDirs_inline();
  std::string stdstrLogFilename = strCurrentProcessFileNameWithoutDirs +
    //("PentiumM_DLL_log.txt") ;
    stdtstrDLLfileName + "_log.txt";

  g_logger.OpenFileA( stdstrLogFilename ) ;
//  g_logger.AddExcludeFromLogging(
//    "BYTE ReadMSR(DWORD, uint32_t*, uint32_t*, DWORD_PTR)--index:412");

//  g_std_ofstream.open(logfilename);
//  g_std_ofstream << "begin";
//  g_std_ofstream.flush();
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
//  DEBUGN("chars for module name needed:" //<< dwChars //<< ar_strModuleName
//      << strExeFileNameWithoutDirs )
//  LPSTR = new STR[dwChars] ;
//  DEBUGN()
}
#endif

#define DLL_CALLING_CONVENTION
#define DYN_LIB_CALLING_CONVENTION

/** The array pointed to by the return value must be freed by the caller (i.e.
* x86I&C GUI or service) of this function. */
EXPORT float * DLL_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNumberOfArrayElements
    )
{
  DEBUGN( FULL_FUNC_NAME << " begin")
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
  return Intel::Core2::GetAvailableMultipliers( * p_wNumberOfArrayElements ) ;
}

/** @return The array pointed to by the return value must be freed by the caller (i.e.
* x86I&C GUI or service) of this function. */
EXPORT float * DLL_CALLING_CONVENTION GetAvailableVoltagesInVolt(
  WORD wCoreID
  , WORD * p_wNum )
{
  DEBUGN( FULL_FUNC_NAME << " begin")
  /** See "Intel® Core™2 Duo Mobile Processor, Intel® Core™2 Solo Mobile
   * Processor and Intel® Core™2 Extreme Mobile Processor on 45-nm Process
   * Datasheet
   * For platforms based on Mobile Intel® 4 Series Express Chipset Family
   * Document Number: 320120-004.32012001
   * March 2009"
   *
   * chapter "3.10 Processor DC Specifications" */

//  * p_wNum = 0 ;
//  return NULL ;
  /** Should return 0.8500V-1.500V according to 
   * http://ark.intel.com/de/products/29765/Intel-Core2-Quad-Processor-Q6600-8M-Cache-2_40-GHz-1066-MHz-FSB */
  return Intel::Core2::GetAvailableVoltagesInVolt( * p_wNum ) ;
}

//for getting ref clock:  Vol 3B
// "30.10.5 Cycle Counting and Opportunistic Processor Operation"
EXPORT BYTE DLL_CALLING_CONVENTION GetCurrentVoltageAndFrequency(
  float * p_fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float * p_fMultiplier
  , float * p_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin core ID:" << wCoreID)
  return Intel::Core2::GetCurrentVoltageAndFrequency(
    * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , * p_fMultiplier
    , * p_fReferenceClockInMHz
    , wCoreID
    ) ;
//    return byRet ;
}

EXPORT float DLL_CALLING_CONVENTION GetTemperatureInCelsius ( WORD wCoreID
  )
{
  return //(float) byTempInDegCelsius ;
    Intel::CoreAndCore2::GetTemperatureInDegCelsius( wCoreID ) ;
}

#ifdef INSERT_DEFAULT_P_STATES
void InsertDefaultPstates(I_CPUaccess * pi_cpuaccess)
{
  float fVoltageForMiddleMulti ;
  float fMiddleMulti ;
  float fVoltageForHighestMulti ;
  float fHighestMulti ;
#ifdef _DEBUG
      g_std_ofstream << "InsertDefaultPstates:" << std::endl;
      g_std_ofstream.flush();
#endif

  if( Intel::Core2::GetDefaultPstates(
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
#endif //INSERT_DEFAULT_P_STATES

float GetReferenceClockMultiplier(DWORD affMask)
{
  float multi;
  static uint32_t h, l;
//  Intel::Core2::GetVoltageAndMultiplier(//coreID
//    affMask, & r_fVoltageInVolt,
//    & multi);

  //see Intel 30.10.5
  //The multiplier for Intel Celeron 900 whose product with the reference clock
  // matches its core clock equals :
  //-MSR_PLATFORM_ID:MaximumQualifiedBusRatio
  ::ReadMSR(
    IA32_PLATFORM_ID, & l, & h, affMask
    );
  multi = (l >> 8) & BITMASK_FOR_LOWMOST_5BIT;
  DEBUGN( FULL_FUNC_NAME << " multiplier:" << multi)
  return multi;
}

bool Init()
{
  //#ifdef COMPILE_WITH_LOG
#ifdef _DEBUG
  OpenLogFile() ;
  #endif
  //Intel Core 2 CPU aren't hyperthreaded.
  g_IsHyperThreaded = /*Intel::IsHyperThreaded()*/ false;
#ifdef _DEBUG
  g_std_ofstream << "before AssignPointersToExportedExeMSRfunctions_inline(...)"
    << std::endl;
  g_std_ofstream.flush();
#endif
  AssignPointersToExportedExeMSRfunctions_inline(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  g_fReferenceClockMultiplier = GetReferenceClockMultiplier(1);
  DEBUGN( FULL_FUNC_NAME <<  "ReadMSR fct. pointer :" << (void *) g_pfnreadmsr
      << " WriteMSR fct. pointer :" << (void *) g_pfn_write_msr
      )
#ifdef _DEBUG
  g_std_ofstream << "after AssignPointersToExportedExeMSRfunctions_inline(...)"
    << std::endl;
  g_std_ofstream.flush();
#endif
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
  Intel::CoreAndCore2::GetReferenceClockFromMSR_FSB_FREQ() ;
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

EXPORT void DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
#ifdef INSERT_DEFAULT_P_STATES
  /** CPUaccess object inside the exe.*/
  I_CPUaccess * pi_cpuaccess
#else
  void * p_v
#endif //#ifdef INSERT_DEFAULT_P_STATES
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
#ifdef INSERT_DEFAULT_P_STATES
  #ifdef _DEBUG
//  g_std_ofstream << "Init(*)--pi_cpuaccess:" << pi_cpuaccess
//      << " sizeof(*pi_cpuaccess):" << sizeof(*pi_cpuaccess)
//      md5(*pi_cpuaccess)
//      << std::endl;
//  g_std_ofstream.flush();
  #endif
#endif
  Init() ;
#ifdef INSERT_DEFAULT_P_STATES
  InsertDefaultPstates(pi_cpuaccess);
#endif
#ifdef _DEBUG
  g_std_ofstream << "Init(*)--end:" << std::endl;
  g_std_ofstream.flush();
#endif
}

EXPORT BYTE DLL_CALLING_CONVENTION //can be omitted.
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
  return Intel::Core2::SetCurrentVoltageAndMultiplier(
      fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
}

#include "../ODCM/ThrottleRatioFunctions.hpp"

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
EXPORT BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved
  )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
    g_hModuleThisDLL = hModule;
#endif
    break;
    //return Init() ;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
#endif //#ifdef _WIN32

//#define DLL_CALLING_CONVENTION __stdcall
