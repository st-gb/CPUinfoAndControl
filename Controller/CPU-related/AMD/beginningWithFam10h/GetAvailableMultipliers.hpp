/*
 * GetAvailableMultipliers.hpp
 *
 *  Created on: 21.04.2012
 *      Author: Stefan
 */

#ifndef GETAVAILABLEMULTIPLIERS_HPP_
#define GETAVAILABLEMULTIPLIERS_HPP_

//#define MAX_VALUE_FOR_FREQUENCY_ID 63
//#include "AMD_family17.h" //MAX_VALUE_FOR_FID
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)

//For divisor ID 0 other FIDs than max. FID cause a crash -> so don't use all
//FIDs for this divisor ID.
#define NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0 3
#define NUM_USABLE_DIVISOR_IDS 2 //multis 1/2max-1/4max,1/4max-1/8max
/** "41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG p.236":
* "5:0 CpuFid: core frequency ID." ->
*  highest value for 6 bits is 111111bin = 63dec */
#define MAX_VALUE_FOR_FID 63//=0x3F hex//=111111 bin; has bits 5:0 = 6 bits

#ifdef FAMILY11H
  #include <Controller/CPU-related/AMD/Griffin/GetMultiplier.hpp>
  using namespace AMD::family11h;
#else
  #include <Controller/CPU-related/AMD/beginningWithFam10h/GetMultiplier.hpp>
  using namespace AMD::fromK10;
#endif

extern float g_fMainPllOpFreqIdMax;

namespace AMD
{
  namespace fromK10
  {
    inline float * CreateMultiplierArray(
      BYTE byNumMultis,
    //  BYTE byMaxMultiDiv2
      const float fMaxMultiplier
      )
    {
      DEBUGN( /* FULL_FUNC_NAME << "--" */ "begin--# multis:" << (WORD) byNumMultis
        << "max multi:" << fMaxMultiplier)
      float * ar_f = new float[byNumMultis] ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "array address:" << ar_f)
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
    //    const float fHalfOfMaxMulti = fMaxMultiplier / 2.0f;
    //    const float fLowestMultiplieForDID0 = fMaxMultiplier - ;
        for( BYTE byDivisorIDIndex = 3 ; byDivisorIDIndex > 1
          ; -- byDivisorIDIndex )
        {
          fDivisor = (float)
            //^= "2^byDivisorIDIndex"
            ( 1 << byDivisorIDIndex ) ;
          DEBUGN("fDivisor:" << fDivisor )
          for( float fMultiplierForDID0 = //byMaxMultiDiv2 ;
            //If max multi is max freq in MHz / 200 MHz
            //-e.g. max. multi 11 for max. 2200 MHz: 11 + 1 = 12 ->
            // -lst multi for DivisorID 2 (fDivisor=4): 12/4=3
            // -last multiplier for DivisorID 2 (fDivisor=4): 22/4=5.5
            // last multiplier for DivisorID 3 (fDivisor=8): 22/8=2.75
            // -[3...5.5] -> steps in "0.5"
            //-e.g. max. multi 10.5 for max. 2100 MHz:10.5 + 1 = 11.5 ->
            // -lst multi for DivisorID 2 (fDivisor=4): 11.5/4=2.875
            // -last multiplier for DivisorID 2 (fDivisor=4): 21/4=5.25
            // -last multiplier for DivisorID 3 (fDivisor=8): 21/8=2.625
            // -[2.875...5.25] -> steps in "0.5"
            fMaxMultiplier + 1.0f; //Start with max multi because it is guaranteed to be
              //a natural number.
            fMultiplierForDID0 <= fDoubleOfMaxMulti
            //fMultiplierForDID0 > fHalfOfMaxMulti
            //< fMaxMultiplier
            ; ++ fMultiplierForDID0
            //-- fMultiplierForDID0
            )
          {
    //      8:6 CpuDid: core divisor ID. Read-write.
    //      Specifies the CPU frequency divisor; see CpuFid.
    //      0h=Divisor of 1   3h=Divisor of 8
    //      1h=Divisor of 2   4h - 7h=Reserved
    //      2h=Divisor of 4
            fMultiplier = fMultiplierForDID0 /
              fDivisor ;
            DEBUGN( /*FULL_FUNC_NAME << "--" */ "adding multi " << fMultiplier)
            ar_f[ byMultiplierIndex ] = fMultiplier ;
    //       stdstrstream << fMulti << " " ;
            ++ byMultiplierIndex ;
          }
        }
        DEBUGN( /*FULL_FUNC_NAME << "--"*/ "adding max multi" << fMaxMultiplier )
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

