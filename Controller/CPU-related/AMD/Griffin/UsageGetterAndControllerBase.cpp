/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * UsageGetterAndControllerBase.cpp
 *
 *  Created on: 09.04.2010
 *      Author: Stefan
 */

#include <Controller/AMD/Griffin/UsageGetterAndControllerBase.hpp>
#include <Controller/AMD/Griffin/AMD_family17.h>
#include <Controller/I_CPUaccess.hpp>
#include <preprocessor_helper_macros.h> //BITMASK_FOR_LOWMOST_8BIT

namespace Griffin
{

UsageGetterAndControllerBase::UsageGetterAndControllerBase() {
  // TODO Auto-generated constructor stub

}

UsageGetterAndControllerBase::~UsageGetterAndControllerBase() {
  // TODO Auto-generated destructor stub
}

//this method is for this purpose:
//AMD BIOS and Kernel Dev Guide:
//"To accurately start counting with the write that enables the counter,
//disable the counter when changing the event and then enable the counter
//with a second MSR write."
void UsageGetterAndControllerBase::AccuratelyStartPerformanceCounting(
  DWORD dwAffinityBitMask ,
  BYTE byPerformanceCounterNumber ,
  WORD wEventSelect ,
  bool bInvertCounterMask
  )
{
  //Disables the performance counter for accurately start performance
  //counting.
  PrepareForNextPerformanceCounting(
    dwAffinityBitMask , byPerformanceCounterNumber) ;

  PerformanceEventSelectRegisterWrite(
    dwAffinityBitMask ,
    byPerformanceCounterNumber ,
    //wEventSelect
    //0x0C1 ,
    wEventSelect ,
//      0x076 ,
    //byCounterMask: 00h: The corresponding PERF_CTR[3:0] register is incremented by the number of events
    //occurring in a clock cycle. Maximum number of events in one cycle is 3.
    0,
    bInvertCounterMask ,
    //0,
    //bEnablePerformanceCounter
    1,
    //bEnableAPICinterrupt
    0,
    //bEdgeDetect
    0,
    //bOSmode
    //0
    true
    ,
    //bUserMode
    true
    ,
   //byEventQualification
    0
    ) ;
}

BYTE UsageGetterAndControllerBase::GetNumberOfCPUCores()
{
  BYTE byCoreNumber = 0 ;
  DWORD dwEAX;
  DWORD dwEBX;
  DWORD dwECX;
  DWORD dwEDX;
  DEBUG_COUTN("UsageGetterAndControllerBase::GetNumberOfCPUCores()");
  if( //CpuidEx(
    mp_cpuaccess->CpuidEx(
    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
    0x80000008,
    &dwEAX,
    &dwEBX,
    &dwECX,
    &dwEDX,
    1
      )
    )
  {
    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT )
      //"ECX 7:0 NC: number of physical cores - 1.
      //The number of cores in the processor is NC+1 (e.g., if
      //NC=0, then there is one core).
      //See also section 2.9.2 [Number of Cores and Core Number]."
      + 1 ;
    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
    DEBUG_COUTN("UsageGetterAndControllerBase::GetNumberOfCPUCores() "
      "Number of CPU cores according to CPUID: " << (WORD) byCoreNumber );
  }
  else
  {
    DEBUG_COUTN("UsageGetterAndControllerBase::GetNumberOfCPUCores() "
      "mp_cpuaccess->CpuidEx failed" );
  }
  return byCoreNumber ;
}

