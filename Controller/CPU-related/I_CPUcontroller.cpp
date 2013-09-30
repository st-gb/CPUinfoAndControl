/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany
 * ("Trilobyte SE") 2010-at least 2012.
 * You are allowed to modify and use the source code from Trilobyte SE for free
 * if you are not making profit directly or indirectly with it or its adaption.
 * Else you may contact Trilobyte SE. */
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
//for GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt
//  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  );
#include "GetInterpolatedVoltageFromFreq.hpp"

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
  m_p_std_set_voltageandfreqUseForDVFS(NULL)
  , m_fReferenceClockInMHz(0)
  //1 power plane for all CPU cores is usual->set as default.
  , m_b1CPUcorePowerPlane( true )
  , m_llLastTimeTooHot(0)
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
    LOGN( "interpolated voltage:" << fInterpolatedVoltage )
    if(//Voltage to set is _above_ default voltage (-> too high/ dangerous).
      fVoltageInVolt > fInterpolatedVoltage )
    {
      LOGN("voltage is above default voltage-> invalid")
      bVoltageIsValid = //VoltageIsAboveDefaultVoltage ;
        VoltageIsOutsideSafeRange ;
    }
    else if( fVoltageInVolt == fInterpolatedVoltage )
      bVoltageIsValid = VoltageMatchesInterpolatedVoltage;
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
  LOGN( "begin")
  float fInterpolatedVoltage ;
  if( //mp_cpucontroller->GetInterpolatedVoltageFromFreq(
    GetInterpolatedVoltageFromFreq(
      (WORD) fCPUcoreFrequencyinMHz ,
      fInterpolatedVoltage ,
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable
      )
    )
  {
    LOGN(/*"VoltageIsValid--" */ "interpolated voltage:" << fInterpolatedVoltage )
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

/** This function is useful for testing unstable CPU core voltage detection code:
 *  It shall cancel if it is 1 voltage step below the highest unstable voltage
 *  (that was found with a good voltage detection code.*/
BYTE I_CPUcontroller::CheckWhetherCPUcoreVoltageIsBelowHighestInstableVoltage(
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
    const WORD voltageInVoltArrayIndex = mp_model->m_cpucoredata.
      GetIndexForClosestVoltage(fInterpolatedVoltage);
    LOGN( "interpolated voltage:" << fInterpolatedVoltage
      << " array index:" << voltageInVoltArrayIndex )
    if( voltageInVoltArrayIndex > 0 )
    {
      const float highestInstableCPUcoreVoltage = mp_model->m_cpucoredata.
        m_arfAvailableVoltagesInVolt[voltageInVoltArrayIndex - 1];
      LOGN( "highestInstableCPUcoreVoltage:"
        << highestInstableCPUcoreVoltage )
      if(//Voltage to set is _below_ lowest settable voltage (-> too low/
        //device may freeze/ malfunction/ can cause wrong CPU cache content).
        fVoltageInVolt < highestInstableCPUcoreVoltage )
      {
        LOGN("voltage is below highest instable voltage-> invalid")
        bVoltageIsValid = //VoltageIsBelowLowestStableVoltage ;
          VoltageIsOutsideSafeRange ;
      }
    }
  }
  else
    bVoltageIsValid = No2PstatesForVoltageInterpolationFound ;
  return bVoltageIsValid ;
}

/** The CPUID instruction (in contrast to wrmsr) is rather not able to harm the
 * CPU. So it can be implemented for every CPU controller in this base class. */
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

/** The method is situated in this class because the reference clock in member
 * variable of this class */
