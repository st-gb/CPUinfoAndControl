/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include <Controller/time/GetTickCount.hpp> //DWORD ::GetTickCount()

#include "DynFreqScalingThreadBase.hpp"

#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
#include <ModelData/CPUcoreData.hpp> //class CPUcoreData
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
//LOGN(...), DEBUGN(...)
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF(...)
#include <UserInterface/UserInterface.hpp> //for UserInterface.m_bConfirmedYet
#include <algorithms/binary_search/binary_search.hpp> //GetClosestLess()
//SUPPRESS_UNUSED_VARIABLE_WARNING(...)
#include <preprocessor_macros/suppress_unused_variable.h>
//#include <global.h> //LOGN
#include <Controller/Sleep.hpp> //for OperatingSystem::Sleep(...)

//DynFreqScalingThreadBase::DynFreqScalingThreadBase(
//  ICPUcoreUsageGetter * p_icpu
//  , I_CPUcontroller * p_cpucontroller
//  , CPUcoreData & r_cpucoredata
//  )
//  : //m_bSuccFullyGotPStateFromMSR(false)
//  //Initialize in the same order as textual in the declaration?
//  //(to avoid g++ warnings)
//  mp_cpucoredata(&r_cpucoredata)
//  , mp_cpucontroller ( p_cpucontroller )
//  , m_bCalledInit(false)
//  , m_wMilliSecondsToWait(100)
//  , m_vbRun(true)
//  , m_vbDVFSthreadStopped(true)
//{
////  //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
////  // the mutex should be initially locked
////  mp_cpucoredata->m_mutexDVFSthreadMayChangeData.Lock() ;
//
//  DEBUG("constructor of freq scaling thread base--begin\n");
//  mp_icpu = p_icpu ;
//  LOGN("core usage address: " << p_icpu )
//  //mp_icpu->Init();
////  m_wMaxFreqInMHz = //2200
////    r_cpucoredata.m_wMaxFreqInMHz ;
//  //wxTimer();
//  m_fPercentileIncrease = 1.5f ;
//  mp_cpucontroller = p_cpucontroller ;
//  DEBUG("constructor of freq scaling thread base--end\n");
//}

DynFreqScalingThreadBase::DynFreqScalingThreadBase(
  CPUcontrolBase & r_cpucontrolbase
  , CPUcoreData & r_cpucoredata
  )
  : //m_bSuccFullyGotPStateFromMSR(false)
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  mp_cpucoredata( & r_cpucoredata)
  , mr_cpucontrolbase (r_cpucontrolbase)
  , m_ar_fPreviousTemperaturesInDegCelsius ( NULL )
  , m_ar_fPreviousMinusCurrentTemperature( NULL)
  , mp_cpucontroller ( r_cpucontrolbase.mp_cpucontroller )
  , mp_icpu ( r_cpucontrolbase.mp_cpucoreusagegetter)
  , m_arp_percpucoreattributes ( r_cpucoredata.m_arp_percpucoreattributes )
//  , m_bCalledInit(false)
//  , m_wMilliSecondsToWait(100)
  , m_wMilliSecondsToWaitForCoolDown( //2000
      r_cpucoredata.m_wMilliSecondsWaitToCheckWhetherTemperatureDecreased)
  , m_vbRun(true)
  , m_vbDVFSthreadStopped(true)
{
//  //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//  // the mutex should be initially locked
//  mp_cpucoredata->m_mutexDVFSthreadMayChangeData.Lock() ;
  LOGN( FULL_FUNC_NAME << "--"
    << "CPU control base: " << & r_cpucontrolbase
    << "CPU controller:" << r_cpucontrolbase.mp_cpucontroller
    << "CPU usage getter: " << r_cpucontrolbase.mp_cpucoreusagegetter )

  DEBUG("constructor of freq scaling thread base--begin\n");
//  mp_icpu = p_icpu ;
//  LOGN("dyn freq scaling thread base constructor--core usage address: "
//    << mp_icpu )
  //mp_icpu->Init();
  //wxTimer();
  m_fPercentileIncrease = 1.5f ;
//  mp_cpucontroller = p_cpucontroller ;
  DEBUG("constructor of freq scaling thread base--end\n");
}

