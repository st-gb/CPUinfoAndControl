/*
 * AMD_K7.hpp
 *
 *  Created on: 23.11.2012
 *      Author: Stefan
 *
 *  maybe see AMD Athlon Processor x86 Code Optimization Guide, Publication
     No. 22007, Advanced Micro Devices, Inc., February 2002.
 */

#ifndef AMD_K7_HPP_
#define AMD_K7_HPP_

//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
#include <fastest_data_type.h> //typedef fastestUnsignedDataType

#define FIDVID_STATUS_MSR_ADDRESS 0xC0010042
#define MAX_FID_START_ADDRESS_IN_BIT 16
#define MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 16
//mobile Duron: VID 13 = 1.35V, VID 12 = 1.4V -> VID 0 = 1.4V + 0.05 * 12= 2V
#define AMD_MOBILE_K7_MAX_VOLTAGE_IN_VOLT 2.0f
#define AMD_K7_MOBILE_MIN_VOLTAGE_IN_VOLT 0.925f //according to RMclock for mobile Duron

#define FIRST_FREQUENCY_ID 4
#define VOLTAGE_ID_ABOVE_FINER_VOLTAGES 15

namespace AMD
{
  namespace K7
  {
    /** @see AMD Duron™ Processor Model 7 Data Sheet 24310G—January 2002 
     * for mobile AMD Duron 1000 MHz (CPUID family 6, model 7, stepping 1)
     * FID|multi|multi = FID * 0.5 + 3
     *  4   5     4 * 0.5 + 3 = 2 + 3
     *  5   5.5   5 * 0.5 + 3 = 2.5 + 3
     * 14  10    14 * 0.5 + 3 = 7 + 3
     */
    const float frequencyID2multiplierStartingWithLowestFID [] = { 5.0f, 5.5f, 6.0f, 6.5f, 7.0f
      , 7.5f, 8.0f, 8.5f, 9.0f, 9.5f, 10.0f, 10.5f, 11.0f, 11.5f};
    const float frequencyID2multiplier [] = {3.0f, 3.5f, 4.0f, 4.5f,//FIDs 0-3
      5.0f, 5.5f, 6.0f, 6.5f, 7.0f, 7.5f, 8.0f, 8.5f, 9.0f, 9.5f, //FIDs 4-..
      10.0f, 10.5f, 11.0f, 11.5f};
    const fastestUnsignedDataType frequencyIDs [] = { 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15};
    /** mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 12 = 1.4f
    * mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 13 = 1.35f
    * mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 14=1.3V =2-(14*0.5)=2-0.7
    * mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 16=1.275V=1.3-1*0.25=1.3-0.25
    * mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 17=1.25V=1.3-2*0.25=1.3-0.5
    * mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 18=1.225V=1.3-3*0.25=1.3-0.75 */
    const float voltagesInVolt [] = { 1.4f, 1.35f, 1.3f, 1.275f, 1.25f };

    /** @see frequencyID2multiplierStartingWithLowestFID */
    inline float calculateMultiplierFromFID(
      const fastestUnsignedDataType frequencyID)
    {
      return (float) frequencyID * 0.5f + 3;
    }

    inline float GetMultiplier(const fastestUnsignedDataType frequencyID)
    {
      DEBUGN( "begin--FID:" << frequencyID)
      return //calculateMultiplierFromFID(frequencyID)
//        frequencyID2multiplierStartingWithLowestFID[frequencyID - FIRST_FREQUENCY_ID];
        frequencyID2multiplier[frequencyID];
    }

    inline float calculateFIDfromMultiplier(const float multiplier)
    {
      //multiplier = FID * 0.5 + 3 | - 3
      //multiplier - 3 = FID * 0.5 | / 0.5
      //(multiplier - 3) / 0.5  = FID
      //FID|multi|FID =
      // 4  5     (multi - 3) / 0.5 = (5 - 3) / 0.5 = 2 / 0.5
      // 5  6     FID * 0.5 + 3 = 2.5 + 3
      //14  10    (10 - 3) / 0.5 = 7 / 0.5 = 14
      return (multiplier - 3.0f) /*/ 0.5f*/ * 2.0f;
    }

