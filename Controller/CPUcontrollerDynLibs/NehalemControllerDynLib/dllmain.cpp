//This file is intellectual property of Trilobyte SE GmbH, Berlin, Germany.
//Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany.
//It must not be used commercially without the permission of Trilobyte
//SE GmbH, Berlin, Germany.
//It may be used for educational / academic purposes for free.
//It may be used for personal use for free.
//If you want to publish (parts) this source code or generated binaries for other
// purposes than for a DLL for x86Info&Control you have to ask Trilobyte.
//If you use (parts) of this source code then this license text must be contained.
#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//For GetCurrentReferenceClock(...)
#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
#include <Controller/CPU-related/Intel/Intel_registers.h>
//Used by "Nehalem.hpp". The alternative would be:
// -in "Nehalem.hpp" that may be used in both I_CPUcontroller-derived class and
// this DLL source code would need to include the appropriate version of
// the 2 "inline_register_access_functions.hpp"
// -or: include <inline_register_access_functions.hpp> and set include path
// for DLL to "-I Windows/AssignPointersToExportedExeFunctions/\_
// inline_register_access_functions.hpp", for I_CPUcontgroller-derived to
// "-I Conroller/.../_
// inline_register_access_functions.hpp"

//#include <Windows/AssignPointersToExportedExeFunctions/\_
//inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
//For PrepareForNextPerformanceCountingNehalem(...) (etc.)
#include <Controller/CPU-related/Intel/Nehalem/Nehalem.hpp>
#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
#ifdef _DEBUG
  #include <Controller/Logger/Logger.hpp> //class Logger
#endif
//#include <ModelData/ModelData.hpp>

//for BITMASK_FOR_LOWMOST_5BIT
#include <preprocessor_macros/bitmasks.h>
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
//#define _DEBUG
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>

//_WIN32: Built-in macro for MSVC, MinGW (also for 64 bit Windows)
#if defined(_DEBUG) && defined(_WIN32)
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif

#include <float.h> //for FLT_MIN
//#include <sstream> //std::stringstream
#include <tchar.h> //_T()
#include <windef.h> //for APIENTRY
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for MessageBox(...)
#endif
#ifdef _DEBUG
  //This logger variable _must_ have another name than in the executable this
  //dynamic library is attached to under _Linux_ . Else when this dyn lib is
  //unloaded the exe's logger destructor is called.
  Logger g_loggerDynLib ;
#endif

//defined in "/Windows/AssignPointersToExportedExeFunctions/
// AssignPointersToExportedExeMSRfunctions.cpp
extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;
//  extern float g_fReferenceClockInMHz ;
float g_fReferenceClockInMHz ;
static float gs_fTimeStampCounterMultiplier = 0.0 ;

//Use global vars instead of allocating them for each function call (->faster)
BYTE g_byValue1 , g_byValue2 ;
DWORD g_dwValue1 , g_dwValue2 ;

#define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define EXPORT extern "C" __declspec(dllexport)
#else
  //http://www.linuxquestions.org/questions/programming-9/
  // how-to-export-function-symbols-750534/:
  //"__attribute__ ((visibility("default")))  // (similar to __declspec(dllexport))"
  #define EXPORT extern "C" //__attribute__ ((visibility("default")))
#endif

#ifndef APIENTRY
  #define APIENTRY
#endif

