/*
 * AMD_K7_SetMulti.hpp
 *
 *  Created on: 23.11.2012
 *      Author: Stefan
 */

#ifndef AMD_K7_SETMULTI_HPP_
#define AMD_K7_SETMULTI_HPP_

inline BYTE TransitionFrequencyID_StepByStep(
//  float fCurrentVoltageInVolt
  BYTE currentVoltageID
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
    retVal = SetVIDorFID_AMD_NPT_family_0FH(
//      fCurrentVoltageInVolt,
      currentVoltageID,
      byCurrentFrequencyID,
      s_StpGntTOCnt
      );
    WaitIsochronousReliefTime();
  } while( direction * byCurrentFrequencyID < direction * TargetFrequencyID);
  return retVal;
}

inline BYTE TransitionToLowerFrequencyID_StepByStep(
//  float fCurrentVoltageInVolt
  BYTE currentVoltageID
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
      retVal = SetVIDorFID_AMD_NPT_family_0FH(
//        fCurrentVoltageInVolt,
        currentVoltageID,
        byCurrentFrequencyID,
        s_StpGntTOCnt
        );
      WaitIsochronousReliefTime();
    } while(byCurrentFrequencyID > TargetFrequencyID);
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
  return retVal;
}

/** This function uses VCO portal frequencies to transition faster.
* s_minimumFID must have been set before  */
//inline BYTE PossibyTransitionToMinimumFID(
//  BYTE & byCurrentFrequencyID,
//  const BYTE TargetFID,
////  float fCurrentVoltageInVolt
//  const BYTE currentVoltageID
//  )
//{
//  BYTE retVal = 1;
//  const MinAndMaxFID & minandmaxfidForMinimumFID =
//    s_ar_minandmaxfid[s_minimumFID];
////  BYTE VID = GetVoltageID_AMD_NPT_family_0FH(fCurrentVoltageInVolt);
//  // but where stands the min FID? e.g. min FID can be = FID for 1400 MHz
//  if( TargetFID == s_minimumFID
//      && byCurrentFrequencyID //> 7 // > 1600 MHz
//      >= minandmaxfidForMinimumFID.m_maxFID
//    )
//  {
//      //s_ar_minandmaxfidVCO_FIDtoLowerFID[targetFID - 8];
//    if( byCurrentFrequencyID //> 7 // > 1600 MHz
//        >= minandmaxfidForMinimumFID.m_maxFID
//      )
//    {
//      retVal = TransitionToLowerFrequencyID_StepByStep(
//  //      fVoltageInVoltToSet,
////        fCurrentVoltageInVolt,
//        currentVoltageID,
//        byCurrentFrequencyID,
//        minandmaxfidForMinimumFID.m_maxFID
//        );
//    }
//    //Now we are at max FID for portal frequency to minimum FID.
//    if( retVal )
//    {
//      //Do the change from high FID frequency table
//      // (minandmaxfidForMinimumFID.m_maxFID) to low FID frequency table
//      // (s_minimumFID).
//      // (byCurrentFrequencyID - s_minimumFID > 2)
//      retVal = SetVIDorFID_AMD_NPT_family_0FH(
////        VID,
//        currentVoltageID,
//        s_minimumFID,
//        s_StpGntTOCnt);
//      WaitIsochronousReliefTime();
//      //Now we are at minimum FID.
//    }
//  //    const MinAndMaxFID & minandmaxfid = s_ar_minandmaxfidVCO_FIDtoLowerFID[
//  //      byCurrentFrequencyID - 8];
//  //    if( TargetFID >= minandmaxfid.m_minFID && TargetFID <=
//  //        minandmaxfid.m_maxFID)
//  //    {
//  //    }
////    }
////    else
////  //"Is TargetFID in the “High FID Frequency Table”?"
////  if( TargetFID > 7
////      //1
////      )
//  }
//  return retVal;
//}

BYTE TransitionToLowerFrequencyID(
  float fMultiplierToSet,
  BYTE & byCurrentFrequencyID,
  //When the FID is changed the VID is not allowed to change.
  const float fCurrentVoltageInVolt
//  const BYTE currentVoltageID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin--byCurrentFrequencyID:"
    << (WORD) byCurrentFrequencyID << " ")
  BYTE retVal = 1;
  BYTE FinalFID = GetFrequencyID_AMD_K7(fMultiplierToSet);
  const BYTE currentVoltageID = GetVoltageID_AMD_K7(
    fCurrentVoltageInVolt);

