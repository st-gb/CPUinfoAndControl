/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany
 * ("Trilobyte SE") 2010-at least 2012.
 * You are allowed to modify and use the source code from Trilobyte SE for free
 * if you are not making profit directly or indirectly with it or its adaption.
 * Else you may contact Trilobyte SE. */
/*
 * AMD_K7_SetVoltageAndMulti.hpp
 *
 *  Created on: 23.11.2012
 *      Author: Stefan
 */

#ifndef AMD_K7_SETVOLTAGEANDMULTI_HPP_
#define AMD_K7_SETVOLTAGEANDMULTI_HPP_

#include <Controller/time/GetTickCount.hpp> //DWORD ::GetTickCount()
#include <Controller/Sleep.hpp> //OperatingSystem::Sleep()
//FIDVID_CTL_MSR_ADDRESS etc.
#include <Controller/CPU-related/AMD/AMD_MSR_adresses.h>
  #include <Controller/CPU-related/AMD/K7/AMD_K7.hpp>

#define BIT_31_SET 2147483648UL

  static const float fRampVoltageOffsetInVolt =
//    32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
//    Processors  Table 73. RVO Values
//    "50 mV BIOS default"
    0.05f;

  //TODO calculate/ get MVS
  static const float MaximumVoltageStepInVolt =
  // 32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors: Table 72. MVS Values:
  // "25 mV BIOS default."
    0.025f;

//  "To translate the PLL lock time to an StpGntTOCnt value, multiply
//  PLL_LOCK_TIME by 1000 to get nanoseconds, then divide by 5 which is the clock period of
//  the counter in ns. Therefore, StpGntTOCnt value = PLL_LOCK_TIME * 1000/5. For example,
//  a PLL lock time of 2 μs results in an StpGntTOCnt value of 400 decimal and 190h."

  //14.2.9.1 FIDVID_CTL Register p. 423:
  //"The processor core clock PLL lock time is 2 μs for AMD NPT Family 0Fh
  // Processors. This time
//  refers to all components of PLL lock including frequency lock, phase lock,
  //and settling time."
  static uint16_t s_StpGntTOCnt = 400;
    //4000;

//enum voltageIDincOrDec{ decrementVoltageID = -1, incrementVoltageID = 1};
//enum incOrDecVoltage{ incrementVoltage = decrementVoltageID,
//  decrementVoltage = incrementVoltageID };
enum incOrDecVoltage{ decrementVoltage = -1, incrementVoltage = 1};
enum voltageIDincOrDec{ decrementVoltageID = incrementVoltage,
  incrementVoltageID = decrementVoltage };

//extern static BYTE s_minimumFID;
extern BYTE s_minimumFID;

//#ifdef COMPILE_WITH_LOG
//  #include <Windows/Logger/Logger.hpp> //class Windows_API::Logger
//  extern Windows_API::Logger g_windows_api_logger;
//#endif

void WaitNanoSeconds(uint64_t WaitTimeInNanoseconds)
{
//  uint64_t initialTimeCountInNanoSeconds, currentTimeCountInNanoSeconds;
  long double currentTimeCountInSeconds;
  long double initialTimeCountInSeconds;
//  GetTimeCountInNanoSeconds(//LARGE_INTEGER *lpPerformanceCount
//    initialTimeCountInNanoSeconds );
  GetTimeCountInSeconds( initialTimeCountInSeconds );
  do
  {
//    GetTimeCountInNanoSeconds(//LARGE_INTEGER *lpPerformanceCount
//      currentTimeCountInNanoSeconds );
    GetTimeCountInSeconds( currentTimeCountInSeconds );
    DEBUGN( FULL_FUNC_NAME << "time count in s:" << currentTimeCountInSeconds)
  }while( //(currentTimeCountInNanoSeconds - initialTimeCountInNanoSeconds)
    ( currentTimeCountInSeconds - initialTimeCountInSeconds ) * 1000000000.0
    < WaitTimeInNanoseconds);
  DEBUGN( FULL_FUNC_NAME << "--end--" << //currentTimeCountInNanoSeconds
    //<< "-" << initialTimeCountInNanoSeconds << ">= " << WaitTimeInNanoseconds
    currentTimeCountInSeconds << "s-" << initialTimeCountInSeconds << "s >= "
    << ( WaitTimeInNanoseconds / 1000000000.0 ) << "s")
}

