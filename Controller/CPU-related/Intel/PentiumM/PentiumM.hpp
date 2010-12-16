/*
 * PentiumM.hpp
 *  For usage ( "inline" for no performance loss)
 *  by both I_CPUcontroller-derived class and by DLL code.
 *  Created on: Aug 3, 2010
 *      Author: Stefan
 */

#ifndef PENTIUMM_HPP_
#define PENTIUMM_HPP_

//BITMASK_FOR_LOWMOST_3BIT
#include <preprocessor_macros/bitmasks.h>
//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
//1 include path must match the path where the header file is in
//#include <inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
#include <preprocessor_macros/value_difference.h> //ULONGLONG_VALUE_DIFF
//PERFORMANCE_COUNTER_VALUE_DIFF(...)
#include <Controller/CPU-related/PerformanceCounterValueDiff.h>
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <winnt.h> //ULONGLONG
#include "PentiumM_registers.h" //MAXIMUM_PERFORMANCE_COUNTER_VALUE

//These variables should be defined in a source (.cpp file) to avoid multiple
// definitions if this header file is more than once.
extern DWORD g_dwLowmostBits ;
extern DWORD g_dwHighmostBits ;
//These variables may be defined in file "PentiumM.cpp" .
extern DWORD g_dwValue1 ;
extern float g_fReferenceClockInMHz ;
extern bool gs_b2ndTimeOrLaterReadTSCandFIDchange ;

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
  #define PERFORMANCE_COUNTER_FOR_FID_CHANGE IA32_PMC0
  inline void SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent() ;
  #include "GetReferenceClockViaTSCdiffDivMultiIfNoFiDchange.hpp"
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE

//Forward declarations of functions.
inline float GetVoltage(BYTE byVoltageID) ;
inline float GetVoltageID_PentiumM_asFloat(float fVoltage ) ;

//inline void GetBaseClockViaTSCdiffdivMuliplierIfNoFIDchange()
//{
//  //Workaround for unabilility to detect ACPI resume (from standy, hibernation)
//  //e.g. by wxWidgets if not on Windows.
//  #ifndef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  ReadMSR(
//    // MSR index
//    IA32_PERFEVTSEL0 ,
//    & g_dwLowmostBits ,//eax,     // bit  0-31
//    & g_dwHighmostBits , //edx,     // bit 32-63
//    1 // Thread Affinity Mask
//    ) ;
//  BYTE byPerfEvtSelect = g_dwLowmostBits & BITMASK_FOR_LOWMOST_8BIT ;
//  //After an ACPI resume the performance event select it is set to 0.
//  if( //dwLow & BITMASK_FOR_LOWMOST_8BIT
//    byPerfEvtSelect == EMON_EST_TRANS )
//  {
//    ReadMSR(
//      // MSR index
//      PERFORMANCE_COUNTER_FOR_FID_CHANGE ,
//      & g_dwLowmostBits ,//eax,     // bit  0-31
//      & g_dwHighmostBits , //edx,     // bit 32-63
//      1 // Thread Affinity Mask
//      ) ;
//  }
//  else
//  {
//    //TODO the performance counter value is reset to zero after standy/
//    //hibernate? then the following assignment is needed for the next
//    //difference to be correct.
//    gs_b2ndTimeOrLaterReadTSCandFIDchange = false ;
//    SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent(//1
//      ) ;
//  }
//  #endif //#ifndef _WIN32
//}

//Purpose of this function: converting from float to integer causes rounding
// errors:
// Analogy: for an AMD Griffin at 1.18 V the voltage ID in float is 29.999998
//  and would get 29 (because automatically rounded down 1.164 V) when
//  converting to an integer.
// For 1.164 voltage ID in float is 29.000004
inline BYTE GetClosestVoltageID(float fVoltageInVolt )
{
  //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
//  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  float fVoltageID = //fVoltageMinusLowestVoltage / 0.016 ;
    GetVoltageID_PentiumM_asFloat(fVoltageInVolt) ;
  DEBUGN("GetClosestVoltageID--fVoltageID" << fVoltageID )
  //ceil( fVoltageID );
  WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //Avoid g++ warning "warning: converting to `WORD' from `float'"
    (WORD) fVoltageID ;
  DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
  if( fVoltageID - (float) wVoltageID >=
      //e.g. if desired voltage is 7.014: 7.014 - 7 = 0.014; 0.014/0.016=0.875
      0.5 )
    ++ wVoltageID ;
  DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
  DEBUGN("GetClosestVoltageID--voltage in Volt" << fVoltageInVolt << "-> voltage ID:"
    << wVoltageID )
  return wVoltageID ;
}

