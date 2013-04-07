/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * UsageGetterAndControllerBase.hpp
 *
 *  Created on: 09.04.2010
 *      Author: Stefan
 */

#ifndef USAGEGETTERANDCONTROLLERBASE_HPP_
#define USAGEGETTERANDCONTROLLERBASE_HPP_

//#include <windef.h> // for BYTE
#ifdef _WINDOWS
#else
  #include <Windows_compatible_typedefs.h>
#endif

#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/I_CPUaccess.hpp>
#include <Controller/CPU-related/UsageGetterAndControllerBase.hpp>

//inheritance:
//  CPUcoreUsageGetterAndControllerBase (member: I_CPUaccess * mp_cpuaccess)
//    I_CPUcontroller (member:CPUcoreUsageGetterAndControllerBase::mp_cpuaccess)
//  Griffin::UsageGetterAndControllerBase
//
namespace Griffin
{
//Base class for Griffin usage getter and controller because both of them may
// use these functions.
class UsageGetterAndControllerBase
  : //for mp_cpuaccess
#ifndef COMPILE_AS_USAGE_GETTER
    public I_CPUcontroller
#else
    //If not defined as CPU core usage getter we don't want to have
    //dependancies to I_CPUcontroller.
    public CPUcoreUsageGetterAndControllerBase
#endif
{
public:
//  //hides CPUcoreUsageGetterAndControllerBase::mp_cpuaccess
//  I_CPUaccess * mp_cpuaccess ;

  UsageGetterAndControllerBase();
  virtual ~UsageGetterAndControllerBase();

  void AccuratelyStartPerformanceCounting(
   DWORD dwAffinityBitMask ,
   BYTE byPerformanceCounterNumber ,
   WORD wEventSelect
   //BYTE byCounterMask
   , bool bInvertCounterMask
   );
//  I_CPUaccess * GetCPUaccess()
//  {
//    return mp_cpuaccess ;
//  }
  BYTE GetNumberOfCPUCores() ;

  void PerformanceEventSelectRegisterWrite(
    DWORD dwAffinityBitMask ,
    BYTE byPerformanceEventSelectRegisterNumber ,
    WORD wEventSelect ,
    BYTE byCounterMask ,
    bool bInvertCounterMask ,
    bool bEnablePerformanceCounter,
    bool bEnableAPICinterrupt,
    bool bEdgeDetect,
    bool bOSmode,
    bool bUserMode,
    BYTE byEventQualification
  ) ;
  void PrepareForNextPerformanceCounting(
    DWORD dwAffinityBitMask
    , BYTE byPerformanceEventSelectRegisterNumber
    ) ;
  //There was linker error 2019 when it was declared as inline under MSVC 2005.
   //inline
   bool // TRUE: success, FALSE: failure
    ReadPerformanceEventCounterRegister(
       //DWORD dwIndex,   // MSR index
     BYTE byPerformanceEventCounterNumber ,
       //DWORD & dwLowmostBits,     // bit  0-31 ( register "EAX" )
       //DWORD & dwHighmostBits,   // bit 32-63 (register "EDX")
     ULONGLONG & r_ull ,
       DWORD_PTR dwAffinityMask  // Thread Affinity Mask
     ) ;
//   void SetCPUaccess(I_CPUaccess * p_cpu_access )
//   {
//     mp_cpuaccess = p_cpu_access ;
//   }
};

}

#endif /* USAGEGETTERANDCONTROLLERBASE_HPP_ */
