/*
 * GetMaxMultiIfGreaterCurrentPstateLimitMulti.hpp
 *
 *  Created on: 21.04.2012
 *      Author: Stefan
 */

#ifndef GETMAXMULTIIFGREATERCURRENTPSTATELIMITMULTI_HPP_
#define GETMAXMULTIIFGREATERCURRENTPSTATELIMITMULTI_HPP_

extern float g_fMainPllOpFreqIdMax ;

#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

//class I_CPUcontroller ;
//  #include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/CPU-related/GetInterpolatedVoltageFromFreq.hpp>
#include "AMD_family17.h" //GET_MULTIPLIER(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)

//  extern I_CPUcontroller * g_p_cpucontroller ;
extern std::set<VoltageAndFreq> * g_p_stdsetvoltageandfreqWanted ;
extern float g_fReferenceClockInMHz;

inline BYTE GetMaxMultiIfGreaterCurrentPstateLimitMulti(
  BYTE byCoreID , float & fVoltageInVolt , float & fMultiplier )
{
  static BYTE bySuccess = false ;
  static uint32_t ui32MSRlowmost;
  static uint32_t ui32MSRhighmost;

  //At my laptop the current limit was set to "0" on Alternate Current power,
  //"1" on Direct Current (battery) power.
  //If 100% CPU load and trying to set a frequency/ multiplier above
  // "max_multi" / "current p-state limit",
  //then the previous multiplierfreq persists. To avoid this, retrieve the current limit.
  bySuccess = ::ReadMSR(
    P_STATE_CURRENT_LIMIT_REGISTER,
    & ui32MSRlowmost,
    & ui32MSRhighmost,
    1 << byCoreID ) ;
  //TODO to use this variable we would have to get the "desired voltages"
  static BYTE byCurPstateLimit ;
  byCurPstateLimit = ui32MSRlowmost & BITMASK_FOR_LOWMOST_3BIT ;
  //TODO to use this variable we would have to get the "desired voltages"
  // for the performance state (from the executable) the limit applies to
  static float fMaxMultiplierAccordingCurrentLimit ;
  fMaxMultiplierAccordingCurrentLimit =
    //g_fMaxMultiplier
//    ( g_fMainPllOpFreqIdMax + 8 ) / ( 1 << byCurPstateLimit ) ;
    GET_MULTIPLIER(g_fMainPllOpFreqIdMax, byCurPstateLimit);
  DEBUGN("p-state current limit:" << //dwMSRlowmost
    (WORD) byCurPstateLimit
    << " -> current max. Frequency ID:" << fMaxMultiplierAccordingCurrentLimit )
  if( fMultiplier > fMaxMultiplierAccordingCurrentLimit )
  {
    //Use the max multiplier according to the limit because the multiplier
    //wouldn't be applied.
    fMultiplier = fMaxMultiplierAccordingCurrentLimit ;

    fVoltageInVolt = 0.0 ;
    //TODO because the voltage changed (is too high):
    //get voltage for new multiplier.
    bySuccess = //g_p_cpucontroller->
      GetInterpolatedVoltageFromFreq(
      (WORD) ( fMultiplier *
//      //Reference clock for AMD Griffin usually is 100 MHz.
//      100.0
      g_fReferenceClockInMHz
      ) ,
      fVoltageInVolt ,
      * g_p_stdsetvoltageandfreqWanted
      ) ;
    DEBUGN("multiplier > max. multplier according to current P state limit"
      "current pstate multi:" << fMultiplier
      << "voltage:" << fVoltageInVolt )
    return bySuccess ;
//    //This indirectly calls _this_ function.
//      SetFreqAndVoltageFromFreq
//      ( wMaxFreqAccordingCurrentLimit, byCoreID) ;
  }
#ifdef _DEBUG
  else
  {
//    float fVoltageInVoltTest ;
//    g_p_cpucontroller->GetInterpolatedVoltageFromFreq(
//      (WORD) ( fMultiplier * 100.0 ) ,
//      fVoltageInVoltTest ,
//      * g_p_stdsetvoltageandfreqWanted
//        ) ;
    DEBUGN("multiplier <= max. multplier according to current P state limit"
      "current pstate multi:" << fMultiplier
      //<< "interpolated voltage (in Volt) would be:" << fVoltageInVoltTest
      )
  }
#endif
//  else
  return bySuccess ;
}
#endif //#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

#endif /* GETMAXMULTIIFGREATERCURRENTPSTATELIMITMULTI_HPP_ */
