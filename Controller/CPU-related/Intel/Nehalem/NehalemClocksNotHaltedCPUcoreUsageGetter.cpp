//#ifdef _MSC_VER //MicroSoft-compiler
//  #include "StdAfx.h"
//#endif

//Maybe see Intel_Volume 3A System Programming Guide--253668.pdf,
//chapter 14.2 P-STATE HARDWARE COORDINATION,
// IA32_MPERF MSR and IA32_APERF MSR
// ,Example 14-1.  Determine Target P-state From Hardware Coordinated Feedback

#include "NehalemClocksNotHaltedCPUcoreUsageGetter.hpp"
//#include <ModelData/ModelData.hpp>

//#include <PentiumM_registers.h> //for MSR_TIME_STAMP_COUNTER_REGISTER
#include <Controller/CPU-related/Intel/Intel_registers.h> //for MSR_TIME_STAMP_COUNTER_REGISTER
//#include <Controller/GriffinController.hpp>
//#include <Controller/Intel/Nehalem/NehalemController.hpp>
#include <Controller/I_CPUaccess.hpp>
#include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#include <preprocessor_helper_macros.h>
#include <Windows.h> //for GetTickCount: include Winbase.h (include Windows.h)
//#include <Controller/I_CPUcontroller.hpp> //ReadmsrEx

//TODO correct treatment of value overflow? bitwidth of value is important
Nehalem::ClocksNotHaltedCPUcoreUsageGetter::ClocksNotHaltedCPUcoreUsageGetter(
  //BYTE byCoreID ,
  DWORD dwAffinityMask
  //, NehalemController * p_nehalemcontroller
  , I_CPUaccess * p_i_cpuaccess
  , BYTE byNumLogicalCPUcores
  )
  //: 
  //mp_nehalem_controller (p_nehalemcontroller)
  //, m_bAtLeastSecondTime (false)
  //,
  //, mp_model( p_nehalemcontroller->mp_model )
{
  mp_cpuaccess = p_i_cpuaccess ;
  m_byNumberOfLogicalCPUcores = byNumLogicalCPUcores ;
  Init(dwAffinityMask, byNumLogicalCPUcores ) ;
}

