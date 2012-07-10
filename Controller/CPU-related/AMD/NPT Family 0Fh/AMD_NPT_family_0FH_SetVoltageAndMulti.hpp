/*
 * AMD_NPT_family_0FH_SetVoltageAndMulti.hpp
 *
 *  Created on: 13.06.2012
 *      Author: Stefan
 */

#ifndef AMD_NPT_FAMILY_0FH_SETVOLTAGEANDMULTI_HPP_
#define AMD_NPT_FAMILY_0FH_SETVOLTAGEANDMULTI_HPP_

  static const float fRampVoltageOffsetInVolt =
//    32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
//    Processors  Table 73. RVO Values
//    "50 mV BIOS default"
    0.05f;

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

#include <Controller/time/GetTickCount.hpp> //DWORD ::GetTickCount()
#include <Controller/Sleep.hpp> //OperatingSystem::Sleep()

enum voltageIDincOrdec{ decrementVoltageID = -1, incrementVoltageID = 1};
enum incOrdecVoltage{ incrementVoltage = decrementVoltageID,
  decrementVoltage = incrementVoltageID };

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

//10.6.2.1.5 Isochronous Relief Time:
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

  GetTimeCountInSeconds( initialTimeCountInSeconds );
  while( (MSRaccessRetVal = ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & lowmostMSRbits,
      & highmostMSRbits, 1) )
    )
  {
    DEBUGN( FULL_FUNC_NAME << "--lowmost bits:" << getBinaryRepresentation(
      lowmostMSRbits) )
    GetTimeCountInSeconds( currentTimeCountInSeconds );
    //"Loop on reading the FidVidPending bit (bit 31) of FIDVID_STATUS
    //(MSR C001_0042h) until the bit returns 0. The FidVidPending bit stays set
    //to 1 until the new FID code is in effect."
    if( ! ( lowmostMSRbits & BIT_31_SET )
      )
      break;
    if(//It happened that the bit had never been set->endless loop (when both
        //FID and VID to set differed from the current FID and VID).
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

inline BYTE WriteVoltageIDandFID_AMD_NPT_family_0FH(
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
  lowmostMSRbits = 1 << 16;
  lowmostMSRbits |= ( (uint16_t) byVoltageIDtoSet << FIDVID_CTL_VOLTAGE_ID_START_BIT);
  lowmostMSRbits |= byFrequencyIDtoSet;

  MSRaccessRetVal = FinallyWriteVIDandFIDtoMSR(lowmostMSRbits, highmostMSRbits);
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) MSRaccessRetVal)
  return MSRaccessRetVal;
}

