/*
 * AMD_K7.hpp
 *
 *  Created on: 23.11.2012
 *      Author: Stefan
 */

#ifndef AMD_K7_HPP_
#define AMD_K7_HPP_

//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()

#define FIDVID_STATUS_MSR_ADDRESS 0xC0010042
#define MAX_FID_START_ADDRESS_IN_BIT 16
#define MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 16
//mobile Duron: VID 13 = 1.35V, VID 12 = 1.4V -> VID 0 = 1.4V + 0.05 * 12= 2V
#define MAX_VOLTAGE_IN_VOLT 2.0f
#define MIN_VOLTAGE_IN_VOLT 0.925f //according to RMclock for mobile Duron

  //for mobile Duron 1000 MHz(fam 6 model 7 stepp 1)
  //FID|multi|multi =
  // 4  5     FID * 0.5 + 3 = 2 + 3
  // 5  6     FID * 0.5 + 3 = 2.5 + 3
  //14  10    FID * 0.5 + 3 = 7 + 3
  /** @see AMD Duron™ Processor Model 7 Data Sheet 24310G—January 2002 */
inline float GetMultiplier_AMD_K7(BYTE frequencyID)
{
  DEBUGN( FULL_FUNC_NAME << "--begin--FID:" << (WORD) frequencyID)
  //FID multi multi =
  // 4  5     FID * 0.5 + 3 = 2 + 3
  // 5  6     FID * 0.5 + 3 = 2.5 + 3
  //14  10    FID * 0.5 + 3 = 7 + 3
  /** @see AMD Duron™ Processor Model 7 Data Sheet 24310G—January 2002 */
  return (float) frequencyID * 0.5f + 3;
}

inline float * GetAvailableMultipliersAMD_K7(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  BYTE maxFID = ( lowmostMSRbits >> MAX_FID_START_ADDRESS_IN_BIT ) &
    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN( FULL_FUNC_NAME << "--maximum FID:" << (WORD) maxFID)
//  float maxMultiplier = GetMultiplier_AMD_NPT_family_0FH(maxFID);
//e.g. 14 - 4 = 10   x ele R { N [10...20] / 2.0 }
  r_wNumberOfArrayElements = maxFID - 3;
  DEBUGN( FULL_FUNC_NAME << " # array elements:" << r_wNumberOfArrayElements)
  float * multipliers = new float[ r_wNumberOfArrayElements];
  if( multipliers)
  {
    BYTE currentFID = 4;
    BYTE arrayIndex = 0;
    while( arrayIndex < r_wNumberOfArrayElements )
    {
      multipliers[arrayIndex] = GetMultiplier_AMD_K7(currentFID);
      DEBUGN( FULL_FUNC_NAME << "--added multiplier " << multipliers[arrayIndex]
        << " for FID:" << (WORD) currentFID << " at array index "
        << (WORD) arrayIndex)
      ++ arrayIndex;
      ++ currentFID;
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << multipliers)
  return multipliers;
}

inline BYTE GetVoltageID_AMD_K7(float fVoltageInVoltToSet)
{
#ifdef _DEBUG
  float fSubtrahent, fDivisor;
  if( fVoltageInVoltToSet >= 0.775f )
  {
    fSubtrahent = 1.550f;
    fDivisor = 0.025f;
  }
  else
  {
    fSubtrahent = 0.7625f;
    fDivisor = 0.0125f;
  }
  static float fMaxVoltageMinusVoltageInVoltToSet, fQuotient;
  fMaxVoltageMinusVoltageInVoltToSet = fSubtrahent -
    fVoltageInVoltToSet;
  fQuotient = fMaxVoltageMinusVoltageInVoltToSet / fDivisor;
  DEBUGN( FULL_FUNC_NAME << "--difference:" << fMaxVoltageMinusVoltageInVoltToSet
    << " Quotient:" << fQuotient)
#endif
  float fVoltageID = fVoltageInVoltToSet >= 1.3f ?
    //where VID <= 01_1111b
    (MAX_VOLTAGE_IN_VOLT - fVoltageInVoltToSet) / 0.05f :
    //VID 14 ^= 1.3 V
    14 + (1.3f - fVoltageInVoltToSet) / 0.025f;
  // fVoltageID may be < real result due to miscalculations.
  BYTE voltageID = //ceil(fVoltageID);
    (BYTE) fVoltageID;
  if( fVoltageID - voltageID > 0.5 )
  {
    DEBUGN( FULL_FUNC_NAME << "--float voltage ID - integer voltage ID > 0.5")
    ++ voltageID;
  }
  DEBUGN( FULL_FUNC_NAME << "--voltage ID for " << fVoltageInVoltToSet <<
    " Volt=" << (WORD) voltageID)
  return voltageID;
}

inline float GetVoltageInVolt_AMD_K7(BYTE voltageID)
{
  static float fVoltageInVolt;
  if( voltageID < 15)
  {
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 12 = 1.4f
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 13 = 1.35f
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1): VID 14=1.3V =2-(14*0.5)=2-0.7
    fVoltageInVolt = (MAX_VOLTAGE_IN_VOLT - (float) voltageID * 0.05f);
    DEBUGN( FULL_FUNC_NAME << " voltage[V]=" << MAX_VOLTAGE_IN_VOLT << "-VID("
      << (WORD) voltageID << ")*0.05f=" << fVoltageInVolt)
  }
  else
  {
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 16=1.275V=1.3-1*0.25=1.3-0.25
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 17=1.25V=1.3-2*0.25=1.3-0.5
    //mobile Duron 1000 MHz(fam 6 model 7 stepp 1):VID 18=1.225V=1.3-3*0.25=1.3-0.75
    fVoltageInVolt = 1.3f - (voltageID - 15) * 0.025f;
    DEBUGN( FULL_FUNC_NAME << " voltage[V]=1.3-(VID(" << (WORD) voltageID
      << ")-14) * 0.025= 1.3-" << (voltageID-14) << "*0.025=" << fVoltageInVolt)
  }
  return fVoltageInVolt;
}