//Used for static creation (e.g. as a global variable ): number if CPU cores 
//is not known at the moment of the creation.
Nehalem::ClocksNotHaltedCPUcoreUsageGetter::ClocksNotHaltedCPUcoreUsageGetter() 
  //Initially setting to 1 core is safe: this is the minimum.
  : m_byNumberOfLogicalCPUcores( 1)
  , m_ar_cnh_cpucore_ugpca ( NULL )
{ 
  //m_dwAffinityMask = 0 ;
  Init(0, 1 ) ;
}

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::AllocateCPUcoreAttributeArray()
{
  m_ar_cnh_cpucore_ugpca = new 
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts[ m_byNumberOfLogicalCPUcores ];
}

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::Init(
  DWORD dwAffinityMask
  , WORD wNumLogicalCPUcores
  )
{
  m_byNumberOfLogicalCPUcores = (BYTE) wNumLogicalCPUcores ;
  m_dwAtMask2ndTimeCPUcoreMask = 0 ;
  m_dReferenceClockInHz = 133000000.0 ;

//m_bAtLeastSecondTime
  m_dwAffinityMask = //1 << byCoreID ;
    dwAffinityMask ;
}

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::GetPerfCounterAttributes()
{
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
  //mp_nehalem_controller->CpuidEx(
  //mp_i_cpucontroller->CpuidEx(
  mp_cpuaccess->CpuidEx(
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
  m_ullMaximumPerfCounterValue = 1 ;
  m_ullMaximumPerfCounterValue 
    //example: 1bin << 1 = 10bin - 1 = 1bin
    //example: 1bin << 2 = 100bin - 1 = 11bin
    //example: 1bin << 4 = 10000bin - 1 = 1111bin
    //example: 1bin << 59 = 
    //  1000 00000000 00000000 00000000 00000000 00000000 00000000 00000000bin 
    //   - 1 
    //=  111 11111111 11111111 11111111 11111111 11111111 11111111 11111111bin
    = ( m_ullMaximumPerfCounterValue << m_byPerfCounterBitWidth ) - 1 ;

  //for Pentium Ms the perf counter bit width seems to be 40:
  //                                    <8 bit > <8 bit > <8 bit > <8 bit > <8 bit >
  //previous value: 5497519814387   100 11111111 11111101 10110111 00110110 11110011
  //current value:  4398069455221   100 00000000 00000001 01011110 00011001 01110101
  //#infdef _MSC_VER
  //suffix "ULL" is not needed by MS compiler, but by g++
  m_ullMaximumPerfCounterValue = 0xFFFFFFFFFFULL ; //10xF = 10x 4 bit =40 bit
  //#else
  //m_ullMaximumPerfCounterValue = 0xFFFFFFFFFF ; //10xF = 10x 4 bit =40 bit
  //#endif
}

BYTE Nehalem::ClocksNotHaltedCPUcoreUsageGetter::GetCurrentPstate(
  BYTE & r_byFreqID
  , BYTE & r_byVoltageID
  , BYTE byCoreID
  )
{
  DWORD dwHigh, dwLow ;
  if( //mp_i_cpucontroller->RdmsrEx(
    mp_cpuaccess->RdmsrEx(
      IA32_PERF_STATUS
      , & dwLow
      , & dwHigh
      , 1 << byCoreID
      )
    )
  {
    r_byVoltageID = dwLow >> 8 ;
    r_byFreqID = ( dwLow & 255 ) ;
  }
  return 1 ;
}

//The Performance event select is cleared after ACPI S3 or S4
//so rewrite it.
BYTE Nehalem::ClocksNotHaltedCPUcoreUsageGetter::Init(
  //DWORD dwCPUcoreAffinityBitMask 
  )
{
  //LOGN("Init. CPU core usage")
  //m_dwAffinityMask = dwCPUcoreAffinityBitMask ;
  if( //get for load all cores 
    //dwCPUcoreAffinityBitMask 
    m_dwAffinityMask == 0 
    )
  {
    BYTE byNumCPUs = //mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
      //Nehalems have 4 or 8 logical CPU cores; every logical core can 
      //process performance monitoring.
      //8 ;
      m_byNumberOfLogicalCPUcores ;
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
      //mp_nehalem_controller->PerformanceEventSelectRegisterWrite(
      PerformanceEventSelectRegisterWrite(
      //((NehalemController*)mp_i_cpucontroller)->PerformanceEventSelectRegisterWrite(
        1 << byCoreID ,
        //Pentium M has 1 or 2 "Performance Event Select Register" from 
        //  MSR ... to MSR ...  for 
        // 1 or 2 "Performance Event Counter Registers" from 
        //  ... to ...
        //  that store the 48 bit counter value
        0 ,
        //CPU_CLOCKS_NOT_HALTED , //event select, 8 bit
        UnHalted_Core_Cycles ,
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
      //Count Halted CPU cores: the clocks were not unhalted
      //mp_nehalem_controller->PerformanceEventSelectRegisterWrite(
      PerformanceEventSelectRegisterWrite(
        1 << byCoreID ,
        //Pentium M has 1 or 2 "Performance Event Select Register" from
        //  MSR ... to MSR ...  for
        // 1 or 2 "Performance Event Counter Registers" from
        //  ... to ...
        //  that store the 48 bit counter value
        1 ,
        //CPU_CLOCKS_NOT_HALTED , //event select, 8 bit
        UnHalted_Core_Cycles ,
        // 8 bit unit mask
        0 , 
        //1 ,
        1, //User Mode
        1, //OS mode
        0, //edge
        0, //pin control
        0, //APIC
        1, //enable counters
        //Inverting the unhated core cycles gives the halted core  cycles
        1 , //invert counter mask
        //counter mask
        //"Inverts the result of the counter-mask comparison 
        // when set, so that both greater than and less than comparisons can be made."
        //When Inv = 1, the corresponding PERF_CTR[3:0] register is incremented by 1, if the
        //number of events occurring in a clock cycle is less than CntMask value.
        //Less than 1 = 0 -> so if Clocks not halted and 0 times "not halted": ->"halted"
        1 
        //0
        ) ;
//      mp_nehalem_controller->PerformanceEventSelectRegisterWrite(
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

ULONGLONG Nehalem::ClocksNotHaltedCPUcoreUsageGetter::
  PerformanceCounterValueDiff(
  ULONGLONG ullPerformanceCounterCurrent ,
  ULONGLONG ullPerformanceCounterPrevious 
  )
{
  ULONGLONG ullDiff =
  /* the performance counter values are increasing in time except a value wrap
  occured */ \
  (ullPerformanceCounterCurrent) < (ullPerformanceCounterPrevious) ? 
  /* for understanding this calculation: 
    example: wrap at 255: current value: 2 old value : 250 
    -> correct value is "maximum value" minus "old value" + 
    "current value" + 1 (because first value is "0")
    = 255 - 250 + 2 + 1 = 5 + 2 + 1 = 8 
    example: wrap at 255: current value: 0 old value : 255: 255-255 + 0 + 1 = 1
  */ \
    (m_ullMaximumPerfCounterValue - (ullPerformanceCounterPrevious) ) + 
      (ullPerformanceCounterCurrent) + 1 
    : //else
    (ullPerformanceCounterCurrent) - (ullPerformanceCounterPrevious) ;
  return ullDiff ;
}

  //double 
float Nehalem::ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForCore(
  BYTE byCoreID
  )
{
	double dClocksNotHaltedDiffDivTCSdiff = -1.0 ;
  //mp_nehalem_controller->RdmsrEx(
  //mp_i_cpucontroller->RdmsrEx(
  mp_cpuaccess->RdmsrEx(
    IA32_TIME_STAMP_COUNTER,
    & m_dwLowmostBits,// bit  0-31 (register "EAX")
    & m_dwHighmostBits, 
    //m_dwAffinityMask
    1 << byCoreID
    ) ;
  //mp_nehalem_controller->RdmsrEx(
  //mp_i_cpucontroller->RdmsrEx(
  mp_cpuaccess->RdmsrEx(
    //IA32_PERFEVTSEL0
    //Intel vol. 3B:
    //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
    //address space; the number of MSRs per logical processor is reported using
    //CPUID.0AH:EAX[15:8]."
    //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
    //The bit width of an IA32_PMCx MSR is reported using the
    //CPUID.0AH:EAX[23:16]
    //
    IA32_PMC1
    , & m_dwLowmostBits
    , & m_dwHighmostBits
    , //1 
    1 << byCoreID
    ) ;
  m_ull = m_dwHighmostBits ;
  m_ull <<= 32 ;
  m_ull |= m_dwLowmostBits ;

	//mp_griffincontroller->ReadPerformanceEventCounterRegister(
  //mp_pentium_m_controller->ReadPerformanceEventCounterRegister(
  //  //2
  //  //1 + 
  //  byCoreID //performance counter ID/ number
  //  , m_ullPerformanceEventCounter3 ,
  //  //m_dwAffinityMask 
  //  1 << byCoreID
  //  ) ;
  DWORD dwLow, dwHigh ;

  //mp_nehalem_controller->RdmsrEx(
  //mp_i_cpucontroller->RdmsrEx(
  mp_cpuaccess->RdmsrEx(
    //IA32_PERFEVTSEL0
    //Intel vol. 3B:
    //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
    //address space; the number of MSRs per logical processor is reported using
    //CPUID.0AH:EAX[15:8]."
    //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
    //The bit width of an IA32_PMCx MSR is reported using the
    //CPUID.0AH:EAX[23:16]
    //
    IA32_PMC0
    , & dwLow
    , & dwHigh
    , //1 
    1 << byCoreID
    ) ;
  ////It seems that only 40 bits of the PMC are used with Pentium Ms although also
  ////higher bits are set.
  ////with simply making a difference, I got "1099516786500" (~10^12) although it was a 100 ms
  ////interval, so the max. diff. could be ~ 1800 M/ 10 = 180 M (180 * 10^6)
  //dwHigh &= BITMASK_FOR_LOWMOST_8BIT ;
  m_ullPerformanceEventCounter3 = dwHigh ;
  m_ullPerformanceEventCounter3 <<= 32 ;
  m_ullPerformanceEventCounter3 |= dwLow ;
      
  m_dwTickCount = ::GetTickCount();

	//For the first time there are no previous values for difference .
  if( //m_bAtLeastSecondTime 
	  (m_dwAtMask2ndTimeCPUcoreMask >> byCoreID ) & 1 )
  {

#ifdef _DEBUG
    if( m_ull < 
      m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue 
      )
    {
//      //Breakpoint possibility
//      int i = 0;
    }
    if( m_ullPerformanceEventCounter3 < m_ar_cnh_cpucore_ugpca[ byCoreID ].
      m_ullPreviousPerformanceEventCounter3 
      )
    {
//      //Breakpoint possibility
//      int i = 0;
    }
#endif //#ifdef _DEBUG
      //ULONGLONG ullTimeStampCounterValueDiff 
  	m_ullTimeStampCounterValueDiff  = //ull - 
      //  m_ullPreviousTimeStampCounterValue;
      ULONGLONG_VALUE_DIFF( m_ull , //m_ullPreviousTimeStampCounterValue
        m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
        ) ;

    //ULONGLONG ullPerformanceEventCounter3Diff = 
    m_ullPerformanceEventCounter3Diff = 
      //PERFORMANCE_COUNTER_VALUE_DIFF( m_ullPerformanceEventCounter3 , 
      PerformanceCounterValueDiff( 
        m_ullPerformanceEventCounter3 , 
        //m_ullPreviousPerformanceEventCounter3 
        m_ar_cnh_cpucore_ugpca[ byCoreID ].
          m_ullPreviousPerformanceEventCounter3
      ) ;

    BYTE byFreqID ;
    BYTE byVoltageID ;
    //mp_nehalem_controller->GetCurrentPstate(
    GetCurrentPstate(
      byFreqID //BYTE & r_byFreqID
      , byVoltageID //BYTE & r_byVoltageID
      , byCoreID //BYTE byCoreID
      ) ;
    m_dwTickCountDiff = 
      //http://msdn.microsoft.com/en-us/library/ms724408%28VS.85%29.aspx:
      //"Therefore, the time will wrap around to zero if the system is run 
      //continuously for 49.7 days."
      ULONG_VALUE_DIFF(m_dwTickCount, m_ar_cnh_cpucore_ugpca[ byCoreID ].
        m_dwTickCount ) ;
    float fFrequencyClocksPassed =  
      ( (double) byFreqID * m_dReferenceClockInHz ) * 
        ( (float) m_dwTickCountDiff / 1000.0 ) ;
    //double 
  	dClocksNotHaltedDiffDivTCSdiff =
      //clocks where the CPU was busy / clocks not halted.
      //(double) m_ullPerformanceEventCounter3Diff /
      //Clocks passed = clocks not halted + clocks halted.
      //(double) ( m_ullPerformanceEventCounter3Diff + m_ullTimeStampCounterValueDiff ) ;
      (double) m_ullPerformanceEventCounter3Diff /
        fFrequencyClocksPassed ;
  #ifdef _DEBUG
    //if( byCoreID == 0 )
    //TRACE("fFrequencyClocksPassed:            %f\n,
    //      "m_ullPerformanceEventCounter3Diff: %f\n"
    //      , fFrequencyClocksPassed, 
    //      (double) m_ullPerformanceEventCounter3Diff ) ;
  	if( dClocksNotHaltedDiffDivTCSdiff > 1.1 || 
      dClocksNotHaltedDiffDivTCSdiff < 0.02 )
  	{
//      //Breakpoint possibility
//  		int i = 0 ;
  	}
  #endif
    //return (float) dClocksNotHaltedDiffDivTCSdiff ;
  }
  else
    //m_bAtLeastSecondTime = true ;
	  m_dwAtMask2ndTimeCPUcoreMask |= ( 1 << byCoreID ) ;
  
  m_ar_cnh_cpucore_ugpca[ byCoreID ].m_dwTickCount = m_dwTickCount ;
    //m_ullPreviousTimeStampCounterValue 
  m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue 
    = m_ull ;
  //m_ullPreviousPerformanceEventCounter3 
  m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousPerformanceEventCounter3
    = m_ullPerformanceEventCounter3 ;

  //Workaround for unabilility to detect ACPI resume if not on Windows.
  #ifndef __WXMSW__
  //mp_nehalem_controller->
  mp_cpuaccess->RdmsrEx(
    // MSR index
    IA32_PERFEVTSEL0 ,
    & dwLow ,//eax,			// bit  0-31
    & dwHigh , //edx,			// bit 32-63
    1	// Thread Affinity Mask
      << byCoreID
    ) ;
  BYTE byPerfEvtSelect = dwLow & BITMASK_FOR_LOWMOST_8BIT ;
  //After an ACPI resume the performance event select it is set to 0.
  if( //dwLow & BITMASK_FOR_LOWMOST_8BIT
    byPerfEvtSelect !=
    UnHalted_Core_Cycles )
  {
    Init() ;
  }
  #endif
  
  return //-1.0 ;
	  (float) dClocksNotHaltedDiffDivTCSdiff ;
}

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::PerformanceEventSelectRegisterWrite(
  DWORD dwAffinityBitMask ,
  //Pentium M has 1 or 2 "Performance Event Select Register" from 
  //  MSR ... to MSR ...  for 
  // 1 or 2 "Performance Event Counter Registers" from 
  //  ... to ...
  //  that store the 48 bit counter value
  BYTE byPerformanceEventSelectRegisterNumber ,
  BYTE byEventSelect , //8 bit
  BYTE byUnitMask , // 8 bit
  bool bUserMode,
  bool bOSmode,
  bool bEdgeDetect,
  bool bPINcontrol,
  bool bEnableAPICinterrupt,
  //Intel vol. 3B (document # 253659):
  //"When set, performance counting is
  //enabled in the corresponding performance-monitoring counter; when clear, the
  //corresponding counter is disabled. The event logic unit for a UMASK must be
  //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
  //IA32_PMCx."
  bool bEnablePerformanceCounter,
  bool bInvertCounterMask ,
  BYTE byCounterMask
  )
{
  //DWORD dwLow = byCounterMask ;
  //dwLow <<= 24 ;
  //dwLow = 
  //dwLow <<= 24 ;
  DWORD dwLow = 0 |
    ( byCounterMask << 24 ) |
    ( bInvertCounterMask << 23 ) |
    ( bEnablePerformanceCounter << 22 ) |
    ( bEnableAPICinterrupt << 20 ) |
    ( bPINcontrol << 19 ) |
    ( bEdgeDetect << 18 ) |
    ( bOSmode << 17 ) |
    ( bUserMode << 16 ) |
    ( byUnitMask << 8 ) |
    ( byEventSelect )
    ;
  //Intel i7 700 spec update:
  // "AAP53. Performance Monitor Counters May Count Incorrectly"
  //-> before selecting the event, the values 
  //  -0x4300D2 (4391122 decimal)
  //  -0x4300B1 (4391089 decimal)
  //  -0x4300B5 (4391093 decimal)
  //  must be written into the Performance Monitor Select registers
  //mp_i_cpucontroller->WrmsrEx(
  mp_cpuaccess->WrmsrEx(
    // MSR index
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber ,
    dwLow ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    // Thread Affinity Mask
    //1	
    dwAffinityBitMask
    ) ;
}

BYTE Nehalem::ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForAllCores(
  float arf[] 
  )
{
  BYTE byReturn = 1 ;
  BYTE byNumCPUs = //mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
    //mp_i_cpucontroller->mp_model->GetNumberOfCPUCores() ;
    //Pentium Ms usually only have 1 CPU core.
    //1 ;
    //mp_model->m_cpucoredata.m_byNumberOfCPUCores ;
    m_byNumberOfLogicalCPUcores ;
    //8 ;
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

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::SetCPUaccess(
  I_CPUaccess * p_i_cpuaccess
  )
{
  mp_cpuaccess = p_i_cpuaccess ;
  //LOGN("CPU core usage ctor --address: " << this)
  //mp_i_cpucontroller = p_nehalemcontroller ;
  BYTE byNumCPUs = //p_griffincontroller->mp_model->GetNumberOfCPUCores() ;
    //Nehalems have 2 or 4 physical CPU core.
    //4 ;
    //8 ;
    //mp_model->m_cpucoredata.m_byNumberOfCPUCores ;
    //p_i_cpuaccess->mp_model->m_cpucoredata.m_byNumberOfCPUCores ;
    m_byNumberOfLogicalCPUcores ;
  //m_byNumverOfLogicalCPUcores = byNumCPUs ;
    //if( p_pentium_m_controller )
  //  mp_model = p_pentium_m_controller->mp_model ;
}

void Nehalem::ClocksNotHaltedCPUcoreUsageGetter::SetNumberOfLogicalCPUcores(
  BYTE byNum
  )
{
  m_byNumberOfLogicalCPUcores = byNum ;
}