inline BYTE SetCurrentVoltage_AMD_NPT_family_0FH(
  float fVoltageInVoltToSet,
  BYTE byFrequencyIDtoSet,
  uint16_t StpGntTOCnt
  )
{
  DEBUGN( FULL_FUNC_NAME << "--fVoltageInVoltToSet:" << fVoltageInVoltToSet
    << " byFrequencyIDtoSet:" << (WORD) byFrequencyIDtoSet
    << " StpGntTOCnt:" << StpGntTOCnt)
  static BYTE MSRaccessRetVal;
//  float fMultiplierFromFID = GetMultiplier( lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT);

//  BYTE FID = GetFrequencyID_AMD_NPT_family_0FH(fMultiplierToSet);
  BYTE VoltageID = GetVoltageID_AMD_NPT_family_0FH(fVoltageInVoltToSet);

  MSRaccessRetVal = WriteVoltageIDandFID_AMD_NPT_family_0FH(
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

inline BYTE TransitionFrequencyID_StepByStep(
  float fCurrentVoltageInVolt
  , BYTE byCurrentFrequencyID,
  BYTE TargetFrequencyID,
  char FIDstep // positive: increase FID; negative: decrease FID
  )
{
  BYTE retVal;
  BYTE posFIDstep = makePositive(FIDstep);
  char direction = FIDstep / posFIDstep;
  do
  {
//    -- byCurrentFrequencyID;
    byCurrentFrequencyID += FIDstep; //next lower or higher even FID.
    retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
      fCurrentVoltageInVolt,
      byCurrentFrequencyID,
      s_StpGntTOCnt
      );
  } while( direction * byCurrentFrequencyID < direction * TargetFrequencyID);
  return retVal;
}

inline BYTE TransitionToLowerFrequencyID_StepByStep(
  float fCurrentVoltageInVolt
  , BYTE & byCurrentFrequencyID,
  BYTE TargetFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--byCurrentFrequencyID:"
    << (WORD) byCurrentFrequencyID
    << " TargetFrequencyID: " << (WORD) TargetFrequencyID )
  //TODO FID steps may be "1" for certain CPU steppings/ revisions (see AMD
  //BIOS and kernel dev guide).
  static BYTE FIDstep = //1;
    2;
  BYTE retVal = 1;
  if( byCurrentFrequencyID - FIDstep >= TargetFrequencyID)
    do
    {
  //    -- byCurrentFrequencyID;
      byCurrentFrequencyID -= FIDstep; //next even lower FID.
      DEBUGN( FULL_FUNC_NAME << "--byCurrentFrequencyID:" << (WORD) byCurrentFrequencyID)
      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
        fCurrentVoltageInVolt,
        byCurrentFrequencyID,
        s_StpGntTOCnt
        );
      WaitIsochronousReliefTime();
    } while(byCurrentFrequencyID > TargetFrequencyID);
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
  return retVal;
}

BYTE TransitionToLowerFrequencyID(
  float fMultiplierToSet,
  BYTE & byCurrentFrequencyID,
  float fCurrentVoltageInVolt
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--byCurrentFrequencyID:"
    << (WORD) byCurrentFrequencyID << " ")
  BYTE retVal;
  BYTE FinalFID = GetFrequencyID_AMD_NPT_family_0FH(fMultiplierToSet);
#ifdef ALLOW_ODD_FIDS_FOR_STEPPING_NUMBER_BELOW_3
  BYTE TargetFID = FinalFID;
#else
  BYTE TargetFID = FinalFID % 2 == 1 ? // odd FID?
    FinalFID - 1 : FinalFID; // make even or already even FID
#endif
  DEBUGN( FULL_FUNC_NAME << "--begin--FinalFID:"
    << (WORD) FinalFID << " TargetFID:" << (WORD) TargetFID)
  //TODO use VCO portal frequencies to transition faster.
//  if( byCurrentFrequencyID > 7 // > 1600 MHz
//      )
//  {
//    const MinAndMaxFID & minandmaxfid = s_ar_minandmaxfidVCO_FIDtoLowerFID[
//      byCurrentFrequencyID - 8];
//    if( TargetFID >= minandmaxfid.m_minFID && TargetFID <=
//        minandmaxfid.m_maxFID)
//    {
//
//    }
//  }
  //"Is TargetFID in the “High FID Frequency Table”?"
//  if( //TargetFID > 7
//      1
//      )
  {
    retVal = TransitionToLowerFrequencyID_StepByStep(
//      fVoltageInVoltToSet,
      fCurrentVoltageInVolt,
      byCurrentFrequencyID,
      TargetFID
//      FinalFID
      );
  }
//  else // Target FID is in low FID frequency table
//  {
//    //"Is TargetFID below VCO portal of CurrentFID?"
//    if( TargetFIDisBelowVCOportal(byCurrentFrequencyID) )
//    {
////      TransitionToLowerFrequencyID_StepByStep(fCurrentVoltageInVolt, byCurrentFrequencyID,
////        TargetFID);
//      do
//      {
//        --byCurrentFrequencyID;
//        retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
//          fCurrentVoltageInVolt,
//          byCurrentFrequencyID,
//          s_StpGntTOCnt);
//        WaitIsochronousReliefTime();
//      } while( retVal && TargetFIDisBelowVCOportal(byCurrentFrequencyID) );
//    }
//    else
//    {
//      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
//        fCurrentVoltageInVolt,
//        TargetFID,
//        s_StpGntTOCnt);
//      WaitIsochronousReliefTime();
//    }
//  }
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
  return retVal;
}

/** BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors
    32559 Rev. 3.16 November 2009
    "10.5.7.1 FID to VCO Frequency Relationship":
 *  "The processor supports direct transitions between the minimum core
 *  frequency in Table 69 and any of the core frequencies from Table 70 listed
 *  in the Portal Core Frequencies column associated with the minimum core
 *  frequency."
 */
inline BYTE GetVCOportalFrequencyID(
  BYTE byCurrentFrequencyID,
  BYTE byTargetFrequencyID
  )
{
  BYTE retVal = 255;
  //"The processor supports direct transitions between the minimum core
  //frequency [...]"
  if( byCurrentFrequencyID == s_minimumFID )
  {
    const MinAndMaxFID & c_r_minandmaxfid =
      s_ar_minandmaxfid[byCurrentFrequencyID];
    if( byTargetFrequencyID > c_r_minandmaxfid.m_maxFID )
      retVal = c_r_minandmaxfid.m_maxFID;
    else // <= c_r_minandmaxfid.m_maxFID
      if( byTargetFrequencyID >= c_r_minandmaxfid.m_minFID ) //[minFID...maxFID]
        retVal = byTargetFrequencyID;
  //    else
  //      retVal = byCurrentFrequencyID;
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
  return retVal;
}

inline BYTE TransitionToHigherFrequencyID_StepByStep(
  float fVoltageInVoltToSet
  , BYTE & byCurrentFrequencyID,
  BYTE TargetFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  BYTE retVal = 0;
  do
  {
//    ++ byCurrentFrequencyID; //higher FID <=> higher multiplier.
    byCurrentFrequencyID += 2; //next even higher FID <=> higher multiplier.
    DEBUGN( FULL_FUNC_NAME << "byCurrentFrequencyID:"
      << (WORD) byCurrentFrequencyID )
    retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
      fVoltageInVoltToSet,
      byCurrentFrequencyID,
      s_StpGntTOCnt);
    WaitIsochronousReliefTime();
  } while( retVal && byCurrentFrequencyID < TargetFrequencyID);

  if( byCurrentFrequencyID < TargetFrequencyID )
  {
    DEBUGN( FULL_FUNC_NAME << "--byCurrentFrequencyID < TargetFrequencyID" )
    ++ byCurrentFrequencyID;
    retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
      fVoltageInVoltToSet,
      byCurrentFrequencyID,
      s_StpGntTOCnt);
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << retVal)
  return retVal;
}