inline BYTE GetCurrentVoltageAndFrequencyPentium_M(
  float & r_fVoltageInVolt
  , float & r_fMultiplier
  , float & r_fReferenceClockInMHz
  )
{
//  BYTE bySuccess = 0 ;
  if( ReadMSR(
      //Intel: 198H 408 IA32_PERF_STATUS
      IA32_PERF_STATUS
      , & g_dwLowmostBits
      , & g_dwHighmostBits
      , 1
      )
    )
  {
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
//    byFID = g_dwLowmostBits >> 8 ;
//    r_fMultiplier = byFID ;
    r_fMultiplier = ( g_dwLowmostBits >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
//#ifdef _DEBUG
//    if( r_wFreqInMHz > 1800 )
//      r_wFreqInMHz = r_wFreqInMHz ;
//#endif
//    BYTE byVoltageID = ( g_dwLowmostBits & 255 ) ;
    r_fVoltageInVolt = GetVoltage( //byVoltageID
      ( g_dwLowmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
#ifdef _DEBUG
//    byVoltageID = byVoltageID ;
#endif
//    bySuccess = 1 ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
    GetBaseClockViaTSCdiffdivMultiplierIfNoFIDchange(1) ;
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
    r_fReferenceClockInMHz = g_fReferenceClockInMHz ;
    return 1 ;
  }
#ifdef _DEBUG
  else
  {
//    bySuccess = 0 ;
    return 0 ;
  }
#endif
  return 0 ;
//  return bySuccess ;
}

//Pentium M has default voltages for min and max multiplier stored in MSRegister
inline BYTE GetDefaultPstates(
  float & fVoltageForLowestMulti,
  float & fLowestMulti,
  float & fVoltageForHighestMulti,
  float & fHighestMulti
  )
{
  if( ReadMSR(
    //MIN_AND_MAX_FID ,
    //According to the MSR walker of CrystalCPUID:
    //for Pentium M reg. addr. 0x198:
    //Bit 24-32 showed hex "0E" for a max. multipl. "14" for 1.86 133 MHz FSB.
    //Bit 24-32 showed hex "0C" for a max. multipl. "12" for 1.6 133 MHz FSB.
    IA32_PERF_STATUS ,
    & g_dwLowmostBits, // bits 0-31 (register "EAX")
    & g_dwHighmostBits,
    //m_dwAffinityMask
//    1 << wCoreID
    1
    )
  )
  {
    //CrystalCPUID's MSR walker:
    //00000198 : 06120C26 06000C26
    //           06 : min multi
    //             12: default voltage for min multi
    //               0C : max multi
    //                 26: default voltage for max multi
    fVoltageForHighestMulti = GetVoltage(
      ( g_dwHighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
    DEBUGN("fVoltageForHighestMulti: " << fVoltageForHighestMulti )

    g_dwHighmostBits >>= 8 ;
    fHighestMulti = ( g_dwHighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
    DEBUGN("fHighestMulti: " << fHighestMulti )

    g_dwHighmostBits >>= 8 ;
    fVoltageForLowestMulti = GetVoltage(
      ( g_dwHighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
    DEBUGN("fVoltageForLowestMulti: " << fVoltageForLowestMulti )

    g_dwHighmostBits >>= 8 ;
    fLowestMulti = ( g_dwHighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
    DEBUGN("fLowestMulti: " << fLowestMulti )
    return 1 ;
  }
  return 0 ;
}

inline void SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M(
  //BYTE byCoreID
  ) ;

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
  static DWORD dwLow, dwHigh ;
  ReadMSR(
    IA32_TIME_STAMP_COUNTER,
    & g_dwLowmostBits,// bit  0-31 (register "EAX")
    & g_dwHighmostBits,
//    1 << byCoreID
    //Use fixed core 0, because Pentium M has just 1 core
    1
    ) ;

  s_ullTimeStampCounterValue = g_dwHighmostBits ;
  s_ullTimeStampCounterValue <<= 32 ;
  s_ullTimeStampCounterValue |= g_dwLowmostBits ;

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

  //Workaround for unabilility to detect ACPI resume (from standy, hibernation)
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

inline void GetReferenceClockAccordingToStepping()
{
  DWORD dwEAX, dwEBX, dwECX , dwEDX ;
  bool bRet = CPUID(
    //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
    //"EAX=1: Processor Info and Feature Bits"
    0x00000001
    , & dwEAX
    , & dwEBX
    , & dwECX
    , & dwEDX
    , 1
    ) ;
  if( bRet )
  {
    //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
    //"The format of the information in EAX is as follows:"
    //"3:0 - Stepping"
    BYTE byStepping = dwEAX & BITMASK_FOR_LOWMOST_4BIT ;
    DEBUGN("stepping via CPUID:" << (WORD) byStepping )
    if( byStepping == 8) //e.g. 1.86 GHz
      g_fReferenceClockInMHz = 133.3 ;
    else //e.g. 1.8 GHz 745
      g_fReferenceClockInMHz = 100.0 ;
  }
}

inline float GetTemperatureInDegCelsiusPentiumM()
{
  static DWORD dwEAX ;
  static DWORD dwEDX ;
  ReadMSR(
    IA32_THERM_STATUS ,
    & dwEAX,     // bit  0-31
    & dwEDX,     // bit 32-63
    //1bin =core 0; 10bin=2dec= core 1
    1  // Thread Affinity Mask
    ) ;
  //See Intel Vol. 3A doc # 253668 14-19, chapter
  // "14.5.5.2 Reading the Digital Sensor" :
  //"Critical temperature conditions are detected
//  using the “Critical Temperature Status” bit. When this bit is set, the processor is
//  operating at a critical temperature and immediate shutdown of the system should
//  occur. Once the “Critical Temperature Status” bit is set, reliable operation is not guar-
//  anteed."

  //"Thermal Threshold #1 Status (bit 6, RO) — Indicates whether the actual
//  temperature is currently higher than or equal to the value set in Thermal
//  Threshold #1. If bit 6 = 0, the actual temperature is lower. If bit 6 = 1, the
//  actual temperature is greater than or equal to TT#1. Quantitative information of
//  actual temperature can be inferred from Digital Readout, bits 22:16.""

//  "Critical Temperature Status (bit 4, RO) — Indicates whether the critical
//  temperature detector output signal is currently active. If bit 4 = 1, the critical
//  temperature detector output signal is currently active."
  //bit 2 PROCHOT # or FORCEPR# event (RO)
  return dwEAX &
    //4dec = 100 bin
    //
    Critical_Temperature_Status_bit_mask |
    dwEAX & Thermal_Threshold_1_Status_bit_mask
    ? 99.0
    : 30.0 ;
}

inline float GetVoltage(BYTE byVoltageID)
{
  //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
  //Examined following values :
  //Voltage ID 0 = 0.7V 1=0.716 V,...40=1.340 Volt
  return 0.7 + 0.016 * byVoltageID ;
}

inline float GetVoltageID_PentiumM_asFloat(float fVoltageInVolt)
{
  //Pentium M datasheet: Table 3-1: 0.016 Volt difference = 1 Voltage ID step
  //Examined following values :
  //Voltage ID 0 = 0.7V 1=0.716 V,...27=1.132V,...,40=1.340 Volt
  DEBUGN("GetVoltageID_PentiumM--voltage in Volt:" << fVoltageInVolt
    << "-> voltage ID:" << (fVoltageInVolt - 0.7 ) / 0.016
    << "-> as integer:" << (WORD) ( (fVoltageInVolt - 0.7 ) / 0.016 )
    )
  return (fVoltageInVolt - 0.7 ) / 0.016 ;
}

inline BYTE GetVoltageID_PentiumM(float fVoltageInVolt )
{
  return (BYTE) GetVoltageID_PentiumM_asFloat( fVoltageInVolt) ;
}

inline void PerformanceEventSelectRegisterWrite_PentiumM(
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
    1 // Thread Affinity Mask
    ) ;
}

inline void SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M(
  //BYTE byCoreID
  )
{
  PerformanceEventSelectRegisterWrite_PentiumM(
//    1 << byCoreID ,
    1 ,
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
}

inline BYTE SetCurrentVoltageAndMultiplierPentiumM(
  float fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float fMultiplier
  , WORD wCoreID
  )
{
  //    std::stringstream ss ;
  //    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
  //        << dwLowmostBits ;
  //    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;

  //  MessageBox
  return WriteMSR (
    //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA   since: 06_0FH (Mobile)
    //  "63:33 Reserved"
    IA32_PERF_CTL,
//    g_dwLowmostBits,// bit  0-31 (register "EAX")
    ( (BYTE) fMultiplier << 8 ) |
      GetClosestVoltageID(fVoltageInVolt) ,
//      GetVoltageID_PentiumM(fVoltageInVolt) ,
    0,
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
}

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
inline void SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent()
{
  PerformanceEventSelectRegisterWrite_PentiumM(
    1 //<< byCoreID ,
    ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from
    //  MSR ... to MSR ...  for
    // 1 or 2 "Performance Event Counter Registers" from
    //  ... to ...
    //  that store the 48 bit counter value
    //Performance Event Counter number
//    1 ,
    0 , //for Pentium M: If no performance event counter 0 than counter 1
    // can't be used?!
    EMON_EST_TRANS ,
    //LAST_LEVEL_CACHE_MISSES_UMASK , // 8 bit unit mask
    ONLY_FREQUENCY_TRANSITIONS_MASK ,
    1, //User Mode
    1, //OS mode
    0, //edge
    0, //pin control
    0, //APIC
    1, //enable counters
    0 , //invert counter mask
    0 //counter mask
    ) ;
  WriteMSR( //IA32_PMC1
//    IA32_PMC0
    PERFORMANCE_COUNTER_FOR_FID_CHANGE
    //Set to 1 so that next time no more 0 so that this function is called again.
    , 1
    , 0
    , 1 ) ;
  //Cause to fetch 2 values for taking a diff for _each_ value type (TSC, time,
  //  FID changes)
  gs_b2ndTimeOrLaterReadTSCandFIDchange = false ;
}
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE

#endif /* PENTIUMM_HPP_ */