void WaitVoltageStabilizationTime()
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  //"The default is 05h (100 μs). All other values are reserved."
//  ::Sleep(1);
  //"Table 71. Sample VST Values"
  //100000 nanoseconds = 100 μs
  //http://en.wikipedia.org/wiki/%CE%9Cs:
  // "A microsecond is equal to 1000 nanoseconds"
  //Rightmark: VID transition statbilization time (us): "100"
  WaitNanoSeconds(100000
    //1000000
    );
//  OperatingSystem::Sleep(10);
}

//"10.6.2.1.5 Isochronous Relief Time" :
//"The Isochronous Relief Time (IRT) is the amount of time the processor driver must count after each
//FID change step in a given P-state transition."
void WaitIsochronousReliefTime()
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  //Chapter 10 Power and Thermal Management page 321:  Table 75. IRT Values
  //"80 μs (BIOS default)"
  //" 10 μs ...80 μs"
  //RMclock: FID transition stabilization time (us): "80"
  WaitNanoSeconds( //80000
    //Use more than default because the time count may be inadequate.
    //100000
    80000
    );
//  OperatingSystem::Sleep(1);
}

//inline void CreateFIDtoPortalCoreFIDmapping()
//{
//  // 800 MHz -> 1600, 1700, 1800 MHz
//  s_std_mapFID2MinAndMaxFID.push_back( std::make_pair(0, MinAndMaxFID(8, 10)) );
//  // 900 -> 1600, 1700, 1800, 1900, 2000
//  s_std_mapFID2MinAndMaxFID.push_back( std::make_pair(1, MinAndMaxFID(8, 12)) );
//  s_std_mapFID2MinAndMaxFID.push_back( std::make_pair(1, MinAndMaxFID(8, 12)) );
//}

//  Only sets the voltage, the multiplier must stay the same (else undefined
//   * behaviour/ errors possible)

/**
  //32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh:
  //  10.5.7.2.1 Changing the VID
  //  Note: Software must hold the FID constant when changing the VID.
 *
  //p. 315:
  //"Note: Software must hold the VID constant when changing the FID."
 * */
inline BYTE FinallyWriteVIDandFIDtoMSR( uint32_t lowmostMSRbits,
  uint32_t highmostMSRbits)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  BYTE MSRaccessRetVal = WriteMSR(
    FIDVID_CTL_MSR_ADDRESS,
    lowmostMSRbits,
    highmostMSRbits, 1);

  long double currentTimeCountInSeconds;
  long double initialTimeCountInSeconds;
  unsigned currentVoltageID, currentFrequencyID;

  GetTimeCountInSeconds( initialTimeCountInSeconds );
  while( (MSRaccessRetVal = ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & lowmostMSRbits,
      & highmostMSRbits, 1) )
    )
  {
    DEBUGN( FULL_FUNC_NAME << "--lowmost bits of FIDVID_STATUS_MSR:"
      << getBinaryRepresentation(
      lowmostMSRbits) )
    currentVoltageID = highmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
    currentFrequencyID = lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
    DEBUGN( FULL_FUNC_NAME << "--current FID:" << currentFrequencyID
      << " current VID:" << currentFrequencyID )

    GetTimeCountInSeconds( currentTimeCountInSeconds );
    //"Loop on reading the FidVidPending bit (bit 31) of FIDVID_STATUS
    //(MSR C001_0042h) until the bit returns 0. The FidVidPending bit stays set
    //to 1 until the new FID code is in effect."
    if( ! ( lowmostMSRbits & BIT_31_SET )
      )
      break;
    if(//It happened that the bit had never been set->endless loop (when
        // written to FIDVID_CONTROL_MSR and _both_ FID and VID to set
        // differed from the current FID and VID).
        //This comparison prevents the endless loop.
        (currentTimeCountInSeconds - initialTimeCountInSeconds) > 0.0001
      )
    {
      DEBUGN( FULL_FUNC_NAME << "--timed out")
      break;
    }
    DEBUGN("VID code has NOT been driven to the voltage regulator")
  }
  DEBUGN("VID code has been driven to the voltage regulator")
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) MSRaccessRetVal)
  return MSRaccessRetVal;
}