inline BYTE TransitionToHigherFrequencyID(
  float fVoltageInVoltToSet
  , float fMultiplierToSet
  , BYTE & byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  BYTE retVal = 0;
  BYTE FinalFID = GetFrequencyID_AMD_NPT_family_0FH(fMultiplierToSet);
  BYTE TargetFID = FinalFID % 2 == 1 ? FinalFID - 1 : FinalFID;
  DEBUGN( FULL_FUNC_NAME
    << "--TargetFID:" << (WORD) TargetFID
    << "current FID: " << (WORD) byCurrentFrequencyID )

  //"Is CurrentFID in the “High FID Frequency Table”?"
  if( byCurrentFrequencyID > 7
      //1
      )
  {
    retVal = TransitionToHigherFrequencyID_StepByStep(
      fVoltageInVoltToSet,
      byCurrentFrequencyID,
      TargetFID);
  }
  else
  {
    //TODO: only transition to VCO freq if byCurrentFrequencyID = min FID?
//    if( byCurrentFrequencyID == minimumFrequencyID )
    BYTE VCOportalFrequencyID = GetVCOportalFrequencyID(
      byCurrentFrequencyID,
      TargetFID);
    //"Is TargetFID above VCO portal of CurrentFID?"
    if( VCOportalFrequencyID != 255 )
    {
      //"FID Transition Process To highest portal FID in “High FID Frequency
      // Table”"
      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
        fVoltageInVoltToSet,
        VCOportalFrequencyID,
        s_StpGntTOCnt);
      WaitIsochronousReliefTime();
      if( retVal )
        byCurrentFrequencyID = VCOportalFrequencyID;
    }
    if( byCurrentFrequencyID < TargetFID )
    {
//      byCurrentFrequencyID = VCOportalFrequencyID;
      TransitionToHigherFrequencyID_StepByStep(
        fVoltageInVoltToSet,
//        VCOportalFrequencyID,
        byCurrentFrequencyID,
        TargetFID);
      //"FID Transition Process To TargetFID"
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
  return retVal;
}

//see 32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
// Processors
//"Figure 11. TargetVID Calculation"
inline void CalculateTargetVoltageID()
{
  //InitialFID = CurrentFID truncated to an even FID
  //"TargetVID = VID after RVO applied
}

inline BYTE TransitionToVoltageInSingleVIDsteps(
  char direction, /** -1: decrement voltage ID<=>increment voltage,
    1: increment voltage ID*/
  float & fCurrentVoltageInVolt,
  float fVoltageInVoltToSet,
  BYTE byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--fCurrentVoltage:" <<
    fCurrentVoltageInVolt << "V fVoltageToSet:" << fVoltageInVoltToSet << "V"
    << " current FID:" << (WORD) byCurrentFrequencyID)
  BYTE retVal = 0;
  BYTE voltageIDtoSet = GetVoltageID_AMD_NPT_family_0FH(fVoltageInVoltToSet);
  BYTE currentVoltageID = GetVoltageID_AMD_NPT_family_0FH(fCurrentVoltageInVolt);
  do
  {
    currentVoltageID += direction;

    retVal = WriteVoltageIDandFID_AMD_NPT_family_0FH(
      currentVoltageID,
      byCurrentFrequencyID,
      0);
    //10.6.2.1.1 Voltage Stabilization Time:
    //" The processor driver counts VST between VID steps that increase
    //the processor’s core voltage, not between steps that decrease it.""
    if( direction == incrementVoltage )
      WaitVoltageStabilizationTime();
    // Lower Voltage ID = higher voltage
    // case of: increment voltage: while ( -1 * 5 < -1 * 0)
    // case of: decrement voltage: while ( 1 * 0 < 1 * 5)
  }while( direction * currentVoltageID < direction * voltageIDtoSet);

  fCurrentVoltageInVolt = GetVoltageInVolt_AMD_NPT_family_0FH(currentVoltageID);

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
  BYTE VID = GetVoltageID_AMD_NPT_family_0FH(fCurrentVoltageInVolt);
  float fVoltageInVoltFromVID = GetVoltageInVolt_AMD_NPT_family_0FH(VID);
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

inline BYTE ChangeVoltageByMaximumVoltageStep(
  float & fCurrentVoltageInVolt,
  const BYTE byCurrentFrequencyID,
  float MaximumVoltageStepInVolt, //negative: decrease voltage
  float fVoltageInVoltToSet
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--fCurrentVoltage:"
    << fCurrentVoltageInVolt << "V current FID:"
    << (WORD) byCurrentFrequencyID
    << " MaximumVoltageStep:" << MaximumVoltageStepInVolt << "V"
    << " fVoltageToSet:" << fVoltageInVoltToSet << "V"
    )
  BYTE retVal = 0;
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
    do
    {
      fCurrentVoltageInVolt += MaximumVoltageStepInVolt;
      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
        fCurrentVoltageInVolt,
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
    )
  BYTE retVal = 0;

  //TODO calculate/ get MVS
  static float MaximumVoltageStepInVolt =
  // 32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors: Table 72. MVS Values:
  // "25 mV BIOS default."
    0.025f;

  //  "To change the processor voltage:
  //  1. Write the following values to FIDVID_CTL (MSR C001_0041h):
  //  – NewVID field (bits 13–8) with the VID code associated with the target voltage
  //  – NewFID field (bits 5–0) with the CurrFID value indicated in the FIDVID_STATUS MSR
  //  – InitFidVid bit (bit 16) set to 1. Setting this bit initiates the VID change.
  //  – Clear all other bits to 0."
//  static uint16_t StpGntTOCnt = 0;
  if( fCurrentVoltageInVolt < fVoltageInVoltToSet)
  {
//    do
//    {
//      fCurrentVoltageInVolt += MaximumVoltageStepInVolt;
//      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
//        fCurrentVoltageInVolt,
//        byCurrentFrequencyID,
//        StpGntTOCnt);
//      WaitVoltageStabilizationTime();
//    }
//    while( retVal && fCurrentVoltageInVolt <= ( fVoltageInVoltToSet -
//        MaximumVoltageStepInVolt)
//      );
    ChangeVoltageByMaximumVoltageStep(
      fCurrentVoltageInVolt,
      byCurrentFrequencyID,
      MaximumVoltageStepInVolt,
      fVoltageInVoltToSet
      );
    //if MaximumVoltageStepInVolt was too higher (higher than 1 voltage step)
    if( fCurrentVoltageInVolt < fVoltageInVoltToSet)
    {
      DEBUGN( FULL_FUNC_NAME << " fCurrentVoltageInVolt < fVoltageInVoltToSet")
      DEBUGN( FULL_FUNC_NAME << "--"
        << getBinaryRepresentation( * ((unsigned long *) & fCurrentVoltageInVolt) )
        << "--fCurrentVoltageInVolt")
      DEBUGN( FULL_FUNC_NAME << "--"
        << getBinaryRepresentation( * ((unsigned long *) & fVoltageInVoltToSet) )
        << "--fVoltageInVoltToSet"
        )
      TransitionToVoltageInSingleVIDsteps(
//        -1,
        incrementVoltage,
        fCurrentVoltageInVolt,
        fVoltageInVoltToSet,
        byCurrentFrequencyID);
    }
  }
  else if( fCurrentVoltageInVolt > fVoltageInVoltToSet )
  {
//    do
//    {
//      fCurrentVoltageInVolt -= MaximumVoltageStepInVolt;
//      retVal = SetCurrentVoltage_AMD_NPT_family_0FH(
//        fCurrentVoltageInVolt,
//        byCurrentFrequencyID,
//        StpGntTOCnt);
//      WaitVoltageStabilizationTime();
//      SetToClosestConfigurableVoltage(fCurrentVoltageInVolt);
//    }
//    while( retVal && fCurrentVoltageInVolt >=
//      //If not checking so then "current voltage" could go below
//      //fVoltageInVoltToSet.
//       (fVoltageInVoltToSet + MaximumVoltageStepInVolt) );
    ChangeVoltageByMaximumVoltageStep(
      fCurrentVoltageInVolt,
      byCurrentFrequencyID,
      MaximumVoltageStepInVolt * -1.0f,
      fVoltageInVoltToSet
      );
    if( fCurrentVoltageInVolt > fVoltageInVoltToSet)
    {
      DEBUGN( FULL_FUNC_NAME << " fCurrentVoltageInVolt > fVoltageInVoltToSet")
      DEBUGN( FULL_FUNC_NAME << "--"
        << getBinaryRepresentation( * ((unsigned long *) & fCurrentVoltageInVolt) )
        << "--fCurrentVoltageInVolt")
      DEBUGN( FULL_FUNC_NAME << "--"
        << getBinaryRepresentation( * ((unsigned long *) & fVoltageInVoltToSet) )
        << "--fVoltageInVoltToSet"
        )
      TransitionToVoltageInSingleVIDsteps(
//        -1,
//        incrementVoltageID,
        decrementVoltage,
        fCurrentVoltageInVolt,
        fVoltageInVoltToSet,
        byCurrentFrequencyID);
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--fCurrentVoltageInVolt:" << fCurrentVoltageInVolt )
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal )
  return retVal;
}

inline BYTE TransitionToVoltageRequiredForFrequencyTransition(
  float fVoltageInVoltToSet
  , float fMultiplierToSet
  , float & fCurrentVoltageInVolt
  , float fCurrentMultiplier
  , BYTE byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--voltage to set:" << fVoltageInVoltToSet
    << "V current voltage:" << fCurrentVoltageInVolt << "V")
  static BYTE MSRaccessRetVal;
  static float fVoltageToTransitionTo;

  //  "Processor driver: perform a series of VID-only transitions each increasing the
  //  voltage by the maximum voltage step (MVS) to change the voltage:
  if( fMultiplierToSet > fCurrentMultiplier)
  {
    //  • To the voltage for the requested P-state plus the voltage indicated by the ramp
    //  voltage offset (RVO), if the requested P-state is greater in frequency than the
    //  current P-state.
    fVoltageToTransitionTo = fVoltageInVoltToSet + fRampVoltageOffsetInVolt;
  }
  else if( fMultiplierToSet < fCurrentMultiplier)
  {
    //  "• To the voltage indicated by the CurrVID plus the voltage indicated by the RVO,
    //  if the requested P-state is lower in frequency than the current P-state.
    //  Software counts off voltage stabilization time (VST) after each voltage step"
    fVoltageToTransitionTo = fCurrentVoltageInVolt + fRampVoltageOffsetInVolt;
  }
  MSRaccessRetVal = TransitionVoltage(
    fCurrentVoltageInVolt,
    fVoltageToTransitionTo,
    byCurrentFrequencyID);
//  DEBUGN( FULL_FUNC_NAME << "--fCurrentVoltageInVolt: "
//    << fCurrentVoltageInVolt)
  DEBUGN( FULL_FUNC_NAME << "--return "  << (WORD) MSRaccessRetVal
    << "voltage to set:" << fVoltageInVoltToSet
    << "V current voltage:" << fCurrentVoltageInVolt << "V"
    << " current VID:" << (WORD) GetVoltageID_AMD_NPT_family_0FH(
      fCurrentVoltageInVolt)
    )
  return MSRaccessRetVal;
}

