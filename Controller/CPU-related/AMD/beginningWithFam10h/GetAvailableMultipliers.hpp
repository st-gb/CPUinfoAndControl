/*
 * GetAvailableMultipliers.hpp
 *
 *  Created on: 21.04.2012
 *      Author: Stefan
 */

#ifndef GETAVAILABLEMULTIPLIERS_HPP_
#define GETAVAILABLEMULTIPLIERS_HPP_

#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)

/** "31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG" 
*    MSRC001_0070 COFVID Control Register 
*     "5:0 CpuFid: core frequency ID."
*  -> highest value for 6 bits is 111111bin = 63dec
*  41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG p.236":
*     "5:0 CpuFid: core frequency ID."
*  -> highest value for 6 bits is 111111bin = 63dec */
#define MAX_VALUE_FOR_FID 63//=0x3F hex//=111111 bin; has bits 5:0 = 6 bits
//#define MAX_VALUE_FOR_FREQUENCY_ID 63

#ifdef FAMILY11H
  #include <Controller/CPU-related/AMD/Griffin/GetMultiplier.hpp>
  #include <Controller/CPU-related/AMD/family11h/GetMinAndMaxCoreDivisorID.hpp>
  using namespace AMD::family11h;
#else
  #include <Controller/CPU-related/AMD/beginningWithFam10h/GetMultiplier.hpp>
  #include <Controller/CPU-related/AMD/beginningWithFam10h/GetMinAndMaxCoreDivisorID.hpp>
  #include <Controller/CPU-related/AMD/beginningWithFam10h/GetNumberOfMultipliers.hpp>
  using namespace AMD::fromK10;
#endif

extern float g_fMainPllOpFreqIdMax;

namespace AMD
{
  namespace fromK10
  {
    /** Applies to AMD family 10h, also to family 15h? */
    inline float * CreateMultiplierArray(
      fastestUnsignedDataType & numMultis,
      const float fMaxMultiplier,
      /** These 2 parameters are only necessary if also other CPU families like 
       *  11h are regarded.->Maybe delete these parameters. */
      const fastestUnsignedDataType minCoreDivisorID,
      const fastestUnsignedDataType maxCoreDivisorID
      )
    {
      DEBUGN( "begin--# multis:" << numMultis
        << "max multi:" << fMaxMultiplier)
			const fastestUnsignedDataType numDifferentMultipliersForDivisorID1 = 
        /** E.g. for a P960 max. multiplier is 9.0 resulting ca. 1800 MHz using
         * a 200 MHz reference clock. 
         * So multiply: 9.0*4=36  36-16=20 */
         (fMaxMultiplier * 4.0f - AMD_K10_CPU_FID_SUMMAND) + 1.0f;
      numMultis = numDifferentMultipliersForDivisorID1;
      float * ar_f = new float[numDifferentMultipliersForDivisorID1] ;
      DEBUGN( "array address:" << ar_f)
      /** If allocating the array on the heap succeeded. */
      if( ar_f )
      {
        fastestUnsignedDataType byMultiplierIndex = 0 ;
        float fMultiplier;
    //     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;

    //    const float fLowestMultiplieForDID0 = fMaxMultiplier - ;
//        for( fastestUnsignedDataType byDivisorIDIndex = maxCoreDivisorID ; 
//            byDivisorIDIndex > minCoreDivisorID; -- byDivisorIDIndex )
        {
//          fDivisor = (float)
//            //^= "2^byDivisorIDIndex"
//            ( 1 << 2 ) ;
//          DEBUGN("fDivisor:" << fDivisor )
					/** Adds mulipliers [AMD_K10_CPU_FID_SUMMAND/2...fMaxMultiplier]  */
          for( fastestUnsignedDataType frequencyIDforDivisorID1 = 0; 
            frequencyIDforDivisorID1 < numDifferentMultipliersForDivisorID1
            ; ++ frequencyIDforDivisorID1 )
          {
            fMultiplier = (float) (frequencyIDforDivisorID1 + 
              AMD_K10_CPU_FID_SUMMAND) / 4.0f ;
            DEBUGN( "adding multiplier " << fMultiplier)
            ar_f[ byMultiplierIndex ] = fMultiplier ;
    //       stdstrstream << fMulti << " " ;
            ++ byMultiplierIndex ;
          }
        }
        return ar_f ;
      }
      return NULL ;
    }