/** Should also work with AMD family 15 dec = 0xF hex = K8
 * @param StpGntTOCnt can be set to 0 when only changing the VID? */
inline BYTE WriteVoltageIDandFID_AMD_K7(
  BYTE byVoltageIDtoSet,
  BYTE byFrequencyIDtoSet,
  uint16_t StpGntTOCnt
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--VoltageIDtoSet:"
    << (WORD) byVoltageIDtoSet
    << " FrequencyIDtoSet: " << (WORD) byFrequencyIDtoSet )
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  static BYTE MSRaccessRetVal;

  // "51–32 StpGntTOCnt"
  //"FIDVID_CTL Register MSR C001_0041h"
  //"The BIOS or processor driver must program this field with the
  //PLL lock time, defined above, prior to a FID change."
  highmostMSRbits = StpGntTOCnt;

  //TODO 1. compose MSR value from NewVID, NewFID etc.
  //"InitFidVid bit (bit 16) set to 1."
  lowmostMSRbits = //1 << 16;
    65536; //0x10000 hex
  lowmostMSRbits |= ( (uint16_t) byVoltageIDtoSet << FIDVID_CTL_VOLTAGE_ID_START_BIT);
  lowmostMSRbits |= byFrequencyIDtoSet;

  MSRaccessRetVal = FinallyWriteVIDandFIDtoMSR(lowmostMSRbits, highmostMSRbits);
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) MSRaccessRetVal)
  return MSRaccessRetVal;
}

//  "To change the processor voltage:
//  1. Write the following values to FIDVID_CTL (MSR C001_0041h):
//  – NewVID field (bits 13–8) with the VID code associated with the target voltage
//  – NewFID field (bits 5–0) with the CurrFID value indicated in the FIDVID_STATUS MSR
//  – InitFidVid bit (bit 16) set to 1. Setting this bit initiates the VID change.
//  – Clear all other bits to 0."
inline BYTE SetVIDorFID_AMD_NPT_family_0FH(
//  float fVoltageInVoltToSet,
  BYTE VoltageID,
  BYTE byFrequencyIDtoSet,
  uint16_t StpGntTOCnt
  )
{
  DEBUGN( FULL_FUNC_NAME //<< "--fVoltageInVoltToSet:" << fVoltageInVoltToSet
    << "VoltageID:" << (WORD) VoltageID
    << " byFrequencyIDtoSet:" << (WORD) byFrequencyIDtoSet
    << " StpGntTOCnt:" << StpGntTOCnt)
  static BYTE MSRaccessRetVal;
//  float fMultiplierFromFID = GetMultiplier( lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT);

//  BYTE FID = GetFrequencyID_AMD_NPT_family_0FH(fMultiplierToSet);

  MSRaccessRetVal = WriteVoltageIDandFID_AMD_K7(
    VoltageID, byFrequencyIDtoSet, StpGntTOCnt);

//  //2. Fetch VID from MSR value that should be written.
//  float fVoltageInVoltFromVIDcode = GetVoltageInVolt_AMD_NPT_family_0FH(
//    (lowmostMSRbits >>
//    FIDVID_CTL_VOLTAGE_ID_START_BIT ) &
//    BITMASK_FOR_LOWMOST_6BIT);
//  DEBUGN("fVoltageInVoltFromVIDcode \"" << (WORD) VoltageID << "\":"
//    << fVoltageInVoltFromVIDcode
//    << " fVoltageInVoltToSet:" << fVoltageInVoltToSet)

//  float fVoltageInVoltFromVoltageID = GetVoltageInVolt_AMD_NPT_family_0FH(VoltageID);

//  if( fVoltageInVoltFromVIDcode == fVoltageInVoltToSet)
  {
//    DEBUGN("fVoltageInVoltFromVIDcode equals fVoltageInVoltToSet-> writing lowmost: "
//      << lowmostMSRbits << " highmost: " << highmostMSRbits
//      << "bits to MSR")
  }

//  ChangeVoltageAndOrMultiplier();
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) MSRaccessRetVal)
  return MSRaccessRetVal;
}