inline BYTE TransitionFrequency(
  float fVoltageInVoltToSet
  , float fMultiplierToSet
  , float fCurrentVoltageInVolt
  , BYTE byCurrentVoltageID
  , float fCurrentMultiplier
  , BYTE & byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--"
    "CurrentFID: " << (WORD) byCurrentFrequencyID
    << " fMultiplierToSet: " << fMultiplierToSet
    << " current VID:" << (WORD) byCurrentVoltageID
    )
  static BYTE MSRaccessRetVal = 0;

  if( fMultiplierToSet < fCurrentMultiplier)
  {
    MSRaccessRetVal = TransitionToLowerFrequencyID(
      fMultiplierToSet,
      byCurrentFrequencyID,
      fCurrentVoltageInVolt//,
//      StpGntTOCnt
      );
  }
  else if(fMultiplierToSet > fCurrentMultiplier)
  {
    //TODO heighten voltage before.
    MSRaccessRetVal = TransitionToHigherFrequencyID(
      fCurrentVoltageInVolt,
      fMultiplierToSet,
      byCurrentFrequencyID//,
//      fCurrentVoltageInVolt//,
//      StpGntTOCnt
      );
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) MSRaccessRetVal)
  return MSRaccessRetVal;
}

inline BYTE SetCurrentMultiplier_AMD_NPT_family_0FH(
  float fVoltageInVoltToSet
  , float fMultiplierToSet
  , float fCurrentVoltageInVolt
  , BYTE byCurrentVoltageID
  , float fCurrentMultiplier
  , BYTE byCurrentFrequencyID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--")
  static BYTE MSRaccessRetVal;
//  static uint32_t lowmostMSRbits;
//  static uint32_t highmostMSRbits;

//  CalculateTargetVoltageID();

//  10.5.7.2 P-state Transition Algorithm
//  "The P-state transition algorithm has three phases.
//  1 During phase 1 the processor voltage is transitioned
//    to the level required to support frequency transitions.
  TransitionToVoltageRequiredForFrequencyTransition(
    fVoltageInVoltToSet
    , fMultiplierToSet
    , fCurrentVoltageInVolt
    , fCurrentMultiplier
    , byCurrentFrequencyID
    );
//  BYTE byCurrentFrequencyID;
//  BYTE byCurrentVoltageID;
  GetCurrentVoltageIDAndFrequencyID_AMD_NPT_family_0Fh(
    byCurrentVoltageID, byCurrentFrequencyID);
  DEBUGN( FULL_FUNC_NAME << "--after TransitionToVoltageRequiredForFrequency"
    "Transition--curr VID:" << (WORD) byCurrentVoltageID << " curr FID:"
    << (WORD) byCurrentFrequencyID )
//  2 During phase 2 the processor frequency is transitioned to frequency
//    associated with the OS-requested P-state.
  TransitionFrequency(
    fVoltageInVoltToSet
    , fMultiplierToSet
    , fCurrentVoltageInVolt
    , byCurrentVoltageID
    , fCurrentMultiplier
    , byCurrentFrequencyID
    );
//  3 During phase 3 the processor voltage is transitioned to the voltage
//    associated with the OS-requested P-state."
  MSRaccessRetVal = TransitionVoltage(
    fCurrentVoltageInVolt,
    fVoltageInVoltToSet,
    byCurrentFrequencyID);

//  MSRaccessRetVal = WriteMSR(FIDVID_CTL_MSR_ADDRESS, lowmostMSRbits,
//    highmostMSRbits, 1);
  return MSRaccessRetVal;
}

