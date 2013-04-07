/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * ExportedExeFunctionsCPUaccess.hpp
 *
 *  Created on: 02.04.2010
 *      Author: Stefan
 */

#ifndef EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_
#define EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_

//#include <windows.h> //for ::Sleep()
#include "ExportedExeFunctions.h" //ReadMSR_func_type, WriteMSR_func_type
#include <Controller/I_CPUaccess.hpp> //Base class I_CPUaccess

//Purpose of this class: that CPU controller
//classes that can be used as built-into-exe-file
//can also be used in a DLL:
//Then the CPU access class is this class and thus
// the controller within the DLL calls exported functions
//from the executable.
class ExportedExeFunctionsCPUaccess
  : public I_CPUaccess
{
  public:
    //inline
    bool AssignPointersToExportedExeFunctions() ;

    ExportedExeFunctionsCPUaccess() ; //{}
    ~ExportedExeFunctionsCPUaccess() ; //{}
    ReadMSR_func_type m_pfn_read_msr ;
    WriteMSR_func_type m_pfn_write_msr ;

//  BOOL CpuidEx(
//      DWORD dwIndex,
//      PDWORD p_dwEAX,
//      PDWORD p_dwEBX,
//      PDWORD p_dwECX,
//      PDWORD p_dwEDX,
//      DWORD_PTR affinityMask
//    ) ;

  BYTE GetNumberOfCPUCores() ; //{ return 1 ; }
//  void InitPerCPUcoreAccess( BYTE by ) { }
  //inline
  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    RdmsrEx(
      DWORD index,    // MSR index
      PDWORD eax,     // bit  0-31
      PDWORD edx,     // bit 32-63
            //1bin =core 0; 10bin=2dec= core 1
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  //inline
//  BOOL ReadTSC(DWORD & r_dw , DWORD & r_dw ) ;
  inline
    BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    RdpmcEx(
      DWORD index,    // MSR index
      PDWORD eax,     // bit  0-31
      PDWORD edx,     // bit 32-63
            //1bin =core 0; 10bin=2dec= core 1
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  BOOL ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    ) ;

  //TODO make inline
  void Sleep(WORD wMillis) ;

  BOOL
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    WritePciConfigDwordEx (
    DWORD pciAddress,
    DWORD regAddress,
    DWORD value) ;

  //inline
    BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    WrmsrEx(
      DWORD index,    // MSR index
      DWORD dwLow ,//eax,     // bit  0-31
      DWORD dwHigh, //edx,      // bit 32-63
      DWORD affinityMask  // Thread Affinity Mask
    ) ;
};

#endif /* EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_ */