//inline bool IsInHighFIDfrequencytable(BYTE byFrequencyID)
//{
//
//}

inline bool TargetFIDisBelowVCOportal(BYTE byFrequencyID)
{
  return byFrequencyID < 8;
//  s_ar_minandmaxfid[0].
}

#define MAKE_POSITIVE(x) if( (x) < 0) (x)*=-1;
inline BYTE makePositive(char by)
{
  if( by < 0)
    return by * -1;
  return by;
}

//see 32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
// Processors
//"Figure 11. TargetVID Calculation"
inline void CalculateTargetVoltageID()
{
  //InitialFID = CurrentFID truncated to an even FID
  //"TargetVID = VID after RVO applied
}

/**Use this function for both increase and decrease of voltage for better code
 * maintainance/ to avoid redundancy.*/
inline BYTE TransitionToVoltageInSingleVIDsteps(
  char voltageIDdirection, /** -1: decrement voltage ID<=>increment voltage,
    1: increment voltage ID*/
  float & fCurrentVoltageInVolt,
  float fVoltageInVoltToSet,
  BYTE byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--VIDdirection:" << (WORD) voltageIDdirection
    << "fCurrVoltage:" << fCurrentVoltageInVolt
    << "V VoltageToSet:" << fVoltageInVoltToSet << "V"
    << " currentFID:" << (WORD) byCurrentFrequencyID)
  BYTE retVal = 0;
  BYTE voltageIDtoSet = GetVoltageID_AMD_K7(fVoltageInVoltToSet);
  BYTE currentVoltageID = GetVoltageID_AMD_K7(fCurrentVoltageInVolt);
  do
  {
    currentVoltageID += voltageIDdirection;

    retVal = WriteVoltageIDandFID_AMD_K7(
      currentVoltageID,
      byCurrentFrequencyID,
      //0
      s_StpGntTOCnt);
    //10.6.2.1.1 Voltage Stabilization Time:
    //" The processor driver counts VST between VID steps that increase
    //the processor’s core voltage, not between steps that decrease it.""
    if( voltageIDdirection == incrementVoltage )
      WaitVoltageStabilizationTime();
    // Lower Voltage ID = higher voltage
    // case of: increment voltage: while ( -1 * 5 < -1 * 0)
    // case of: decrement voltage: while ( 1 * 0 < 1 * 5)
  }while( voltageIDdirection * currentVoltageID < voltageIDdirection * voltageIDtoSet);

  fCurrentVoltageInVolt = GetVoltageInVolt_AMD_K7(currentVoltageID);

  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal
    << " current voltage:" << fCurrentVoltageInVolt << "V")
  return retVal;
}

/** This needs to be done because the voltage was < in comparison (but
 * identical when output as a float) : an amount that equals a multiple
 * of a VID step was added for multiple times.
 *
 * log output:
 * 00111111 01111111 11111111 11111110  fCurrentVoltageInVolt:1
 * 00111111 01111111 11111111 11111111 fVoltageInVoltToSet:1
 * (maybe the output precision was not sufficient to show the difference)
 * */
inline float SetToClosestConfigurableVoltage(float fCurrentVoltageInVolt)
{
  BYTE VID = GetVoltageID_AMD_K7(fCurrentVoltageInVolt);
  float fVoltageInVoltFromVID = GetVoltageInVolt_AMD_K7(VID);
  if( fCurrentVoltageInVolt != fCurrentVoltageInVolt)
  {
    DEBUGN( FULL_FUNC_NAME << "--"
      << getBinaryRepresentation( * ((unsigned long *) & fCurrentVoltageInVolt) )
      << "--fCurrentVoltageInVolt")
    DEBUGN( FULL_FUNC_NAME << "--"
      << getBinaryRepresentation( * ((unsigned long *) & fVoltageInVoltFromVID) )
      << "--fVoltageInVoltFromVID"
      )
  }
  return fVoltageInVoltFromVID;
}

/**Use this function for both increase and decrease of voltage for better code
 * maintainance/ to avoid code redundancy.*/
