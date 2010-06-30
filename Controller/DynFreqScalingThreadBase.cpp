#include "DynFreqScalingThreadBase.hpp"

#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <ModelData/CPUcoreData.hpp>
#include <UserInterface/UserInterface.hpp> //for UserInterface.m_bConfirmedYet
#include <binary_search.cpp> //GetClosesLess()
#include <global.h> //LOGN
#include "Sleep.h"

DynFreqScalingThreadBase::DynFreqScalingThreadBase(
  ICPUcoreUsageGetter * p_icpu
  , I_CPUcontroller * p_cpucontroller
  , CPUcoreData & r_cpucoredata
  )
  : //m_bSuccFullyGotPStateFromMSR(false)
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  mp_cpucoredata(&r_cpucoredata)
  , mp_cpucontroller ( p_cpucontroller )
  , m_bCalledInit(false)
  , m_wMilliSecondsToWait(100)
  , m_vbRun(true)
  , m_vbDVFSthreadStopped(true)
{
//  //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//  // the mutex should be initially locked
//  mp_cpucoredata->m_mutexDVFSthreadMayChangeData.Lock() ;

    DEBUG("constructor of freq scaling thread base--begin\n");
    mp_icpu = p_icpu ;
    LOGN("core usage address: " << p_icpu )
    //mp_icpu->Init();
    m_wMaxFreqInMHz = //2200
      r_cpucoredata.m_wMaxFreqInMHz ;
    //wxTimer();
    m_wCurrentFreqInMHz = //550
      m_wMaxFreqInMHz / 4;
    m_fPercentileIncrease = 1.5f ;
    m_wAHalfOfMaxFreq = m_wMaxFreqInMHz / 2 ;
    m_wAQuarterOfMaxFreq = m_wMaxFreqInMHz / 4 ;
    //mp_pumastatectrl = p_pumastatectrl ;
    mp_cpucontroller = p_cpucontroller ;
    DEBUG("constructor of freq scaling thread base--end\n");
}

void DynFreqScalingThreadBase::ChangeOperatingPointByLoad( 
  BYTE byCoreID 
  , float fLoad 
  )
{
  DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad")
  PerCPUcoreAttributes & r_percpucoreattributes = mp_cpucoredata->
    m_arp_percpucoreattributes[byCoreID] ;
//  DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad "
//    " address of r_percpucoreattributes:" << & r_percpucoreattributes)
  //if ( //m_fPreviousCPUusage 
  //  ////mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
  //  //r_percpucoreattributes.
  //  ////If the CPU usage was NOT retrieved for the first time.
  //  //m_fPreviousCPUusage != -1.0f 
  //  )
  {
    if ( //dClocksNotHaltedDiffDivTCSdiff > 
      //mp_cpucoredata->m_arfCPUcoreLoadInPercent[byCoreID] >
      fLoad >
      //m_fPreviousCPUusage 
      //r_percpucoreattributes.m_fPreviousCPUusage * 0.75 //* 1.5f 
      //0.75
      mp_cpucoredata->m_fCPUcoreLoadThresholdForIncreaseInPercent
      ////e.g. 1100 / 2200 * 0.75 = 0.5 * 0.75 = 0.375
      //p_percpucoreattributes->m_wCurrentFreqInMHz / 
      //  mp_cpucoredata->m_wMaxFreqInMHz 
      //  //This is important for increasing the frequency: if NOT multiplied
      //  //this condition ">" is not fullfilled. 
      //  * 0.75
      )
    {
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "load > threshold for increase")
      //m_fPreviousCPUusage 
      r_percpucoreattributes.m_fPreviousCPUusage = 
        //dClocksNotHaltedDiffDivTCSdiff 
        fLoad ;
      //raise the freq.
      //dClocksNotHaltedDiffDivTCSdiff 

      //fLoad *= 1.5 ;
//      float fVoltage ;
      WORD wFreqInMHz ;
//      mp_cpucontroller->GetCurrentPstate(
//        wFreqInMHz
//        , fVoltage
//        , byCoreID
//        ) ;
//      mp_cpucontroller->GetCurrentVoltageAndFrequency(
//        byCoreID
//        ) ;
      wFreqInMHz = //"(WORD)" to avoid g++ compiler warning
          //"converting to `WORD' from `float'"
          (WORD) mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
          GetFreqInMHz() ;
      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "before mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << wFreqInMHz * mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor
        << (WORD) byCoreID << ")" )
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( wFreqInMHz * //1.5f
            mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor )
        , byCoreID
        );
      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "after mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << wFreqInMHz * mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor << ","
        << (WORD) byCoreID << ")" )
    }
    else
    {
//      float fVoltage ;
      WORD wFreqInMHz ;
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
//        wFreqInMHz
//        , fVoltage
//        , byCoreID
//        ) ;
//      mp_cpucontroller->GetCurrentVoltageAndFrequency(
//        byCoreID
//        ) ;
      wFreqInMHz =
        //Avoid g++ compiler warning "converting to `WORD' from `float'"
        (WORD) mp_cpucoredata->m_arp_percpucoreattributes[byCoreID].
        GetFreqInMHz() ;
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "before mp_cpucontroller->SetFreqAndVoltageFromFreq")
      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "before mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << wFreqInMHz * fLoad << ","
        << (WORD) byCoreID << ")" )
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( wFreqInMHz * fLoad )
        , byCoreID
        );
      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad("
        << (WORD) byCoreID << ","
        << fLoad << ") "
        "after mp_cpucontroller->SetFreqAndVoltageFromFreq("
        << wFreqInMHz * fLoad << ","
        << (WORD) byCoreID << ")" )
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "after mp_cpucontroller->SetFreqAndVoltageFromFreq")
    }
  }
  //else
    r_percpucoreattributes.m_fPreviousCPUusage = //dClocksNotHaltedDiffDivTCSdiff ;
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

