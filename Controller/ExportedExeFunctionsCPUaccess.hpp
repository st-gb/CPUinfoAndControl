/*
 * ExportedExeFunctionsCPUaccess.hpp
 *
 *  Created on: 02.04.2010
 *      Author: Stefan
 */

#ifndef EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_
#define EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_

#include <windows.h> //for ::Sleep()

typedef BOOL (//WINAPI
  * ReadMSR_func_type)
   (
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
  ) ;
typedef BOOL (//WINAPI
  * WriteMSR_func_type)
   (
    DWORD dwIndex,    // MSR index
    DWORD dwEAX,      // bit  0-31
    DWORD dwEDX,      // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
  ) ;

//Purpose of this class: that CPU controller
//classes that can be used as built-into-exe-file
//can also be used in a DLL:
//Then the CPU access class is this class and thus it
// the controller within the DLL calls exported functions
//from the executable.
class ExportedExeFunctionsCPUaccess
  : public I_CPUaccess
{
    public:
    ExportedExeFunctionsCPUaccess() {}
    ~ExportedExeFunctionsCPUaccess() {}
    ReadMSR_func_type m_pfn_read_msr ;
    WriteMSR_func_type m_pfn_write_msr ;

  BOOL CpuidEx(
      DWORD dwIndex,
      PDWORD p_dwEAX,
      PDWORD p_dwEBX,
      PDWORD p_dwECX,
      PDWORD p_dwEDX,
      DWORD_PTR affinityMask
    ) { return FALSE ; }
  BYTE GetNumberOfCPUCores() { return 1 ; }
  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    RdmsrEx(
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
    inline BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    RdpmcEx(
      DWORD index,    // MSR index
      PDWORD eax,     // bit  0-31
      PDWORD edx,     // bit 32-63
            //1bin =core 0; 10bin=2dec= core 1
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) { return FALSE ; }
    BOOL ReadPciConfigDwordEx(
      DWORD dwPCIaddress
      , DWORD dwRegAddress
      , PDWORD p_dwValue
      ) { return FALSE ; }
    void Sleep(WORD wMillis)
    {
      ::Sleep( wMillis ) ;
    }
    BOOL
      //In g++ virtual methods can't be declared as stdcall
      //WINAPI
      WritePciConfigDwordEx (
      DWORD pciAddress,
      DWORD regAddress,
      DWORD value) { return FALSE ; }
    BOOL // TRUE: success, FALSE: failure
      //In g++ virtual methods can't be declared as stdcall
      //WINAPI
      WrmsrEx(
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
};

#endif /* EXPORTEDEXEFUNCTIONSCPUACCESS_HPP_ */
