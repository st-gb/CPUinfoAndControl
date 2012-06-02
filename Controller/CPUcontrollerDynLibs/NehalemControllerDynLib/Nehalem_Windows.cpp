/*
 * Windows.cpp
 *
 *  Created on: 17.05.2012
 *      Author: Stefan
 */

#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#include <windows.h> //for MessageBox(...)
#include <tchar.h> //_T()

//GetTimeStampCounterMultiplier(...)
#include <Controller/CPU-related/Intel/Nehalem/Nehalem.hpp>

//_WIN32: Built-in macro for MSVC, MinGW (also for 64 bit Windows)
#if defined(_DEBUG) && defined(_WIN32)
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/\
GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif

////defined in "/Windows/AssignPointersToExportedExeFunctions/
//// AssignPointersToExportedExeMSRfunctions.cpp
//extern ReadMSR_func_type g_pfnreadmsr ;
//extern WriteMSR_func_type g_pfn_write_msr ;
extern float gs_fTimeStampCounterMultiplier;

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
bool InitWindows()
{
#ifdef _TEST_VERSION
//  MessageBox(
//    NULL,
//    _T("InitWindows"),
//    _T("error"),
//    MB_OK
//    );
#endif
  #ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = GetStdString(
    ::GetExeFileNameWithoutDirs() ) ;
#ifdef WESTMERE
  std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("WestmereControllerDLL_log.txt") ;
#else
  std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("NehalemControllerDLL_log.txt") ;
#endif
//  g_loggerDynLib.OpenFile2( stdstrFilename ) ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN_LOGGER_NAME(//g_loggerDynLib
    g_logger , "Windows version--this Log file is open")
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
//  //The reference clock is needed for setting the current frequency. So it
//  //must be determined prior to any call of this function.
//  GetCurrentReferenceClock(
//    //12.0,
//    gs_fTimeStampCounterMultiplier ,
//    g_fReferenceClockInMHz ,
//    1000 ,
//    MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
//  DEBUGN("first calculated reference clock in MHz: "
//    << g_fReferenceClockInMHz )
  return TRUE ;
}
#endif //#ifdef _WIN32