void DynFreqScalingThreadBase::CalcDiffBetweenCurrentAndPreviousTemperature()
{
//  LOGN("DynFreqScalingThreadBase::CalcDiffBetweenCurrentAndPrevious"
//    "Temperature() begin")
//  if( m_bGotCPUcoreDataAtLeast1Time )
  {
    for( BYTE byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
    {
      m_ar_fPreviousMinusCurrentTemperature[byCoreID] =
        m_ar_fPreviousTemperaturesInDegCelsius [byCoreID] -
        m_arp_percpucoreattributes[ byCoreID ].m_fTempInDegCelsius ;
    }
  }
//  LOGN("DynFreqScalingThreadBase::CalcDiffBetweenCurrentAndPrevious"
//    "Temperature() end")
}

void DynFreqScalingThreadBase::ChangeOperatingPointByLoad( 
  BYTE byCoreID 
  , float fLoad 
  )
{
  LOGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad"
//#ifdef _DEBUG
    "(core ID:"
    << (WORD) byCoreID << ",load:"
    << fLoad
    << ")"
//#endif
    )
  PerCPUcoreAttributes & r_percpucoreattributes = //mp_cpucoredata->
//    m_arp_percpucoreattributes[byCoreID] ;
    m_arp_percpucoreattributes[byCoreID] ;
//  DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad "
//    " address of r_percpucoreattributes:" << & r_percpucoreattributes)
  //if ( //m_fPreviousCPUusage 
  //  ////mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
  //  //r_percpucoreattributes.
  //  ////If the CPU usage was NOT retrieved for the first time.
  //  //m_fPreviousCPUusage != -1.0f 
  //  )
  LOGN( FULL_FUNC_NAME << " reference clock in MHz:" <<
    r_percpucoreattributes.m_fReferenceClockInMhz)
  //If the reference clock is 0 then it could not be calculated,
  if( r_percpucoreattributes.m_fReferenceClockInMhz != 0.0f )
  {
    if ( //mp_cpucoredata->m_arfCPUcoreLoadInPercent[byCoreID] >
      fLoad >
      //m_fPreviousCPUusage 
      //r_percpucoreattributes.m_fPreviousCPUusage * 0.75 //* 1.5f 
      mp_cpucoredata->m_fCPUcoreLoadThresholdForIncreaseInPercent
      )
    {
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "load > threshold for increase")
      //m_fPreviousCPUusage 
      r_percpucoreattributes.m_fPreviousCPUusage = fLoad ;
      //raise the freq.

//      float fVoltage ;
//      WORD wFreqInMHz ;
//      mp_cpucontroller->GetCurrentPstate(
//        m_fCPUcoreFreqInMHz
//        , fVoltage
//        , byCoreID
//        ) ;
//      mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
//        byCoreID
//        ) ;
      m_fCPUcoreFreqInMHz = //"(WORD)" to avoid g++ compiler warning
          //"converting to `WORD' from `float'"
          (WORD)
          mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
          GetFreqInMHz() ;

//      float m_fCPUcoreFrequencyFactor ;
      //TODO
//      if( mp_cpucoredata->m_fCPUcoreFreqFactor >
//        mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor )
//        m_fCPUcoreFrequencyFactor = mp_cpucoredata->m_fCPUcoreFreqFactor ;
//      else
//        m_fCPUcoreFrequencyFactor = mp_cpucoredata->
//          m_fCPUcoreFreqIncreaseFactor ;

      float fFreqInMHz = m_fCPUcoreFreqInMHz * //m_fCPUcoreFrequencyFactor
        mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor;
      LOGN( FULL_FUNC_NAME << " current CPUcoreFreqInMHz("
        << m_fCPUcoreFreqInMHz << ")* factor ("
        << mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor << ")="
        << fFreqInMHz)
      float fCPUcoreMultiplierToUse = fFreqInMHz / mp_cpucoredata->
        m_arp_percpucoreattributes[byCoreID].m_fReferenceClockInMhz;
//        mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].m_fMultiplier *
//        mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor;
      LOGN( FULL_FUNC_NAME << " multiplier belonging to "
        << fFreqInMHz << "MHz=\"" << fCPUcoreMultiplierToUse << "\"")
      //Ensure the frequency is upped.
//      WORD IndexForMultiplierToUse = mp_cpucoredata->
        //GetIndexForClosestMultiplier(fCPUcoreMultiplierToUse);
      WORD indexForClosestGreaterEqualMultiplier = mp_cpucoredata->
        GetIndexForClosestGreaterEqualMultiplier(fCPUcoreMultiplierToUse);
      WORD IndexForPreviousMultiplier = mp_cpucoredata->
        GetIndexForClosestMultiplier(r_percpucoreattributes.m_fMultiplier);
      LOGN( FULL_FUNC_NAME << " indexForClosestGreaterEqualMultiplier: "
        << indexForClosestGreaterEqualMultiplier
        << " IndexForPreviousMultiplier:" << IndexForPreviousMultiplier)
      if(
//          ++ IndexForMultiplierToUse < mp_cpucoredata->
//          m_stdset_floatAvailableMultipliers.size()
//          fCPUcoreMultiplierToUse < closestGreaterEqualMultiplier
          indexForClosestGreaterEqualMultiplier - IndexForPreviousMultiplier == 1
          )
      {
//        if( fCPUcoreMultiplierToUse > mp_cpucoredata->m_arfAvailableMultipliers[
//          IndexForMultiplierToUse] )
//        float nextHigherMultiplier
//        if( fCPUcoreMultiplierToUse == r_percpucoreattributes.m_fMultiplier )
//        fCPUcoreMultiplierToUse = mp_cpucoredata->m_arfAvailableMultipliers[
//          IndexForMultiplierToUse];
        if( indexForClosestGreaterEqualMultiplier != MAXWORD )
        {
          float closestGreaterEqualMultiplier = mp_cpucoredata->
            m_arfAvailableMultipliers[indexForClosestGreaterEqualMultiplier];
          LOGN( FULL_FUNC_NAME << " closestGreaterEqualMultiplier: "
            << closestGreaterEqualMultiplier)
          fCPUcoreMultiplierToUse = closestGreaterEqualMultiplier;
        }
        fFreqInMHz = fCPUcoreMultiplierToUse * mp_cpucoredata->
          m_arp_percpucoreattributes[byCoreID].m_fReferenceClockInMhz;
      }
      if( fCPUcoreMultiplierToUse > mp_cpucoredata->m_fMaximumCPUcoreMultiplier)
      {
        fCPUcoreMultiplierToUse = mp_cpucoredata->m_fMaximumCPUcoreMultiplier;
        fFreqInMHz = fCPUcoreMultiplierToUse *
          m_arp_percpucoreattributes[byCoreID].m_fReferenceClockInMhz;
      }
      if( mp_cpucoredata->m_stdset_floatAvailableVoltagesInVolt.size() < 2)
      {
        LOGN( FULL_FUNC_NAME << " usage > threshold for frequency increase "
          " -> calling SetCurrentVoltageAndMultiplier("
          << fCPUcoreMultiplierToUse )
        mp_cpucontroller->SetCurrentVoltageAndMultiplier(0.0f,
          fCPUcoreMultiplierToUse,
          byCoreID ) ;
      }
      else
      {
        LOGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
          "usage > threshold for frequency increase "
          << (WORD) byCoreID << ","
          << fLoad << ") "
          "before mp_cpucontroller->SetFreqAndVoltageFromFreq("
          << //m_fCPUcoreFreqInMHz * mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor
          fFreqInMHz
          << (WORD) byCoreID << ")" )
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( fFreqInMHz
          )
        , byCoreID
        );
      LOGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "after mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << m_fCPUcoreFreqInMHz * mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor << ","
        << (WORD) byCoreID << ")" )
      }
    }
    else
    {
//      float fVoltage ;
//      WORD m_fCPUcoreFreqInMHz ;
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "load <= threshold for increase")
      //m_fPreviousCPUusage 
      r_percpucoreattributes.m_fPreviousCPUusage = 
        //dClocksNotHaltedDiffDivTCSdiff ;
        //mp_cpucoredata->m_arfCPUcoreLoadInPercent[byCoreID] ;
        fLoad ;
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "before mp_cpucontroller->GetCurrentPstate")
//      mp_cpucontroller->GetCurrentPstate(
//        m_fCPUcoreFreqInMHz
//        , fVoltage
//        , byCoreID
//        ) ;
//      mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
//        byCoreID
//        ) ;
      m_fCPUcoreFreqInMHz =
        //Avoid g++ compiler warning "converting to `WORD' from `float'"
        (WORD) mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
        GetFreqInMHz() ;

      float fFreqInMHz = m_fCPUcoreFreqInMHz * fLoad;
      float fCPUcoreMultiplierToUse = fFreqInMHz / mp_cpucoredata->
          m_arp_percpucoreattributes[byCoreID].m_fReferenceClockInMhz;
      if( fCPUcoreMultiplierToUse < //mp_cpucoredata->m_fMaximumCPUcoreMultiplier
          mp_cpucoredata->m_arfAvailableMultipliers[0] )
      {
        fCPUcoreMultiplierToUse = //mp_cpucoredata->m_fMaximumCPUcoreMultiplier;
            mp_cpucoredata->m_arfAvailableMultipliers[0];
        fFreqInMHz = fCPUcoreMultiplierToUse *
          m_arp_percpucoreattributes[byCoreID].m_fReferenceClockInMhz;
      }
      if( mp_cpucoredata->m_stdset_floatAvailableVoltagesInVolt.size() < 2)
      {
        LOGN( FULL_FUNC_NAME << " usage <= threshold for frequency increase "
          " -> calling SetCurrentVoltageAndMultiplier("
          << fCPUcoreMultiplierToUse )
        mp_cpucontroller->SetCurrentVoltageAndMultiplier(0.0f,
          fCPUcoreMultiplierToUse,
          byCoreID ) ;
      }
      else
      {
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "before mp_cpucontroller->SetFreqAndVoltageFromFreq")
      LOGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        "usage <= threshold for frequency increase "
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "before mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << m_fCPUcoreFreqInMHz * fLoad << ","
        << (WORD) byCoreID << ")" )
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( fFreqInMHz )
        , byCoreID
        );
      LOGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "after mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << m_fCPUcoreFreqInMHz * fLoad << ","
        << (WORD) byCoreID << ")" )
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "after mp_cpucontroller->SetFreqAndVoltageFromFreq")
      }
    }
  }
  //else
    r_percpucoreattributes.m_fPreviousCPUusage =
      //mp_cpucoredata->m_arfCPUcoreLoadInPercent[byCoreID] ;
      fLoad ;
  //mp_i_cpucontroller->SetFreqAndVoltage(
  //  byCoreID ,
  //  //mp_cpucoredata->m_arfCPUcoreLoadInPercent[byCoreID] 
  //  fLoad );
}

