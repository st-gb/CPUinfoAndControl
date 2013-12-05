#pragma once

  /** "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG"
   *  "MSRC001_00[6B:64] P-state [7:0] Registers" :
   *  "100 MHz * (CpuFid + 08h)) / (2^CpuDid)" */
  #define AMD_K11_CPU_FID_SUMMAND 8

  #define GET_MULTIPLIER_DID(frequencyID, divisorID) \
    ( (float) (frequencyID + AMD_K11_CPU_FID_SUMMAND) / (float) ( 1 <<
      divisorID ) )
  namespace AMD { namespace family17 {
     inline float GetMultiplier2(unsigned frequencyID, unsigned divisorID)
     {
       return (float) (frequencyID + AMD_K11_CPU_FID_SUMMAND) /
         (float) ( 1 << divisorID );
     }
  } }
  //multi = Frequency IDentifier + 8 / 2 ^ "Divisor IDentifier"
//  #ifdef MAX_MULTI_IS_FREQUENCY_ID_PLUS_8
  #ifdef MAX_MULTI_IS_MAIN_PLL_OP_FREQ_ID_MAX_PLUS_8
    #define GET_MULTIPLIER(frequencyID, divisorID) \
      /*( (float) (frequencyID + 8) / (float) ( 1 << divisorID ) ) */ \
      GET_MULTIPLIER_DID(frequencyID, divisorID )
    namespace AMD { namespace family17 {
      float GetMultiplier(unsigned frequencyID, unsigned divisorID)
      {
        return GetMultiplier2(frequencyID, divisorID);
      }
    } }
//    #define GET_MAX_MULTIPLIER(frequencyID) GET_MULTIPLIER( frequencyID + 8, 1)
    #define GET_FREQUENCY_ID(multiplier, max_multiplier) ( \
      (max_multiplier / multiplier) - 8
  #else //Maximum multiplier is "MainPLLOpFreqIDMax / 2"
    // "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
    // "CLKIN. The reference clock provided to the processor, nominally 200Mhz."
    // For ZM-82: 2200 TSC/ second, max_freqID=14 -> max multi = (14+8)/2=22/2=11
//    #define GET_MAX_MULTIPLIER(frequencyID) ( ( frequencyID + 8) / 2)
    #define GET_MULTIPLIER(frequencyID, divisorID) \
      /*( (float) (frequencyID + 8) / (float) ( 1 << (divisorID + 1) ) )*/ \
      GET_MULTIPLIER_DID(frequencyID, (divisorID + 1))
    namespace AMD { namespace family17 {
      float GetMultiplier(unsigned frequencyID, unsigned divisorID)
      {
        return GetMultiplier2(frequencyID, divisorID + 1);
      }
    } }
  #endif
  #define GET_MAX_MULTIPLIER(frequencyID) GET_MULTIPLIER( frequencyID, 0)
  
namespace AMD
{
  namespace family11h
  {
    inline float GetMaxMultiplier()
    {

    }
  }
}
