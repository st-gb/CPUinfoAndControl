/** File:   GetAvailableMultipliers.hpp
 * Author: s.gebauer
 * Created on 18. Juli 2017, 10:41 */

#ifndef GETAVAILABLEMULTIPLIERS_HPP
#define GETAVAILABLEMULTIPLIERS_HPP

namespace AMD
{
	namespace family11h
	{
    inline float * CreateMultiplierArray(
      const fastestUnsignedDataType byNumMultis,
      const float fMaxMultiplier,
      const fastestUnsignedDataType minCoreDivisorID,
      const fastestUnsignedDataType maxCoreDivisorID
      )
    {
      DEBUGN( /* FULL_FUNC_NAME << "--" */ "begin--# multis:" << (WORD) byNumMultis
        << "max multi:" << fMaxMultiplier)
      float * ar_f = new float[byNumMultis] ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "array address:" << ar_f)
      //If allocating the array on the heap succeeded.
      if( ar_f )
      {
        fastestUnsignedDataType byMultiplierIndex = 0 ;
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
        for( fastestUnsignedDataType byDivisorIDIndex = maxCoreDivisorID ; 
            byDivisorIDIndex > minCoreDivisorID; -- byDivisorIDIndex )
        {
          fDivisor = (float)
            //^= "2^byDivisorIDIndex"
            ( 1 << byDivisorIDIndex ) ;
          DEBUGN("fDivisor:" << fDivisor )
          for( float fMultiplierForDID0 = //byMaxMultiDiv2 ;
					/** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG : 
					 * "5:0 CpuFid: core frequency ID. Read-write. 
					 * Specifies the CPU frequency multiplier.
						• The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is 
					 * ((100 MHz * (CpuFid + 08h)) / (2^CpuDid)).
						• The frequency specified by (100 MHz * (CpuFid + 08h)) must always 
					 * be >50% of and <= 100% of the frequency specified by 
					 * F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
					 * 
					 * => for max. mulit of 22 CpuFid must be [4...14] -> multi [12...22]
						 */									
									
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
            fMultiplier = fMultiplierForDID0 /
              fDivisor ;
            DEBUGN( /*FULL_FUNC_NAME << "--" */ "adding multi " << fMultiplier)
            ar_f[ byMultiplierIndex ] = fMultiplier ;
    //       stdstrstream << fMulti << " " ;
            ++ byMultiplierIndex ;
          }
        }
        DEBUGN( /*FULL_FUNC_NAME << "--"*/ "adding max multi" << fMaxMultiplier )
        return ar_f ;
      }
      return NULL ;
    }
		
		inline float * GetAvailableMultipliers(WORD * p_wNumberOfArrayElements)
		{
//			float ar_f [] = AMD::fromK10::GetAvailableMultipliers(p_wNumberOfArrayElements);
		  //TODO CreateMultiplierArray()
							
			/** for AMD Turion X2 Ultra (family 11h): The max multi is the only multi that can 
			 * be set without a crash for Divisor ID "0", so add it. 
			 * 
			 * This may be due to what is stated here:
			 * Revision Guide for AMD Family 11h Processors 41788 Rev. 3.00 July 2008 :
				 "311 Certain Clock Divisors May Result in Unpredictable System Behavior" */
			ar_f[ * p_wNumberOfArrayElements - 1 ] = fMaxMultiplier;
			return ar_f;
		}
	}
}

#endif /* GETAVAILABLEMULTIPLIERS_HPP */