inline BYTE ChangeVoltageByMaximumVoltageStep(
  float & fCurrentVoltageInVolt,
  const BYTE byCurrentFrequencyID,
  /**if negative: decrease voltage by this value in Volt*/
  float MaximumVoltageStepInVolt,
  float fVoltageInVoltToSet
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--CurrVoltage:"
    << fCurrentVoltageInVolt << "V currFID:"
    << (WORD) byCurrentFrequencyID
    << " MaximumVoltageStep:" << MaximumVoltageStepInVolt << "V"
    << " VoltageToSet:" << fVoltageInVoltToSet << "V"
    )
  BYTE retVal = 0;
//  //Max exceed max voltage when the ramp voltage offset is added to a voltage.
//  if( fVoltageInVoltToSet > s_maxVoltageInVolt)
//  {
//    DEBUGN( FULL_FUNC_NAME << " setting to max voltage")
//    fVoltageInVoltToSet = s_maxVoltageInVolt;
//  }
  float comparativeVoltageInVoltToSet = fVoltageInVoltToSet;
//  float absoluteCurrentVoltageInVolt;
  float absoluteMaximumVoltageStepInVolt = MaximumVoltageStepInVolt;
  float direction;
  if( MaximumVoltageStepInVolt < 0 )
  {
    absoluteMaximumVoltageStepInVolt *= -1.0f;
    comparativeVoltageInVoltToSet *= -1.0f;
    direction = -1.0f;
  }
  else
    direction = 1.0f;
  //Avoid exceeding the voltage to set by including MaximumVoltageStepInVolt.
  float fBorderVoltage = comparativeVoltageInVoltToSet
    //Gets lower for decreasing voltage, higher for increasing voltage.
    - absoluteMaximumVoltageStepInVolt;
  DEBUGN( FULL_FUNC_NAME << "--"
    "comparativeVoltageToSet:" << comparativeVoltageInVoltToSet
    << "V absoluteMaximumVoltageStep:" << absoluteMaximumVoltageStepInVolt
    << "V fBorderVoltage:" << fBorderVoltage
    )

  if( fCurrentVoltageInVolt * direction <= fBorderVoltage)
  {
    BYTE VoltageID;
    do
    {
      fCurrentVoltageInVolt += MaximumVoltageStepInVolt;
      VoltageID = GetVoltageID_AMD_K7(//fVoltageInVoltToSet
        fCurrentVoltageInVolt);
      retVal = SetVIDorFID_AMD_NPT_family_0FH(
//        fCurrentVoltageInVolt,
        VoltageID,
        byCurrentFrequencyID,
        0);
      //10.6.2.1.1 Voltage Stabilization Time:
      //" The processor driver counts VST between VID steps that increase
//      the processor’s core voltage, not between steps that decrease it."
      if( direction == incrementVoltage )
        WaitVoltageStabilizationTime();
      fCurrentVoltageInVolt = SetToClosestConfigurableVoltage(
        fCurrentVoltageInVolt);
  //    absoluteCurrentVoltageInVolt = fCurrentVoltageInVolt < 0.0f ?
  //      fCurrentVoltageInVolt * -1.0 : fCurrentVoltageInVolt;
    }
    while( retVal && //absoluteCurrentVoltageInVolt //)
      fCurrentVoltageInVolt * direction
      <= //(absoluteVoltageInVoltToSet - absoluteMaximumVoltageStepInVolt)
  //      comparativeVoltageInVoltToSet - MaximumVoltageStepInVolt
      fBorderVoltage
      );
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal
    << "--current voltage:" << fCurrentVoltageInVolt << "V")
  return retVal;
}

inline BYTE TransitionVoltage(
  float & fCurrentVoltageInVolt,
  float fVoltageInVoltToSet,
  BYTE byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin"
    "--currentVoltage:" << fCurrentVoltageInVolt << "V"
    << " VoltageToSet:" << fVoltageInVoltToSet << "V"
    << " MaximumVoltageStep:" << MaximumVoltageStepInVolt << "V"
    )
  BYTE retVal = 0;
  static float currVoltCmp, voltToSetCmp;

  int direction = incrementVoltage;
//  static uint16_t StpGntTOCnt = 0;
  if( fCurrentVoltageInVolt > fVoltageInVoltToSet)
  {
    direction = decrementVoltage;
    //MaximumVoltageStepInVolt *= -1.f;
  }