BYTE DynFreqScalingThreadBase::GetCPUcoreWithHighestLoad(
  float & fHighestCPUcoreLoadInPercent )
{
  BYTE byCPUcoreWithHighestLoad = 0 ;
//  float fHighestCPUcoreLoadInPercent = 0.0 ;
  fHighestCPUcoreLoadInPercent = 0.0 ;
  for( BYTE byCoreID = 0 ; byCoreID < mp_cpucoredata->
    GetNumberOfCPUcores() ; ++ byCoreID )
  {
    if( fHighestCPUcoreLoadInPercent < mp_cpucoredata->
      m_arfCPUcoreLoadInPercent [byCoreID] )
    {
      fHighestCPUcoreLoadInPercent = mp_cpucoredata->
        m_arfCPUcoreLoadInPercent [byCoreID] ;
      byCPUcoreWithHighestLoad = byCoreID ;
    }
  }
  return byCPUcoreWithHighestLoad ;
}

void DynFreqScalingThreadBase::HandleCPUnotTooHot()
{
  LOGN("DynFreqScalingThreadBase::HandleCPUnotTooHot()")
  BYTE by;
  for( unsigned coreID = 0 ; coreID < m_wNumCPUcores ; ++ coreID )
  {
    by = mp_cpucontroller->SetThrottleLevel(1.0, coreID);
    if(by == I_CPUcontroller::not_implemented )
      LOGN_DEBUG(FULL_FUNC_NAME << "SetThrottleLevel is not implemented")
  }
}

