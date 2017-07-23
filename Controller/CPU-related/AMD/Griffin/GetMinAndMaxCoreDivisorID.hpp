/* 
 * File:   GetMinAndManDivisorID.hpp
 * Author: root
 *
 * Created on 18. Juli 2017, 15:03
 */

#ifndef GETMINANDMANDIVISORID_HPP
#define GETMINANDMANDIVISORID_HPP

namespace AMD
{
	namespace fromK10
	{
    void GetMinAndMaxCoreDivisorID(
			fastestUnsignedDataType & minCoreDivisor, 
			fastestUnsignedDataType & maxCoreDivisor)
		{
        /** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG : 
         * MSRC001_00[6B:64] P-state [7:0] Registers :
         * "8:6 CpuDid: core divisor ID. Read-write.
          Specifies the CPU frequency divisor; see CpuFid.
          0h=Divisor of 1   3h=Divisor of 8
          1h=Divisor of 2   4h - 7h=Reserved
          2h=Divisor of 4" 
          => only iterate from 3 to 0 */
			
		/** The is specific to AMD family 11h due to a bug where for divisor ID 0
		 *  only the highest frequency ID ca be used. :
		 * 
		 * 1788 Rev. 3.00 July 2008 Revision Guide for AMD Family 11h Processors :
		 *  311 Certain Clock Divisors May Result in Unpredictable System Behavior :
		 *  "The processor may violate internal timing requirements at certain 
		 *   core clock divisors. This may occur if software modifies the CpuFid in 
		 *   the P-state registers (MSRC001_00[6B:64][5:0])" 
		 * => so use 1 as minimal core divisor ID */			
			minCoreDivisor = 1;
			maxCoreDivisor = 3;
		}
	}
}

#endif /* GETMINANDMANDIVISORID_HPP */

