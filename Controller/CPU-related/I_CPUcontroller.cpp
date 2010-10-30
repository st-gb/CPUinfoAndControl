#include "I_CPUcontroller.hpp"
//#include <windef.h> //for BYTE
#include <Controller/CPUindependentHelper.h>
#include <Controller/I_CPUaccess.hpp>
#include <Controller/IDynFreqScalingAccess.hpp>
//#include <Controller/tchar_conversion.h> //for GetCharPointer(...)
#include <Controller/character_string/stdtstr.hpp> //get...
#include <ModelData/ModelData.hpp> //class Model
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <UserInterface/UserInterface.hpp>
#include <preprocessor_macros/Windows_compatible_typedefs.h>

#ifdef COMPILE_WITH_XERCES
  #include "Xerces/XMLAccess.hpp" //for "ReadXMLfileInitAndTermXerces(...)"
  #include <Xerces/SAX2MainConfigHandler.hpp>
#endif

#ifndef MAXWORD
  #define MAXWORD 65535
#endif

I_CPUcontroller::I_CPUcontroller()
  :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_fReferenceClockInMHz(0)
  //1 power plane for all CPU cores is usual->set as default.
  , m_b1CPUcorePowerPlane( true )
//  , mp_cpuaccess (NULL)
  , mp_userinterface (NULL)
  , mp_model (NULL)
#ifdef COMPILE_WITH_CALC_THREAD
  , mp_calculationthread (NULL)
#endif //#ifdef COMPILE_WITH_CALC_THREAD
  , mp_dynfreqscalingaccess (NULL)
  , mp_icpucoreusagegetter (NULL)
{
}

BYTE I_CPUcontroller::CheckWhetherVoltageIsAboveDefaultVoltage(
  float fVoltageInVolt, float fCPUcoreFrequencyinMHz)
{
  BYTE bVoltageIsValid = //not_above ;
    in_safe_range ;
  float fInterpolatedVoltage ;
  if( //mp_cpucontroller->GetInterpolatedVoltageFromFreq(
    GetInterpolatedVoltageFromFreq(
      (WORD) fCPUcoreFrequencyinMHz ,
      fInterpolatedVoltage ,
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
      )
    )
  {
    LOGN("VoltageIsValid--interpolated voltage:" << fInterpolatedVoltage )
    if(//Voltage to set is _above_ default voltage (-> too high/ dangerous).
      fVoltageInVolt > fInterpolatedVoltage )
    {
      LOGN("voltage is above default voltage-> invalid")
      bVoltageIsValid = //VoltageIsAboveDefaultVoltage ;
        VoltageIsOutsideSafeRange ;
    }
  }
  else
    bVoltageIsValid = No2PstatesForVoltageInterpolationFound ;
  return bVoltageIsValid ;
}

BYTE I_CPUcontroller::CheckWhetherVoltageIsBelowLowestStableVoltage(
  float fVoltageInVolt, float fCPUcoreFrequencyinMHz)
{
  BYTE bVoltageIsValid = //not_below ;
    in_safe_range ;
  float fInterpolatedVoltage ;
  if( //mp_cpucontroller->GetInterpolatedVoltageFromFreq(
    GetInterpolatedVoltageFromFreq(
      (WORD) fCPUcoreFrequencyinMHz ,
      fInterpolatedVoltage ,
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable
      )
    )
  {
    LOGN("VoltageIsValid--interpolated voltage:" << fInterpolatedVoltage )
    if(//Voltage to set is _below_ lowest settable voltage (-> too low/
      //device may freeze/ malfunction/ can cause wrong CPU cache content).
      fVoltageInVolt < fInterpolatedVoltage )
    {
      LOGN("voltage is below lowest stable voltage-> invalid")
      bVoltageIsValid = //VoltageIsBelowLowestStableVoltage ;
        VoltageIsOutsideSafeRange ;
    }
  }
  else
    bVoltageIsValid = No2PstatesForVoltageInterpolationFound ;
  return bVoltageIsValid ;
}

//This CPU instruction (in contrast to wrmsr) is not dangerous.
//So it can be implemented for every CPU controller in this base class.
BOOL I_CPUcontroller::CpuidEx(
  DWORD dwIndex,
  PDWORD p_dwEAX,
  PDWORD p_dwEBX,
  PDWORD p_dwECX,
  PDWORD p_dwEDX,
  DWORD_PTR affinityMask
)
{
  //May be NULL.
  if( mp_cpuaccess )
  {
    return mp_cpuaccess->CpuidEx(
      dwIndex,
      p_dwEAX,
      p_dwEBX,
      p_dwECX,
      p_dwEDX,
      affinityMask
    ) ;
  }
  return FALSE ;
}


BYTE I_CPUcontroller::DisableFrequencyScalingByOS() 
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
  return 0 ;
}

