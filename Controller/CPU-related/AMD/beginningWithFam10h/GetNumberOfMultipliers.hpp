/** File:   GetNumberOfMultipliers.hpp
 * Author: s.gebauer
 * Created on 18. Juli 2017, 15:28 */

#ifndef GETNUMBEROFMULTIPLIERS_HPP
#define GETNUMBEROFMULTIPLIERS_HPP

#include "GetMinAndMaxCoreDivisorID.hpp"


/** "31116 Rev 3.00 - September 07, 2007 AMD Family 10h Processor BKDG" 
 *  "MSRC001_00[68:64] P-State [4:0] Registers" page 325 :
 *  "CPU COF = 100 MHz * (CpuFid + 10h) / (2^CpuDid)" 
 *  
 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh 
 *   Processors" page 570
 *  "CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) / (2^MSRC001_00[6B:64][CpuDid])."
 *  */
#define AMD_K10_CPU_FID_SUMMAND 0x10

namespace AMD
{
	namespace fromK10
	{
		inline BYTE GetNumberOfNonRedundantMultipliers(float fMaxMultiplier)
		{
			
		}
		
		/** Applies to AMD family 10h (and 15h?) but _not_ 11h!*/
		inline BYTE GetNumberOfMultipliers(float fMaxMultiplier)
		{			
			 /* different Frequency IDs for a 1800 MHz P960:
			 *  there was a frequ ID 10, div ID 1 -> multiplier 13.0
			 *  
			 * 0,1,2  for Divisor ID 0 -> multipliers 16.0, 17.0, 18.0
			 * 0...36-10hex= 0...20  for Divisor ID 1 -> 
			 *   (max. multiplier-AMD_K10_CPU_FID_SUMMAND/2)*2+1=(18-8)*2=10*2+1=20+1
			 *   multipliers 8.0, 8.5 ... 17.5, 18.0
			 * 0...72-10hex= 0...56  for Divisor ID 2 -> multipliers 4.0, 4.25 ... 17.75
			 * 0...144-10hex= 0...128  for Divisor ID 3 -> multipliers 2.0, 2.125 ... 17.875
			 * => 5 * fMaxMultiplier - 10hex different multipliers */
//			const fastestUnsignedDataType numDifferentFrequencyIDsForDivisorID0 = 
//			  fMaxMultiplier - AMD_K10_CPU_FID_SUMMAND;
//			const fastestUnsignedDataType numDifferentMultipliers = 
//				numDifferentFrequencyIDsForDivisorID0 + 1
//			  + 2 * fMaxMultiplier - AMD_K10_CPU_FID_SUMMAND - numDifferentFrequencyIDsForDivisorID0;
			
			fastestUnsignedDataType divisor, numMultipliersForDivisor, 
				numDifferentMultipliers = 0;
			//TODO test/verify
			for(fastestSignedDataType currentDivisorID = 
				AMD_FAMILY10H_MAX_CORE_DIVISOR_ID; currentDivisorID >=
				AMD_FAMILY10H_MIN_CORE_DIVISOR_ID; currentDivisorID -- )
			{
				divisor = (1 << currentDivisorID);
				numMultipliersForDivisor = (fMaxMultiplier - 
					AMD_K10_CPU_FID_SUMMAND/divisor) * divisor + 1;
				numDifferentMultipliers += numMultipliersForDivisor; 
			}
//			numDifferentFrequencyIDsForDivisorID0 * divisorID * fMaxMultiplier + 1
			return /*5 * numDifferentFrequencyIDs*/ numDifferentMultipliers;
		}
	}
}

#endif /* GETNUMBEROFMULTIPLIERS_HPP */
