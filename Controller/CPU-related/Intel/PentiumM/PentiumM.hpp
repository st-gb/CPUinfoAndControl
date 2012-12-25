/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//for CPU_TEMP_IS_BELOW_CRITICAL, CPU_TEMP_IS_CRITICAL
#include <Controller/CPU-related/CPU_core_temperature_defs.h>
//PERFORMANCE_COUNTER_VALUE_DIFF(...)
#include <Controller/CPU-related/PerformanceCounterValueDiff.h>
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
//Intel::Pentium_M::GetVoltage(...)
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_GetVoltage.hpp>
#include <winnt.h> //ULONGLONG
#include "PentiumM_registers.h" //MAXIMUM_PERFORMANCE_COUNTER_VALUE

//These variables should be defined in a source (.cpp file) to avoid multiple
// definitions if this header file is more than once.
extern uint32_t g_ui32LowmostBits ;
extern uint32_t g_ui32HighmostBits ;
//These variables may be defined in file "PentiumM.cpp" .
extern DWORD g_dwValue1 ;
extern float g_fReferenceClockInMHz ;
extern bool gs_b2ndTimeOrLaterReadTSCandFIDchange ;

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
//  #define PERFORMANCE_COUNTER_FOR_FID_CHANGE IA32_PMC0
  inline void SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent() ;
  #include "GetReferenceClockViaTSCdiffDivMultiIfNoFiDchange.hpp"
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE

//Forward declarations of functions.
//inline float GetVoltage(BYTE byVoltageID) ;
inline float GetVoltageID_PentiumM_asFloat(float fVoltage ) ;

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
  DEBUGN( FULL_FUNC_NAME << "--begin")
//  BYTE bySuccess = 0 ;
  if( ReadMSR(
      //Intel: 198H 408 IA32_PERF_STATUS
      IA32_PERF_STATUS
      , & g_ui32LowmostBits
      , & g_ui32HighmostBits
      , 1
      )
    )
  {
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
    static BYTE byFID;
    byFID = ( g_ui32LowmostBits >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
//    r_fMultiplier = byFID ;
    r_fMultiplier = (float) byFID;
//#ifdef _DEBUG
//    if( r_wFreqInMHz > 1800 )
//      r_wFreqInMHz = r_wFreqInMHz ;
//#endif
//    BYTE byVoltageID = ( g_ui32LowmostBits & 255 ) ;
    r_fVoltageInVolt = Intel::Pentium_M::GetVoltage( //byVoltageID
      ( g_ui32LowmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
#ifdef _DEBUG
//    byVoltageID = byVoltageID ;
#endif
//    bySuccess = 1 ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
    r_fReferenceClockInMHz = GetBaseClockViaTSCdiffdivMultiplierIfNoFIDchange(byFID) ;
#else
    r_fReferenceClockInMHz = g_fReferenceClockInMHz ;
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
    DEBUGN( FULL_FUNC_NAME << "--return 1")
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

inline void SelectClocksNotHaltedPerformanceCounterEvent_Pentium_M(
  //BYTE byCoreID
  ) ;

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
  static uint32_t dwEAX ;
  static uint32_t dwEDX ;
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
  return (dwEAX &
    //4dec = 100 bin
    //
    CRITICAL_TEMPERATURE_STATUS_BIT_MASK) ||
    (dwEAX & THERMAL_THRESHOLD_1_STATUS_BIT_MASK)
    ? //99.0
    CPU_TEMP_IS_CRITICAL
    : //30.0
    CPU_TEMP_IS_BELOW_CRITICAL
    ;
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
//    g_ui32LowmostBits,// bit  0-31 (register "EAX")
    ( (BYTE) fMultiplier << 8 ) |
      GetClosestVoltageID(fVoltageInVolt) ,
//      GetVoltageID_PentiumM(fVoltageInVolt) ,
    0,
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
}

#endif /* PENTIUMM_HPP_ */
