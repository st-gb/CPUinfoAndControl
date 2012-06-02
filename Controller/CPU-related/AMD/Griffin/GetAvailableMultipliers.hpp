/*
 * GetAvailableMultipliers.hpp
 *
 *  Created on: 21.04.2012
 *      Author: Stefan
 */

#ifndef GETAVAILABLEMULTIPLIERS_HPP_
#define GETAVAILABLEMULTIPLIERS_HPP_

//#define MAX_VALUE_FOR_FREQUENCY_ID 63
#include "AMD_family17.h" //MAX_VALUE_FOR_FID
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)

//For divisor ID 0 other FIDs than max. FID cause a crash -> so don't use all
//FIDs for this divisor ID.
#define NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0 3

inline float * CreateMultiplierArray(
  BYTE byNumMultis,
//  BYTE byMaxMultiDiv2
  float fMaxMultiplier
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--# multis:" << (WORD) byNumMultis
    << "max multi:" << fMaxMultiplier)
  float * ar_f = new float[byNumMultis] ;
  DEBUGN( FULL_FUNC_NAME << "--array address:" << ar_f)
  //If allocating the array on the heap succeeded.
  if( ar_f )
  {
    BYTE byMultiplierIndex = 0 ;
    float fDivisor ;
    float fMultiplier //= //The minimum multiplier for Griffin
      //(float) byMaxMultiDiv2
//      g_fMaxMultiplier / 8.0
      ;
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;

    static float fDoubleOfMaxMulti ;
    fDoubleOfMaxMulti = fMaxMultiplier * 2.0f;
    const float fHalfOfMaxMulti = fMaxMultiplier / 2.0f;
//    const float fLowestMultiplieForDID0 = fMaxMultiplier - ;
    for( BYTE byDivisorIDIndex = 3 ; byDivisorIDIndex > 1
      ; -- byDivisorIDIndex )
    {
      fDivisor = (float)
        //^= "2^byDivisorIDIndex"
        ( 1 << byDivisorIDIndex ) ;
      DEBUGN("fDivisor:" << fDivisor )
      for( float fMultiplierForDID0 = //byMaxMultiDiv2 ;
        fMaxMultiplier; //Start with max multi because it is guaranteed to be
          //a natural number.
        fMultiplierForDID0 <= fDoubleOfMaxMulti
        //< fMaxMultiplier
        ; ++ fMultiplierForDID0 )
      {
//      8:6 CpuDid: core divisor ID. Read-write.
//      Specifies the CPU frequency divisor; see CpuFid.
//      0h=Divisor of 1   3h=Divisor of 8
//      1h=Divisor of 2   4h - 7h=Reserved
//      2h=Divisor of 4
        fMultiplier = fMultiplierForDID0 /
          fDivisor ;
        DEBUGN( FULL_FUNC_NAME << "--adding multi " << fMultiplier )
        ar_f[ byMultiplierIndex ] = fMultiplier ;
//       stdstrstream << fMulti << " " ;
        ++ byMultiplierIndex ;
      }
    }
    DEBUGN( FULL_FUNC_NAME << "--adding max multi" << fMaxMultiplier )
    //The max multi is the only multi that can be set without a crash for
    //Divisor ID "0":
    ar_f[ byMultiplierIndex ] = fMaxMultiplier ;
    return ar_f ;
  }
  return NULL ;
}

inline BYTE GetNumberOfMultipliers(float fMaxMultiplier)
{
//  static float fDoubleOfMaxMulti ;
//  fDoubleOfMaxMulti = g_fMaxMultiplier * 2.0f;
  BYTE byMaxMultiplier = fMaxMultiplier;

//  DEBUGN("fMainPllOpFreqIdDiv2:" << fMaxMultiDiv2)
//  BYTE byMaxMultiDiv2 = (BYTE) fMaxMultiDiv2 ;

  //"The frequency specified by (100 MHz * (CpuFid + 08h)) must always be
  //>50% of and <= 100% of the frequency specified by
  //F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."

  //E.g. Max multi = 10.5 -> double: 21
  while( byMaxMultiplier <= fMaxMultiplier )
    ++ byMaxMultiplier ;

  BYTE byNumMultisPerDivisor = (BYTE)
    // e.g. 21-11+1 = 10+1=11
    //( fDoubleOfMaxMulti - byMaxMultiplier + 1 ) ;
    byMaxMultiplier;

  BYTE byNumMultis =
    //e.g. 22 - 12 + 1 = 11 multipliers (12,13,14,15,16,17,18,19,20,21,22)
    byNumMultisPerDivisor *
    //Number of divisor IDs excluding divisor ID "0".
    NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0
    //For divisor ID 0 other FIDs than max. FID cause a crash.
    + 1 ;
  DEBUGN("byMaxMultiDiv2: " << (WORD) byMaxMultiplier
    << " byNumMultisPerDivisor: " << (WORD) byNumMultisPerDivisor
    << " byNumMultis: " << (WORD) byNumMultis )
  return byNumMultis;
}

inline float * GetAvailableMultipliersAMDfamilyFh(WORD * p_wNumberOfArrayElements )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
//  static float fMaxMultiDiv2 ;
//  static float fMaxFreqIDPlus8 ;

  //TODO should be MainPllOpFreqId from F3xD4 Clock Power/Timing Control 0 Register
  g_fMainPllOpFreqId = g_fMainPllOpFreqIdMax ;

  //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
  //" if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
  if( g_fMainPllOpFreqIdMax )
  {
    // "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
    // "CLKIN. The reference clock provided to the processor, nominally 200Mhz."
    g_fMaxMultiplier = GET_MAX_MULTIPLIER(g_fMainPllOpFreqId);
    DEBUGN( FULL_FUNC_NAME << "--getting max multi from MainPllOpFreqIdMax ("
      << g_fMainPllOpFreqIdMax << ")" )
  }
  else
  {
    //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
    //" if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
    // -> use the max possible number.
    g_fMaxMultiplier = GET_MAX_MULTIPLIER(MAX_VALUE_FOR_FID) ;
  }
  DEBUGN("g_fMaxMultiplier:" << g_fMaxMultiplier)

  BYTE byNumMultis = GetNumberOfMultipliers(g_fMaxMultiplier);

  float * ar_f = CreateMultiplierArray(
    byNumMultis,
//    byMaxMultiDiv2
    g_fMaxMultiplier
    );
  if(ar_f)
    * p_wNumberOfArrayElements = byNumMultis ;
  else
    * p_wNumberOfArrayElements = 0 ;
  DEBUGN( FULL_FUNC_NAME << "--returning " << ar_f)
  return ar_f;
}

#endif /* GETAVAILABLEMULTIPLIERS_HPP_ */