void UsageGetterAndControllerBase::PerformanceEventSelectRegisterWrite(
   DWORD dwAffinityBitMask ,
   //Griffin has 4 "Performance Event Select Register" from
   //  MSR 0xC001_0000_00 to MSRC001_00_03  for
   // 4 "Performance Event Counter Registers" from
   //  MSR 0xC001_0004 to 0xC001_0007
   //  that store the 48 bit counter value
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
 )
 {
   if( bInvertCounterMask == true )
     //When Inv = 1, the corresponding PERF_CTR[3:0] register is incremented by 1, if the
     //number of events occurring in a clock cycle is less than CntMask value.
     //Less than 1 = 0 -> so if Clocks not halted and 0 times "not halted": ->"halted"
     byCounterMask = 1 ;
   //see AMD Family 11h Processor BKDG, paragraph
   //"MSRC001_00[03:00] Performance Event Select Register (PERF_CTL[3:0])"
   //bits:
   //31:24 CntMask: counter mask. Read-write. Controls the number of events
       //counted per clock cycle.
     //00h The corresponding PERF_CTR[3:0] register is incremented by the number of events
       //occurring in a clock cycle. Maximum number of events in one cycle is 3.
     //01h-03h When Inv = 0, the corresponding PERF_CTR[3:0] register is incremented by 1, if the
       //number of events occurring in a clock cycle is greater than or equal to the CntMask value.
        //When Inv = 1, the corresponding PERF_CTR[3:0] register is incremented by 1, if the
        //number of events occurring in a clock cycle is less than CntMask value.
     //04h-FFh Reserved.
   //23 | Inv: invert counter mask. Read-write. See CntMask.
   DWORD dwLow = 0 |
     ( byCounterMask << 24 ) |
     ( bInvertCounterMask << 23 ) |
     ( bEnablePerformanceCounter << 22 ) |
     ( bEnableAPICinterrupt << 20 ) |
     ( bEdgeDetect << 18 ) |
     ( bOSmode << 17 ) |
     ( bUserMode << 16 ) |
     ( byEventQualification << 8 ) |
     ( wEventSelect & BITMASK_FOR_LOWMOST_8BIT )
     ;
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
#else
   mp_cpuaccess->WrmsrEx(
     PERF_CTL_0 + byPerformanceEventSelectRegisterNumber ,
     dwLow ,
     wEventSelect >> 8 ,
     //1=core 0
     //1
     dwAffinityBitMask
     ) ;
#endif //_EMULATE_TURION_X2_ULTRA_ZM82
 }

void UsageGetterAndControllerBase::PrepareForNextPerformanceCounting(
  DWORD dwAffinityBitMask
  , BYTE byPerformanceEventSelectRegisterNumber
  )
{
  PerformanceEventSelectRegisterWrite(
    dwAffinityBitMask ,
    //byPerformanceCounterNumber ,
    byPerformanceEventSelectRegisterNumber ,
    //wEventSelect ,
    0x0C1 ,
    //      0x076 ,
//      0x076 ,
    //byCounterMask: 00h: The corresponding PERF_CTR[3:0] register is incremented by the number of events
    //occurring in a clock cycle. Maximum number of events in one cycle is 3.
    //0,
    //"When Inv = 0, the corresponding PERF_CTR[3:0] register is
    //incremented by 1, if the number of events occurring in a clock
    //cycle is greater than or equal to the CntMask"
    1 ,
    //bInvertCounterMask
    0,
    //bEnablePerformanceCounter
    0,
    //bEnableAPICinterrupt
    0,
    //bEdgeDetect
    0,
    //bOSmode[...]1=Events are only counted when CPL=0.
    //(CPL=Current Privilege Level?) http://en.wikipedia.org/wiki/Current_privilege_level
    1,
    //bUserMode
    1,
   //byEventQualification
    0
    ) ;
}

//inline
  bool UsageGetterAndControllerBase::ReadPerformanceEventCounterRegister(
    BYTE byPerformanceEventCounterNumber ,
    ULONGLONG & r_ull ,
      DWORD_PTR dwAffinityMask  // Thread Affinity Mask
    )
  {
    DWORD dwLow , dwHigh ;
    bool bRet =
    //TODO better use ReadPMC? : AMD Family 11h Processor BKDG :
    //"The RDPMC instruction is not serializing, and it can be executed
    //out-of-order with respect to other instructions around it.
    //Even when bound by serializing instructions, the system environment at
    //the time the instruction is executed can cause events to be counted
    //before the counter value is loaded into EDX:EAX."
    RdmsrEx(
      PERFORMANCE_EVENT_COUNTER_0_REGISTER + byPerformanceEventCounterNumber ,
      //PERFORMANCE_EVENT_COUNTER_1_REGISTER ,
      dwLow,
      dwHigh,
      //1=core 0
      dwAffinityMask
      ) ;
    //RdpmcEx seemed to cause a blue screen (maybe because of wrong param values)
    //mp_cpuaccess->RdpmcEx(
    //  PERFORMANCE_EVENT_COUNTER_0_REGISTER + byPerformanceEventCounterNumber ,
    //  //PERFORMANCE_EVENT_COUNTER_1_REGISTER ,
    //  & dwLow,
    //  & dwHigh,
    //  //1=core 0
    //  dwAffinityMask
    //  ) ;
    r_ull = dwHigh ;
    r_ull <<= 32 ;
    r_ull |= dwLow ;
    return bRet ;
  }
} //end namespace
