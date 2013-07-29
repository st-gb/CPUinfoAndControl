/*
 * Core2_GetConfigurableMultipliers.hpp
 *
 *  Created on: 29.05.2013
 *      Author: Stefan
 */

#ifndef CORE2_GETCONFIGURABLEMULTIPLIERS_HPP_
#define CORE2_GETCONFIGURABLEMULTIPLIERS_HPP_

#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_8BIT
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)

#ifdef _DEBUG
extern std::ofstream g_std_ofstream;
#endif

namespace Intel
{
  namespace Core2
  {
    //MSR 000000CE : Core 2 duo P8600
    //8   |0   |1   |7   |4   |9   |2   |A   | <- 4 bit value in hex
    //1000|    |0001|0111|0100|1001|0010|1010| <- 4 bit value in bin
    //          +-------+  |   +--+ +-------+       ++
    //          VID=23dec half|FID  VID=42dec
    //          =1.0V    multi|=9  | =1.2375V
    //                    +-------+
    //                    multi=9.5

    //4    3   |4   |5   |0   |6   |0   |D   <- abs min & max FID (06, 49=1001001=9,SLFM=3) & VID (17, 0D)
    //0100|0011|0100|0101|
    // |           0D=13=0.875V= min VID?
    //half   3
    //+-------+ +-------+
    //multi3.5?

    //Celeron 900:
    //0   0   2   5   0   B   2   5 00000625 <- absolute min & max FID (0B, 06) & VID (25,25) = 1.175V (16-31 = "4345" for P8600)
    //        ++++++  multi   VID
    //        VID=
    inline float GetMaxMultiFrom0xCE()
    {
      uint32_t lowmostBits, highmostBits;
      BYTE byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
        0xCE,
        & lowmostBits,// bits 0-31 (register "EAX")
        & highmostBits,
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( byValue1 ) //successfully read from MSR.
      {
        //highmostBits >> 8;
      }
      return 0.0f;
    }

    inline float GetMaxMultiFromPERF_STATUS()
    {
      uint32_t lowmostBits, highmostBits;
      //                        byte index: 7  6  5  4  3  2  1  0
      //example: "value at MSR address 408:06 23 73 42 06 00 73 42 "
      // 6: min. multiplier (if SuperLowFrequencyMode= min. Multi= 6/2=3)
      // 73: max. multiplier: 73dec = 1001001bin
      //                                 1001bin = 9dec
      //  -> max. Multi ~= 9 -> 9 or 9.5
      // 0xCE byte 4, bits 8-14: 49dec=110001bin 10001bin = 17dec
      BYTE byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
        IA32_PERF_STATUS,
        & lowmostBits,// bits 0-31 (register "EAX")
        & highmostBits,
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( byValue1 ) //successfully read from MSR.
      {
        /*BYTE*/ unsigned LFMmultiplier;
        LFMmultiplier = ( ( highmostBits >> (BITPOS_FOR_CURRENT_MAX_FID-32) )
          & BITMASK_FOR_LOWMOST_5BIT ) ;
        DEBUGN("LowestFrequencyMode (LFM) multiplier:" << (WORD) LFMmultiplier )
#ifdef _DEBUG
        BYTE currentMaxFID = ( ( highmostBits >> (BITPOS_FOR_CURRENT_MAX_FID-32) )
          & BITMASK_FOR_LOWMOST_5BIT );
        DEBUGN("current maximum Frequency ID:" << (WORD) currentMaxFID )
        BYTE currentMinFID = ( ( highmostBits >> (BITPOS_FOR_CURRENT_MIN_FID-32) )
          & BITMASK_FOR_LOWMOST_5BIT );
#endif
//        DEBUGN("current maximum Frequency ID:" << (WORD) currentMaxFID )
        float multiplier;
        //Max multiplier.
//        multiplier = GetMultiplierAsEncodedInMSR( highmostBits ) ;
      //LowestFrequencyMode (LFM) multiplier.
      //mobile_Core2_Duo_P8600: MSR index 0x198
  //  60 | 56 | 52 | 48 | 44 | 40 | 36 | 32 | 28 | 24 | 20 | 16 | 12 | 8   |4   |0  <- bit index
  // 0   |6   |1   | 7  |0   |9   |2   |0   0   6   0   0|   8|6|0   |D  <- hex value for 4 bit
  //          0   |    |              |     6   |13
  // 0000|0110|0001|0111|0000|1001|0010|0000                 |1000|
  //    +----+ +-------+ || +----+ +-------+
  //   CurrMin CurrMin   | \  Curr   CurrMax                  CurrMaxVID
  //      FID  VID      /   \ MaxFID    VID
  //                   /     \  "
  //    Super Low Freq Mode| half multi

  //Celeron 900 (max multi=11) at 2.2 GHz Core 2 solo, no speed step:
  //00000198 : 0B250B25 06000B25
  //  60 | 56 | 52 | 48 | 44 | 40 | 36 | 32 | 28 | 24 | 20 | 16 | 12 | 8   |4   |0  <- bit index
  // 0   |B   |2   | 5  |0   |B   |2   |5  0600|8|6|0   |D
  //          0   |    |              |     6   |13
  // 0000|1011|0010|0111|0000|1011|0010|0000                 |1000|
  //    +----+ +-------+ || +----+ +-------+
  //   CurrMin CurrMin   | \  Curr   CurrMax                  CurrMaxVID
  //      FID  VID      /   \ MaxFID    VID
  //                   /     \  "
  //    Super Low Freq Mode| half multi
        multiplier = GetMultiplier(highmostBits >> 8 );
        DEBUGN("Max multiplier:" << multiplier )
        return multiplier;
      }
      return 0.0f;
    }

