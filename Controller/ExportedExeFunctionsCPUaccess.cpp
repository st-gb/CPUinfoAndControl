/*
 * ExportedExeFunctionsCPUaccess.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */
#include "ExportedExeFunctionsCPUaccess.hpp"
//#include <Controller/CPU-related/ReadTimeStampCounter.h>
//#include <Controller/CPU-related/AssignPointersToExportedExeMSRfunctions.hpp>
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
//#include <global.h>

  ExportedExeFunctionsCPUaccess::ExportedExeFunctionsCPUaccess() {}
  ExportedExeFunctionsCPUaccess::~ExportedExeFunctionsCPUaccess() {}

  bool ExportedExeFunctionsCPUaccess::AssignPointersToExportedExeFunctions()
  {
    AssignPointersToExportedExeMSRfunctions(m_pfn_read_msr,m_pfn_write_msr) ;
    return m_pfn_read_msr && m_pfn_write_msr ;
  }

//  BOOL ExportedExeFunctionsCPUaccess::CpuidEx(
//      DWORD dwIndex,
//      PDWORD p_dwEAX,
//      PDWORD p_dwEBX,
//      PDWORD p_dwECX,
//      PDWORD p_dwEDX,
//      DWORD_PTR affinityMask
//    )
//  {
//    //from http://www.cs.usfca.edu/~cruse/cs686s07/cpuid.cpp:
////    int reg_eax, reg_ebx, reg_ecx, reg_edx;
//    //TODO creates error code 988 when a release DLL created with this code was loaded
////#ifdef _DEBUG
////    asm(" movl %0, %%eax " :: "m" (dwIndex) );
////    DEBUGN("before executing \"cpuid\" instruction" )
////    asm(" cpuid ");
////    asm(" mov %%eax, %0 " : "=m" (*p_dwEAX) );
////    DEBUGN("after \"cpuid\": eax:" << *p_dwEAX )
////    asm(" mov %%ebx, %0 " : "=m" (*p_dwEBX) );
////    asm(" mov %%ecx, %0 " : "=m" (*p_dwECX) );
////    asm(" mov %%edx, %0 " : "=m" (*p_dwEDX) );
////    return TRUE ;
////#else
//    ////When using "*p_dwEAX" etc. directly inside the CPUID instruction
//    //// the stepping was "
//    //from http://www.ibm.com/developerworks/library/l-ia.html:
//    asm ("cpuid"
//          : "=a" (*p_dwEAX),
//            "=b" (*p_dwEBX),
//            "=c" (*p_dwECX),
//            "=d" (*p_dwEDX)
//          : "a" (dwIndex));
////    *p_dwEAX = reg_eax ;
//    DEBUGN("after \"cpuid\": eax:" << *p_dwEAX << "stepping:" << (*p_dwEAX & 0xF) )
//    return TRUE ;
////#endif
//  }

  BYTE ExportedExeFunctionsCPUaccess::GetNumberOfCPUCores()
  {
    return 1 ;
  }

  BOOL ExportedExeFunctionsCPUaccess::ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    )
  {
    return FALSE ;
  }

  inline BOOL ExportedExeFunctionsCPUaccess::RdpmcEx(
    DWORD index,    // MSR index
    PDWORD eax,     // bit  0-31
    PDWORD edx,     // bit 32-63
          //1bin =core 0; 10bin=2dec= core 1
    DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
    return FALSE ;
  }

//  BOOL ExportedExeFunctionsCPUaccess::ReadTSC(
//    DWORD & r_dwLowEAX ,
//    DWORD & r_dwHighEDX
//    )
//  {
//    ReadTimeStampCounter(r_dwLowEAX, r_dwHighEDX ) ;
//    return TRUE ;
//  }

  //inline
  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
  ExportedExeFunctionsCPUaccess::RdmsrEx(
      DWORD index,    // MSR index
      PDWORD eax,     // bit  0-31
      PDWORD edx,     // bit 32-63
            //1bin =core 0; 10bin=2dec= core 1
      DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
    return (*m_pfn_read_msr)
      (
      index,    // MSR index
      eax,     // bit  0-31
      edx,     // bit 32-63
            //1bin =core 0; 10bin=2dec= core 1
      affinityMask ) ;
  }

  void ExportedExeFunctionsCPUaccess::Sleep(WORD wMillis)
  {
    ::Sleep( wMillis ) ;
  }

  BOOL
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
  ExportedExeFunctionsCPUaccess::WritePciConfigDwordEx (
    DWORD pciAddress,
    DWORD regAddress,
    DWORD value)
  {
    return FALSE ;
  } ;

  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
  ExportedExeFunctionsCPUaccess::WrmsrEx(
      DWORD index,    // MSR index
      DWORD dwLow ,//eax,     // bit  0-31
      DWORD dwHigh, //edx,      // bit 32-63
      DWORD affinityMask  // Thread Affinity Mask
    )
  {
    return (*m_pfn_write_msr) (
      index,    // MSR index
      dwLow ,//eax,     // bit  0-31
      dwHigh, //edx,      // bit 32-63
      affinityMask
    ) ;
  }