    //TODO maybe use a table (->faster) rather than calculating.
    inline float GetFrequencyID(const float multiplier)
    {
      DEBUGN( "begin--multiplier:" << multiplier)

      return calculateMultiplierFromFID(multiplier);
        //frequencyIDs[multiplier * 2.0f - 10.0f ];
    }

    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 12 = 1.4f
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 13 = 1.35f
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 14=1.3V =2-(14*0.5)=2-0.7
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 16=1.275V=1.3-1*0.25=1.3-0.25
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 17=1.25V=1.3-2*0.25=1.3-0.5
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 18=1.225V=1.3-3*0.25=1.3-0.75
    inline fastestUnsignedDataType GetVoltageID(const float fVoltageInVoltToSet)
    {
    #ifdef _DEBUG
      float fSubtrahent, fDivisor;
      if( fVoltageInVoltToSet >= 1.3f )
      {
        fSubtrahent = 1.550f;
        fDivisor = 0.05f;
      }
      else
      {
        fSubtrahent = 0.7625f;
        fDivisor = 0.025f;
      }
      static float fMaxVoltageMinusVoltageInVoltToSet, fQuotient;
      fMaxVoltageMinusVoltageInVoltToSet = fSubtrahent -
        fVoltageInVoltToSet;
      fQuotient = fMaxVoltageMinusVoltageInVoltToSet / fDivisor;
      DEBUGN( "subtrahent:" << fSubtrahent
        << " divisor:" << fDivisor
        << " difference:" << fMaxVoltageMinusVoltageInVoltToSet
        << " Quotient:" << fQuotient)
    #endif
      float fVoltageID = fVoltageInVoltToSet >= 1.3f ?
        //where VID <= 01_1111b
        (AMD_MOBILE_K7_MAX_VOLTAGE_IN_VOLT - fVoltageInVoltToSet) / 0.05f :
        //VID 16 ^= 1.275 V
        //e.g. 15 + (1.3 - 1.275) / 0.025 = 15 + 0.025 / 0.025 = 15 + 1 = 16 ^= 1.275
        //e.g. 15 + (1.3 - 1.25) / 0.025 = 15 + 0.05 / 0.025 = 15 + 2= = 17
        //
        VOLTAGE_ID_ABOVE_FINER_VOLTAGES + (1.3f - fVoltageInVoltToSet) / 0.025f;
      DEBUGN( "fVoltageID:" << fVoltageID)
      // fVoltageID may be < real result due to miscalculations.
      fastestUnsignedDataType voltageID = //ceil(fVoltageID);
        (fastestUnsignedDataType) fVoltageID;
      if( fVoltageID - voltageID > 0.5f )
      {
        DEBUGN( "float voltage ID - integer voltage ID > 0.5")
        ++ voltageID;
      }
      DEBUGN( "voltage ID for " << fVoltageInVoltToSet << " Volt=" << voltageID)
      return voltageID;
    }