float I_CPUcontroller::GetClosestMultiplier(WORD wFrequencyInMHz//, float fMultiplier
  )
{
  float fMultiplierFromCoreFreq = (float) wFrequencyInMHz /
    m_fReferenceClockInMHz;
  LOGN( "multiplier from " << wFrequencyInMHz << " Mhz="
    << fMultiplierFromCoreFreq)
  if( mp_model )
  {
//      const std::set<float> & c_r_stdset_floatAvailableMultipliers = mp_model->
//        m_cpucoredata.m_stdset_floatAvailableMultipliers;
//      std::set<float>::const_iterator c_iter =
//        c_r_stdset_floatAvailableMultipliers.
//        //http://www.cplusplus.com/reference/stl/set/lower_bound/:
//        //"Returns an iterator pointing to the first element in the container
//        //which does not compare less than x (using the container's comparison
//        //object), i.e. it is either equal or greater."
//        lower_bound(fMultiplierFromCoreFreq);
//      if( c_iter != c_r_stdset_floatAvailableMultipliers.end() )
//      {
//        float greaterOrEqualMulti = *c_iter;
//        if( c_iter != c_r_stdset_floatAvailableMultipliers.begin() )
//        {
//          -- c_iter;
//          float lessMulti = *c_iter;
//          return fMultiplierFromCoreFreq - lessMulti < greaterOrEqualMulti -
//            fMultiplierFromCoreFreq ? lessMulti : greaterOrEqualMulti;
//        }
//        else
//          return greaterOrEqualMulti;
//      }
//      else
//      {
//        if( c_iter != c_r_stdset_floatAvailableMultipliers.begin() )
//        {
//          -- c_iter;
//          return * c_iter;
//        }
//      }
    const fastestUnsignedDataType arrayIndexForClosestValue =
      mp_model->m_cpucoredata.GetIndexForClosestMultiplier(
      fMultiplierFromCoreFreq);
    if( arrayIndexForClosestValue != BinarySearch::no_element)
      return mp_model->m_cpucoredata.m_arfAvailableMultipliers[
        arrayIndexForClosestValue];
  }
  return 0.0f;
}

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
  LOGN( "begin core ID:" << wCoreID)
  PerCPUcoreAttributes * arp_percpucoreattributes = mp_model->m_cpucoredata.
    m_arp_percpucoreattributes ;
//  LOGN(" I_CPUcontroller:GetCurrentVoltageAndFrequencyAndStoreValues")
  arp_percpucoreattributes[wCoreID].m_fThrottleLevel = GetThrottleLevel(wCoreID);
  LOGN_INFO( "throttle_ratio:"
    << arp_percpucoreattributes[wCoreID].m_fThrottleLevel )
  return GetCurrentVoltageAndFrequency(
    arp_percpucoreattributes[wCoreID].m_fVoltageInVolt,
    arp_percpucoreattributes[wCoreID].m_fMultiplier,
    arp_percpucoreattributes[wCoreID].m_fReferenceClockInMhz ,
    wCoreID ) ;
}

void I_CPUcontroller::GetCurrentTemperatureInCelsiusAndStoreValues(
  WORD wCoreID )
{
  LOGN( "begin")
  PerCPUcoreAttributes * arp_percpucoreattributes = mp_model->m_cpucoredata.
    m_arp_percpucoreattributes ;
  arp_percpucoreattributes[wCoreID].m_fTempInDegCelsius =
    GetTemperatureInCelsius(wCoreID) ;
  LOGN( "end")
}

//BYTE I_CPUcontroller::GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt 
//  , std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  )
//{
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
//        //ci_stdsetvoltageandfreqNearestLowerEqual == 
//        c_rev_iter_stdsetvoltageandfreqNearestLowerEqual ==
//        ci_stdsetvoltageandfreqNearestHigher
//        )
//      {
//        r_fVoltageInVolt = //ci_stdsetvoltageandfreqNearestLowerEqual->
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->
//          m_fVoltageInVolt ;
//        return true ;
//      }
//      else
//      {
//        WORD wFreqInMHzFromNearFreqAboveWantedFreq =
//          ci_stdsetvoltageandfreqNearestHigher->m_wFreqInMHz ;
//        WORD wFreqInMHzFromNearFreqBelowWantedFreq =
//          //ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//        float fVoltageInVoltFromNearFreqAboveWantedFreq ;
//        float fVoltageInVoltFromNearFreqBelowWantedFreq ;
//        fVoltageInVoltFromNearFreqAboveWantedFreq =
//          ci_stdsetvoltageandfreqNearestHigher->
//            m_fVoltageInVolt ;
//        fVoltageInVoltFromNearFreqBelowWantedFreq =
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

/** A generic # of CPU cores is not easy: AMD and Intel do it different */
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