#ifdef ALLOW_ODD_FIDS_FOR_STEPPING_NUMBER_BELOW_3
  BYTE TargetFID = FinalFID;
#else
  BYTE TargetFID = FinalFID % 2 == 1 ? // odd FID?
    FinalFID - 1 : FinalFID; // make even or already even FID
#endif
  DEBUGN( FULL_FUNC_NAME << "--begin--FinalFID:"
    << (WORD) FinalFID << " TargetFID:" << (WORD) TargetFID)

#ifdef USE_STARTUP_FID_AS_MIN_FID
  retVal = PossibyTransitionToMinimumFID(
    byCurrentFrequencyID,
    TargetFID,
//    fCurrentVoltageInVolt
    currentVoltageID
    );
#endif //#ifdef USE_STARTUP_FID_AS_MIN_FID
  if( byCurrentFrequencyID > TargetFID)
  {
    retVal = TransitionToLowerFrequencyID_StepByStep(
//      fVoltageInVoltToSet,
//      fCurrentVoltageInVolt,
      currentVoltageID,
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
//        retVal = SetVIDorFID_AMD_NPT_family_0FH(
//          fCurrentVoltageInVolt,
//          byCurrentFrequencyID,
//          s_StpGntTOCnt);
//        WaitIsochronousReliefTime();
//      } while( retVal && TargetFIDisBelowVCOportal(byCurrentFrequencyID) );
//    }
//    else
//    {
//      retVal = SetVIDorFID_AMD_NPT_family_0FH(
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
  // but where stands the min FID? e.g. min FID can be = FID for 1400 MHz
#ifdef USE_STARTUP_FID_AS_MIN_FID
  if( byCurrentFrequencyID == s_minimumFID )
#endif //#ifdef USE_STARTUP_FID_AS_MIN_FID
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
//  float fVoltageInVoltToSet
  const BYTE currentVoltageID
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
    retVal = SetVIDorFID_AMD_NPT_family_0FH(
//      fVoltageInVoltToSet,
      currentVoltageID,
      byCurrentFrequencyID,
      s_StpGntTOCnt);
    WaitIsochronousReliefTime();
  } while( retVal && byCurrentFrequencyID < TargetFrequencyID);

  if( byCurrentFrequencyID < TargetFrequencyID )
  {
    DEBUGN( FULL_FUNC_NAME << "--byCurrentFrequencyID < TargetFrequencyID" )
    ++ byCurrentFrequencyID;
    retVal = SetVIDorFID_AMD_NPT_family_0FH(
//      fVoltageInVoltToSet,
      currentVoltageID,
      byCurrentFrequencyID,
      s_StpGntTOCnt);
    WaitIsochronousReliefTime();
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
  const BYTE VoltageID = GetVoltageID_AMD_NPT_family_0FH(fVoltageInVoltToSet);
  BYTE TargetFID = FinalFID % 2 == 1 ? FinalFID - 1 : FinalFID;
  DEBUGN( FULL_FUNC_NAME
    << "--TargetFID:" << (WORD) TargetFID
    << "current FID: " << (WORD) byCurrentFrequencyID )

//  //"Is CurrentFID in the “High FID Frequency Table”?"
//  if( byCurrentFrequencyID > 7
//      //1
//      )
//  {
//    retVal = TransitionToHigherFrequencyID_StepByStep(
//      fVoltageInVoltToSet,
//      byCurrentFrequencyID,
//      TargetFID);
//  }
//  else
//  {
#ifdef USE_STARTUP_FID_AS_MIN_FID
  //TODO: only transition to VCO freq if byCurrentFrequencyID = minimum FID?
   //    but where stands the min FID? e.g. min FID can be = FID for 1400 MHz
    if( byCurrentFrequencyID == s_minimumFID )
    {
      BYTE VCOportalFrequencyID = GetVCOportalFrequencyID(
        byCurrentFrequencyID,
        TargetFID);
      //"Is TargetFID above VCO portal of CurrentFID?"
      if( VCOportalFrequencyID != 255 )
      {
        //"FID Transition Process To highest portal FID in “High FID Frequency
        // Table”"
        retVal = SetVIDorFID_AMD_NPT_family_0FH(
//          fVoltageInVoltToSet,
          VoltageID,
          VCOportalFrequencyID,
          s_StpGntTOCnt);
        WaitIsochronousReliefTime();
        if( retVal )
          byCurrentFrequencyID = VCOportalFrequencyID;
      }
      if( byCurrentFrequencyID < TargetFID )
#endif //USE_STARTUP_FID_AS_MIN_FID
      {
  //      byCurrentFrequencyID = VCOportalFrequencyID;
        TransitionToHigherFrequencyID_StepByStep(
//          fVoltageInVoltToSet,
          VoltageID,
  //        VCOportalFrequencyID,
          byCurrentFrequencyID,
          TargetFID);
        //"FID Transition Process To TargetFID"
      }
#ifdef USE_STARTUP_FID_AS_MIN_FID
  }
#endif //USE_STARTUP_FID_AS_MIN_FID
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) retVal)
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

  //Cites from "BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors", document number 32559 Rev. 3.16 November 2009:
  // , "Figure 8. High-Level P-state Transition Flow"
  //"Processor driver: perform a series of VID-only transitions each increasing
  // the voltage by the maximum voltage step (MVS) to change the voltage:"

  //TODO idea: avoid setting a voltage that is too high(if lowest multi then
  //do not set to the max voltage)
  if( fMultiplierToSet > fCurrentMultiplier)
  {
    fVoltageToTransitionTo =
    //"• To the voltage for the requested P-state plus"
      fVoltageInVoltToSet +
      //"the voltage indicated by the ramp voltage offset (RVO),"
      fRampVoltageOffsetInVolt;
    //"if the requested P-state is greater in frequency than the current
    //P-state."
  }
  else if( fMultiplierToSet < fCurrentMultiplier)
  {
    fVoltageToTransitionTo =
    //"• To the voltage indicated by the CurrVID plus"
      fCurrentVoltageInVolt +
    //"the voltage indicated by the RVO,"
      fRampVoltageOffsetInVolt;
    // "if the requested P-state is lower in frequency than the current P-state.
  }
  //Max exceed max voltage when the ramp voltage offset is added to a voltage.
  if( fVoltageToTransitionTo > s_maxVoltageInVolt)
  {
    DEBUGN( FULL_FUNC_NAME << " setting to max voltage")
    fVoltageToTransitionTo = s_maxVoltageInVolt;
  }
  //"Software counts off voltage stabilization time (VST) after each voltage
  // step"
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

