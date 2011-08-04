/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * Nehalem.hpp
 *
 *  Created on: Aug 14, 2010
 *      Author: Stefan
 */

#ifndef NEHALEM_HPP_
#define NEHALEM_HPP_

  #include <preprocessor_macros/bitmasks.h>

//  extern DWORD g_dwValue1, g_dwValue2 ;
//from http://stackoverflow.com/questions/911035/uint32-int16-and-the-like-are-they-standard-c:
#include <stdint.h>
  //Use uint32_t to ensure its 32 bit on either platform (32, 64bit)
  extern uint32_t g_dwValue1, g_ui32Value2 ;

  //Name Nehalem-specific functions "...Nehalem", also because more than 1
  //(e.g. Intel Core, Nehalem) CPU header file may be included, so 1 name like
  //"GetMaximumMultiplier()" in both files could be misleading.

  inline BOOL GetMaximumMultiplier_Nehalem(WORD wCoreID, BYTE & byMaxMulti)
  {
    static BOOL boolReturnValue = FALSE;
    boolReturnValue = ReadMSR(
      MSR_TURBO_RATIO_LIMIT ,
      & g_dwValue1,// bit  0-31 (register "EAX")
      & g_ui32Value2,
      1 << wCoreID //m_dwAffinityMask
      ) ;
    //Maximum  multiplier
    byMaxMulti = (BYTE) ( g_dwValue1 & 255 ) ;
    return boolReturnValue;
  }

  inline float GetTimeStampCounterMultiplier()
  {
    static uint32_t ui32Value = 0;
    //see Intel document "Intel(R) 64 and IA-32 Architectures Software
    //Developer's Manual, Volume 3B: System Programming Guide, Part 2"
    //"Order Number: 253669-032US" from "September 2009"
    // "Table B-5.  MSRs in Processors Based on Intel Microarchitecture
    //(Contd.)(Nehalem)" (page "B-74" / 690)
    if( ReadMSR(
      0xCE, //MSR_PLATFORM_INFO
      & g_dwValue1,
      & //g_ui32Value2
      ui32Value,
      1 //scope: package
      )
      )
      return
        // "15:8 Maximum Non-Turbo Ratio. (R/O)
        // The is the ratio of the frequency that invariant TSC runs at.
        // The invariant TSC frequency can be computed by multiplying this ratio
        // by 133.33 MHz."
        (g_dwValue1 >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
    return 0.0 ;
  }

  inline void PerformanceEventSelectRegisterWrite(
    DWORD dwAffMask ,
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
    WriteMSR(
      // MSR index
      IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber ,
      dwLow ,//eax,     // bit  0-31
      0 , //edx,      // bit 32-63
      // Thread Affinity Mask
      //1
      dwAffMask
      ) ;
  }

  inline WORD PrepareForNextPerformanceCountingNehalem(
    DWORD dwAffMask
    , BYTE byPerformanceEventSelectRegisterNumber
    )
  {
    BYTE byRet = 0 ;
    //Intel i7 mobile spec update AAP53. Workaround step 1:
    // "[...] disabling, saving valid events and clearing from the select
    // registers [...]"
    PerformanceEventSelectRegisterWrite(
      dwAffMask ,
      //Pentium M has 1 or 2 "Performance Event Select Register" from
      //  MSR ... to MSR ...  for
      // 1 or 2 "Performance Event Counter Registers" from
      //  ... to ...
      //  that store the 48 bit counter value
      byPerformanceEventSelectRegisterNumber ,
      0 , //byEventSelect , //8 bit
      0 , //byUnitMask , // 8 bit
      0 , //bUserMode,
      0 , //bOSmode,
      0 , //bEdgeDetect,
      0 , //bPINcontrol,
      0 , //bEnableAPICinterrupt,
      //Intel vol. 3B (document # 253659):
      //"When set, performance counting is
      //enabled in the corresponding performance-monitoring counter; when clear, the
      //corresponding counter is disabled. The event logic unit for a UMASK must be
      //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
      //IA32_PMCx."
      //bool bEnablePerformanceCounter,
      false , //-> disable PerforManceCounter
      0 ,//bInvertCounterMask ,
      0 //byCounterMask
      ) ;
    //Intel i7 mobile spec update AAP53. Workaround step 2:
    // "[...] programming three event values 0x4300D2, 0x4300B1 and 0x4300B5
    //  into the IA32_PERFEVTSELx MSRs [...]"
    WriteMSR(
      IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
      0x4300D2 ,//eax,     // bit  0-31
      0 , //edx,      // bit 32-63
      //1 << wCoreID   // Thread Affinity Mask
      dwAffMask
    ) ;
    WriteMSR(
      IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
      0x4300B1 ,//eax,      // bit  0-31
      0 , //edx,      // bit 32-63
      //1 << wCoreID   // Thread Affinity Mask
      dwAffMask
    ) ;
    WriteMSR(
      IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
      0x4300B5 ,//eax,      // bit  0-31
      0 , //edx,      // bit 32-63
      //1 << wCoreID   // Thread Affinity Mask
      dwAffMask
    ) ;
    return byRet ;
  }
#endif /* NEHALEM_HPP_ */