void InitOtherOSthanWindows()
{
  #ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = "" ;
    //GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("NehalemControllerDLL_log.txt") ;
  g_loggerDynLib.OpenFile2( stdstrFilename ) ;
  DEBUGN_LOGGER_NAME(g_loggerDynLib,"this Log file is open")
  //  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
  #endif //#ifdef _DEBUG
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  DEBUGN("g_pfnreadmsr:" << g_pfnreadmsr
      << "g_pfn_write_msr:" << g_pfn_write_msr)
  gs_fTimeStampCounterMultiplier = GetTimeStampCounterMultiplier() ;
}

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
bool InitWindows()
{
  #ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
  ("NehalemControllerDLL_log.txt") ;
  g_loggerDynLib.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
  //  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
  #endif
  //gp_nehalem_clocksnothaltedcpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
  //  ) ;
  //Reaches here when compiled with MSC but not when MinGW?!
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr , g_pfn_write_msr ) ;
  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
  {
    MessageBox( NULL,
      //_T() macro: ANSI-> "", unicode: ->L""; for Microsoft's compiler
      //each line needs a _T() macro.
      _T("Pointers could not be assigned to the execu-tables export functions\n")
      _T("Does the executable that loads this DLL have ReadMSR and WriteMSR")
      _T("export functions at all?(analyze this with a tool)")
      //Title
      ,_T("error")
      , MB_OK) ;
    return FALSE ;
  }
  gs_fTimeStampCounterMultiplier = GetTimeStampCounterMultiplier() ;
  //Force the cond. "< min. time diff" to become true.
  g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
  g_dwPreviousTickCountInMilliseconds
    //->time diff gets > max. time diff, so it calcs a ref clock.
    -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );
  //The reference clock is needed for setting the current frequency. So it
  //must be determined prior to any call of this function.
  GetCurrentReferenceClock(
    //12.0,
    gs_fTimeStampCounterMultiplier ,
    g_fReferenceClockInMHz ,
    1000 ,
    MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
  DEBUGN("first calculated reference clock in MHz: "
    << g_fReferenceClockInMHz )
  return TRUE ;
}
#endif //#ifdef _WIN32

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //For exporting this function with the same name as here in the source file.
  //Especially for MinGW this line is needed in order to be called automatically
  //for DLL attach / detach etc. actions.
  #define DLLMAIN_FRONT_SIGNATURE EXPORT BOOL APIENTRY
#else //#ifdef _WIN32
  //from http://tdistler.com/2007/10/05/
  // implementing-dllmain-in-a-linux-shared-library:
  #define DLLMAIN_FRONT_SIGNATURE void __attribute__ ((constructor))
#endif //#ifdef _WIN32

DLLMAIN_FRONT_SIGNATURE DllMain(
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved
#endif //#ifdef _WIN32
  )
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {
    return InitWindows() ;
  }
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
#else //#ifdef _WIN32
  InitOtherOSthanWindows() ;
#endif //#ifdef _WIN32
}
//#endif //#ifdef _WIN32

//Calling convention--must be the same as in the DLL
//function signature that calls this function?!
//WINAPI
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
    , WORD * p_wNumberOfArrayElements )
{
//    BYTE byMaxMultiplier = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-generated version has no problems
//#ifndef _MSC_VER
//    std::stringstream stdstrstream ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //<MaximumTurboRatioLimit1C startbit="0" bitlength="8"/>
//  //<MaximumTurboRatioLimit4C startbit="24" bitlength="8"/>
//  #endif
   //g_pi_cpuaccess->RdmsrEx(
  g_byValue1 = (*g_pfnreadmsr) (
    MSR_TURBO_RATIO_LIMIT ,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits,
    1 << wCoreID //m_dwAffinityMask
    ) ;
  DEBUGN("Dyn Lib--return value of Exe's ReadMSR:" << (WORD) g_byValue1)
  if( g_byValue1 ) //successfully read from MSR
  {
    //Maximum  multiplier
    g_byValue1 = (BYTE) ( dwLowmostBits & 255 ) ;
    DEBUGN("Dyn Lib--max mulit:" << (WORD) g_byValue1)
    BYTE byNumMultis = g_byValue1 -
      //min. multi - 1
      6 ;
    float * ar_f = new float[byNumMultis] ;
    //If allocating the array on the heap succeeded.
    if( ar_f )
    {
      *p_wNumberOfArrayElements = byNumMultis ;
      float fMulti = 7.0 ; //The minimum multiplier for Nehalem is 7.
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
      for( BYTE by = 0 ; by < byNumMultis ; ++ by )
      {
        ar_f[by] = fMulti ++ ;
//       stdstrstream << fMulti << " " ;
      }
    }
//     MessageBox(NULL, stdstrstream.str().c_str(), "info", MB_OK) ;
#ifdef _DEBUG
  //if( wCoreID == 0 )
  //{
  // str.clear() ;
  //str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess
  //  << "multiplier:" << (WORD)byMaxMultiplier ;
  ////MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //}
#endif
    return ar_f ;
  }
  * p_wNumberOfArrayElements = 0 ;
  return NULL ;
}