BYTE I_CPUcontroller::GetPstate(
  const std::set<VoltageAndFreq> & c_r_std_set_voltages,
  WORD wPstateID,
  //TODO maybe use a pointer here to avoid to copy from the std::set item.
  VoltageAndFreq & r_voltageandfreq
  )
{
  BYTE byPstateExists = 0 ;
  //std::set<VoltageAndFreq>::const_iterator iter = 
  //  mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin( ) ;
  //std::set<VoltageAndFreq>::const_reverse_iterator reverse_iter =
  //Must use "reverse_iterator" for "operator !=" of STLport's "set"
  std::set<VoltageAndFreq>::reverse_iterator reverse_iter =
    c_r_std_set_voltages.rbegin( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  WORD wElemenIndex = 0 ;
  while( //iter != 
    reverse_iter != c_r_std_set_voltages.
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
//  DEBUGN(
  LOGN(//"I_CPUcontroller::SetFreqAndVoltageFromFreq(" <<
//#ifdef _DEBUG
    "freq in MHZ:" << wFreqInMHz << ","
    << "core ID:" << (WORD) byCoreID << ")"
//#endif
    )

  //Sync with thread that may change the pointer.
  mp_model->m_cpucoredata.m_mutexDVFSthreadMayChangeData.Lock();
  const std::set<VoltageAndFreq> * const p_std_set_voltageandfreqUseForDVFS =
    m_p_std_set_voltageandfreqUseForDVFS;
  mp_model->m_cpucoredata.m_mutexDVFSthreadMayChangeData.Unlock();

  byRet = SetFreqAndVoltageFromFreq(
    wFreqInMHz 
//    , mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
    , * p_std_set_voltageandfreqUseForDVFS
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
    //for Pentium M multipliers 6,8,...
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
          const fastestUnsignedDataType multiplierArrayIndex =
            mp_model->m_cpucoredata.
            GetIndexForClosestMultiplier( fCalculatedMultiplier) ;
          if( multiplierArrayIndex != BinarySearch::no_element )
          {
            float fMultiplier = mp_model->m_cpucoredata.
              m_arfAvailableMultipliers[multiplierArrayIndex] ;
//            LOGN("I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE):"
//              " lower and higher element -> calc. multi:"
//              << fCalculatedMultiplier
//              << "multi array index:" << multiplierArrayIndex
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
            else
              LOGN(//"I_CPUcontroller::SetFreqAndVoltageFromFreq(..., std::set,"
                "...): voltage not set because "
                "\"GetInterpolatedVoltageFromFreq(...)\" returned false")
          }
          else
            LOGN(//"I_CPUcontroller::SetFreqAndVoltageFromFreq(..., std::set,"
              "...): no array index for multiplier \""
              << fCalculatedMultiplier << "\"found")
        }
        else
        {
          LOGN(//"I_CPUcontroller::SetFreqAndVoltageFromFreq(..., std::set,...):"
            "reference clock is 0 _and_ no available multipliers")
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
  LOGN( "begin")
  m_p_std_set_voltageandfreqUseForDVFS = & p_model->m_cpucoredata.
    m_stdsetvoltageandfreqWanted;
  LOGN( "after assigning desired voltages for DVFS")
  mp_model = p_model ;
  WORD w = GetNumberOfCPUcores() ;
  if( w )
    p_model->SetNumberOfCPUCores( w ) ;
}

void I_CPUcontroller::SetNextLowerThrottleLevel(unsigned coreID,
  float fCPUcoreMultiplierToUse)
{
  float fThrottleLevel = mp_model->m_cpucoredata.GetNextLowerThrottleLevel(coreID);
  LOGN( "next lower throttle level:" << fThrottleLevel)
  if( fThrottleLevel != -1.0f)
  {
    const std::set<VoltageAndFreq> & r_std_set_voltageandfreqDefault =
      mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault;
      m_stdsetvoltageandfreqAvailableFreq;
    std::set<VoltageAndFreq>::const_iterator c_iter =
      r_std_set_voltageandfreqDefault.begin();
    const float resultingFreqInMHZ = fThrottleLevel * fCPUcoreMultiplierToUse *
      m_fReferenceClockInMHz;
    LOGN_DEBUG( "resulting effective frequency is "
      << fThrottleLevel << "*" << fCPUcoreMultiplierToUse << "*"
      << m_fReferenceClockInMHz << "=" << resultingFreqInMHZ)
    if( c_iter != r_std_set_voltageandfreqDefault.end() )
    {
      if( resultingFreqInMHZ >= c_iter->m_wFreqInMHz  )
      {
        SetThrottleLevel(fThrottleLevel, coreID);
      }
      else
      {
        LOGN_DEBUG("not setting throttle ratio to "
          << fThrottleLevel <<
          " because the effective frequency (" << resultingFreqInMHZ
          << "MHz)  would be < lowest frequency with default voltage set ("
          << c_iter->m_wFreqInMHz << "MHz)" )
      }
    }
    else
      LOGN_WARNING("no default voltage available")
  }
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

//void I_CPUcontroller::SetCPUaccess(I_CPUaccess * p_cpu_access)
//{
//  return mp_cpuaccess ;
//}