    inline float GetVoltageInVolt(const fastestUnsignedDataType voltageID)
    {
      static float fVoltageInVolt;
      if( voltageID < 15)
      {
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 12 = 1.4f
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 13 = 1.35f
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 14=1.3V =2-(14*0.5)=2-0.7
        fVoltageInVolt = (AMD_MOBILE_K7_MAX_VOLTAGE_IN_VOLT - (float) voltageID * 0.05f);
        DEBUGN( "voltage[V]=" << AMD_MOBILE_K7_MAX_VOLTAGE_IN_VOLT << "-VID("
          << (WORD) voltageID << ")*0.05f=" << fVoltageInVolt)
      }
      else
      {
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 16=1.275V=1.3-1*0.25=1.3-0.25
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 17=1.25V=1.3-2*0.25=1.3-0.5
        //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 18=1.225V=1.3-3*0.25=1.3-0.75
        fVoltageInVolt = 1.3f - (voltageID - 15) * 0.025f;
        DEBUGN( "voltage[V]=1.3-(VID(" << (WORD) voltageID
          << ")-14) * 0.025= 1.3-" << (voltageID-14) << "*0.025=" << fVoltageInVolt)
      }
      return fVoltageInVolt;
    }
    inline float * GetAvailableMultipliers(
      uint16_t & r_wNumberOfArrayElements)
    {
      DEBUGN( "begin")
      static uint32_t lowmostMSRbits;
      static uint32_t highmostMSRbits;
      ReadMSR(
        FIDVID_STATUS_MSR_ADDRESS,
        & lowmostMSRbits,
        & highmostMSRbits,
        1
        );
      const fastestUnsignedDataType maxFID = ( lowmostMSRbits >> MAX_FID_START_ADDRESS_IN_BIT ) &
        BITMASK_FOR_LOWMOST_6BIT;
      DEBUGN( "maximum FID:" << (WORD) maxFID)
    //  float maxMultiplier = GetMultiplier_AMD_NPT_family_0FH(maxFID);
    //e.g. 14 - 4 = 10   x ele R { N [10...20] / 2.0 }
      r_wNumberOfArrayElements = maxFID - 3;
      DEBUGN( "# array elements:" << r_wNumberOfArrayElements)
      float * multipliers = new float[ r_wNumberOfArrayElements];
      if( multipliers)
      {
        fastestUnsignedDataType currentFID = 4;
        fastestUnsignedDataType arrayIndex = 0;
        while( arrayIndex < r_wNumberOfArrayElements )
        {
          multipliers[arrayIndex] = AMD::K7::GetMultiplier(currentFID);
          DEBUGN( "added multiplier " << multipliers[arrayIndex]
            << " for FID:" << (WORD) currentFID << " at array index "
            << (WORD) arrayIndex)
          ++ arrayIndex;
          ++ currentFID;
        }
      }
      DEBUGN( "return " << multipliers)
      return multipliers;
    }

    float GetVoltageAndMultiplier(DWORD affMask, float & r_fVoltageInVolt)
    {
      static uint32_t lowmostMSRbits, highmostMSRbits;
      static fastestUnsignedDataType currentVoltageID, retVal;
      static fastestUnsignedDataType currentFrequencyID;
      //    byRet = GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
      //      p_fVoltageInVolt,
      //      p_fMultiplier ,
      //      p_fReferenceClockInMHz ,
      //      wCoreID
      //      ) ;
      retVal = ReadMSR(
        FIDVID_STATUS_MSR_ADDRESS,
        & lowmostMSRbits,
        & highmostMSRbits,
        1
        );
      currentVoltageID = highmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
      currentFrequencyID = lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
      // VID 12 = 1.4V
      r_fVoltageInVolt = AMD::K7::GetVoltageInVolt(currentVoltageID);
      return AMD::K7::GetMultiplier(currentFrequencyID);
    }
  } /** namespace K7 */
} /** namespace AMD */

#ifdef TSC_DIFF_WHILE_BUSY_WAITING
#include <hardware/CPU/x86/GetCPUclockAndMultiplier.hpp>
#endif

inline BYTE GetCurrentVoltageAndFrequency_AMD_K7(
  float * p_fVoltageInVolt,
  float * p_fMultiplier,
  float * p_fReferenceClockInMHz,
  fastestUnsignedDataType & currentVoltageID,
  fastestUnsignedDataType & currentFrequencyID
  )
{
  static BYTE byRet;

//#ifdef CALCULATE_FSB_VIA_TSC_DIFF
#ifdef TSC_DIFF_WHILE_BUSY_WAITING
  //warning: the TSC frequency changes wih the clock speed.
  //so the reference clock may be calculated falsly (too high/ too low)
  // and so a wrong voltage based
  //on CPU frequ may be set.
  CPU::CalculateReferenceClockViaTSCdiff(
    * p_fVoltageInVolt,
    * p_fMultiplier,
    * p_fReferenceClockInMHz
    );
  if( * p_fMultiplier > 0.0f )
    return 1;
#else
  * p_fMultiplier = AMD::K7::GetVoltageAndMultiplier(1, * p_fVoltageInVolt);
  #ifdef _100MHZ_FSB
  * p_fReferenceClockInMHz = 100.0f;
  #endif
#endif

  DEBUGN( "return " << (WORD) byRet << " ref.clock:"
    << * p_fReferenceClockInMHz << "MHz multi:" << * p_fMultiplier
    << " " << * p_fVoltageInVolt << "V")
  return byRet;
}

#endif /* AMD_K7_HPP_ */