//For exporting this function with the same name as here in the source file.
EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  NEHALEM_DLL_CALLING_CONVENTION
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
//    BYTE byMaxMultiplier = 0 ;
//    DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-generated version has no problems
//#ifndef _MSC_VER
//    std::stringstream stdstrstream ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  #endif
   //g_pi_cpuaccess->RdmsrEx(
   BYTE byNumVoltages = 2 ;
   float * ar_f = new float[byNumVoltages] ;
   //If allocating the array on the heap succeeded.
   if( ar_f )
   {
     ar_f[0] = 0.65 ;
     ar_f[1] = 0.9 ;
     *p_wNum = byNumVoltages ;
   }
   else
     *p_wNum = 0 ;
//     MessageBox(NULL, stdstrstream.str().c_str(), "info", MB_OK) ;
#ifdef _DEBUG
  //if( wCoreID == 0 )
  //{
  // str.clear() ;
  //str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess
  //  << "multiplier:" << (WORD)byMaxMultiplier ;
  ////MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //}
#endif
  return ar_f ;
}

EXPORT
  BYTE
  NEHALEM_DLL_CALLING_CONVENTION
  GetCurrentVoltageAndFrequency(
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    //
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
//  //Intel: 198H 408 IA32_PERF_STATUS
  g_byValue1 =
//    g_pi_cpuaccess->RdmsrEx(
    (*g_pfnreadmsr) (
    IA32_PERF_STATUS,
    & g_dwValue1,// lowmost bit 0-31 (register "EAX")
    & g_dwValue2, //highmost bit 32-63
    1 << wCoreID //m_dwAffinityMask
    ) ;
  if( g_byValue1 )
  {
    * p_fVoltageInVolt = 0 ;
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
    * p_fMultiplier = ( g_dwValue1 & 255 ) ;
    DEBUGN("dyn lib GetCurrentVoltageAndFrequency--voltage:"
      << * p_fVoltageInVolt )
//#if !defined(_DEBUG) && defined(__linux_)
#ifdef STATIC_133MHZ_REFERENCE_CLOCK
    * p_fReferenceClockInMHz = 133.3 ;
#else
    //This call sets g_fReferenceClockInMHz to the current reference clock.
    //This update of the value would be senseful for setting the CPU core
    //via "frequency" as parameter value the next time.
    GetCurrentReferenceClock(
      ////720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
//      12.0 ,
      gs_fTimeStampCounterMultiplier ,
      * p_fReferenceClockInMHz ,
      1000 , //min. timespan in ms
      10000 ) ;
#endif
    DEBUGN("dyn lib GetCurrentVoltageAndFrequency--ref clock:"
      << * p_fReferenceClockInMHz )
    //Timespan too high or too low.
    if( * p_fReferenceClockInMHz == 0.0 )
      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
    else
      g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;
    DEBUGN("for core " << wCoreID << ": calculated reference clock in MHz: "
      << //g_fReferenceClockInMHz
      * p_fReferenceClockInMHz )
//      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
  }
  return g_byValue1 ;
}

//  extern "C" __declspec(dllexport)
//  WORD
//    //Calling convention--must be the same as in the DLL
//    //function signature that calls this function?!
//    NEHALEM_DLL_CALLING_CONVENTION
//    GetNumberOfCPUcores()
//  {
//    WORD wNumberOfCPUcores = 0 ;
//    wNumberOfCPUcores = GetNumLogicalCPUs() ;
//    return wNumberOfCPUcores ;
//  }

