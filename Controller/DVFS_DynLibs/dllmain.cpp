/*
 * dllmain.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: Stefan
 */
#include <Controller/DynLibCPUcontroller.hpp>
#include <Controller/DynLibCPUcoreUsageGetter.hpp>

//EXPORT
//void
BYTE Init( const DynLibCPUcontroller * p_dynlibcpucontroller ,
  const DynLibCPUcoreUsageGettter * p_dynlibcpucoreusagegettter
//  WORD wNumLogicalCPUcores
  )
{
  p_dynlibcpucontroller

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