    //  //E.g. Max multi = 10.5 -> double: 21
    //  while( byMaxMultiplier <= fMaxMultiplier )
    //    ++ byMaxMultiplier ;

      BYTE byNumMultisPerDivisor = (BYTE)
        // e.g. 21-11+1 = 10+1=11
        //( fDoubleOfMaxMulti - byMaxMultiplier + 1 ) ;
        byMaxMultiplier;

      BYTE byNumMultis =
        //e.g. 22 - 12 + 1 = 11 multipliers (12,13,14,15,16,17,18,19,20,21,22)
        byNumMultisPerDivisor *
        //Number of divisor IDs excluding divisor ID "0".
    //    NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0
        NUM_USABLE_DIVISOR_IDS
        //For divisor ID 0 other FIDs than max. FID cause a crash.
        + 1 ;
      DEBUGN("byMaxMultiDiv2: " << (WORD) byMaxMultiplier
        << " byNumMultisPerDivisor: " << (WORD) byNumMultisPerDivisor
        << " byNumMultis: " << (WORD) byNumMultis )
      return byNumMultis;
    }

    inline float GetMaximumMultiplier(const float fMainPllOpFreqIdMax)
    {
      float fMaxMultiplier;
      /** 41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
      * " if MainPllOpFreqIdMax = 00h, then there is no frequency limit." */
      if( fMainPllOpFreqIdMax )
      {
        /** "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
        * "CLKIN. The reference clock provided to the processor, nominally 200Mhz." */
        fMaxMultiplier = GetMultiplier(fMainPllOpFreqIdMax, 0);
        DEBUGN( /*FULL_FUNC_NAME << "--"*/
          "max multi from MainPllOpFreqIdMax ("
          << fMainPllOpFreqIdMax << "):" << fMaxMultiplier )
      }
      else //MainPllOpFreqIdmax = 0 -> use multiplier for max value for FID
      {
        /** 41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
        * " if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
        * -> use the max possible number. */
        fMaxMultiplier = GetMultiplier(MAX_VALUE_FOR_FID, 0) ;
      }
      DEBUGN("MaxMultiplier:" << fMaxMultiplier)
      return fMaxMultiplier;
    }

    /** @param MainPllOpFreqIdMax may also be half multiplier (e.g. 11.5) */
    inline float * GetAvailableMultipliers(WORD * p_wNumberOfArrayElements,
      const float fMainPllOpFreqIdMax)
    {
      DEBUGN( /* FULL_FUNC_NAME << "--"*/ "begin")
      float fMaxMultiplier;
    //  static float fMaxMultiDiv2 ;
    //  static float fMaxFreqIDPlus8 ;

      //TODO should be MainPllOpFreqId from F3xD4 Clock Power/Timing Control 0 Register??
//      g_fMainPllOpFreqId = fMainPllOpFreqIdMax ;
      fMaxMultiplier = GetMaximumMultiplier(fMainPllOpFreqIdMax);

      BYTE byNumMultis = GetNumberOfMultipliers(fMaxMultiplier);

      float * ar_f = CreateMultiplierArray(
        byNumMultis,
    //    byMaxMultiDiv2
        fMaxMultiplier
        );
      if(ar_f)
        * p_wNumberOfArrayElements = byNumMultis ;
      else
        * p_wNumberOfArrayElements = 0 ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "returning " << ar_f)
      return ar_f;
    }
  }
}

#endif /* GETAVAILABLEMULTIPLIERS_HPP_ */