//BYTE I_CPUcontroller::EnableOwnDVFS()
//{
//  LOGN("Enable own Dynamic Voltage and Frequency Scaling--ptrs:" <<
//    mp_model << mp_dynfreqscalingaccess )
//  if( mp_model && mp_dynfreqscalingaccess )
//  {
//    LOGN("Should enable Dynamic Voltage and Frequency Scaling?:" <<
//      mp_model->m_cpucoredata.m_bEnableDVFS )
//    if( mp_model->m_cpucoredata.m_bEnableDVFS )
//    {
//      if( mp_dynfreqscalingaccess->OtherDVFSisEnabled()
//        )
//        DisableFrequencyScalingByOS();
//      PerCPUcoreAttributes * p_percpucoreattributes = ModelDataModelData
//        & mp_model->m_cpucoredata.
//        m_arp_percpucoreattributes[ //p_atts->m_byCoreID
//        0 ] ;
//      #ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
//      LOGN("freq scaling thread:" << p_percpucoreattributes->mp_dynfreqscalingthread )
//      //Keep away the dependance on Logger class for dyn libs.
//      //DynFreqScalingThread * p_dynfreqscalingthread
//      if ( ! p_percpucoreattributes->mp_dynfreqscalingthread )
//      {
//        LOGN("Other Dynamic Voltage and Frequency Scaling (register write)"
//          " is enabled?:" << mp_dynfreqscalingaccess->OtherDVFSisEnabled()
//          )
//        if( ! mp_dynfreqscalingaccess->OtherDVFSisEnabled()
//          )
//        {
//          //p_percpucoreattributes->mp_dynfreqscalingthread
//          p_percpucoreattributes->SetCPUcontroller( this ) ;
//          p_percpucoreattributes->CreateDynFreqScalingThread(
//            mp_icpucoreusagegetter
//            ) ;
//          return 1 ;
//        }
//      }
//      #endif
//    }
//  }
//  return 0 ;
//}

float I_CPUcontroller::GetCPUcoreFrequencyInMHz( WORD wMultiplierIndex )
{
  float fCPUcoreFrequencyInMHz = 0 ;
  if( mp_model && m_fReferenceClockInMHz )
  {
//    std::set<float>::const_iterator c_iter_stdset_fAvaiableMultipliers =
//      mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.find(
//        wMultiplierIndex ) ;
    std::set<float>::const_iterator c_iter_stdset_fAvaiableMultipliers =
          mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.begin() ;
    for( WORD wCurrentMultiplierIndex = 0 ;
      c_iter_stdset_fAvaiableMultipliers !=
          mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.end() &&
      wCurrentMultiplierIndex < wMultiplierIndex ;
      ++ c_iter_stdset_fAvaiableMultipliers , ++ wCurrentMultiplierIndex
      )
    {

    }
    if (c_iter_stdset_fAvaiableMultipliers !=
        mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.end()
        )
    {
      fCPUcoreFrequencyInMHz = *c_iter_stdset_fAvaiableMultipliers *
          m_fReferenceClockInMHz ;
    }
  }
  return fCPUcoreFrequencyInMHz ;
}

//Stores multiplier, reference clock and voltage into the model data.
BYTE I_CPUcontroller::GetCurrentVoltageAndFrequencyAndStoreValues(
  WORD wCoreID )
{
  PerCPUcoreAttributes * arp_percpucoreattributes = mp_model->m_cpucoredata.
    m_arp_percpucoreattributes  ;
  return GetCurrentVoltageAndFrequency(
    arp_percpucoreattributes[wCoreID].m_fVoltageInVolt,
    arp_percpucoreattributes[wCoreID].m_fMultiplier,
    arp_percpucoreattributes[wCoreID].m_fReferenceClockInMhz ,
    wCoreID ) ;
}

void I_CPUcontroller::GetCurrentTemperatureInCelsiusAndStoreValues(
  WORD wCoreID )
{
  PerCPUcoreAttributes * arp_percpucoreattributes = mp_model->m_cpucoredata.
    m_arp_percpucoreattributes  ;
  arp_percpucoreattributes[wCoreID].m_fTempInDegCelsius =
    GetTemperatureInCelsius(wCoreID) ;
}

// returns: true: p-state with freq >= wanted freq and 
//    p-state with freq <= wanted freq was found and
//    voltage could be interpolated 
BYTE I_CPUcontroller::GetInterpolatedVoltageFromFreq(
  WORD wFreqInMHzToGetVoltageFrom
  , float & r_fVoltageInVolt 
  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
  )
{
  LOGN("GetInterpolatedVoltageFromFreq("
#ifdef _DEBUG
    << wFreqInMHzToGetVoltageFrom
#endif
    << ", ..." )
  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq = 
    r_stdsetvoltageandfreq.begin() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.end() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestHigherEqual = r_stdsetvoltageandfreq.end() ;
  while( ci_stdsetvoltageandfreq != r_stdsetvoltageandfreq.end() )
  {
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz <= wFreqInMHzToGetVoltageFrom )
      ci_stdsetvoltageandfreqNearestLowerEqual = ci_stdsetvoltageandfreq ;
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz >= wFreqInMHzToGetVoltageFrom )
    {
      ci_stdsetvoltageandfreqNearestHigherEqual = ci_stdsetvoltageandfreq ;
      //The entries are sorted ascending by frequency. So break to avoid 
      //assigning a higher value (that would not be the nearest higher freq
      //any more).
      break ;
    }
    ++ ci_stdsetvoltageandfreq ;
  }
//  LOGN("GetInterpolatedVoltageFromFreq(...) after loop")
#ifdef COMPILE_WITH_LOG
  std::ostringstream ostringstreamLog ;
  ostringstreamLog << "GetInterpolatedVoltageFromFreq("
    << wFreqInMHzToGetVoltageFrom
    << ", ...)--lower freq:" ;
  if( ci_stdsetvoltageandfreqNearestLowerEqual != r_stdsetvoltageandfreq.end()
    )
  {
    ostringstreamLog << ci_stdsetvoltageandfreqNearestLowerEqual->
      m_wFreqInMHz ;
  }
  else
    ostringstreamLog << " no lower freq than " << wFreqInMHzToGetVoltageFrom ;
  ostringstreamLog << ", ...)--higher freq:" ;
  if( ci_stdsetvoltageandfreqNearestHigherEqual != r_stdsetvoltageandfreq.end()
    )
    ostringstreamLog << ci_stdsetvoltageandfreqNearestHigherEqual->
      m_wFreqInMHz ;
  else
    ostringstreamLog << " no higher freq than " << wFreqInMHzToGetVoltageFrom ;
