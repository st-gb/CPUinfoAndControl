/*
 * GetPercentalUsageForCore_Pentium_M.hpp
 *
 *  Created on: 07.09.2011
 *      Author: Stefan
 */

#ifndef GETPERCENTALUSAGEFORCORE_PENTIUM_M_HPP_
#define GETPERCENTALUSAGEFORCORE_PENTIUM_M_HPP_

inline float GetPercentalUsageForCore_Pentium_M(//BYTE byCoreID
  )
{
  //"static": variable is not created every time one stack (like global var.,
  //but visibility/ scope is local)
  static BYTE s_byAtMask2ndTimeCPUcoreMask = 0 ;
  static double dClocksNotHaltedDiffDivTCSdiff = -1.0 ;
  static ULONGLONG s_ullTimeStampCounterValue = 0 ;
  static ULONGLONG s_ullPerformanceCounterValue = 0 ;
  static ULONGLONG s_ullPreviousTimeStampCounterValue = 0 ;
  static ULONGLONG s_ullPreviousPerformanceCounterValue = 0 ;
  static ULONGLONG s_ullPerformanceCounterValueDiff = 0 ;
  static ULONGLONG s_ullTimeStampCounterValueDiff = 0 ;
  static uint32_t dwLow, dwHigh ;
  ReadMSR(
    IA32_TIME_STAMP_COUNTER,
    & g_ui32LowmostBits,// bit  0-31 (register "EAX")
    & g_ui32HighmostBits,
//    1 << byCoreID
    //Use fixed core 0, because Pentium M has just 1 core
    1
    ) ;

  s_ullTimeStampCounterValue = g_ui32HighmostBits ;
  s_ullTimeStampCounterValue <<= 32 ;
  s_ullTimeStampCounterValue |= g_ui32LowmostBits ;

  //mp_pentium_m_controller->ReadPerformanceEventCounterRegister(
  //  //2
  //  //1 +
  //  byCoreID //performance counter ID/ number
  //  , m_ullPerformanceEventCounter3 ,
  //  //m_dwAffinityMask
  //  1 << byCoreID
  //  ) ;

  ReadMSR(
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
    , 1 ) ;
  //It seems that only 40 bits of the PMC are used with Pentium Ms although also
  //higher bits are set.
  //with simply making a difference, I got "1099516786500" (~10^12) although it was a 100 ms
  //interval, so the max. diff. could be ~ 1800 M/ 10 = 180 M (180 * 10^6)
  dwHigh &= BITMASK_FOR_LOWMOST_8BIT ;
  s_ullPerformanceCounterValue = dwHigh ;
  s_ullPerformanceCounterValue <<= 32 ;
  s_ullPerformanceCounterValue |= dwLow ;
  //For the first time there are no previous values for difference .
  if( //m_bAtLeastSecondTime
//    ( m_dwAtMask2ndTimeCPUcoreMask >> byCoreID ) & 1
      s_byAtMask2ndTimeCPUcoreMask == 1
    )
  {
  #ifdef _DEBUG
    if( s_ullTimeStampCounterValue <
      m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
      )
    {
  //      //Breakpoint possibility
  //      int i = 0;
    }
    if( s_ullPerformanceCounterValue < m_ar_cnh_cpucore_ugpca[ byCoreID ].
      m_ullPreviousPerformanceEventCounter3
      )
    {
  //      //Breakpoint possibility
  //      int i = 0;
    }
  #endif //#ifdef _DEBUG
      //ULONGLONG ullTimeStampCounterValueDiff
    s_ullTimeStampCounterValueDiff  = //ull -
      //  m_ullPreviousTimeStampCounterValue;
      ULONGLONG_VALUE_DIFF( s_ullTimeStampCounterValue ,
  //      m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
        s_ullPreviousTimeStampCounterValue
        ) ;

    s_ullPerformanceCounterValueDiff =
      PERFORMANCE_COUNTER_VALUE_DIFF(
//      PerformanceCounterValueDiff(
        s_ullPerformanceCounterValue ,
  //      m_ar_cnh_cpucore_ugpca[ byCoreID ].
  //        m_ullPreviousPerformanceEventCounter3
        s_ullPreviousPerformanceCounterValue
      ) ;

    //double
    dClocksNotHaltedDiffDivTCSdiff =
      (double) s_ullPerformanceCounterValueDiff /
      (double) s_ullTimeStampCounterValueDiff ;
    #ifdef _DEBUG
    if( dClocksNotHaltedDiffDivTCSdiff > 1.1 ||
      dClocksNotHaltedDiffDivTCSdiff < 0.02 )
    {
    //    //Breakpoint possibility
    //    int i = 0 ;
    }
    #endif
    //return (float) dClocksNotHaltedDiffDivTCSdiff ;
  }
  else
    //m_bAtLeastSecondTime = true ;
//    m_dwAtMask2ndTimeCPUcoreMask |= ( 1 << byCoreID ) ;
    s_byAtMask2ndTimeCPUcoreMask = 1 ;

//  m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
  s_ullPreviousTimeStampCounterValue
    = s_ullTimeStampCounterValue ;
//  m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousPerformanceEventCounter3
  s_ullPreviousPerformanceCounterValue
    = s_ullPerformanceCounterValue ;

  //Workaround for inability to detect ACPI resume (from standy, hibernation)
  //e.g. by wxWidgets if not on Windows.
  #ifndef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  ReadMSR(
    // MSR index
    IA32_PERFEVTSEL0 ,
    & dwLow ,//eax,     // bit  0-31
    & dwHigh , //edx,     // bit 32-63
    1 // Thread Affinity Mask
    ) ;
  BYTE byPerfEvtSelect = dwLow & BITMASK_FOR_LOWMOST_8BIT ;
  //After an ACPI resume the performance event select it is set to 0.
  if( //dwLow & BITMASK_FOR_LOWMOST_8BIT
    byPerfEvtSelect !=
    INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED )
  {
    //TODO the performance counter value is reset to zero after standy/
    //hibernate? then the following assignment is needed for the next
    //difference to be correct.
    s_byAtMask2ndTimeCPUcoreMask = 0 ;
    SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M(//1
      ) ;
  }
  #endif //#ifndef _WIN32
  return (float) dClocksNotHaltedDiffDivTCSdiff ;
}

#endif /* GETPERCENTALUSAGEFORCORE_PENTIUM_M_HPP_ */