inline BYTE
  SetCurrentVoltageAndMultiplier_AMD_NPT_family_0FH(
    float fVoltageInVoltToSet
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplierToSet
    , WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--should set" << fVoltageInVoltToSet
    << "V, multiplier:" << fMultiplierToSet)
  DEBUGN_LOGGER_NAME(g_windows_api_logger, FULL_FUNC_NAME << "--begin--"
    << fVoltageInVoltToSet << "V, " << fMultiplierToSet)
  float fCurrentVoltageInVolt;
  float fCurrentMultiplier;
  float fCurrentReferenceClockInMHz;

  BYTE byCurrentFrequencyID, byCurrentVoltageID;
  BYTE MSRaccessRetVal = GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
    & fCurrentVoltageInVolt,
    & fCurrentMultiplier,
    & fCurrentReferenceClockInMHz,
    wCoreID,
    byCurrentFrequencyID,
    byCurrentVoltageID);
  DEBUGN( FULL_FUNC_NAME << "--fCurrentMultiplier:" << fCurrentMultiplier
    << " fCurrentVoltageInVolt:" << fCurrentVoltageInVolt)
  if( fCurrentMultiplier == fMultiplierToSet)
  {
//    SetCurrentVoltage_AMD_NPT_family_0FH(
//      fVoltageInVoltToSet,
//      byCurrentFrequencyID,
//      0);
    TransitionVoltage(
      fCurrentVoltageInVolt,
      fVoltageInVoltToSet,
      byCurrentFrequencyID);  }
  else
    SetCurrentMultiplier_AMD_NPT_family_0FH(
      fVoltageInVoltToSet,
      fMultiplierToSet,
      fCurrentVoltageInVolt,
      byCurrentVoltageID,
      fCurrentMultiplier,
      byCurrentFrequencyID);
  return MSRaccessRetVal;
}


#endif /* AMD_NPT_FAMILY_0FH_SETVOLTAGEANDMULTI_HPP_ */
