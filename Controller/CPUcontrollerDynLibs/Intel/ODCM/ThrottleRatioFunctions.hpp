/*
 * ThrottleRatioFunctions.hpp
 *
 *  Created on: 26.05.2013
 *      Author: Stefan
 */

#ifndef THROTTLERATIOFUNCTIONS_HPP_
#define THROTTLERATIOFUNCTIONS_HPP_

//GetODCMdutyCyclePercentage(...)
#include <Controller/CPU-related/Intel/Pentium4/ODCM.hpp>

EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  DYN_LIB_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetAvailableThrottleLevels(
  WORD wCoreID
  , WORD * p_wNumberOfArrayElements
  )
{
  float * ar_f = new float[NUMBER_OF_ODCM_VALUES] ;
  //If allocating the array on the heap succeeded.
  if( ar_f )
  {
    * p_wNumberOfArrayElements = NUMBER_OF_ODCM_VALUES ;
    AddODCMdutyCyclesAsMultipliers(ar_f, /*lowestMultiplier*/ 1.0f);
  }
  else
    * p_wNumberOfArrayElements = 0.0f;
  return ar_f;
}

EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float
  DYN_LIB_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetThrottleLevel(WORD wCoreID)
{
  float fThrottleLevel = /*1.0f -*/ GetODCMdutyCyclePercentage(wCoreID);
  return fThrottleLevel;
}

/** @param fThrottleLevel [0.0...1.0]:1.0:not throttled at all;0.25:75%throttled
 * @param coreID: unsigned-> same width as CPU arch->fast */
EXPORT BYTE DYN_LIB_CALLING_CONVENTION
  SetThrottleLevel(float fThrottleLevel, unsigned coreID)
{
  //"If the programmed duty cycle is not identical for all the logical
  //processors, the processor clock will modu-late to the highest duty cycle
  //programmed."
  if( g_IsHyperThreaded)
  {
    if( coreID % 2 == 0  )
      SetODCMdutyCycle(fThrottleLevel, coreID + 1);
    else
      SetODCMdutyCycle(fThrottleLevel, coreID - 1);
  }
  return SetODCMdutyCycle(fThrottleLevel, coreID);
}

#endif /* THROTTLERATIOFUNCTIONS_HPP_ */
