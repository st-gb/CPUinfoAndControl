/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2012-at.
 * least 2012. You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * dynlib_main.cpp
 *
 *  Created on: 23.12.2012
 *      Author: Stefan
 */

#ifdef INSERT_DEFAULT_P_STATES
  #include <ModelData/ModelData.hpp>
  //A I_CPUaccess pointer is passed as parameter in Init(...)
  #include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#endif //#ifdef INSERT_DEFAULT_P_STATES

#include <Controller/CPU-related/Intel/Core/Core.hpp>

#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
//#ifdef _DEBUG
#ifdef COMPILE_WITH_LOG
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
//  GLOBAL_LOGGER_FULLY_QUALIFIED_CLASS_NAME g_logger;
#endif
// Intel::Pentium_M::GetAvailableMultipliers(...)
#include <Controller/CPU-related/Intel/PentiumM/Pentium_M_and_Core1.hpp>

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for LOGFONTA etc. for winuser.h
#endif
#include <windef.h> //for BYTE; WORD

uint32_t g_dwValue1, g_dwValue2 ;
//Define global variables here, so that I_CPUcontroller-derived class and
// DLL sourcecode both do not need to define them (or better in a file named
// "Core.cpp" ? ). These variables have a generic name ( "...Value">>#<<)
// because they are used for different purposes, else more global variables
// would be needed.
  BYTE g_byValue1, g_byValue2 ;

#ifndef APIENTRY
  #define APIENTRY
#endif
//Calling convention--must be the same as in the DLL
//function signature that calls this function?!
//#define DLL_CALLING_CONVENTION __stdcall
#define DLL_CALLING_CONVENTION

#ifdef COMPILE_WITH_LOG
//g_logger, DEBUGN()
#include <preprocessor_macros/logging_preprocessor_macros.h>
void OpenLogFile()
{
  std::string strExeFileNameWithoutDirs = //GetExeFileNameWithoutDirs() ;
    //"Intel_Core_controller_log.txt"
    "";
  std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("CoreControllerDLL_log.txt") ;
  g_logger.OpenFileA( stdstrFilename ) ;
  DEBUGN("this Log file is open")
  DEBUGN("chars for module name needed:" //<< dwChars //<< ar_strModuleName
      << strExeFileNameWithoutDirs )
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
/** The array pointed to by the return value must be freed by the caller (i.e.
* x86I&C GUI or service) of this function. */
float * DLL_CALLING_CONVENTION
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
  return Intel::Pentium_M_and_Core1::GetAvailableMultipliers(
    p_wNumberOfArrayElements ) ;
}

EXPORT
/** @return The array pointed to by the return value must be freed by the
*     caller (i.e.x86I&C GUI or service) of this function. */
float *
  DLL_CALLING_CONVENTION
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
  return Intel::Core::GetAvailableVoltagesInVolt( * p_wNum ) ;
}

EXPORT BYTE DLL_CALLING_CONVENTION
  GetCurrentVoltageAndFrequency(
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  return Intel::Core::GetCurrentVoltageAndFrequency(
    p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , p_fMultiplier
    , p_fReferenceClockInMHz
    , wCoreID
    ) ;
//    return byRet ;
}

EXPORT float DLL_CALLING_CONVENTION
  GetTemperatureInCelsius ( WORD wCoreID
  )
{
  return //(float) byTempInDegCelsius ;
    Intel::CoreAndCore2::GetTemperatureInDegCelsius( wCoreID ) ;
}

EXPORT void DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
#ifdef INSERT_DEFAULT_P_STATES
  /** CPUaccess object inside the exe.*/
  I_CPUaccess * pi_cpuaccess
#else
  void * p_v
#endif //#ifdef INSERT_DEFAULT_P_STATES
  )
{
  Init() ;
#ifdef INSERT_DEFAULT_P_STATES
//  InsertDefaultPstates(pi_cpuaccess);
  Intel::Pentium_M_and_Core1::InsertDefaultVoltagePstates(pi_cpuaccess);
#endif
}

EXPORT BYTE DLL_CALLING_CONVENTION SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  return Intel::Core::SetCurrentVoltageAndMultiplier(
    fVoltageInVolt , fMultiplier, (BYTE) wCoreID ) ;
  return 1;
}