    inline float GetMinMultiFromPERF_STATUS()
    {
      uint32_t lowmostBits, highmostBits;
      //                        byte index: 7  6  5  4  3  2  1  0
      //example: "value at MSR address 408:06 23 73 42 06 00 73 42 "
      // 6: min. multiplier (if SuperLowFrequencyMode= min. Multi= 6/2=3)
      // 73: max. multiplier: 73dec = 1001001bin
      //                                 1001bin = 9dec
      //  -> max. Multi ~= 9 -> 9 or 9.5
      // 0xCE byte 4, bits 8-14: 49dec=110001bin 10001bin = 17dec
      BYTE byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
        IA32_PERF_STATUS,
        & lowmostBits,// bits 0-31 (register "EAX")
        & highmostBits,
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( byValue1 ) //successfully read from MSR.
      {
        BYTE FIDbyte = highmostBits >> 24;
        float multi = GetMultiplier(FIDbyte /*BITPOS_FOR_MIN_FID*/ );
        DEBUGN( FULL_FUNC_NAME << " multi for FID byte " << (WORD) FIDbyte
          << "=" << multi)
        return multi;
      }
      return 0.0f;
    }

    inline float GetMaxMulti()
    {
      //return GetMaxMultiFrom0xCE();
      return GetMaxMultiFromPERF_STATUS();
    }

    inline float GetMinMulti()
    {
      return GetMinMultiFromPERF_STATUS();
    }

    inline bool EISTenabled()
    {
      uint32_t lowmostBits, highmostBits;
      BYTE byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
        IA32_MISC_ENABLE,
        & lowmostBits,// bits 0-31 (register "EAX")
        & highmostBits,
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( byValue1 ) //successfully read from MSR.
      {
        return highmostBits & 1;
      }
      return false;
    }

    /** @return Use "unsigned" data type beause it has the same bit size as CPU arch
     *   --> fastest datatype
     *  @param LFMmultiplier : lowest LowFrequencyMode /
     *       non-SuperLowFrequencyMode multiplier */
    unsigned GetNumMultis(const float /*minMulti*/ lowestLowFreqModemultiplier,
      float maxMulti)
    {
      unsigned numMultis;
      //float lowestMultiplier;
      //ex. # multi if LowFrequencyMode (LFM=6-> SuperLowFrequencyMode=3), max.
      // multi = 9.5: {3,4,5,  6;6.5;7;7.5;8,8.5,9,9.5} = 3 + 8 = 11 multipliers
      //Number of different multipliers = highest multiplier-lowest multiplier + 1
//      g_byValue1 =
      if( EISTenabled() ) //e.g. P8600 dual core
      {
        //lowestMultiplier = lowestLowFreqModemultiplier / 2.0f;
        const float lowestSuperLowFreqModeMultiplier =
          //SuperLowFrequencyMode: LowFrequencyMode multiplier / 2
          lowestLowFreqModemultiplier / 2.0f;
        numMultis = (
          //# SuperLowFrequencyMode multis
  //        LFMmultiplier / 2 +
          //e.g. ( 9.5 - 6 )*2+1 = (3.5)*2 +1 = 7.0+1 = 8
          //Maximum multiplier.
          ( maxMulti - /*lowestMultiplier*/ lowestSuperLowFreqModeMultiplier
          )
          //2 half multis for each integer step
          * 2
          //add 1, else 1 multiplier to few: 9-9*2=0
          + 1 ) ;
      }
      else //e.g. Celeron 900 Core 2 solo without EIST
      {
        //lowestMultiplier = lowestLowFreqModemultiplier;
        numMultis = 1;
      }
      return numMultis;
    }

    inline float * GetAvailableMultipliers(WORD & r_byNumMultis)
    {
      DEBUGN( FULL_FUNC_NAME << " begin")
      float * ar_f = NULL ;
      float maxMulti = GetMaxMulti();
      float /*minMulti*/ LFMmultiplier = GetMinMulti();
      DEBUGN( FULL_FUNC_NAME << "min multi:" << LFMmultiplier << " max multi:"
        << maxMulti )
      if( maxMulti != 0.0f && LFMmultiplier != 0.0f )
      {
          //    |
    //199: 00000000 0000860D
        unsigned numMultis = GetNumMultis(/*minMulti*/ LFMmultiplier, maxMulti);
        DEBUGN( FULL_FUNC_NAME << "# multis=size of array:" << numMultis )
  #ifdef _DEBUG
        g_std_ofstream << "# multis=size of array:" << numMultis;
        g_std_ofstream.flush();
  #endif

        ar_f = new float [ numMultis ] ;
        r_byNumMultis = numMultis;
        if( ar_f ) //Allocating memory on heap succeeded.
        {
          if( numMultis > 1)
          {
            float multiplier;
            unsigned multiplierIndex = 0;
            //loop "Number of different multipliers" times.
            //for( -- multiplierIndex ; multiplierIndex > 255 ; -- multiplierIndex )
            for( ; multiplierIndex < r_byNumMultis ; ++ multiplierIndex )
            {
              multiplier = LFMmultiplier / 2.0 ;
              ar_f[ multiplierIndex ] =
                //Minimum multi + Index
    //            ( LFMmultiplier + multiplierIndex )
                multiplier + (float) multiplierIndex * 0.5 ;
              DEBUGN("adding multiplier #" << multiplierIndex << ":"
                << ar_f[ multiplierIndex ] )
            }
          }
          else
          {
            ar_f[ 0 ] = maxMulti;
          }
        }
      }
      return ar_f ;
    }
  }
}

#endif /* CORE2_GETCONFIGURABLEMULTIPLIERS_HPP_ */
