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
       *    => multiplier = (CpuFid + 10h) / (2^CpuDid) */
      return (float) (frequencyID + AMD_K10_CPU_FID_SUMMAND) / 
        /** 2^0=1<<0=1   2^1=1<<1=2  */
        (float) ( 1 << divisorID );
    }

  } 
}
