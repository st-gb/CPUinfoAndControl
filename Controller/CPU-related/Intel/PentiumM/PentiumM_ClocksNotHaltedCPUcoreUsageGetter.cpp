/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "PentiumM_ClocksNotHaltedCPUcoreUsageGetter.hpp"
//#include <ModelData/ModelData.hpp>

//#include <PentiumM_registers.h> //for MSR_TIME_STAMP_COUNTER_REGISTER
#include <Controller/CPU-related/Intel/Intel_registers.h> //for MSR_TIME_STAMP_COUNTER_REGISTER
//#include <Controller/GriffinController.hpp>
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_Controller.hpp>
#include <preprocessor_helper_macros.h>
#include <Windows_compatible_typedefs.h> //DWORD, ...
//#include <Controller/I_CPUcontroller.hpp> //ReadmsrEx

//TODO correct reatment of value overflow? bitwidth of value is important
PentiumM::ClocksNotHaltedCPUcoreUsageGetter::ClocksNotHaltedCPUcoreUsageGetter(
  //BYTE byCoreID ,
  DWORD dwAffinityMask
  , PentiumM_Controller * p_pentium_m_controller
  )
  : 
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_dwAtMask2ndTimeCPUcoreMask ( 0 )
  , mp_pentium_m_controller (p_pentium_m_controller)
  //, m_bAtLeastSecondTime (false)
{
  LOGN("CPU core usage ctor --address: " << this)
  BYTE byNumCPUs = //p_griffincontroller->mp_model->GetNumberOfCPUCores() ;
    //Pentium Ms usually only have 1 CPU core.
    1 ;
  m_ar_cnh_cpucore_ugpca = new 
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts[ byNumCPUs ];
//m_bAtLeastSecondTime
  m_dwAffinityMask = //1 << byCoreID ;
    dwAffinityMask ;
  //TODO the needed TimeStampCounter may be disabled.
  //Intel Intel� 64 and IA-32 Architectures sotfw. dev. manual vol. 3A (253668): 
  //  chapter 16.11: TSC is enabled if register "CR4".TSD[bit 2] = 1
  //http://www.cs.inf.ethz.ch/stricker/lab/doc/intel-part1.pdf:
  //"The time stamp disable (TSD) flag in register CR4 restricts the use 
  //of the RDTSC instruction.
  //When the TSD flag is clear, the RDTSC instruction can be executed at 
  //any privilege level; when the flag is set, the instruction can only be 
  //executed at privilege level 0. 
  //The time-stamp counter can also be read with the RDMSR instruction, 
  //when executing at privilege level 0."
  DWORD dwEAX ;
  DWORD dwEBX ;
  //DWORD dwECX ;
  //DWORD dwEDX ;
  mp_pentium_m_controller->CpuidEx(
    0x0A , //DWORD index,
    & dwEAX,
    & dwEBX,
    & dwEBX,
    & dwEBX,
    1 //DWORD_PTR affinityMask
    ) ;
  //TODO this calculated value is not correct at least for Pentium M 
  // (value = "24", but should be "2")
  m_byPerfCounterNumber = 
    //Intel..softw. dev. manual vol.3B:
    //chapter 30.2.2.2 Architectural Performance Monitoring Version 3 Facilities
    //"The number of general-purpose performance counters (IA32_PMCx) is
    //reported in CPUID.0AH:EAX[15:8]"
    ( dwEAX >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
  //TODO this calculated value is not correct at least for Pentium M
  //   (value = "70", but should be "40")
  m_byPerfCounterBitWidth = 
  //"the bit width of general-purpose
  //performance counters (see also Section 30.2.1.1) is reported in
  //CPUID.0AH:EAX[23:16]"
    ( dwEAX >> 16 ) & BITMASK_FOR_LOWMOST_8BIT ;
//  m_ullMaximumPerfCounterValue = 1 ;
//  m_ullMaximumPerfCounterValue
//    //example: 1bin << 1 = 10bin - 1 = 1bin
//    //example: 1bin << 2 = 100bin - 1 = 11bin
//    //example: 1bin << 4 = 10000bin - 1 = 1111bin
//    //example: 1bin << 59 =
//    //  1000 00000000 00000000 00000000 00000000 00000000 00000000 00000000bin
//    //   - 1
//    //=  111 11111111 11111111 11111111 11111111 11111111 11111111 11111111bin
//    = ( m_ullMaximumPerfCounterValue << m_byPerfCounterBitWidth ) - 1 ;
}

//The Performance event select is cleared after ACPI S3 or S4
//so rewrite it.
BYTE PentiumM::ClocksNotHaltedCPUcoreUsageGetter::Init(
  //DWORD dwCPUcoreAffinityBitMask 
  )
{
  LOGN("Init. CPU core usage")
  //m_dwAffinityMask = dwCPUcoreAffinityBitMask ;
  if( //get for load all cores 
    //dwCPUcoreAffinityBitMask 
    m_dwAffinityMask == 0 
    )
  {
    BYTE byNumCPUs = //mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
      //Pentium Ms usually only have 1 CPU core.
      1 ;
    for( BYTE byCoreID = 0 ; byCoreID < byNumCPUs ; ++ byCoreID )
    {
    //  mp_pentium_m_controller->AccuratelyStartPerformanceCounting( 
    //    1 << byCoreID ,
    //    //m_dwAffinityMask ,
    //    //2 
    //    //1 + 
    //    byCoreID //performance counter ID/ number
    //    , CPU_CLOCKS_NOT_HALTED
    //    , false //InvertCounterMask
    //    ) ;
    //The Performance event select is cleared after ACPI S3 or S4
    //so rewrite it.
      mp_pentium_m_controller->PerformanceEventSelectRegisterWrite(
        1 << byCoreID ,
        //Pentium M has 1 or 2 "Performance Event Select Register" from 
        //  MSR ... to MSR ...  for 
        // 1 or 2 "Performance Event Counter Registers" from 
        //  ... to ...
        //  that store the 48 bit counter value
        0 ,
        //CPU_CLOCKS_NOT_HALTED , //event select, 8 bit
        INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED ,
        0 , // 8 bit unit mask
        1, //User Mode
        1, //OS mode
        0, //edge
        0, //pin control
        0, //APIC
        1, //enable counters
        0 , //invert counter mask
        0 //counter mask
        ) ;
//      mp_pentium_m_controller->PerformanceEventSelectRegisterWrite(
//        1 << byCoreID ,
//        //Pentium M has 1 or 2 "Performance Event Select Register" from
//        //  MSR ... to MSR ...  for
//        // 1 or 2 "Performance Event Counter Registers" from
//        //  ... to ...
//        //  that store the 48 bit counter value
//        1 , //Performance Event Counter number
//        LAST_LEVEL_CACHE_MISSES_EVENT_SELECT ,
//        //LAST_LEVEL_CACHE_MISSES_UMASK , // 8 bit unit mask
//        LAST_LEVEL_CACHE_REFERENCES_UMASK ,
//        1, //User Mode
//        1, //OS mode
//        0, //edge
//        0, //pin control
//        0, //APIC
//        1, //enable counters
//        0 , //invert counter mask
//        0 //counter mask
//        ) ;
    }
  }
  //else
  //  mp_pentium_m_controller->AccuratelyStartPerformanceCounting( 
  //    //1 << byCoreID ,
  //    m_dwAffinityMask ,
  //    2, 
  //    CPU_CLOCKS_NOT_HALTED
  //    , false //InvertCounterMask
  //    ) ;
  return 0 ;
}

//double
float PentiumM::ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForCore(
  BYTE byCoreID
  )
{
  return GetPercentalUsageForCore_Pentium_M();
}

BYTE PentiumM::ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForAllCores(
  float arf[] 
  )
{
  BYTE byReturn = 1 ;
  BYTE byNumCPUs = //mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
    //mp_i_cpucontroller->mp_model->GetNumberOfCPUCores() ;
    //Pentium Ms usually only have 1 CPU core.
    1 ;
  for( BYTE byCoreID = 0 ; byCoreID < byNumCPUs ; ++ byCoreID )
  {
    arf[ byCoreID ] = GetPercentalUsageForCore( byCoreID ) ;
    if( arf[ byCoreID ] == 
      //-1.0 = error
      -1.0 
      )
	  {
        byReturn = 0 ;
	    //break ;
	  }
  }
  return byReturn ;
}
