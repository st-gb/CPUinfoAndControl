/** File:   GetNumberOfMultipliers.hpp
 * Author: s.gebauer
 * Created on 18. Juli 2017, 15:29  */

#ifndef GETNUMBEROFMULTIPLIERS_HPP
#define GETNUMBEROFMULTIPLIERS_HPP

#include <hardware/CPU/fastest_data_type.h> //typedef fastestUnsignedDataType

/** Regarding AMD familly 11h (Griffin/Turin X2 Ultra):
* For divisor ID 0 other FIDs than max. FID cause a crash -> so don't use all
* FIDs for this divisor ID. */
#define NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0 3
#define NUM_USABLE_DIVISOR_IDS 2 //multis 1/2max-1/4max,1/4max-1/8max

#define AMD_FAMILY11H_FREQUENCY_ID_SUMMAND 8

namespace AMD
{ 
	namespace family11h
	{
		/** The is specific to AMD family 11h due to a bug where for divisor ID 0
		 *  only the highest frequency ID ca be used. :
		 * 
		 * 1788 Rev. 3.00 July 2008 Revision Guide for AMD Family 11h Processors :
		 *  311 Certain Clock Divisors May Result in Unpredictable System Behavior :
		 *  "The processor may violate internal timing requirements at certain 
		 *   core clock divisors. This may occur if software modifies the CpuFid in 
		 *   the P-state registers (MSRC001_00[6B:64][5:0])" */
		inline fastestUnsignedDataType GetNumberOfMultipliers(float fMaxMultiplier)
		{
		//  static float fDoubleOfMaxMulti ;
		//  fDoubleOfMaxMulti = g_fMaxMultiplier * 2.0f;
			BYTE byMaxMultiplier = fMaxMultiplier;

		//  DEBUGN("fMainPllOpFreqIdDiv2:" << fMaxMultiDiv2)
		//  BYTE byMaxMultiDiv2 = (BYTE) fMaxMultiDiv2 ;

			/** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG :
			 *   MSRC001_00[6B:64] P-state [7:0] Registers :
			 *  "The frequency specified by (100 MHz * (CpuFid + 08h)) must always be
			*    >50% of and <= 100% of the frequency specified by
			*    F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]." 
			
			* e.g. 22 - 12 + 1 = 11 multipliers (12,13,14,15,16,17,18,19,20,21,22) */
			const float fHalfOfMaxMulti = fMaxMultiplier / 2.0f;
			fastestUnsignedDataType halfOfMaxMulti = fMaxMultiplier / 2.0f;
			if( halfOfMaxMulti < fHalfOfMaxMulti)
				halfOfMaxMulti ++;
			fastestUnsignedDataType minimumMultiplierForDivisorID0 = halfOfMaxMulti > 
				AMD_FAMILY11H_FREQUENCY_ID_SUMMAND ? halfOfMaxMulti : 
				AMD_FAMILY11H_FREQUENCY_ID_SUMMAND ; 
		//  //E.g. Max multi = 10.5 -> double: 21
		//  while( byMaxMultiplier <= fMaxMultiplier )
		//    ++ byMaxMultiplier ;

			fastestUnsignedDataType numMultisPerDivisor =
				// e.g. 21-11+1 = 10+1=11
				//( fDoubleOfMaxMulti - byMaxMultiplier + 1 ) ;
				//byMaxMultiplier;
			  fMaxMultiplier - minimumMultiplierForDivisorID0 + 1;

			fastestUnsignedDataType numMultis =
				byNumMultisPerDivisor *
				//Number of divisor IDs excluding divisor ID "0".
		//    NUM_DIVISOR_IDS_EXCLUDING_DIVISOR_ID_0
				NUM_USABLE_DIVISOR_IDS
				/** For family 11hex (AMD Griffin): 
				 *   For divisor ID 0 other FIDs than max. FID cause a crash. */
				+ 1 ;
			DEBUGN("byMaxMultiDiv2: " << (WORD) byMaxMultiplier
				<< " byNumMultisPerDivisor: " << numMultisPerDivisor
				<< " byNumMultis: " << numMultis )
			return numMultis;
		}
	}
}

#endif /* GETNUMBEROFMULTIPLIERS_HPP */