EXPORT
float
  NEHALEM_DLL_CALLING_CONVENTION
  //WINAPI
  GetTemperatureInCelsius ( WORD wCoreID
  )
{
//    BYTE byDigitalReadout ;
//    BYTE byTempInDegCelsius ;
//    BYTE byTempTarget;
//    BYTE byResolutionInDegreesCelsius ;
////    DWORD dwLowmostBits ;
////    DWORD dwHighmostBits ;
//    (*g_pfnreadmsr) (
//       MSR_TEMPERATURE_TARGET , //Intel "B-12Vol. 3"
//       & g_dwValue1, // bits 0-31 (register "EAX")
//       & g_dwValue2, // bits 32-63 (register "EDX")
//       //m_dwAffinityMask
//       1 << wCoreID
//       ) ;
//    //Intel B-76Vol. 3 : "23:16 Temperature Target. (R)"
//    // "The minimum temperature at which PROCHOT# will be asserted.
//    //  The value is degree C."
//    byTempTarget = ( g_dwValue1 >> 16 ) & 255 ;

  g_byValue1 = (*g_pfnreadmsr) (
     IA32_THERM_STATUS , //Address: 1A2H
     & g_dwValue1, // bits 0-31 (register "EAX")
     & g_dwValue2,
     //m_dwAffinityMask
     1 << wCoreID
     ) ;
  if( g_byValue1 )
  {
    //Intel: "22:16 Digital Readout (RO)"
    g_byValue1 = ( g_dwValue1 >> 16 ) & BITMASK_FOR_LOWMOST_7BIT ;

//    //Intel: "30:27 Resolution in Degrees Celsius (RO)"
//    byResolutionInDegreesCelsius = ( g_dwValue1 >> 27 ) &
//      BITMASK_FOR_LOWMOST_5BIT ;

    // TemperatureTarget - "Digital Readout"
//    byTempInDegCelsius = byTempTarget - g_byValue1 ;
    g_byValue2 = 100 - g_byValue1 ;
    return (float) g_byValue2 ;
  }
  return FLT_MIN ;
}

EXPORT
void
  NEHALEM_DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
  //CPUaccess object inside the exe.
//  I_CPUaccess * pi_cpuaccess
  void * p_v
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
//  InitOtherOSthanWindows() ;
}

//  extern "C" __declspec(dllexport)
//  void
//    Init( //I_CPUcontroller * pi_cpu
//    //CPUaccess object inside the exe.
////    I_CPUaccess * pi_cpuaccess
//    void * p_v
//    , ReadMSR_func_type pfnreadmsr
//    //BYTE by
//    )
//  {
//  //  g_pi_cpuaccess = pi_cpuaccess ;
//  #ifdef _DEBUG
//    //ReadMSR_func_type rdmsr = (ReadMSR_func_type) (void*) & pi_cpuaccess->RdmsrEx ;
//    std::stringstream stdstrstream ;
//    //For checking if the members are on the same RAM address between MSVC and MinGW:
//    stdstrstream << "DLL::Init(...)--\naddress of I_CPUaccess:" << & pi_cpuaccess << "\n"
//      << "address of I_CPUaccess::mp_model: " << & pi_cpuaccess->mp_model <<"\n"
//      << "address in I_CPUaccess::mp_model: " << pi_cpuaccess->mp_model <<"\n"
//      //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
//      << "address of I_CPUaccess::mp_cpu_controller: " << & pi_cpuaccess->mp_cpu_controller <<"\n"
//      << "address in I_CPUaccess::mp_cpu_controller: " << pi_cpuaccess->mp_cpu_controller ;
//    MessageBoxA( NULL, stdstrstream.str().c_str() , //TEXT("")
//      "", MB_OK) ;
//  #endif
//    //MSC-generated version has no problems
//  //#ifndef _MSC_VER
//  //  std::stringstream str ;
//  //  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
//  //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //#endif
//  }

EXPORT
WORD
  NEHALEM_DLL_CALLING_CONVENTION
  PrepareForNextPerformanceCounting(
   DWORD dwAffMask
   , BYTE byPerformanceEventSelectRegisterNumber
   )
{
  return PrepareForNextPerformanceCountingNehalem(
    dwAffMask,
    byPerformanceEventSelectRegisterNumber) ;
}

//For exporting this function with the same name as here in the source file.
EXPORT
  BYTE
  //NEHALEM_DLL_CALLING_CONVENTION
  //WINAPI
//    __stdcall
  SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << g_dwValue1 ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
  DEBUGN("before calling Exe's WriteMSR function")
  g_byValue1 =
    //g_pi_cpuaccess->WrmsrEx(
    (*g_pfn_write_msr) (
    //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W) When set to 1: disengages IDA since:
    //     06_0FH (Mobile)
    //  "63:33 Reserved"
    IA32_PERF_CTL,
    (BYTE) fMultiplier , // bits 0-31 (register "EAX")
    g_dwValue2, //bits 0-31 (register "EDX")
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  DEBUGN("after calling Exe's WriteMSR function. return value: "
    << (WORD) g_byValue1 )
  return g_byValue1 ;
}