ExitCode DynFreqScalingThreadBase::Entry()
{
  m_vbDVFSthreadStopped = false ;
  LOGN("Dynamic Voltage and Frequency Scaling thread should run? " <<
    ( m_vbRun ? "yes" : "no") )
  LOGN("CPU core usage getter--address: " << mp_icpu )
  mp_icpu->Init();
  LOGN("DVFS after initializing CPU core usage getter. Should run?: "
      << (m_vbRun ? "yes" : "no" ) )
  while(//1
    m_vbRun
    )
  {
    //LOGN("DVFS thread running ")
    //MessageBeep( (WORD) -1 );
    //DEBUG("begin of scaling thread loop\n");
    if( ! m_bCalledInit )
    {
      m_bCalledInit = true ;
      //mp_icpu->Init();
    }
    //bool m_wCurrentFreqInMHz = false ;
    if(
      mp_cpucontroller->GetUserInterface()->m_bConfirmedYet
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

      //Refer directly (->faster?!).
      float * ar_fCPUcoreLoadInPercent = mp_cpucoredata->
        m_arfCPUcoreLoadInPercent ;
      //TODO exit thread when getting CPU core load fails?
      if( mp_icpu->//GetPercentalUsageForBothCores
          GetPercentalUsageForAllCores( //mp_cpucoredata->
          //m_arfCPUcoreLoadInPercent
          ar_fCPUcoreLoadInPercent
          )
        )
      {
        WORD wNumCPUcores = mp_cpucoredata->GetNumberOfCPUcores() ;
        //Get the current voltage etc. for ALL cores for sending the data for
        // all cores via IPC, even if not needed for
        //DVFS (if single power plane / all cores always at the same p-state
        // then only the frequency for the core with the highest load is
        //needed).
        for( BYTE byCoreID = 0 ; byCoreID < wNumCPUcores ; ++ byCoreID )
        {
          mp_cpucontroller->GetCurrentVoltageAndFrequency(byCoreID) ;
        }

        PerCPUcoreAttributes * arp_percpucoreattributes =
          mp_cpucoredata->m_arp_percpucoreattributes ;
        //check if a temp. could be received:
        //Pentium M CPUs have no temperature register
        if(
          //mp_i_cpucontroller->GetCurrentTempInDegCelsius(fTempInDegCelsius) ;
          //e.g. the CPU asserts "PROC_HOT" or the temp is above the throttle
          //temperature.
          mp_cpucontroller->TooHot()
          )
        {
          //DEBUG_COUTN(
          DEBUGN("too hot:yes")
            //DEBUG("core 0 usage: %lf\n",mp_cpucoredata->m_arfCPUcoreLoadInPercent);
            //DEBUG("Frequency before (possible) change:%u, core0 load:%f\n",
            //  mp_cpucoredata->m_arwCurrentFreqInMHz[0],mp_cpucoredata->
            //  m_arfCPUcoreLoadInPercent);
            WORD wFreqInMHz ;
//            float fVolt ;
            //If ganged then there is only 1 power plane for ALL CPU cores.
            //-> setting voltage affects _all_ CPU cores. So change it only 
            //1 performance state.
            if( //mp_cpucoredata->m_b1CPUcorePowerPlane
              mp_cpucontroller->m_b1CPUcorePowerPlane )
            {
              BYTE byCPUcoreWithHighestLoad = 0 ;
              float fHighestCPUcoreLoadInPercent = 0.0 ;
              byCPUcoreWithHighestLoad = GetCPUcoreWithHighestLoad(
                  fHighestCPUcoreLoadInPercent ) ;
//              mp_cpucontroller->GetCurrentPstate( wFreqInMHz, fVolt,
//                //0
//                byCPUcoreWithHighestLoad ) ;
              wFreqInMHz =
                //"(WORD)" to avoid g++ compiler warning "converting to `WORD'
                //from `float'"
                (WORD) arp_percpucoreattributes[ byCPUcoreWithHighestLoad ].
                GetFreqInMHz() ;
              //std::set<VoltageAndFreq> stdsetvoltageandfreq ;
              //mp_cpucontroller->GetAllPossibleFrequencies(stdsetvoltageandfreq) ;

              //Reduce the freq because because the CPU is too hot.
              WORD wNewFreqInMHz = mp_cpucontroller->
                GetNearestLowerPossibleFreqInMHz( wFreqInMHz ) ;
              WORD wNewFreqFromLoad = (WORD) (
                fHighestCPUcoreLoadInPercent *
//                mp_cpucoredata->m_arfCPUcoreLoadInPercent[
//                 byCPUcoreWithHighestLoad ] *
                (float) wFreqInMHz ) ;
              if( wNewFreqInMHz != 0 &&
                wNewFreqFromLoad > wNewFreqInMHz )
              {
                mp_cpucontroller->SetFreqAndVoltageFromFreq(
                  wNewFreqInMHz //) ;
                  , //0
                  byCPUcoreWithHighestLoad ) ;
              }
              //if( //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID] >=
              //  (float) mp_cpucoredata->m_wMaxFreqInMHz /
              //  //(float) mp_i_cpucontroller->GetFrequencyInMHz(byCoreID)
              //  wFreqInMHz >=
              //  //50% of max. freq
              //  0.5f
              //  //&& fTempInDegCelsius > 90.0f
              //  )
              //  //mp_cpucontroller->SetFrequencyInMHz(
              //  mp_cpucontroller->SetFreqAndVoltageFromFreq(
              //    //Set half of max. freq because the CPU is too hot
              //    mp_cpucoredata->m_wMaxFreqInMHz / 2 //) ;
              //    , 0 ) ;
              else
                //ChangeOperatingPointByLoad( 0 , fHighestCPUcoreLoadInPercent
                mp_cpucontroller->SetFreqAndVoltageFromFreq(
                    wNewFreqFromLoad, 0
                ) ;
            }
            else //if(mp_cpucontroller->m_b1CPUcorePowerPlane )
            {
              float fFreqInMHz ;
              float fLowerMultiplier ;
              float fNextMultiplierCalculatedFromCurrentLoad ;
              for( BYTE byCoreID = 0 ; byCoreID < wNumCPUcores ; ++ byCoreID )
              {
//                mp_cpucontroller->GetCurrentPstate(wFreqInMHz, fVolt, byCoreID) ;
//                mp_cpucontroller->GetCurrentVoltageAndFrequency(byCoreID) ;
                fFreqInMHz = arp_percpucoreattributes[ byCoreID].GetFreqInMHz() ;
                fNextMultiplierCalculatedFromCurrentLoad =
                    arp_percpucoreattributes[ byCoreID].m_fMultiplier
                    * ar_fCPUcoreLoadInPercent[byCoreID] ;
                fLowerMultiplier = //mp_cpucoredata->GetLowerMultiplier(
                  GetClosestLess(
                  mp_cpucoredata->m_arfAvailableMultipliers
                  , wNumCPUcores
                  , arp_percpucoreattributes[ byCoreID].m_fMultiplier
                  ) ;
                if( //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID] >= 
//                  (float) mp_cpucoredata->m_wMaxFreqInMHz /
//                  //(float) mp_i_cpucontroller->GetFrequencyInMHz(byCoreID)
//                  wFreqInMHz >=
//                  //50% of max. freq
//                  0.5f
                  //&& fTempInDegCelsius > 90.0f 
//                    fFreqInMHz * ar_fCPUcoreLoadInPercent[byCoreID] <
                    fNextMultiplierCalculatedFromCurrentLoad
                       <
//                    arp_percpucoreattributes[ byCoreID].GetFreq( fLowerMuliplier )
                      fLowerMultiplier
                  )
                  //mp_cpucontroller->SetFrequencyInMHz(
                  mp_cpucontroller->SetFreqAndVoltageFromFreq(
                    //Set half of max. freq because the CPU is too hot
//                    mp_cpucoredata->m_wMaxFreqInMHz / 2 //) ;
                    //"(WORD)" to avoid
                    //"passing `float' for converting 1 of `virtual BYTE
                    // I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, BYTE)'"
                    (WORD) fFreqInMHz
                    , byCoreID ) ;
                else
//                  ChangeOperatingPointByLoad( byCoreID ,
//                    //mp_cpucoredata->m_arp_percpucoreattributes[byCoreID]
//                    //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID]
//                    ar_fCPUcoreLoadInPercent[byCoreID]
//                    ) ;
                  mp_cpucontroller->SetFreqAndVoltageFromFreq(
                    //"(WORD)" to avoid
                    //"passing `float' for converting 1 of `virtual BYTE
                    // I_CPUcontroller::SetFreqAndVoltageFromFreq(WORD, BYTE)'"
                    (WORD) ( fLowerMultiplier *
                    arp_percpucoreattributes[ byCoreID].m_fReferenceClockInMhz )
                    , byCoreID
                    ) ;
              }// for-loop
            }
            if( mp_cpucoredata->m_byUpdateViewOnDVFS )
	            //e.g. force redraw if it's a GUI.
              mp_cpucontroller->GetUserInterface()->
                CurrenCPUfreqAndVoltageUpdated() ;
//          }//getperc usage
        }//too hot
        else
        {
          //DEBUG_COUTN("NOT too hot")
          DEBUGN("too hot:no")
  //        if( mp_icpu->//GetPercentalUsageForBothCores
  //            GetPercentalUsageForAllCores(mp_cpucoredata->
  //            m_arfCPUcoreLoadInPercent)
  //          )
          {
            //If ganged then there is only 1 power plane for ALL CPU cores.
            //-> setting voltage affects _all_ CPU cores. So change it only
            //1 performance state.
            if( //mp_cpucoredata->m_b1CPUcorePowerPlane
               mp_cpucontroller->m_b1CPUcorePowerPlane  )
            {
              BYTE byCPUcoreWithHighestLoad = 0 ;
              float fHighestCPUcoreLoadInPercent = 0.0 ;
              //DEBUG_COUTN("1 CPU core power plane")
              DEBUGN("1 CPU core power plane")
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
            else
            {
              DEBUG_COUTN("NOT 1 CPU core power plane")
  //            DEBUGN("NOT 1 CPU core power plane")
              for( BYTE byCoreID = 0 ; byCoreID < mp_cpucoredata->
                GetNumberOfCPUcores() ; ++ byCoreID )
              {
                ChangeOperatingPointByLoad( byCoreID ,
                  //mp_cpucoredata->m_arp_percpucoreattributes[byCoreID]
                  mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID]
                ) ;
              }
            }
          }
        }
      }
      else
        DEBUGN("DynFSB::Entry()--GetPercentalUsageForAllCores() failed()")
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
//      //from http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionctor:
//      // tell the other(s) thread(s) that the CPU core data may be accessed now:
//      //  we must
//      // lock the mutex first or we might signal the condition before the
//      // waiting threads start waiting on it!
//      wxMutexLocker lock(mp_cpucoredata->m_mutexCPUdataCanBeSafelyRead);
      DEBUGN("DynFreqScalingThreadBase::Entry(): before "
          "mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
      //Let all threads waiting (that called condition.Wait() ) on the
      // condition continue.
      //"Wait() atomically unlocks the mutex
      // which allows the thread to continue and starts waiting"
      //blocks here when using wxCondition::Broadcast() (wxWidgets bug?)
      mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast();
      DEBUGN("DynFreqScalingThreadBase::Entry(): after "
          "mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
    }
    //LOGN("End of scaling thread loop");
    Sleep(//1000
      //m_wMilliSecondsToWait 
      mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS );
    DEBUGN("DynFreqScalingThreadBase::Entry(): after "
        "Sleep(" << mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS << ")")
  }//loop
  DEBUGN("ended the Dynamic Voltage and Frequency Scaling loop")
  m_vbDVFSthreadStopped = true ;
  return 0;
}

bool DynFreqScalingThreadBase::IsStopped()
{
  return m_vbDVFSthreadStopped ;
}

//BYTE
//Use DWORD because of GetLastError(...) return code.
DWORD DynFreqScalingThreadBase::Start()
{
  m_vbRun = true ; 
  return 1 ;
}

//SyncOnStoppedState()

//The DVFS thread is only stopped if the loop ended at first.
void DynFreqScalingThreadBase::Stop()
{
  m_vbRun = false ;
}