#endif //#ifdef COMPILE_WITH_LOG
  LOGN( ostringstreamLog.str() )
  if( ci_stdsetvoltageandfreqNearestLowerEqual != r_stdsetvoltageandfreq.end()
    && ci_stdsetvoltageandfreqNearestHigherEqual != 
      r_stdsetvoltageandfreq.end()
    )
  {
    if( //This is the case if wFreqInMHzToGetVoltageFrom has the 
      //same freq as one of the p-states.
      //This case must be catched, else wrong values by the 
      //log_x() function (  log_1(1)  is calculated then ) .
      //p_pstateGreaterEqual == p_pstateLowerEqual 
      ci_stdsetvoltageandfreqNearestLowerEqual == 
      ci_stdsetvoltageandfreqNearestHigherEqual
      )
    {
      //r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
      r_fVoltageInVolt = ci_stdsetvoltageandfreqNearestLowerEqual->
        m_fVoltageInVolt ;
//      DEBUGN("I_CPUcontroller::GetInterpolatedVoltageFromFreq()"
//        "voltage found: " << r_fVoltageInVolt )
      return true ;
    }
    else
    {
      WORD wFreqInMHzFromNearFreqAboveWantedFreq =
        //p_pstateGreaterEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
      WORD wFreqInMHzFromNearFreqBelowWantedFreq =
        //p_pstateLowerEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
    //if( mp_model->m_pstates.m_arp_pstate[1] &&
    //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
    //  wFreqInMHzToGetVoltageFrom
    //  )
    //{
      float fVoltageInVoltFromNearFreqAboveWantedFreq ;
      float fVoltageInVoltFromNearFreqBelowWantedFreq ;
      fVoltageInVoltFromNearFreqAboveWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
        //p_pstateGreaterEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->
          m_fVoltageInVolt ;
      fVoltageInVoltFromNearFreqBelowWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
        //p_pstateLowerEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->
          m_fVoltageInVolt ;
      //example: 1.0625 V - 0.85V = 0,2125 V
      float fVoltageFromFreqAboveAndBelowDiff = 
        fVoltageInVoltFromNearFreqAboveWantedFreq -
        fVoltageInVoltFromNearFreqBelowWantedFreq ;
      WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
        wFreqInMHzFromNearFreqAboveWantedFreq -
       wFreqInMHzFromNearFreqBelowWantedFreq ;
      float fVoltagePerMHz = 
        fVoltageFromFreqAboveAndBelowDiff /
          wFreqInMHzFromNearFreqsWantedFreqDiff ;
      WORD wWantedFreqMinusFreqBelow = 
        (wFreqInMHzToGetVoltageFrom - 
        wFreqInMHzFromNearFreqBelowWantedFreq
         ) ;

      //example: 2200 MHz / 1050 MHz ~= 2,0952380952
//      double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
//          (double) wFreqInMHzFromNearFreqAboveWantedFreq /
//          (double) wFreqInMHzToGetVoltageFrom ;

      r_fVoltageInVolt =
        ////fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //// 1,0625 - 0,22676176661993910555202664192874
        ////  = 0,835738233380060894447973358072
        //fVoltageInVoltFromNearFreqAboveWantedFreq -
        //  //log10(2,0952380952380952380952380952381) =
        //  //  0,32123338175226816317043359884426
        //  //log10(2) = 0,30102999566398119521373889472449
        //  // => log2(2,0952380952380952380952380952381) = 
        //  //  0,32123338175226816317043359884426 / 
        //  //  0,30102999566398119521373889472449 
        //  //  = 1,067114195858536967303654785547
        //  // 1,067114195858536967303654785547 * 0,2125 =
        //  //   0,22676176661993910555202664192874
        //  //log_dualis
        //  log_x
        //  (
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzFromNearFreqBelowWantedFreq
        //    ,
        //  //wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //    //example: 2200 MHz / 1050 MHz ~= 2.0952380952
        //    //example: 2200 MHz / 550 MHz = 4
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzToGetVoltageFrom
        //  )
        //  *
        //  //fVoltageDiffBetwOvVoltProtVnf_pairs
        //  (
        //    //example: 1.0625 V - 0.85V = 0,2125 V
        //    //example: 1.0625 V - 0.75V = 0.3125 V
        //    fVoltageInVoltFromNearFreqAboveWantedFreq -
        //    fVoltageInVoltFromNearFreqBelowWantedFreq
        //  )
        fVoltageInVoltFromNearFreqBelowWantedFreq +
        fVoltagePerMHz * (float) wWantedFreqMinusFreqBelow
        ;
//      LOGN("GetInterpolatedVoltageFromFreq("
//        << "\nnearest freq from std::set below:"
//        << wFreqInMHzFromNearFreqBelowWantedFreq
//        << "\nnearest freq from std::set above:"
//        << wFreqInMHzFromNearFreqAboveWantedFreq
//        << "\nabove - below freq=" << wFreqInMHzFromNearFreqsWantedFreqDiff
//        << "\nabove - below voltage=" << fVoltageFromFreqAboveAndBelowDiff
//        << "\nvoltage/MHz(voltage above-below/freq above-below)="
//        << std::ios::fixed << fVoltagePerMHz
//        << "\nwanted freq - freq below=" << wWantedFreqMinusFreqBelow
//        << "\ninterpolated voltage (voltage below+voltage/MHz*"
//            "\"wanted freq - freq below\"=" << r_fVoltageInVolt
//        )
      return true ;
    }
  }
  return false ;
}

