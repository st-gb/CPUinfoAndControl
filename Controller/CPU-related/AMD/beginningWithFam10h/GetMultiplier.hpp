#pragma once

  /** "31116 Rev 3.00 - September 07, 2007 AMD Family 10h Processor BKDG" 
   *  "MSRC001_00[68:64] P-State [4:0] Registers" page 325 :
   *  "CPU COF = 100 MHz * (CpuFid + 10h) / (2^CpuDid)" 
   *  
   *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh 
   *   Processors" page 570
   *  "CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) / (2^MSRC001_00[6B:64][CpuDid])."
   *  */
  #define AMD_K10_CPU_FID_SUMMAND 10

namespace AMD
{ 
  namespace fromK10
  {
    /** @brief Should work for MD family 10h, 15h */
    inline float GetMultiplier(unsigned frequencyID, unsigned divisorID)
    {
      return (float) (frequencyID + AMD_K10_CPU_FID_SUMMAND) /
        (float) ( 1 << divisorID );
    }
  } 
}