ExitCode DynFreqScalingThreadBase::Entry()
{
  m_vbDVFSthreadStopped = false ;
  LOGN("Dynamic Voltage and Frequency Scaling thread should run? " <<
    ( m_vbRun ? "yes" : "no") )
  LOGN("CPU core usage getter--address: " << mp_icpu )
  if( mp_icpu )
    mp_icpu->Init();
  LOGN("DVFS after initializing CPU core usage getter. Should run?: "
      << (m_vbRun ? "yes" : "no" ) )
  LOGN("1 CPU core power plane:" << mp_cpucontroller->m_b1CPUcorePowerPlane )

  //Refer directly (->faster?!).
  m_ar_fCPUcoreLoadInPercent = mp_cpucoredata->m_arfCPUcoreLoadInPercent ;
  m_b1stTimeInRowTooHot = true ;
  m_wNumCPUcores = mp_cpucoredata->GetNumberOfCPUcores() ;
  m_ar_fPreviousTemperaturesInDegCelsius = new float[ m_wNumCPUcores ] ;
  m_ar_fPreviousMinusCurrentTemperature = new float[ m_wNumCPUcores ] ;
  LOGN("DVFS thread: number of CPU cores:" << m_wNumCPUcores )
//  PerCPUcoreAttributes * m_arp_percpucoreattributes =
    m_arp_percpucoreattributes =
    mp_cpucoredata->m_arp_percpucoreattributes ;

//  HandleSameCPUcoreVoltageForAllCPUcores() ;
  //  else //if(mp_cpucontroller->m_b1CPUcorePowerPlane )
  {
//    BYTE byCoreID ;
//    //Declare outside of "SetLowerFrequencyFromAvailableMultipliers" to not
//    //be created every time.
//    float fFreqInMHz ;
//    float fLowerMultiplier ;
//    float fNextMultiplierCalculatedFromCurrentLoad ;
//    //Avoid Linux g++ warning "‘fFreqInMHz’ may be used uninitialized in this
//    // function"
//    SUPPRESS_UNUSED_VARIABLE_WARNING(fFreqInMHz)
//    SUPPRESS_UNUSED_VARIABLE_WARNING(fLowerMultiplier)
//    SUPPRESS_UNUSED_VARIABLE_WARNING(fNextMultiplierCalculatedFromCurrentLoad)
//    DWORD dwCurrentTimeInMillis ;
    while( m_vbRun )
    {
      LOGN_DEBUG("DVFS thread running")
      //DEBUG("begin of scaling thread loop\n");
//      if( ! m_bCalledInit )
//      {
//        m_bCalledInit = true ;
//        //mp_icpu->Init();
//      }
      //bool m_wCurrentFreqInMHz = false ;
      if(
        mp_cpucontroller->GetUserInterface()->m_bConfirmedYet
        //&& m_bSuccFullyGotPStateFromMSR
        )
      {
        LOGN("DynFreqScalingThreadBase::Entry: confirmed yet")

  //      //Wait until all threads reading from the CPU core data have finished.
  ////      mp_cpucoredata->m_conditionDVFSthreadMayChangeData.Wait() ;
  //      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
  //      //After the thread that called conditon.Wait() were woken up:
  //      //"It then locks the mutex again and returns."
  //      //So the mutex is unlocked at first if they call again "Wait()"?!
  //      mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Lock() ;
  //      LOGN("DynFreqScalingThreadBase::Entry(): after"
  //          "mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Lock()")
  //      //Now wait for the other threads that read the CPU core attributes
  //      //after Broadcast / while Sleep() at the end of this loop.
  //      //"This method atomically releases the lock on the mutex associated with
  //      //this condition[...]"
  //      mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Wait() ;

  //      LOGN("DynFreqScalingThreadBase::Entry(): after"
  //          "mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Wait()")
        //float fTempInDegCelsius ;

//        mp_cpucoredata.m_arp_percpucoreattributes[]

        if( mr_cpucontrolbase.
          //GetUsageAndVoltageAndFreqAndTempForAllCoresThreadSafe(
          GetUsageAndVoltageAndFreqForAllCoresThreadSafe(
            m_ar_fCPUcoreLoadInPercent, m_wNumCPUcores) )
        {
          LOGN( FULL_FUNC_NAME << " after GetCurrentVoltageAndFrequency")
          //check if a temp. could be received:
          //Pentium M CPUs have no temperature register
          if(
            //mp_i_cpucontroller->GetCurrentTempInDegCelsius(fTempInDegCelsius) ;
            //e.g. the CPU asserts "PROC_HOT" or the temp is above the throttle
            //temperature.
            (mp_cpucoredata->m_bTooHot = mp_cpucontroller->GetCPUcoreTooHot() )
            )
          {
            LOGN("DynFreqScalingThreadBase::Entry()--too hot")
            HandleCPUtooHotDVFS(//dwCurrentTimeInMillis
              ) ;
          }//too hot
          else
          {
            HandleCPUnotTooHot() ;
          }
          m_bGotCPUcoreDataAtLeast1Time = true ;
        }
        else
          DEBUGN("DynFSB::Entry()-GetPercentalUsageForAllCores() failed()")
      }//m_bConfirmedYet
      else
        {
        DEBUGN("DynFreqScalingThreadBase::Entry: NOT confirmed yet")
        }
  //    //All CPU core data is retrieved-> now the IPC can send the data without
  //    // data corruption (that can happen when accessing the data from the IPC
  //    // thread at the same time)
  //    if( mp_cpucoredata->m_eventCurrentCPUdataRetrieved.Broadcast()
  //        )

      //use an inner block so that the mutexlocker object is destroyed early enough
      // and so the mutex is unlocked when leaving this block.
      {
  //      LOGN("DVFS before lock(mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead)")
  ////      //from http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionctor:
  ////      // tell the other(s) thread(s) that the CPU core data may be accessed now:
  ////      //  we must
  ////      // lock the mutex first or we might signal the condition before the
  ////      // waiting threads start waiting on it!
  //      wxMutexLocker lock(mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead);

        SignalCPUdataCanBeSafelyRead() ;
      }
      //LOGN("End of scaling thread loop");
      OperatingSystem::Sleep(//m_wMilliSecondsToWait
        mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS );
      LOGN("DynFreqScalingThreadBase::Entry(): after "
          "Sleep"
#ifdef _DEBUG
          "(" << mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS << ")"
#endif
          )
    }//loop
  }
  //Release clients waiting on the condition.
  SignalCPUdataCanBeSafelyRead() ;
  LOGN("ended the Dynamic Voltage and Frequency Scaling loop")
  m_vbDVFSthreadStopped = true ;
  return 0;
}

//inline void DynFreqScalingThreadBase::GetLowerFrequencyFromAvailableMultipliers(
//  //Because this "function" is inline parameters are not copied but replaced.
//  BYTE byCoreID ,
//  PerCPUcoreAttributes * arp_percpucoreattributes  ,
//  float ar_fCPUcoreLoadInPercent [],
//  float fFreqInMHz,
//  WORD wNumCPUcores ,
//  float fLowerMultiplier ,
//  float fNextMultiplierCalculatedFromCurrentLoad
//  )
//{
//  fNextMultiplierCalculatedFromCurrentLoad =
//    arp_percpucoreattributes[ byCoreID].m_fMultiplier
//    * ar_fCPUcoreLoadInPercent[byCoreID] ;
//  fLowerMultiplier = //mp_cpucoredata->GetLowerMultiplier(
//    GetClosestLess(
//    mp_cpucoredata->m_arfAvailableMultipliers
//    , wNumCPUcores
//    , arp_percpucoreattributes[ byCoreID].m_fMultiplier
//    ) ;
//  LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//    "lower multi:" << fLowerMultiplier
//    << "multi from load:" << fNextMultiplierCalculatedFromCurrentLoad )
//  if( //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID] >=
//  //                  (float) mp_cpucoredata->m_wMaxFreqInMHz /
//  //                  //(float) mp_i_cpucontroller->GetFrequencyInMHz(byCoreID)
//  //                  m_fCPUcoreFreqInMHz >=
//  //                  //50% of max. freq
//  //                  0.5f
//    //&& fTempInDegCelsius > 90.0f
//  //                    fFreqInMHz * ar_fCPUcoreLoadInPercent[byCoreID] <
//      fNextMultiplierCalculatedFromCurrentLoad
//         <
//  //                    arp_percpucoreattributes[ byCoreID].GetFreq( fLowerMuliplier )
//        fLowerMultiplier
//    )
//  {
//    fFreqInMHz = arp_percpucoreattributes[ byCoreID].GetFreqInMHz() ;
//    LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//      "lower multi < multi from load ->setting freq" << fFreqInMHz )
//    //mp_cpucontroller->SetFrequencyInMHz(
//  }
//  else
//  {
//    fFreqInMHz = fLowerMultiplier *
//        arp_percpucoreattributes[ byCoreID].m_fReferenceClockInMhz ;
//    LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//        "lower multi >= multi from load  ->setting freq" << fFreqInMHz )
//  }
//}

