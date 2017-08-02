#pragma once

/** "31116 Rev 3.00 - September 07, 2007 AMD Family 10h Processor BKDG" 
 *  "MSRC001_00[68:64] P-State [4:0] Registers" page 325 :
 *  "CPU COF = 100 MHz * (CpuFid + 10h) / (2^CpuDid)" 
 *  
 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh 
 *   Processors" page 570
 *  "CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) / (2^MSRC001_00[6B:64][CpuDid])."
 *  */
#define AMD_K10_CPU_FID_SUMMAND 0x10

/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
 *    MSRC001_0071 COFVID Status Register 
 *    "8:6 CurCpuDid: current core divisor ID. Read-only." 
 *  42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 
 *    00h-0Fh Processors :
 *    MSRC001_0071 COFVID Status :
 *    "8:6 CurCpuDid: current core divisor ID."  */
#define START_BIT_FOR_CPU_CORE_DIVISOR_ID 6
#define NUM_BITS_FOR_CPU_CORE_DIVISOR_ID 3

/** Does not apply to AMD family 11h! */
#define FREQUENCY_ID_SUMMAND AMD_K10_CPU_FID_SUMMAND

namespace AMD
{ 
  namespace fromK10
  {
    /** @brief Should work for AMD family 10h, 15h */
    inline float GetMultiplier(
        const fastestUnsignedDataType frequencyID,
        const fastestUnsignedDataType divisorID)
    {
      /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models
       *    "00h-0Fh Processors" :
       *    "CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) /
       *     (2^MSRC001_00[6B:64][CpuDid])" 

       *  31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG:
            page 428 : "MSRC001_00[68:64] P-State [4:0] Registers"
       *    "The CPU COF specified by CpuFid and CpuDid is: CPU COF = 
              100 MHz * (CpuFid + 10h) / (2^CpuDid)" 
       *    => multiplier = (CpuFid + 10h) / (2^CpuDid) 
       *  If sum of frequency ID + FID summand is 18 and divisor ID is 0 then 
       *  the multiplier is 18/2=9 because the reference clock is ca. 200 MHz. */
      return (float) (frequencyID + AMD_K10_CPU_FID_SUMMAND) / 
        /** 2^0=1<<0=1   2^1=1<<1=2  */
        (float) ( 1 << divisorID
        /** Double the divisor to get a multiplier.lowered by 2 */
        + 1);
    }

    inline void GetDivisorID(
      fastestUnsignedDataType MSRlowmost,
      fastestUnsignedDataType & divisorID)
    {
      divisorID = ( MSRlowmost >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) &
        BITMASK_FOR_LOWMOST_3BIT ;
    }

    inline fastestUnsignedDataType GetFrequencyID(const /*DWORD*/
      fastestUnsignedDataType MSRlowmost)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *    MSRC001_0071 COFVID Status Register 
			 *   "5:0 CurCpuFid: current core frequency ID. Read-only."  */
      return ( MSRlowmost & BITMASK_FOR_LOWMOST_6BIT ) ;
    }

    fastestUnsignedDataType GetMaxCPU_COF()
    {
      uint32_t lowmostBits, highmostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & lowmostBits,
        & highmostBits,
        1 );
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
			 *   MSRC001_0071 COFVID Status Register :
			 *   "54:49 MaxCpuCof" */
      return (highmostBits >> 17) // bits "54:49" ^= 6 bit
        & BITMASK_FOR_LOWMOST_6BIT;
    }
    
    /** @brief AMD composes the multiplier from 2 operands: divisor ID and
     *   frequency ID.
     *   multiplier=(freqID+summand)/2^divisorID */
    inline void GetFreqIDandDivisorIDfromMulti(
      float fMultiplier,
      fastestUnsignedDataType & r_frequencyID,
      fastestUnsignedDataType & r_divisorID
      )
    {
      DEBUGN(fMultiplier)

      float fFrequencyID = fMultiplier * 2.0f;
      r_divisorID = 0;
      /*** E.g. multiplier 8.75 -> freq ~= 1750,00 MHz, AMD P960 : 
       *    frequency ID = 8.75*4=35 , divisor ID = 2 */
      while (fFrequencyID - (fastestUnsignedDataType) fFrequencyID > 0.0001)
      {
        fFrequencyID *= 2.0f;
        r_divisorID++;
      }
      while (fFrequencyID < FREQUENCY_ID_SUMMAND) {
        fFrequencyID *= 2.0f;
        r_divisorID++;
      }
      r_frequencyID = fFrequencyID - FREQUENCY_ID_SUMMAND;

      DEBUGN(//"GetFreqIDandDivisorIDfromMulti(...)"
        << "CPU core Frequency ID:" << r_frequencyID
        << "CPU core Divisor ID:" << r_divisorID
        << "test (FID,DID)->multi: multi="
        << GetMultiplier(r_frequencyID, r_divisorID)
        )
    }
  } 
}
