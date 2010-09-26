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
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>

//These variables should be defined in a source (.cpp file) to avoid multiple
// definitions if this header file is more than once.
extern DWORD g_dwLowmostBits ;
extern DWORD g_dwHighmostBits ;
//These variables may be defined in file "PentiumM.cpp" .
extern DWORD g_dwValue1 ;
extern float g_fReferenceClockInMHz ;

inline float GetVoltage(BYTE byVoltageID) ;
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
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
    GetBaseClockViaTSCdiffdivMuliplierIfNoFIDchange() ;
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
  //return 1 ;
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

#endif /* PENTIUMM_HPP_ */