//  ChangeVoltageByMaximumVoltageStep(
//    fCurrentVoltageInVolt,
//    byCurrentFrequencyID,
//    MaximumVoltageStepInVolt * direction,
//    fVoltageInVoltToSet
//    );
  currVoltCmp = fCurrentVoltageInVolt * (float) direction;
  voltToSetCmp = fVoltageInVoltToSet * (float) direction;
  BYTE reachedVoltage = (currVoltCmp < voltToSetCmp);
  DEBUGN( FULL_FUNC_NAME << "--" << currVoltCmp << " < " << voltToSetCmp << "?"
    << reachedVoltage )
  //if MaximumVoltageStepInVolt was too high (higher than 1 voltage step)
  if( //if e.g 1.2V->0.8V: -0.825 < -0.8V;
      // 0.8V->1.2V: 1.175V < 1.2V
      reachedVoltage
//      direction * fCurrentVoltageInVolt < direction * fVoltageInVoltToSet
    )
  {
    DEBUGN( FULL_FUNC_NAME << " did not reach fVoltageInVoltToSet")
    DEBUGN( FULL_FUNC_NAME << "--"
      << getBinaryRepresentation( * ((unsigned long *) & fCurrentVoltageInVolt) )
      << "--fCurrentVoltageInVolt")
    DEBUGN( FULL_FUNC_NAME << "--"
      << getBinaryRepresentation( * ((unsigned long *) & fVoltageInVoltToSet) )
      << "--fVoltageInVoltToSet"
      )
    TransitionToVoltageInSingleVIDsteps(
//      incrementVoltage,
      direction * -1,
      fCurrentVoltageInVolt,
      fVoltageInVoltToSet,
      byCurrentFrequencyID);
  }
  DEBUGN( FULL_FUNC_NAME << "--fCurrentVoltageInVolt:" << fCurrentVoltageInVolt )
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal )
  return retVal;
}

inline BYTE
  SetCurrentVoltageAndMultiplier_AMD_K7(
    float fVoltageInVoltToSet
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplierToSet
    , WORD wCoreID
  )
{
  //AMD Duron™ Processor Model 7 Data Sheet--24310.pdf  "4.3 Clock Control":
//  "The processor implements a Clock Control (CLK_Ctl) MSR
//  (address C001_001Bh) that determines the internal clock
//  divisor when the AMD Duron system bus is disconnected."
  DEBUGN( FULL_FUNC_NAME << "--begin--should set" << fVoltageInVoltToSet
    << "V, multiplier:" << fMultiplierToSet)
//  DEBUGN_LOGGER_NAME(g_windows_api_logger, FULL_FUNC_NAME << "--begin--"
//    << fVoltageInVoltToSet << "V, " << fMultiplierToSet)
  float fCurrentVoltageInVolt;
  float fCurrentMultiplier;
  float fCurrentReferenceClockInMHz;

  BYTE byCurrentFrequencyID, byCurrentVoltageID;
  BYTE MSRaccessRetVal = GetCurrentVoltageAndFrequency_AMD_K7(
    & fCurrentVoltageInVolt,
    & fCurrentMultiplier,
    & fCurrentReferenceClockInMHz,
//    wCoreID,
    byCurrentVoltageID,
    byCurrentFrequencyID
    );
  DEBUGN( FULL_FUNC_NAME << "--fCurrentMultiplier:" << fCurrentMultiplier
    << " fCurrentVoltageInVolt:" << fCurrentVoltageInVolt)
  if( fCurrentMultiplier == fMultiplierToSet)
  {
//    SetVIDorFID_AMD_NPT_family_0FH(
//      fVoltageInVoltToSet,
//      byCurrentFrequencyID,
//      0);
    TransitionVoltage(
      fCurrentVoltageInVolt,
      fVoltageInVoltToSet,
      byCurrentFrequencyID);
  }
//  else
//    SetCurrentMultiplier_AMD_K7(
//      fVoltageInVoltToSet,
//      fMultiplierToSet,
//      fCurrentVoltageInVolt,
//      byCurrentVoltageID,
//      fCurrentMultiplier,
//      byCurrentFrequencyID);
  return MSRaccessRetVal;
}


#endif /* AMD_K7_SETVOLTAGEANDMULTI_HPP_ */
