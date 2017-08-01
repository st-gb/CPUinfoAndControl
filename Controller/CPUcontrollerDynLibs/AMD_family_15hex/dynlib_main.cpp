/*
 * dynlib_main.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifdef _MSC_VER //MicroSoft compiler
#include "stdafx.h"
#endif

#include <Controller/CPU-related/AMD/from_K10.h>
#include <Controller/CPU-related/AMD/beginningWithFam10h/GetAvailableMultipliers.hpp>
#include <preprocessor_macros/bitmasks.h>
#include <Controller/CPU-related/AMD/family15h/family15h.hpp>
#ifdef INSERT_DEFAULT_P_STATES
  #include <ModelData/ModelData.hpp>
  //A I_CPUaccess pointer is passed as parameter in Init(...)
  #include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#endif //#ifdef INSERT_DEFAULT_P_STATES
//#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
//  #include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//  #include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointerToExportedExeReadPCIconfig.h>
//#include <Controller/CPU-related/Intel/Core/Core2_GetConfigurableMultipliers.hpp>
//#include <Controller/CPU-related/Intel/HyperThreading.hpp> //Intel::IsHyperThreaded()
#include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
#ifdef _DEBUG
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#else
  #define DEBUGN() /* empty */
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
//std::ofstream g_std_ofstream;
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
fastestUnsignedDataType g_MaxCPU_Cof;
/** this multiplier is used for:
 *  -The TSC diff is divided by this multiplier to get the reference clock
 *  -getting available multpliers */
float g_fMaxMultiplier;
BYTE g_byValue1;
ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;
ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;

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

/** @brief The reference clock might change, also during runtime.
* This is why it is a good idea to get the possible multipliers.
* @return The array pointed to by the return value must be freed by the caller (i.e.
* x86I&C GUI or service) of this function. */
EXPORT float * DLL_CALLING_CONVENTION GetAvailableMultipliers(
  WORD wCoreID
  , WORD * p_wNumberOfArrayElements
  )
{
  DEBUGN( /*FULL_FUNC_NAME <<*/ "begin")
//  * p_wNumberOfArrayElements = 0 ;
  float fMainPllOpFreqIdMax = AMD::fromK10::GetMaxCPU_COF();
  return AMD::fromK10::GetAvailableMultipliers(p_wNumberOfArrayElements,
    fMainPllOpFreqIdMax);
}

/** @return The array pointed to by the return value must be freed by the caller (i.e.
* x86I&C GUI or service) of this function. */
EXPORT float * DLL_CALLING_CONVENTION GetAvailableVoltagesInVolt(
  WORD wCoreID
  , WORD * p_wNum )
{
  DEBUGN("begin")
  return AMD::fromK10::GetAvailableVoltagesInVolt(wCoreID, p_wNum);
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
//    return byRet ;
  return AMD::fromK10::GetCurrentVoltageAndFrequency(
    p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , p_fMultiplier
    , p_fReferenceClockInMHz
    , wCoreID
    ) ;
}

EXPORT float DLL_CALLING_CONVENTION GetTemperatureInCelsius ( WORD wCoreID
  )
{
  return AMD::family15h::GetTemperatureInCelsius(wCoreID);
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

bool Init()
{
  //#ifdef COMPILE_WITH_LOG
#ifdef _DEBUG
  OpenLogFile() ;
  #endif
  /** AMD K10 CPUs aren't hyperthreaded. */
  g_IsHyperThreaded = /*Intel::IsHyperThreaded()*/ false;
#ifdef _DEBUG
//  g_std_ofstream << "before AssignPointersToExportedExeMSRfunctions_inline(...)"
//    << std::endl;
//  g_std_ofstream.flush();
#endif
  AssignPointersToExportedExeMSRfunctions_inline(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  AssignPointerToExportedExeReadPCIconfig(g_pfnReadPCIconfigSpace) ;
//  g_fReferenceClockMultiplier = GetReferenceClockMultiplier(1);
  DEBUGN( /*FULL_FUNC_NAME <<*/ "ReadMSR fct. pointer :" << (void *) g_pfnreadmsr
    << " WriteMSR fct. pointer :" << (void *) g_pfn_write_msr
    )
#ifdef _DEBUG
//  g_std_ofstream << "after AssignPointersToExportedExeMSRfunctions_inline(...)"
//    << std::endl;
//  g_std_ofstream.flush();
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
//  Intel::CoreAndCore2::GetReferenceClockFromMSR_FSB_FREQ() ;
  DEBUGN( /*FULL_FUNC_NAME <<*/ "ref. clock in MHz:" << g_fReferenceClockInMHz)

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

EXPORT void DLL_CALLING_CONVENTION Init( //I_CPUcontroller * pi_cpu
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
  Init() ;
  g_MaxCPU_Cof = AMD::fromK10::GetMaxCPU_COF();
  g_fMaxMultiplier = AMD::fromK10::GetMaximumMultiplier(g_MaxCPU_Cof);
#ifdef INSERT_DEFAULT_P_STATES
  #ifdef _DEBUG
//  g_std_ofstream << "Init(*)--pi_cpuaccess:" << pi_cpuaccess
//      << " sizeof(*pi_cpuaccess):" << sizeof(*pi_cpuaccess)
//      md5(*pi_cpuaccess)
//      << std::endl;
//  g_std_ofstream.flush();
  #endif
#endif
#ifdef INSERT_DEFAULT_P_STATES
  InsertDefaultPstates(pi_cpuaccess);
#endif
#ifdef _DEBUG
//  g_std_ofstream << "Init(*)--end:" << std::endl;
//  g_std_ofstream.flush();
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
  DEBUGN( /*FULL_FUNC_NAME << "--" */ "begin")
//    g_byValue1 = SetCurrentVoltageAndMultiplierIntelCore(
//      fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
//  return Intel::Core2::SetCurrentVoltageAndMultiplier(
//    fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
  return 0;
}

//#include "../Intel/ODCM/ThrottleRatioFunctions.hpp"

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