    inline float GetMaximumMultiplier(
			const fastestUnsignedDataType maxCPUcoreOperatingFrequency)
    {
      float fMaxMultiplier;
      /** 41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
      * " if MainPllOpFreqIdMax = 00h, then there is no frequency limit." */
      if( maxCPUcoreOperatingFrequency )
      {
        /** "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
        * "CLKIN. The reference clock provided to the processor, nominally 200Mhz." 
        *  */
        fMaxMultiplier = (float) maxCPUcoreOperatingFrequency / 2.0f; //GetMultiplier(fMainPllOpFreqIdMax, 0);
        DEBUGN( /*FULL_FUNC_NAME << "--"*/
          "max multi from MainPllOpFreqIdMax ("
          << maxCPUcoreOperatingFrequency << "):" << fMaxMultiplier )
      }
      else //MainPllOpFreqIdmax = 0 -> use multiplier for max value for FID
      {
        /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
				*     MSRC001_0071 COFVID Status Register :
				*      "if MaxCpuCof = 00h, then there is no frequency limit"
				*   41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
        *    "if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
				* 
        *  -> use the max possible number. */
        fMaxMultiplier = GetMultiplier(MAX_VALUE_FOR_FID, 0) ;
      }
      DEBUGN("MaxMultiplier:" << fMaxMultiplier)
      return fMaxMultiplier;
    }

    /** @param MainPllOpFreqIdMax may also be half multiplier (e.g. 11.5) */
    inline float * GetAvailableMultipliers(WORD * p_wNumberOfArrayElements,
      const fastestUnsignedDataType maxCPUcoreOperatingFrequency)
    {
      DEBUGN( /* FULL_FUNC_NAME << "--"*/ "begin")
      float fMaxMultiplier;
    //  static float fMaxMultiDiv2 ;
    //  static float fMaxFreqIDPlus8 ;

      //TODO should be MainPllOpFreqId from F3xD4 Clock Power/Timing Control 0 Register??
//      g_fMainPllOpFreqId = fMainPllOpFreqIdMax ;
      fMaxMultiplier = GetMaximumMultiplier(maxCPUcoreOperatingFrequency);
      g_fMaxMultiplier = fMaxMultiplier;

      fastestUnsignedDataType numMultis = GetNumberOfMultipliers(fMaxMultiplier);
      unsigned minCoreDivisor, maxCoreDivisor;
      GetMinAndMaxCoreDivisorID(minCoreDivisor, maxCoreDivisor);

      float * ar_f = CreateMultiplierArray(
        numMultis,
    //    byMaxMultiDiv2
        fMaxMultiplier,
        minCoreDivisor,
        maxCoreDivisor
        );
      if(ar_f)
        * p_wNumberOfArrayElements = numMultis ;
      else
        * p_wNumberOfArrayElements = 0 ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "returning " << ar_f)
      return ar_f;
    }

    /** @brief applies to AMD family 10h, 15h */
    inline float * GetAvailableMultipliers(WORD * p_wNumberOfArrayElements)
    {
      DEBUGN( "begin")
      //TODO should be MainPllOpFreqId from F3xD4 Clock Power/Timing Control 0 Register??
//      g_fMainPllOpFreqId = fMainPllOpFreqIdMax ;

      unsigned dwEAXlowMostBits, dwEDXhighMostBits;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1 );
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
           MSRC001_0071 COFVID Status Register 
          "54:49  MaxCpuCof: maximum CPU COF. Read-only. Specifies the maximum 
          CPU COF supported by the processor. 
			 *  The maximum frequency is 100 MHz * MaxCpuCof, if MaxCpuCof is greater 
			 *  than zero; if MaxCpuCof = 00h, then there is no frequency limit. Any attempt to change a CPU COF to a
		      frequency greater than specified by this field is ignored."
          COF=Core Operating Frequency? */
      const fastestUnsignedDataType maxCPUcoreOperatingFrequency = 
				(dwEDXhighMostBits >> 17) & BITMASK_FOR_LOWMOST_6BIT;
			return GetAvailableMultipliers(p_wNumberOfArrayElements,
				maxCPUcoreOperatingFrequency);
    }
  }
}

#endif /* GETAVAILABLEMULTIPLIERS_HPP_ */