inline BYTE SetCurrentMultiplier_AMD_K7(
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

//  "10.5.7.2 P-state Transition Algorithm" :
//  "The P-state transition algorithm has three phases.
//  1 During phase 1 the processor voltage is transitioned
//    to the level required to support frequency transitions."
  TransitionToVoltageRequiredForFrequencyTransition(
    fVoltageInVoltToSet
    , fMultiplierToSet
    , fCurrentVoltageInVolt
    , fCurrentMultiplier
    , byCurrentFrequencyID
    );
//  BYTE byCurrentFrequencyID;
//  BYTE byCurrentVoltageID;
  GetCurrentVoltageIDAndFrequencyID_AMD_K7(
    byCurrentVoltageID, byCurrentFrequencyID);
  DEBUGN( FULL_FUNC_NAME << "--after TransitionToVoltageRequiredForFrequency"
    "Transition--curr VID:" << (WORD) byCurrentVoltageID << " curr FID:"
    << (WORD) byCurrentFrequencyID )
//  "2 During phase 2 the processor frequency is transitioned to frequency
//    associated with the OS-requested P-state."
  TransitionFrequency(
    fVoltageInVoltToSet
    , fMultiplierToSet
    , fCurrentVoltageInVolt
    , byCurrentVoltageID
    , fCurrentMultiplier
    , byCurrentFrequencyID
    );
//  "3 During phase 3 the processor voltage is transitioned to the voltage
//    associated with the OS-requested P-state."
  MSRaccessRetVal = TransitionVoltage(
    fCurrentVoltageInVolt,
    fVoltageInVoltToSet,
    byCurrentFrequencyID);

//  MSRaccessRetVal = WriteMSR(FIDVID_CTL_MSR_ADDRESS, lowmostMSRbits,
//    highmostMSRbits, 1);
  return MSRaccessRetVal;
}

#endif /* AMD_K7_SETMULTI_HPP_ */
