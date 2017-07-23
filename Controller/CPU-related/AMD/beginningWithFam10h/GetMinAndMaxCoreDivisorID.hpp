/* 
 * File:   GetMinAndManDivisorID.hpp
 * Author: root
 *
 * Created on 18. Juli 2017, 15:03
 */

#ifndef GETMINANDMANDIVISORID_HPP
#define GETMINANDMANDIVISORID_HPP

/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG : 		
	"8:6 CpuDid: core divisor ID. Read-write. 
 * Specifies the CPU frequency divisor; see CpuFid.
	0h=Divisor of 1    3h=Divisor of 8
	1h=Divisor of 2	   4h=Divisor of 16
	2h=Divisor of 4		 5h - 7h=Reserved"   */
#define AMD_FAMILY10H_MIN_CORE_DIVISOR_ID 0
#define AMD_FAMILY10H_MAX_CORE_DIVISOR_ID 4

namespace AMD
{
	namespace fromK10
	{
    void GetMinAndMaxCoreDivisorID(
			fastestUnsignedDataType & minCoreDivisor, 
			fastestUnsignedDataType & maxCoreDivisor)
		{
	    /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG : 		
				8:6 CpuDid: core divisor ID. Read-write. 
			 * Specifies the CPU frequency divisor; see CpuFid.
				0h=Divisor of 1    3h=Divisor of 8
				1h=Divisor of 2	   4h=Divisor of 16
				2h=Divisor of 4		 5h - 7h=Reserved   */		
			minCoreDivisor = 0;
			maxCoreDivisor = 4;
		}
	}
}

#endif /* GETMINANDMANDIVISORID_HPP */

