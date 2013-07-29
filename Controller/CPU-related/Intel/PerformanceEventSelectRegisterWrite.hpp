/*
 * PerformanceEventSelectRegisterWrite.hpp
 *
 *  Created on: Jul 28, 2013
 *      Author: Stefan
 */

#ifndef PERFORMANCEEVENTSELECTREGISTERWRITE_HPP_
#define PERFORMANCEEVENTSELECTREGISTERWRITE_HPP_

namespace Intel
{
  /** @brief works for Performance Monitoring since P6 family processors
   *  see Intel "Table B-11.  MSRs in the P6 Family Processors  (Contd.)"
   *  see http://en.wikipedia.org/wiki/P6_%28microarchitecture%29
   *  ( Pentium Pro, II, III, Pentium 4, Pentium M, Core 1,2, Core i).
   * see Intel "30.2.1 Architectural Performance Monitoring Version 1" */
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
}

#endif /* PERFORMANCEEVENTSELECTREGISTERWRITE_HPP_ */