//BYTE I_CPUcontroller::GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt 
//  , std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  )
//{
//  //WORD wFreqInMHzFromNearFreqAboveWantedFreq =
//    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() ;
//  //PState * p_pstateGreaterEqual = mp_model->m_pstates.
//  //  GetPstateWithNearestFreqGreaterEqual( wFreqInMHzToGetVoltageFrom , 4) ;
//  ////mp_model->
//  //PState * p_pstateLowerEqual = mp_model->m_pstates.
//  //  GetPstateWithNearestFreqLowerEqual( wFreqInMHzToGetVoltageFrom, 4 ) ;
//  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq = 
//    r_stdsetvoltageandfreq.begin() ;
//  std::set<VoltageAndFreq>::const_iterator 
//    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.
//    //return element with value <= wFreqInMHzToGetVoltageFrom
//    lower_bound (VoltageAndFreq(1.0, wFreqInMHzToGetVoltageFrom) 
//    ) ;
//  std::set<VoltageAndFreq>::const_iterator 
//    ci_stdsetvoltageandfreqNearestHigher = r_stdsetvoltageandfreq.
//    //return element with value > wFreqInMHzToGetVoltageFrom
//    upper_bound(VoltageAndFreq(1.0, wFreqInMHzToGetVoltageFrom) 
//    ) ;
//
//  std::set<VoltageAndFreq>::const_reverse_iterator 
//    c_rev_iter_stdsetvoltageandfreqNearestLowerEqual =
//    ci_stdsetvoltageandfreqNearestHigher ;
//  ++ c_rev_iter_stdsetvoltageandfreqNearestLowerEqual ;
//
//  if( //ci_stdsetvoltageandfreqNearestLowerEqual != 
//      //r_stdsetvoltageandfreq.end()
//    c_rev_iter_stdsetvoltageandfreqNearestLowerEqual !=
//      r_stdsetvoltageandfreq.rend()
//    )
//  {
//    if( ci_stdsetvoltageandfreqNearestHigher != 
//        r_stdsetvoltageandfreq.end()
//      )
//    {
//      if( //This is the case if wFreqInMHzToGetVoltageFrom has the 
//        //same freq as one of the p-states.
//        //This case must be catched, else wrong values by the 
//        //log_x() function (  log_1(1)  is calculated then ) .
//        //p_pstateGreaterEqual == p_pstateLowerEqual 
//        //ci_stdsetvoltageandfreqNearestLowerEqual == 
//        c_rev_iter_stdsetvoltageandfreqNearestLowerEqual ==
//        ci_stdsetvoltageandfreqNearestHigher
//        )
//      {
//        //r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
//        r_fVoltageInVolt = //ci_stdsetvoltageandfreqNearestLowerEqual->
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->
//          m_fVoltageInVolt ;
//        return true ;
//      }
//      else
//      {
//        WORD wFreqInMHzFromNearFreqAboveWantedFreq =
//          //p_pstateGreaterEqual->GetFreqInMHz() ;
//          ci_stdsetvoltageandfreqNearestHigher->m_wFreqInMHz ;
//        WORD wFreqInMHzFromNearFreqBelowWantedFreq =
//          //p_pstateLowerEqual->GetFreqInMHz() ;
//          //ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//      //if( mp_model->m_pstates.m_arp_pstate[1] &&
//      //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
//      //  wFreqInMHzToGetVoltageFrom
//      //  )
//      //{
//        float fVoltageInVoltFromNearFreqAboveWantedFreq ;
//        float fVoltageInVoltFromNearFreqBelowWantedFreq ;
//        fVoltageInVoltFromNearFreqAboveWantedFreq =
//          //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
//          //p_pstateGreaterEqual->GetVoltageInVolt() ;
//          ci_stdsetvoltageandfreqNearestHigher->
//            m_fVoltageInVolt ;
//        fVoltageInVoltFromNearFreqBelowWantedFreq =
//          //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
//          //p_pstateLowerEqual->GetVoltageInVolt() ;
//          //ci_stdsetvoltageandfreqNearestLowerEqual->
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->
//            m_fVoltageInVolt ;
//        //example: 1.0625 V - 0.85V = 0,2125 V
//        float fVoltageFromFreqAboveAndBelowDiff = 
//          fVoltageInVoltFromNearFreqAboveWantedFreq -
//          fVoltageInVoltFromNearFreqBelowWantedFreq ;
//        WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
//          wFreqInMHzFromNearFreqAboveWantedFreq -
//         wFreqInMHzFromNearFreqBelowWantedFreq ;
//        float fIncrease = 
//          fVoltageFromFreqAboveAndBelowDiff /
//            wFreqInMHzFromNearFreqsWantedFreqDiff ;
//        WORD wWantedFreqMinusFreqBelow = 
//          (wFreqInMHzToGetVoltageFrom - 
//          wFreqInMHzFromNearFreqBelowWantedFreq
//           ) ;
//
//        //example: 2200 MHz / 1050 MHz ~= 2,0952380952
//        double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
//            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
//            (double) wFreqInMHzToGetVoltageFrom ;
//
//        r_fVoltageInVolt =
//          fVoltageInVoltFromNearFreqBelowWantedFreq +
//          fIncrease * (float) wWantedFreqMinusFreqBelow
//          ;
//        return true ;
//      }
//    }
//    else
//      return ci_stdsetvoltageandfreqNearestLowerEqual->m_fVoltageInVolt ;
//  }
//  return false ;
//}

float I_CPUcontroller::GetMaximumVoltageInVolt()
{
  std::set<VoltageAndFreq>::const_reverse_iterator r_iter = 
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.rbegin( ) ;
  if( r_iter != mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.rend() )
    return r_iter->m_fVoltageInVolt ;
  return - 1.0 ;
}