inline BYTE GetCurrentVoltageAndFrequency_AMD_K7(
  float * p_fVoltageInVolt,
  float * p_fMultiplier,
  float * p_fReferenceClockInMHz,
  BYTE & byCurrentVoltageID,
  BYTE & byCurrentFrequencyID
  )
{
  static BYTE byRet;
  static uint32_t lowmostMSRbits, highmostMSRbits;
  //    byRet = GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
  //      p_fVoltageInVolt,
  //      p_fMultiplier ,
  //      p_fReferenceClockInMHz ,
  //      wCoreID
  //      ) ;
  byRet = ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  byCurrentVoltageID = highmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
  byCurrentFrequencyID = lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
  * p_fMultiplier = GetMultiplier_AMD_K7(byCurrentFrequencyID);
  * p_fReferenceClockInMHz = 100.0f;
  // VID 12 = 1.4V
  * p_fVoltageInVolt = GetVoltageInVolt_AMD_K7(byCurrentVoltageID);
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) byRet << " ref.clock:"
    << * p_fReferenceClockInMHz << "MHz multi:" << * p_fMultiplier
    << " " << * p_fVoltageInVolt << "V")
  return byRet;
}

inline float * GetAvailableVoltagesAMD_K7(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  const BYTE maximumVoltageVID = ( highmostMSRbits >>
    MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
    BITMASK_FOR_LOWMOST_6BIT;
//  s_maxVoltageInVolt = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);
//#ifdef _DEBUG
//  const BYTE MaxRampVID = ( lowmostMSRbits >> MAX_RAMP_VID_START_ADDRESS_IN_BIT
//    ) & BITMASK_FOR_LOWMOST_6BIT;
//#endif
//  DEBUGN(FULL_FUNC_NAME << "--MaxRampVID:" << (WORD) MaxRampVID << "^=" <<
//    GetVoltageInVolt_AMD_K7(MaxRampVID) << " Volt")

//  BYTE startVID = ( highmostMSRbits >>
//    START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
//    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN(FULL_FUNC_NAME << "--highmost bits:"
    << getBinaryRepresentation(highmostMSRbits) )
  DEBUGN(FULL_FUNC_NAME << "--VID for maximum voltage:" << (WORD) maximumVoltageVID)
//  DEBUGN(FULL_FUNC_NAME << "--StartVID:" << (WORD) startVID)
//  BYTE maximumVID = maximumVoltageVID > startVID ? maximumVoltageVID : startVID;
//  float maxVoltage = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);

  const BYTE VIDforLowestVoltage = GetVoltageID_AMD_K7(MIN_VOLTAGE_IN_VOLT);
  r_wNumberOfArrayElements = //MAXIMUM_VOLTAGE_ID
    //32
    VIDforLowestVoltage
    - maximumVoltageVID + 1;
  DEBUGN(FULL_FUNC_NAME << "--# array elements:" << r_wNumberOfArrayElements)
  float * voltages = new float[r_wNumberOfArrayElements];
  if(voltages)
  {
    BYTE currentVID = //MAXIMUM_VOLTAGE_ID;
      maximumVoltageVID;
    BYTE arrayIndexForCurrentVoltage = 0;
//    while( //currentVID >= maximumVoltageVID
//        currentVID <= maximumVoltageVID + r_wNumberOfArrayElements )
    for( ; arrayIndexForCurrentVoltage < r_wNumberOfArrayElements;
        ++ arrayIndexForCurrentVoltage)
    {
      voltages[arrayIndexForCurrentVoltage] =
        GetVoltageInVolt_AMD_K7(currentVID);
      DEBUGN( FULL_FUNC_NAME << "--added #"
        << (WORD) arrayIndexForCurrentVoltage
        << " : "
        << voltages[arrayIndexForCurrentVoltage]
        << " Volt for VID:" << (WORD) currentVID)
//      -- currentVID;
      ++ currentVID;
//      ++ arrayIndexForCurrentVoltage;
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << voltages)
  return voltages;
}

#endif /* AMD_K7_HPP_ */