void DynFreqScalingThreadBase::HandleSameCPUcoreVoltageForAllCPUcores()
{
  //If ganged then there is only 1 power plane for ALL CPU cores.
  //-> setting voltage affects _all_ CPU cores.
  //So change the performance state only for 1 core .
  if( //mp_cpucoredata->m_b1CPUcorePowerPlane
//    mp_cpucontroller->m_b1CPUcorePowerPlane
    false
    )
  {
    BYTE byCoreID ;
    BYTE byCPUcoreWithHighestLoad = 0 ;
    float fHighestCPUcoreLoadInPercent = 0.0 ;
    float fHighestMultiplier ;
    float fFreqInMHz ;
    float fLowerMultiplier ;
    float fNextMultiplierCalculatedFromCurrentLoad ;
    WORD wCoreWithHighestMultiplier ;
    while( m_vbRun )
    {
      //LOGN("DVFS thread running ")
      //DEBUG("begin of scaling thread loop\n");
//      if( ! m_bCalledInit )
//      {
//        m_bCalledInit = true ;
//        //mp_icpu->Init();
//      }
      //bool m_wCurrentFreqInMHz = false ;
      if( mp_cpucontroller->GetUserInterface()->m_bConfirmedYet
        //&& m_bSuccFullyGotPStateFromMSR
        )
      {
  //      LOGN("DynFreqScalingThreadBase::Entry: confirmed yet")

  //      //Wait until all threads reading from the CPU core data have finished.
  ////      mp_cpucoredata->m_conditionDVFSthreadMayChangeData.Wait() ;
  //      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
  //      //After the thread that called conditon.Wait() were woken up:
  //      //"It then locks the mutex again and returns."
  //      //So the mutex is unlocked at first if they call again "Wait()"?!
  //      mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Lock() ;
  //      LOGN("DynFreqScalingThreadBase::Entry(): after"
  //          "mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Lock()")
  //      //Now wait for the other threads that read the CPU core attributes
  //      //after Broadcast / while Sleep() at the end of this loop.
  //      //"This method atomically releases the lock on the mutex associated with
  //      //this condition[...]"
  //      mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Wait() ;

  //      LOGN("DynFreqScalingThreadBase::Entry(): after"
  //          "mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead.Wait()")
        //float fTempInDegCelsius ;

        //TODO exit thread when getting CPU core load fails?
        if( ! mp_icpu ||
            ( mp_icpu && mp_icpu->//GetPercentalUsageForBothCores
              GetPercentalUsageForAllCores(
              //mp_cpucoredata->m_arfCPUcoreLoadInPercent
  //            ar_fCPUcoreLoadInPercent
              m_ar_fCPUcoreLoadInPercent
              )
            )
          )
        {
          fHighestMultiplier = 0.0 ;
          //Get the current voltage etc. for ALL cores for sending the data for
          // all cores via IPC, even if not needed for
          //DVFS (if single power plane / all cores always at the same p-state
          // then only the frequency for the core with the highest load is
          //needed).
          for( byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
          {
            mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
              byCoreID) ;
            LOGN("multi for core " << (WORD) byCoreID
              << ":" << m_arp_percpucoreattributes[byCoreID].m_fMultiplier )
            //Even if CPU seemed to have 1 power plane (Nehalem) the different
            //cores had different multipliers.
            if( fHighestMultiplier <
              m_arp_percpucoreattributes[byCoreID].m_fMultiplier )
            {
              fHighestMultiplier = m_arp_percpucoreattributes[byCoreID].
                m_fMultiplier ;
              wCoreWithHighestMultiplier = byCoreID ;
            }
          }
          LOGN("highest multi for all cores:" << fHighestMultiplier )
//          //Even if CPU seemed to have 1 power plane (Nehalem) the different
//          //cores had different multipliers.
//          GetHighestMultiplier() ;

          //check if a temp. could be received:
          //Pentium M CPUs have no temperature register
          if(
            //mp_i_cpucontroller->GetCurrentTempInDegCelsius(fTempInDegCelsius) ;
            //e.g. the CPU asserts "PROC_HOT" or the temp is above the throttle
            //temperature.
            (mp_cpucoredata->m_bTooHot = mp_cpucontroller->GetCPUcoreTooHot() )
            )
          {
            LOGN("too hot:yes")
            byCPUcoreWithHighestLoad = GetCPUcoreWithHighestLoad(
                fHighestCPUcoreLoadInPercent ) ;
            LOGN("CPU core with highest load:" <<
              (WORD) byCPUcoreWithHighestLoad )
        //              mp_cpucontroller->GetCurrentPstate( m_fCPUcoreFreqInMHz, fVolt,
        //                //0
        //                byCPUcoreWithHighestLoad ) ;
//            m_fCPUcoreFreqInMHz =
//              //"(WORD)" to avoid g++ compiler warning "converting to `WORD'
//              //from `float'"
//              (WORD) arp_percpucoreattributes[ byCPUcoreWithHighestLoad ].
//              GetFreqInMHz() ;
//            LOGN("freq of CPU core with highest load:" << m_fCPUcoreFreqInMHz )
//            //std::set<VoltageAndFreq> stdsetvoltageandfreq ;
//            //mp_cpucontroller->GetAllPossibleFrequencies(stdsetvoltageandfreq) ;
//
//            //Reduce the freq because because the CPU is too hot.
//            WORD wNewFreqInMHz = mp_cpucontroller->
//              GetNearestLowerPossibleFreqInMHz( m_fCPUcoreFreqInMHz ) ;
//            LOGN("nearest lower freq:" << wNewFreqInMHz )
//            WORD wNewFreqFromLoad = (WORD) (
//              fHighestCPUcoreLoadInPercent *
//        //                mp_cpucoredata->m_arfCPUcoreLoadInPercent[
//        //                 byCPUcoreWithHighestLoad ] *
//              (float) m_fCPUcoreFreqInMHz ) ;
//            if( wNewFreqInMHz != 0 &&
//              wNewFreqFromLoad > wNewFreqInMHz )
//            {
//              LOGN("wNewFreqFromLoad > wNewFreqInMHz")
//              mp_cpucontroller->SetFreqAndVoltageFromFreq(
//                wNewFreqInMHz //) ;
//                , //0
//                byCPUcoreWithHighestLoad ) ;
//            }
//            //if( //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID] >=
//            //  (float) mp_cpucoredata->m_wMaxFreqInMHz /
//            //  //(float) mp_i_cpucontroller->GetFrequencyInMHz(byCoreID)
//            //  m_fCPUcoreFreqInMHz >=
//            //  //50% of max. freq
//            //  0.5f
//            //  //&& fTempInDegCelsius > 90.0f
//            //  )
//            //  //mp_cpucontroller->SetFrequencyInMHz(
//            //  mp_cpucontroller->SetFreqAndVoltageFromFreq(
//            //    //Set half of max. freq because the CPU is too hot
//            //    mp_cpucoredata->m_wMaxFreqInMHz / 2 //) ;
//            //    , 0 ) ;
//            else
//            {
//              LOGN("wNewFreqFromLoad <= wNewFreqInMHz")
//              //ChangeOperatingPointByLoad( 0 , fHighestCPUcoreLoadInPercent
//              mp_cpucontroller->SetFreqAndVoltageFromFreq(
//                  wNewFreqFromLoad, 0
//              ) ;
//            }
//            SetLowerFrequencyFromAvailableMultipliers(
////              byCPUcoreWithHighestLoad ,
//              (BYTE) wCoreWithHighestMultiplier ,
//              arp_percpucoreattributes ,
//              ar_fCPUcoreLoadInPercent ,
//              fFreqInMHz,
//              wNumCPUcores ,
//              fLowerMultiplier ,
//              fNextMultiplierCalculatedFromCurrentLoad
//              ) ;
//            GetLowerFrequencyFromAvailableMultipliers(
////              byCPUcoreWithHighestLoad ,
//              (BYTE) wCoreWithHighestMultiplier ,
//              arp_percpucoreattributes ,
//              ar_fCPUcoreLoadInPercent ,
//              fFreqInMHz,
//              wNumCPUcores ,
//              fLowerMultiplier ,
//              fNextMultiplierCalculatedFromCurrentLoad
//              ) ;
            for( byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
            {
  //                mp_cpucontroller->GetCurrentPstate(m_fCPUcoreFreqInMHz, fVolt, byCoreID) ;
  //                mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(byCoreID) ;
              SetLowerFrequencyFromAvailableMultipliers(
                byCoreID ,
                m_arp_percpucoreattributes ,
//                ar_fCPUcoreLoadInPercent ,
                m_ar_fCPUcoreLoadInPercent ,
                fFreqInMHz,
//                m_wNumCPUcores ,
                fLowerMultiplier ,
                fNextMultiplierCalculatedFromCurrentLoad
                ) ;
            }// for-loop
          }//too hot
          else
          {
            LOGN("too hot:no")
            byCPUcoreWithHighestLoad = GetCPUcoreWithHighestLoad(
              fHighestCPUcoreLoadInPercent ) ;
//            DEBUGN("highest load of a CPU cores:" <<
//                fHighestCPUcoreLoadInPercent )
            DEBUGN("DynFreqScalingThreadBase::Entry(): before"
                "ChangeOperatingPointByLoad("
              << (WORD) byCPUcoreWithHighestLoad << ","
              << fHighestCPUcoreLoadInPercent << ")")
            ChangeOperatingPointByLoad( //0
              //It seems to matter which CPU core is set to the highest freq:
              //if core #6 had the highest load and only core 0 was set to the
              //new freq,
              //-super Pi took 17s when affinity only to core 6 and 100% load at
              //core 6
              //-in contrast:
              //super Pi took 8s when affinity only to core 0 and 100% load at
              //core 6
              byCPUcoreWithHighestLoad , fHighestCPUcoreLoadInPercent
              ) ;
            DEBUGN("DynFreqScalingThreadBase::Entry(): after"
                "ChangeOperatingPointByLoad("
              << (WORD) byCPUcoreWithHighestLoad << ","
              << fHighestCPUcoreLoadInPercent << ")")
          }
        }
      }
      //    //All CPU core data is retrieved-> now the IPC can send the data without
      //    // data corruption (that can happen when accessing the data from the IPC
      //    // thread at the same time)
      //    if( mp_cpucoredata->m_eventCurrentCPUdataRetrieved.Broadcast()
      //        )

      //use an inner block so that the mutexlocker object is destroyed early enough
      // and so the mutex is unlocked when leaving this block.
      {
  //      LOGN("DVFS before lock(mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead)")
  ////      //from http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionctor:
  ////      // tell the other(s) thread(s) that the CPU core data may be accessed now:
  ////      //  we must
  ////      // lock the mutex first or we might signal the condition before the
  ////      // waiting threads start waiting on it!
  //      wxMutexLocker lock(mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead);

        SignalCPUdataCanBeSafelyRead() ;
      }
      //LOGN("End of scaling thread loop");
      OperatingSystem::Sleep(//m_wMilliSecondsToWait
        mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS );
      DEBUGN("DynFreqScalingThreadBase::Entry(): after "
          "Sleep(" << mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS << ")")
    } //loop
  }
}

void DynFreqScalingThreadBase::HandleCPUtooHotDVFS()
{
  if( m_b1stTimeInRowTooHot )
  {
    LOGN("DynFreqScalingThreadBase::HandleCPUtooHotDVFS()--1st time too hot")
    m_dwBeginInMillisOfTooHot = ::GetTickCount() ;
    m_b1stTimeInRowTooHot = false ;
    m_wMilliSecondsPassed = 0 ;
    //In order to calc the difference between the temperature where it
    //began to be too hot and the current temperature.
    SafePreviousTemperatures() ;
  }
  else
  {
    LOGN("DynFreqScalingThreadBase::HandleCPUtooHotDVFS()--at least 2nd time "
      "too hot")
//    dwCurrentTimeInMillis = ::GetTickCount() ;
    m_dwCurrentTimeInMillis = ::GetTickCount() ;
//    m_wMilliSecondsPassed = ULONG_VALUE_DIFF( dwCurrentTimeInMillis
//      , m_dwBeginInMillisOfTooHot ) ;
    m_wMilliSecondsPassed = ULONG_VALUE_DIFF( m_dwCurrentTimeInMillis
      , m_dwBeginInMillisOfTooHot ) ;
    LOGN(" milliseconds passed: " << m_wMilliSecondsPassed
      << "milliSeconds to wait for cooldown:"
      << m_wMilliSecondsToWaitForCoolDown )
    CalcDiffBetweenCurrentAndPreviousTemperature() ;
    for( byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
    {
      LOGN("DynFreqScalingThreadBase::HandleCPUtooHotDVFS()--temperature "
        "difference between prev and now:"
        << m_ar_fPreviousMinusCurrentTemperature[byCoreID] << " deg Celsius "
        "for core " << (WORD) byCoreID
        )
      if( //If temperature did not decrease.
          m_ar_fPreviousMinusCurrentTemperature[byCoreID] <= 0.0f
          && m_wMilliSecondsPassed > m_wMilliSecondsToWaitForCoolDown
  //                  DidNotCoolDownInTime()
          )
      {
        LOGN("DynFreqScalingThreadBase::HandleCPUtooHotDVFS()--temperature "
          "increased more time passed than to wait for cooldown for core "
          << (WORD) byCoreID
          )
  //                mp_cpucontroller->GetCurrentPstate(m_fCPUcoreFreqInMHz, fVolt, byCoreID) ;
  //                mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(byCoreID) ;
        SetLowerFrequencyFromAvailableMultipliers(
          byCoreID ,
          m_arp_percpucoreattributes ,
          m_ar_fCPUcoreLoadInPercent ,
//          fFreqInMHz,
          m_fFreqInMHz,
  //                    m_wNumCPUcores ,
//          fLowerMultiplier ,
          m_fLowerMultiplier ,
//          fNextMultiplierCalculatedFromCurrentLoad
          m_fNextMultiplierCalculatedFromCurrentLoad
          ) ;
//        m_dwBeginInMillisOfTooHot = dwCurrentTimeInMillis ;
        m_dwBeginInMillisOfTooHot = m_dwCurrentTimeInMillis ;
        m_b1stTimeInRowTooHot = true ;
      }
    }// for-loop
  }
  //DEBUG_COUTN(
  //            LOGN("too hot:yes")
    //DEBUG("core 0 usage: %lf\n",mp_cpucoredata->m_arfCPUcoreLoadInPercent);
    //DEBUG("Frequency before (possible) change:%u, core0 load:%f\n",
    //  mp_cpucoredata->m_arwCurrentFreqInMHz[0],mp_cpucoredata->
    //  m_arfCPUcoreLoadInPercent);

  if( mp_cpucoredata->m_byUpdateViewOnDVFS )
    //e.g. force redraw if it's a GUI.
    mp_cpucontroller->GetUserInterface()->
      CurrenCPUfreqAndVoltageUpdated() ;
  //          }//getperc usage
}

bool DynFreqScalingThreadBase::IsStopped()
{
  LOGN( FULL_FUNC_NAME << "--DVFS is stopped?: " << (m_vbDVFSthreadStopped ?
    "yes" : "no") )
  return m_vbDVFSthreadStopped ;
}

void DynFreqScalingThreadBase::SafePreviousTemperatures()
{
//  LOGN("DynFreqScalingThreadBase::SafePreviousTemperatures() begin")
//  if( m_bGotCPUcoreDataAtLeast1Time )
  {
    for( BYTE byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
    {
//      LOGN("DynFreqScalingThreadBase::SafePreviousTemperatures() index:"
//        << (WORD) byCoreID
//        << "temp.:" << m_arp_percpucoreattributes[ byCoreID ].
//        m_fTempInDegCelsius )
      m_ar_fPreviousTemperaturesInDegCelsius [byCoreID] =
        m_arp_percpucoreattributes[ byCoreID ].m_fTempInDegCelsius ;
    }
  }
//  LOGN("DynFreqScalingThreadBase::SafePreviousTemperatures() end")
}

inline void DynFreqScalingThreadBase::SetLowerFrequencyFromAvailableMultipliers(
  //Because this "function" is inline parameters are not copied but replaced.
  BYTE byCoreID ,
  PerCPUcoreAttributes * arp_percpucoreattributes  ,
  float ar_fCPUcoreLoadInPercent [],
  float fFreqInMHz,
//  WORD wNumCPUcores ,
  float fLowerMultiplier ,
  float fNextMultiplierCalculatedFromCurrentLoad
  )
{
//  LOGN("SetLowerFrequencyFromAvailableMultipliers begin"
//    << "core ID:" << (WORD) byCoreID
//    << "current multiplier:" <<
//    arp_percpucoreattributes[ byCoreID].m_fMultiplier )
  const float currentMulti = arp_percpucoreattributes[ byCoreID].m_fMultiplier;
  //fLowerMultiplier = //mp_cpucoredata->GetLowerMultiplier(
  MANUFACTURER_ID_NAMESPACE::BinarySearch::arrayIndexType wArrayIndexOfLowerValue =
    MANUFACTURER_ID_NAMESPACE::BinarySearch::GetClosestLess(
    mp_cpucoredata->m_arfAvailableMultipliers,
    mp_cpucoredata->m_stdset_floatAvailableMultipliers.size(),
    currentMulti
    ) ;
  LOGN( FULL_FUNC_NAME << " array index for closest multiplier to "
    << currentMulti << ":" << wArrayIndexOfLowerValue)
  const float lowestMulti = mp_cpucoredata->m_arfAvailableMultipliers[ 0 ];
  if( wArrayIndexOfLowerValue < MANUFACTURER_ID_NAMESPACE::BinarySearch::no_element )
  {
    fLowerMultiplier = mp_cpucoredata->m_arfAvailableMultipliers[
     wArrayIndexOfLowerValue ] ;
    LOGN( FULL_FUNC_NAME << " closest multiplier to " << currentMulti << ":"
      << fLowerMultiplier)
  }
  else
  {
    fLowerMultiplier = lowestMulti ;
    LOGN( FULL_FUNC_NAME << " closest multiplier to " << currentMulti
      << "= lowest multiplier (" << fLowerMultiplier << ")")
  }
//  LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//    "closest lower multi:" << fLowerMultiplier
////    << " multi from load(mult" << currentMulti
////    << "*" << )":" << fNextMultiplierCalculatedFromCurrentLoad
//    )
  //TODO m_fReferenceClockInMhz  may be 0 / unknown
  fNextMultiplierCalculatedFromCurrentLoad = currentMulti
    //the load may be ~0 or even < 0 (depends on CPU core usage getter code)
    * ar_fCPUcoreLoadInPercent[byCoreID] ;
  LOGN("DynFreqScalingThreadBase::SetLowerFrequency core "
    << (WORD) byCoreID
    << " current multiplier: " << currentMulti
    << ", next lower multiplier:" << fLowerMultiplier
    << " multi from load="
    << "multi(" << currentMulti
    << ")*load(" << ar_fCPUcoreLoadInPercent[byCoreID]
    << ")=" << fNextMultiplierCalculatedFromCurrentLoad )
  float fCPUcoreMultiplierToUse;
  //NextMultiplierCalculatedFromCurrentLoad may be < lowest multi
  if(   //                    fFreqInMHz * ar_fCPUcoreLoadInPercent[byCoreID] <
      fNextMultiplierCalculatedFromCurrentLoad
         <
  //                    arp_percpucoreattributes[ byCoreID].GetFreq( fLowerMuliplier )
      fLowerMultiplier
    )
  {
    fCPUcoreMultiplierToUse = fNextMultiplierCalculatedFromCurrentLoad;
    if( fCPUcoreMultiplierToUse < lowestMulti )
      fCPUcoreMultiplierToUse = lowestMulti;
  }
  else
  {
    fCPUcoreMultiplierToUse = fLowerMultiplier;
  }
  LOGN( FULL_FUNC_NAME << " multiplier to use:" << fCPUcoreMultiplierToUse)
  //If already at lowest multiplier.
  if( currentMulti == fCPUcoreMultiplierToUse )
    mp_cpucontroller->SetNextLowerThrottleLevel(byCoreID,
      fCPUcoreMultiplierToUse);

//  if( fCPUcoreMultiplierToUse > mp_cpucoredata->m_fMaximumCPUcoreMultiplier)
//    fCPUcoreMultiplierToUse = mp_cpucoredata->m_fMaximumCPUcoreMultiplier;
  fFreqInMHz = fCPUcoreMultiplierToUse *
      arp_percpucoreattributes[ byCoreID].m_fReferenceClockInMhz ;
//    LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//        "lower multi >= multi from load  ->setting freq" << fFreqInMHz )
  mp_cpucontroller->SetFreqAndVoltageFromFreq(
    //"(WORD)" to avoid
    //"passing `float' for converting 1 of `virtual BYTE
    // I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, BYTE)'"
    (WORD) ( fFreqInMHz )
    , byCoreID
    ) ;
//    LOGN("DynFreqScalingThreadBase::SetLowerFrequency "
//      "multi from load<lower multi ->setting freq" << fFreqInMHz )
}

inline void DynFreqScalingThreadBase::SignalCPUdataCanBeSafelyRead()
{
  LOGN("DynFreqScalingThreadBase::Entry(): before "
      "mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
  //Let all threads waiting (that called condition.Wait() ) on the
  // condition continue.
  //"Wait() atomically unlocks the mutex
  // which allows the thread to continue and starts waiting"
  //blocks here when using wxCondition::Broadcast() (wxWidgets bug?)
//      mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast();
  mp_cpucoredata->m_condition_typeCPUdataCanBeSafelyRead.Broadcast() ;
//  mp_cpucoredata->m_condition_typeCPUdataCanBeSafelyRead.ResetEvent() ;
  LOGN("DynFreqScalingThreadBase::Entry(): after "
      "mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
}

//SyncOnStoppedState()

//BYTE
//Use DWORD because of GetLastError(...) return code. 0=success
DWORD DynFreqScalingThreadBase::Start()
{
  LOGN("DynFreqScalingThreadBase::Start()")
  m_vbRun = true ;
  LOGN("DynFreqScalingThreadBase::Start()--return" << 0 )
  return 0 ;
}

//The DVFS thread is only stopped if the loop ended at first.
void DynFreqScalingThreadBase::Stop()
{
  LOGN("DynFreqScalingThreadBase::Stop()")
  m_vbRun = false ;
}

DWORD //__stdcall is important for Windows' ::CreateThread()
//Built-in preprocessor macro for MSVC, MinGW (also for 64 bit)
#ifdef _WIN32 //under Linux g++ error if "__stdcall"
  __stdcall
#endif
  //DynFreqScalingThreadBaseNameSpace::ThreadFunction( void * pv )
  DynFreqScalingThreadBase::ThreadFunction( void * pv )
{
  LOGN_DEBUG("DynFreqScalingThreadBase::ThreadFunction begin--pointer to "
    "DynFreqScalingThreadBase object:" << pv )
  DynFreqScalingThreadBase * p_dynfreqscalingthreadbase =
    (DynFreqScalingThreadBase *) pv ;
  if( p_dynfreqscalingthreadbase )
  {
    //Set "m_vbRun" to true.
    p_dynfreqscalingthreadbase->Start() ;
    p_dynfreqscalingthreadbase->Entry() ;
    return 0 ;
  }
  return 1;
}
