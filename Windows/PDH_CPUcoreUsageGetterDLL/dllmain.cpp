// dllmain.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.
#include "stdafx.h"

#include <stdlib.h>///EXIT_SUCCESS
//#include <Controller\I_CPUaccess.hpp>
//#include <Controller/ExportedExeFunctionsCPUaccess.hpp>
#include <iostream>///std::cout
//#include <Controller/exported_functions.h>
#include <OperatingSystem/Windows/GetCPUusage/PerformanceDataHelper/PDH_CPUcoreUsageGetter.hpp>
#include <OperatingSystem/Windows/NumberOfLogicalCPUcores/GetNumberOfLogicalCPUs.h>

#ifdef _DEBUG
//  #include <Controller/Logger/Logger.hpp>
#endif
#ifdef _DEBUG
  #include <Controller/character_string/stdtstr.hpp> //GetStdString()
  #include <OperatingSystem/Windows/Process/GetDLLfileName.hpp> //GetDLLfileName(...)
  #include <OperatingSystem/Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
//#include <Windows/NumberOfLogicalCPUcores/GetNumberOfLogicalCPUs.h>
//#include <windef.h> //for APIENTRY
#include <windows.h>  //for APIENTRY

#ifdef _DEBUG
//Logger g_logger ;
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#endif

#define COMPILE_WITH_I_CPUACCESS_DEFINITION
#ifdef COMPILE_WITH_I_CPUACCESS_DEFINITION
  #include <Controller/I_CPUaccess.hpp>
#else //only declaration
  class I_CPUaccess ;
#endif

I_CPUaccess * g_pi_cpuaccess ;
PDH_CPUcoreUsageGetter g_pdh_cpu_core_usage_getter ;
static HMODULE s_hModule;

BYTE InitCPUaccessAndGetter(WORD wNumLogicalCPUcores) ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	s_hModule = hModule;
	return TRUE;
}

extern "C" __declspec(dllexport) 
float GetCPUcoreUsage( WORD wCPUcoreID )
{
//  DEBUGN("GetCPUcoreUsage for core " << wCPUcoreID )
  float fCPUusage = g_pdh_cpu_core_usage_getter.
    GetPercentalUsageForCore( wCPUcoreID ) ;
  return //100 ;
    //(BYTE) ( fCPUusage * 100 ) ;
    fCPUusage ;
  return -1.0 ;
}

//extern "C" __declspec(dllexport)
//WORD GetNumberOfLogicalCPUcores( )
//{
//#ifdef DEBUG
//  //std::cout << "GetCPUcoreUsage\n" ;
//#endif
//  return Windows::GetNumberOfLogicalCPUcores() ;
//    //1 ;
//}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be 
//declared as extern "C" in C++ code, to prevent them from using C++ naming 
//conventions."
/** @return 0=success */
extern "C" __declspec(dllexport) 
//void
BYTE Init( //I_CPUcontroller * pi_cpu
  I_CPUaccess * pi_cpuaccess
//  WORD wNumLogicalCPUcores
  //BYTE by 
  )
{
#ifdef _DEBUG
  TCHAR fileName[MAX_PATH];
  std::tstring std_tstrDLLfileName = GetDLLfileName(s_hModule, fileName);
  std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
      ("PDH_CPUcoreUsageGetterDLL_log.txt") ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
//  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
#endif
#ifdef COMPILE_WITH_I_CPUACCESS_DEFINITION
  BYTE byNumCores = 0;
  if(pi_cpuaccess)
    byNumCores = pi_cpuaccess->GetNumberOfCPUCores() ;
  else
    byNumCores = Windows::GetNumberOfLogicalCPUcores();
  //This should ensure that no other program can use this DLL:
#ifdef _DEBUG
#endif
  DEBUGN("# CPU cores from I_CPUaccess:" << (WORD) byNumCores )
#else
  byNumCores = Windows::GetNumberOfLogicalCPUcores();
#endif
  if( ! byNumCores )
    return 1;
//  GetCounterTitlesAndIndexesInEnglishViaRegistry() ;

//  WORD wNumCPUs = GetNumberOfLogicalCPUcores() ;
//  DEBUGN("DLL::Init(...): number of CPUs:" << wNumCPUs )
//  if( ! wNumCPUs )
//    wNumCPUs = byNumCores;
  try
  {
    if( ! InitCPUaccessAndGetter(
      //TODO
      //wNumLogicalCPUcores
//      wNumCPUs
      byNumCores
      )
      )
      return 0 ;
  }
  catch ( ExceptionWithStdString & exc )
  {
    DEBUGN("caught a ExceptionWithStdString for InitCPUaccessAndGetter(...):"
      << exc.m_stdstr )
  }
//  catch( std::exception & exc)
//  {
//    DEBUGN("caught an exc for InitCPUaccessAndGetter(...)")
//    return 0 ;
//  }
  return 0;
}

BYTE InitCPUaccessAndGetter(WORD wNumLogicalCPUcores)
{
  BYTE byRet = 1 ;
  DEBUGN("InitCPUaccessAndGetter(...)05.06.2010 16:52")
#ifndef _MSC_VER //if compiled with MS compiler there is an import library for
  DEBUGN("InitCPUaccessAndGetter(...)--before InitPDH_DLLaccess()" )
    //PDH for load-time dynamic linking, so we do not need to do runtime
    //dynamic linking (assigning function pointers to PDH.DLL's functions)
  byRet = g_pdh_cpu_core_usage_getter.InitPDH_DLLaccess() ;
#endif
  if( byRet == EXIT_SUCCESS )
  {
    DEBUGN("InitCPUaccessAndGetter(...)--before SetNumberOfCPUcores()" )
    g_pdh_cpu_core_usage_getter.SetNumberOfCPUcores(wNumLogicalCPUcores ) ;
  }
  return byRet ;
}