WORD I_CPUcontroller::GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld)
{
  std::set<VoltageAndFreq>::const_iterator iter = 
    mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.begin( ) ;
    m_stdsetvoltageandfreqAvailableFreq.begin( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  while( iter != 
    mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.
    m_stdsetvoltageandfreqAvailableFreq.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
  {
    if( iter->m_wFreqInMHz > wFreqInMhzOld )
    {
      DEBUGN("GetNearestHigherPossibleFreqInMHz()--frequency above old freq:"
          << iter->m_wFreqInMHz )
      return iter->m_wFreqInMHz ;
      //break ;
    }
    ++ iter ;
  }
  DEBUGN("GetNearestHigherPossibleFreqInMHz()--return 0 (no higher freq found)" )
  return 0 ;
}

WORD I_CPUcontroller::GetNearestLowerPossibleFreqInMHz(WORD wFreqInMhzOld)
{
  std::set<VoltageAndFreq>::const_iterator iter = 
    mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.begin( ) ;
    m_stdsetvoltageandfreqAvailableFreq.begin( ) ;

    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  std::set<VoltageAndFreq>::const_iterator iterLower = 
    mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.end( ) ;
    m_stdsetvoltageandfreqAvailableFreq.end() ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->end( ) ;
  while( iter != mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.
    m_stdsetvoltageandfreqAvailableFreq.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
  {
    if( iter->m_wFreqInMHz >= wFreqInMhzOld )
    {
      break ;
    }
    else
      iterLower = iter ;
    ++ iter ;
  }
  if( iterLower != mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
    return iterLower->m_wFreqInMHz ;
  return 0 ;
}

WORD I_CPUcontroller::GetNumberOfCPUcores()
{
//  if( mp_cpuaccess )
//  {
//    DWORD dwEAX ;
//    DWORD dwEBX ;
//    DWORD dwECX ;
//    DWORD dwEDX ;
//    //Check if register 8000_0008h is available
//    if( mp_cpuaccess->CpuidEx(
//        0x80000000
//        , & dwEAX,
//        & dwEBX,
//        & dwECX,
//        & dwEDX,
//        1 // CPU (core) affinityMask
//        )
//      )
//    {
//      if( //http://www.sandpile.org/ia32/cpuid.htm:
//        //"EAX=xxxx_xxxxh maximum supported extended level"
//        dwEAX >= 0x80000008
//        )
//      {
//        //http://www.sandpile.org/ia32/cpuid.htm:
//        //extended level 8000_0008h
//        //ECX: 7...0 cores per die - 1
//        if( mp_cpuaccess->CpuidEx(
//            0x80000008
//            , & dwEAX,
//            & dwEBX,
//            & dwECX,
//            & dwEDX,
//            1 // CPU (core) affinityMask
//            )
//          )
//          return (dwECX && 127 ) + 1 ; //127=1111111bin
//      }
//      //e.g. Pentium Ms maximum supported extended level is
//      //0x80000004, so it does not have register 8000_0008h
//      else
//        return 1 ;
//    }
//    else
//      return 1 ;
//  }
  return 0 ;
}

WORD I_CPUcontroller::GetNumberOfPstates()
{
  if( mp_model )
  {
    return mp_model->m_cpucoredata.m_stdsetvoltageandfreqAvailableFreq.size() ;
  }
  return 0 ;
}

BYTE I_CPUcontroller::GetPstate(WORD wPstateID, 
                                    VoltageAndFreq & r_voltageandfreq )
{
  BYTE byPstateExists = 0 ;
  //std::set<VoltageAndFreq>::const_iterator iter = 
  //  mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin( ) ;
  //std::set<VoltageAndFreq>::const_reverse_iterator reverse_iter =
  //Must use "reverse_iterator" for "operator !=" of STLport's "set"
  std::set<VoltageAndFreq>::reverse_iterator reverse_iter =
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.rbegin( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  WORD wElemenIndex = 0 ;
  while( //iter != 
    reverse_iter !=
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    //end() 
    rend()
    )
  {
    if( wElemenIndex ++ == wPstateID )
    {
      r_voltageandfreq = * //iter ;
        reverse_iter ;
      byPstateExists = 1 ;
      break ;
    }
    //++ iter ;
    ++ reverse_iter ;
  }
  return byPstateExists ;
}

BYTE I_CPUcontroller::GetPstateSafefy(
  WORD wFreqInMHz
  , float fVoltageInVolt 
  )
{
  bool bPstateIsSafe = false ;
  float fMaxVoltageInVolt ;
  if( GetInterpolatedVoltageFromFreq(
      wFreqInMHz
      , fMaxVoltageInVolt 
      , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
      //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault
      )
    )
  {
//    int i = 0 ;
    if( fVoltageInVolt <= fMaxVoltageInVolt )
      bPstateIsSafe = //true ;
        SETTING_VOLTAGE_IS_SAFE ;
  }
  return bPstateIsSafe ;
}

//bool IsLowerVoltageThan( float fIsLessThan, float fValueToCompare)
//{
//  float fVolt = mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
//  if( //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) > 
//    fVolt >
//    mp_i_cpucontroller->GetMinimumVoltageInVolt() 
//    )
//  {
//    float f1VIDstepAboveIsLessThan = fIsLessThan ;
//    float f1VIDstepAboveValueToCompare = fValueToCompare ;
//    mp_i_cpucontroller->IncreaseVoltageBy1Step( f1VIDstepAboveIsLessThan ) ;
//    mp_i_cpucontroller->IncreaseVoltageBy1Step( f1VIDstepAboveValueToCompare ) ;
//    WORD wVoltageIDValueToProof = GetVoltageID( IsLessThan ) ;
//
//    WORD wVoltageIDValueToCompare = GetVoltageID( fValueToCompare ) ;
//    
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//}

//Define a function for comparing because of this problem:
//the calculated float value was 1.0999999
//the compared float value was 1.1000000
//both values belong the SAME voltage ID, just a little rounding error.
//By comparing the integer values for the corresponding voltages in Volt
//the problem can be avoided.
bool I_CPUcontroller::VIDisLowerVoltageThan( WORD wVIDisLessThan, WORD wVIDvalueToCompare)
{
  //float f1VIDstepAboveIsLessThan = fIsLessThan ;
  float fVoltageIsLessThan = //GetVoltageID(wVIDisLessThan) ;
    GetVoltageInVolt(wVIDisLessThan) ;
  float fVoltageValueToCompare = //GetVoltageID(wVIDvalueToCompare) ;
    GetVoltageInVolt(wVIDvalueToCompare) ;
  return fVoltageIsLessThan < fVoltageValueToCompare ;
}

//Any WRITE operation to the performance control register by any other software 
//(OS, BIOS, RMclock, GNOME power manager,...) is problematic.
BYTE I_CPUcontroller::OtherPerfCtrlMSRwriteIsActive()
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->OtherDVFSisEnabled() ;
  return 0 ;
}

void I_CPUcontroller::SetCmdLineArgs(
  int argc, 
  //TCHAR * argv[]
#ifdef FORCE_WCHAR_T
  wchar_t ** argv
#else
  TCHAR ** argv
#endif
  )
{
  m_byNumberOfCmdLineArgs = argc ;
  m_arartcharCmdLineArg = argv ;
}

BYTE I_CPUcontroller::SetFreqAndVoltageFromFreq(
  WORD wFreqInMHz 
  , BYTE byCoreID)
{
  BYTE byRet ;
  DEBUGN("I_CPUcontroller::SetFreqAndVoltageFromFreq("
#ifdef _DEBUG
    << wFreqInMHz << ","
    << (WORD) byCoreID << ")"
#endif
    )
  byRet = SetFreqAndVoltageFromFreq(
    wFreqInMHz 
    , mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
    , byCoreID ) ;
  DEBUGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(" << wFreqInMHz << ","
    << (WORD) byCoreID << ") ret value from "
    <<"I_CPUcontroller::SetFreqAndVoltageFromFreq("
    << "wFreqInMHz:" << wFreqInMHz
    << ",stdset:" << & mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
    << ",byCoreID:" << (WORD) byCoreID
    << ") : " << (WORD) byRet
    )
  return byRet ;
}

BYTE I_CPUcontroller::SetFreqAndVoltageFromFreq(
  WORD wFreqInMHz 
  , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
  , BYTE byCoreID )
{
  BYTE byRet = 0 ;
  float fVoltageInVolt ;
//  DEBUGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE"
//      "address of model: " << mp_model )
  const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq =
    //mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
    //mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted ;

    //The set should have been filled by the CPU controller as it should
    //have the knowledge/ Zust�ndigkeit wich multipliers can be set.
    //for instance for AMD Griffin freqs are: "max. , 1/2 max,..."
    //for Pnetium M multipliers 6,8,...
//    mp_model->m_cpucoredata.m_stdsetvoltageandfreqAvailableFreq ;
    cr_stdsetvoltageandfreqForInterpolation ;
    //* mp_model->m_cpucoredata.mp_stdsetvoltageandfreqAvailableFreq ;
  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq = 
    r_stdsetvoltageandfreq.begin() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.end() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestHigherEqual = r_stdsetvoltageandfreq.end() ;
  while( ci_stdsetvoltageandfreq != r_stdsetvoltageandfreq.end() )
  {
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz <= wFreqInMHz )
      ci_stdsetvoltageandfreqNearestLowerEqual = ci_stdsetvoltageandfreq ;
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz >= wFreqInMHz )
    {
      ci_stdsetvoltageandfreqNearestHigherEqual = ci_stdsetvoltageandfreq ;
      //The entries are sorted ascending by frequency. So break to avoid 
      //assigning a higher value (that would not be the nearest higher freq
      //any more).
      break ;
    }
    ++ ci_stdsetvoltageandfreq ;
  }
  //
  if( ci_stdsetvoltageandfreqNearestHigherEqual != 
    r_stdsetvoltageandfreq.end() 
    )
  {
//    LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE):"
//      " higher element found: "
////      << ci_stdsetvoltageandfreqNearestHigherEqual->m_fVoltageInVolt
////      << "," << ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
//      )
#ifdef _DEBUG
    if( ci_stdsetvoltageandfreqNearestLowerEqual !=
      r_stdsetvoltageandfreq.end()
      )
    {
//      DEBUGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE)"
//        " lower element found: "
//        << ci_stdsetvoltageandfreqNearestLowerEqual->m_fVoltageInVolt
//        << "," << ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz )
    }
#endif
    //Freq is lower than the ones in the list.
    //if( wFreqInMHz < ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz )
    //No entry  is lower equal than the freq.
//    if( ci_stdsetvoltageandfreqNearestLowerEqual == r_stdsetvoltageandfreq.end() )
//    {
//      wFreqInMHz = ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
////      if( cr_stdsetvoltageandfreqForInterpolation.find())
////        fVoltageInVolt =
////      byRet = SetVoltageAndFrequency(//wFreqInMHz
////        //fVoltageInVolt
////        //ci_stdsetvoltageandfreqNearestHigherEqual->m_fVoltageInVolt
////        , wFreqInMHz
////        , byCoreID
////        ) ;
//    }
//      //Get the freq from the found element in the set (by default: of the
//      //available frequencies)
//      wFreqInMHz = ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
    if( ci_stdsetvoltageandfreqNearestLowerEqual ==
        r_stdsetvoltageandfreq.end()
      )
    {
      wFreqInMHz = ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
      fVoltageInVolt = ci_stdsetvoltageandfreqNearestHigherEqual->
        m_fVoltageInVolt ;
//      LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE):"
//        " no lower element -> setting" << wFreqInMHz << "MHz,"
//        << fVoltageInVolt << "V=" << (WORD) (fVoltageInVolt * 1000) << "mV")
      byRet = SetVoltageAndFrequency( fVoltageInVolt, wFreqInMHz, byCoreID ) ;
    }
    else
    {
      if( ci_stdsetvoltageandfreqNearestLowerEqual ==
          ci_stdsetvoltageandfreqNearestHigherEqual
          )
      {
        wFreqInMHz = ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
        fVoltageInVolt = ci_stdsetvoltageandfreqNearestHigherEqual->
          m_fVoltageInVolt ;
//        LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE):"
//          " lower element ==higher ele-> setting" << wFreqInMHz << ","
//          << fVoltageInVolt << "V=" << (WORD) (fVoltageInVolt * 1000) << "mV" )
        byRet = SetVoltageAndFrequency( fVoltageInVolt, wFreqInMHz, byCoreID ) ;
      }
      else
      {
        //The voltage _should_ be calculated from the closest of the available
        //multipliers.
        if( m_fReferenceClockInMHz && mp_model->m_cpucoredata.
            m_arfAvailableMultipliers )
        {
    //      GetNearestMultiplier() ;
          float fCalculatedMultiplier = wFreqInMHz /
            m_fReferenceClockInMHz ;
          WORD wMultiplierArrayIndex = mp_model->m_cpucoredata.
            GetIndexForClosestMultiplier( fCalculatedMultiplier) ;
          if( wMultiplierArrayIndex != MAXWORD )
          {
            float fMultiplier = mp_model->m_cpucoredata.
              m_arfAvailableMultipliers[wMultiplierArrayIndex] ;
//            LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE):"
//              " lower and higher element -> calc. multi:"
//              << fCalculatedMultiplier
//              << "multi array index:" << wMultiplierArrayIndex
//              << "closest avail. multi:" << fMultiplier )
            wFreqInMHz = (WORD) ( m_fReferenceClockInMHz * fMultiplier ) ;
            //As we have the Freq now: calc the voltage from it.
            //The freq should, but needn't be in the set (2 different sets:
            //set of available freqs, set of voltages to set).
            if( GetInterpolatedVoltageFromFreq(
                wFreqInMHz
                //ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
                , fVoltageInVolt
                //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
                , cr_stdsetvoltageandfreqForInterpolation
                )
              )
            {
//              LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq("
//                << "wFreqInMHz:" << wFreqInMHz
//                << ",stdset:" << & cr_stdsetvoltageandfreqForInterpolation
//                << ",byCoreID:" << (WORD) byCoreID
//                << ") before "
//                << "SetVoltageAndFrequency("
//                << fVoltageInVolt << ","
//                << wFreqInMHz << ","
//                << (WORD) byCoreID << ")" )
      //        byRet = SetVoltageAndFrequency(//wFreqInMHz
      //          fVoltageInVolt
      //          //, ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
      //          , wFreqInMHz
      //          , byCoreID
      //          ) ;
              SetCurrentVoltageAndMultiplier(fVoltageInVolt,fMultiplier,
                byCoreID ) ;
              DEBUGN("I_CPUcontroller::SetFreqAndVoltageFromFreq("
                << "wFreqInMHz:" << wFreqInMHz
                << ",stdset:" << & cr_stdsetvoltageandfreqForInterpolation
                << ",byCoreID:" << (WORD) byCoreID
                << ") after "
                << "SetVoltageAndFrequency("
                << fVoltageInVolt << ","
                << wFreqInMHz << ","
                << (WORD) byCoreID << ")"
                "ret val: " << (WORD) byRet )
            }
          }
        }
      }
    }
  }
  //No freq in the container that is higher than the freq to set.
  else //if e.g. the wanted freq is higher than the max. freq.
  {
//    LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE"
//      "NO higher freq element found" )
    if( ci_stdsetvoltageandfreqNearestLowerEqual  != 
      r_stdsetvoltageandfreq.end() 
      )
    {
      wFreqInMHz = ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
      fVoltageInVolt = ci_stdsetvoltageandfreqNearestLowerEqual->
        m_fVoltageInVolt ;
//      if( GetInterpolatedVoltageFromFreq(
//          //wFreqInMHz
//          //The freq that is nearest below.
//          ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz
//          , fVoltageInVolt
//          //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
//          , cr_stdsetvoltageandfreqForInterpolation
//          )
//        )
//      {
//      }
//      LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE"
//        "lower freq element found->setting"
//        << wFreqInMHz << "," << fVoltageInVolt
//        << "V=" << (WORD) ( fVoltageInVolt * 1000) << "mV")
      byRet = SetVoltageAndFrequency( fVoltageInVolt, wFreqInMHz, byCoreID ) ;
    }
    //if( ( wFreqInMHz - ci_stdsetvoltageandfreqNearestLowerEqual->
    //    m_wFreqInMHz )        
    //    < 
    //    ( ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz - wFreqInMHz )
    //  )
    //  ci_stdsetvoltageandfreq = ci_stdsetvoltageandfreqNearestLowerEqual ;
    //else
    //  ci_stdsetvoltageandfreq = ci_stdsetvoltageandfreqNearestHigherEqual ;
    //if( GetInterpolatedVoltageFromFreq(
    //    //wFreqInMHz
    //    ci_stdsetvoltageandfreq->m_wFreqInMHz
    //    , fVoltageInVolt 
    //    , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
    //    ) 
    //  )
    //{
    //  SetVoltageAndFrequency(wFreqInMHz,fVoltageInVolt) ;
    //}
  }
  return byRet ;
}

void I_CPUcontroller::SetUserInterface( 
  //By using a pointer instead of a reference one can pass NULL 
  //to show that there is no object.
  UserInterface * p_userinterface)
{
  mp_userinterface = p_userinterface ;
}

void I_CPUcontroller::SetModelData( 
  //By using a pointer instead of a reference one can pass NULL 
  //to show that there is no object.
  Model * p_model )
{
  mp_model = p_model ;
  WORD w = GetNumberOfCPUcores() ;
  if( w )
    p_model->SetNumberOfCPUCores( w ) ;
}

#ifdef COMPILE_WITH_CALC_THREAD
void I_CPUcontroller::SetCalculationThread(
  ICalculationThread * p_calculationthread )
{
  mp_calculationthread = p_calculationthread ;
}
#endif //#ifdef COMPILE_WITH_CALC_THREAD

void I_CPUcontroller::SetOtherDVFSaccess(
  IDynFreqScalingAccess * p_dynfreqscalingaccess)
{
  mp_dynfreqscalingaccess = p_dynfreqscalingaccess ;
}

//bool I_CPUcontroller::CmdLineParamsContain(
//  TCHAR * ptcharOption
//  , std::string & strValue
//  )
//{
//  bool bcmdLineParamsContain = false ;
//  int nIndex = 1 ;
//  signed short wPos = 0 ;
//  DEBUG("cmdLineParamsContain begin\n");
//  for ( ;nIndex < m_byNumberOfCmdLineArgs ; ++ nIndex )
//  {
//    std::string strCmdArg( //GetCharPointer( m_arartcharCmdLineArg[ nIndex ] ) );
//      GetStdString(m_arartcharCmdLineArg[ nIndex ]) ) ;
//    wPos = (WORD) strCmdArg.find( //std::string( GetCharPointer(
//      //ptcharOption ) ) +
//      GetStdString(ptcharOption ) +
//      //TCHAR("=")
//      std::string("=") ) ;
//    if( wPos != std::string::npos && wPos == 0 )
//    {
////#ifdef WIN32
//#ifdef MS_COMPILER
//      strValue = strCmdArg.substr(_tcslen(ptcharOption)
//        //Start after "="
//        +1);
//#else
//      strValue = strCmdArg.substr( //strlen( GetCharPointer( ptcharOption ) )
//        GetStdString( ptcharOption ).length()
//        //Start after "="
//        + 1 );
//#endif //#ifdef WIN32
//      bcmdLineParamsContain = true ;
//      break ;
//    }
//  }
//  DEBUG("cmdLineParamsContain end\n");
//  return bcmdLineParamsContain ;
//}

////this method may be overwritten for CPU-specific configuration
//BYTE I_CPUcontroller::HandleCmdLineArgs()
//{
//  std::string strValue ;
//  if( CmdLineParamsContain(_T("-config"),strValue) )
//  {
//#ifdef COMPILE_WITH_XERCES
//    SAX2MainConfigHandler saxhandler(//pstates
//      *mp_model ,
//      mp_userinterface //,
//      //this
//      );
//    if(
//      ReadXMLfileInitAndTermXerces(//"config.xml"
//
//      //mp_configurationHandler->LoadConfiguration(
//
//      //strValue.c_str(),pstates) )
//      //strValue.c_str(),pstatectrl.m_pstates) )
//      strValue.c_str(), //m_pstates
//      //m_model,
//      *mp_model,
//      mp_userinterface ,
//      //this
//      saxhandler
//      )
//      )
//    {
//      //if( pstates.AreSafe() )
//      //{
//      //  ChangePStates();
//      //}
//      //else
//      //  printf("Unsafe values/voltage for p-state->not applying values\");
//      //pstatectrl.ApplyAllPStates(pstates);
//      //pstatectrl.ApplyAllPStates();
//
//      //If the file is NOT assigned/ opened yet.
//      //if( fileDebug == NULL )
//      if( //An empty string means: do NOT write to the log file.
//          ! mp_model->m_stdstrLogFilePath.empty()
//          &&
//          ! //g_logger.m_ofstream.is_open()
//          g_logger.IsOpen()
//        )
//      {
//        //Convert std::string to wstring or remain std::string.
//        std::tstring stdtstr = Getstdtstring(mp_model->m_stdstrLogFilePath) ;
//          //g_logger = new Logger(mp_model->m_stdstrLogFilePath);
//          g_logger.OpenFile( stdtstr ) ;
//      }
//#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
//      byReturn = SUCCESS ;
//#else
//    //if( //! mp_pstatectrl->m_model.m_bSkipCPUtypeCheck &&
//    //  ! mp_model->m_bSkipCPUtypeCheck &&
//    //  //! IsSupportedCPUModel()
//    //  )
//    //{
//    //  mp_userinterface->Confirm("This CPU model is not supported by this program."
//    //    "Use \"skip_cpu_type_check\" (see help) if you think it makes "
//    //    "sense.\n->exiting");
//    //  return byReturn ;
//    //}
//
//      //byReturn = ApplyAllPStates();
//
//#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
//    }
//    else
//      mp_userinterface->Confirm("Error reading the XML-configuration file");
//#else //COMPILE_WITH_XERCES
//    //byReturn = SUCCESS ;
//#endif //COMPILE_WITH_XERCES
//  }
//  return 1 ;
//}

//void I_CPUcontroller::SetCPUaccess(I_CPUaccess * p_cpu_access)
//{
//  return mp_cpuaccess ;
//}
