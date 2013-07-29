/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"

//#include <Controller\I_CPUaccess.hpp>
//#include <Controller/ExportedExeFunctionsCPUaccess.hpp>
#include <iostream> //for std::cout
#ifdef _DEBUG
  #include <Controller/Logger.hpp>
  #include <Controller/character_string/stdtstr.hpp> //GetStdString()
#endif
//Include for PentiumM.hpp: //ReadMSR(...), WriteMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
//For the inline functions used in this source file.
#include <Controller/CPU-related/Intel/PentiumM/PentiumM.hpp>
//INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED
#include <Controller/CPU-related/Intel/Intel_registers.h>
//for EXPORT, APIENTRY preprocessor macros
#include <Controller/CPUcontrollerDynLibs/function_specifiers.h>
#ifdef _DEBUG
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
//#include <Windows/GetNumberOfLogicalCPUs.h>
#include <windef.h> //for BOOL
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h>  //for APIENTRY
#endif

#ifdef _DEBUG
Logger g_logger ;
#include <global.h> //for DEBUGN(...)
#endif

extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;

#ifdef COMPILE_WITH_I_CPUACCESS_DEFINITION
  #include <Controller/I_CPUaccess.hpp>
#else //only declaration
  class I_CPUaccess ;
#endif

DWORD g_dwLowmostBits ;
DWORD g_dwHighmostBits ;

I_CPUaccess * g_pi_cpuaccess ;

BYTE InitCPUaccessAndGetter(WORD wNumLogicalCPUcores) ;

BOOL APIENTRY DllMain(
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
#endif //#ifdef _WIN32
    AssignPointersToExportedExeMSRfunctions(
      g_pfnreadmsr , g_pfn_write_msr ) ;
    SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M() ;
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
#endif //#ifdef _WIN32
//	g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
//	    GetModuleHandle(NULL),
//	    "ReadMSR");
	return TRUE;
}

EXPORT
float GetCPUcoreUsage( WORD wCPUcoreID )
{
//  DEBUGN("GetCPUcoreUsage for core " << wCPUcoreID )
  return GetPercentalUsageForCore_Pentium_M() ;
}

EXPORT
//void
BYTE Init( //I_CPUcontroller * pi_cpu
  I_CPUaccess * pi_cpuaccess
//  WORD wNumLogicalCPUcores
  //BYTE by 
  )
{
#ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
      ("PentiumM_CPUcoreUsageGetterDLL_log.txt") ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
//  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
#endif
#ifdef COMPILE_WITH_I_CPUACCESS_DEFINITION
  //This should ensure that no other program can use this DLL:
#ifdef _DEBUG
  BYTE byNumCores = pi_cpuaccess->GetNumberOfCPUCores() ;
#endif
  DEBUGN("# CPU cores from I_CPUaccess:" << (WORD) byNumCores )
#endif
//  g_pi_cpuaccess = pi_cpuaccess ;
/// WORD wNumCPUs = GetNumberOfLogicalCPUcores() ;
//  DEBUGN("DLL::Init(...): number of CPUs:" << wNumCPUs )
#ifndef _WIN32
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr , g_pfn_write_msr ) ;
  SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M() ;
#endif
  return 1 ;
}
