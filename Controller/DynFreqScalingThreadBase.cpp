#include "DynFreqScalingThreadBase.hpp"

#include <Controller/ICPUcoreUsageGetter.hpp>
#include <Controller/I_CPUcontroller.hpp>
#include <ModelData/CPUcoreData.hpp>
#include "UserInterface.hpp" //for UserInterface.m_bConfirmedYet
#include <global.h> //LOGN
#include "Sleep.h"

DynFreqScalingThreadBase::DynFreqScalingThreadBase(
  ICPUcoreUsageGetter * p_icpu
  //, GriffinController * p_pumastatectrl
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
{
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
//  DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad")
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
      float fVoltage ;
      WORD wFreqInMHz ;
      mp_cpucontroller->GetCurrentPstate(
        wFreqInMHz 
        , fVoltage
        , byCoreID
        ) ;
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( wFreqInMHz * //1.5f
            mp_cpucoredata->m_fCPUcoreFreqIncreaseFactor )
        , byCoreID
        );
    }
    else
    {
      float fVoltage ;
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
      mp_cpucontroller->GetCurrentPstate(
        wFreqInMHz 
        , fVoltage
        , byCoreID
        ) ;
//      DEBUGN("DynFreqScalingThreadBase::ChangeOperatingPointByLoad: "
//        "before mp_cpucontroller->SetFreqAndVoltageFromFreq")
      mp_cpucontroller->SetFreqAndVoltageFromFreq(
        //r_cpucoredata.m_arp_percpucoreattributes[byCoreID] 
        //Explicit cast to avoid (g++) compiler warning.
        (WORD)
          ( wFreqInMHz * fLoad )
        , byCoreID
        );
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
  LOGN("Dynamic Voltage and Frequency Scaling thread should run? " <<
    ( m_vbRun ? "yes" : "no") )
  LOGN("CPU core usage getter--address: " << mp_icpu )
  mp_icpu->Init();
  LOGN("DVFS after initializing CPU core usage getter. Should run?: "
      << (m_vbRun ? "yes" : "no" ) )
  while(//1
    m_vbRun )
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
//      DEBUGN("DynFreqScalingThreadBase::Entry: confirmed yet")
      //float fTempInDegCelsius ;
      //check if a temp. could be received:
      //Pentium M CPUs have no temperature register
      if( 
        //mp_i_cpucontroller->GetCurrentTempInDegCelsius(fTempInDegCelsius) ;
        mp_cpucontroller->TooHot()
        )
        {
          //DEBUG_COUTN(
//          DEBUGN("too hot:yes")
          //TODO exit thread when getting CPU core load fails?
          if( mp_icpu->//GetPercentalUsageForBothCores
              GetPercentalUsageForAllCores(mp_cpucoredata->
              m_arfCPUcoreLoadInPercent) 
            )
          {
            //DEBUG("core 0 usage: %lf\n",mp_cpucoredata->m_arfCPUcoreLoadInPercent);
          //  if(m_arf[0] == 100.0f )
          //  {
          //    //mp_pumastatectrl->GetMaximumFrequencyInMHz()
          //    if( wCurrentFreqInMHz < mp_model->GetMaxFreq() )
          //      controller->IncreaseFreq() ;
          //  }
          //  else
          //    if(//m_arf[0] < 90.0f && 
          //      m_arf[0] <= wCurrentFreqInMHz / mp_controller->
          //      GetClosestLowerFreq(wCurrentFreqInMHz) )
          //      wCurrentFreqInMHz = (WORD) ( (float) wCurrentFreqInMHz * m_arf[0] ) ;
            //DEBUG("Frequency before (possible) change:%u, core0 load:%f\n",
            //  mp_cpucoredata->m_arwCurrentFreqInMHz[0],mp_cpucoredata->
            //  m_arfCPUcoreLoadInPercent);
            //Percental usage less than 100%
            //if( m_arf[0] < 100.0f )
            //mp_pumastatectrl->SetFrequencyAndVoltageForAllCoresAccCPULoad();
            WORD wFreqInMHz ;
            float fVolt ;
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
              mp_cpucontroller->GetCurrentPstate( wFreqInMHz, fVolt,
                //0
                byCPUcoreWithHighestLoad ) ;
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
              for( BYTE byCoreID = 0 ; byCoreID < mp_cpucoredata->
                GetNumberOfCPUcores() ; ++ byCoreID )
              {
                mp_cpucontroller->GetCurrentPstate(wFreqInMHz, fVolt, byCoreID) ;
                if( //mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID] >= 
                  (float) mp_cpucoredata->m_wMaxFreqInMHz /
                  //(float) mp_i_cpucontroller->GetFrequencyInMHz(byCoreID)
                  wFreqInMHz >=
                  //50% of max. freq
                  0.5f 
                  //&& fTempInDegCelsius > 90.0f 
                  )
                  //mp_cpucontroller->SetFrequencyInMHz(
                  mp_cpucontroller->SetFreqAndVoltageFromFreq(
                    //Set half of max. freq because the CPU is too hot
                    mp_cpucoredata->m_wMaxFreqInMHz / 2 //) ;
                    , byCoreID ) ;
                else
                  ChangeOperatingPointByLoad( byCoreID ,
                    //mp_cpucoredata->m_arp_percpucoreattributes[byCoreID] 
                    mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID]
                    ) ;
              }// for-loop
            }
            if( mp_cpucoredata->m_byUpdateViewOnDVFS )
	            //e.g. force redraw if it's a GUI.
	            //mp_pumastatectrl->GetUserInterface()->CurrenCPUfreqAndVoltageUpdated() ;
              mp_cpucontroller->GetUserInterface()->
                CurrenCPUfreqAndVoltageUpdated() ;
          }//getperc usage
      }//too hot
      else
      {
        //DEBUG_COUTN("NOT too hot")
//        DEBUGN("too hot:no")
        if( mp_icpu->//GetPercentalUsageForBothCores
            GetPercentalUsageForAllCores(mp_cpucoredata->
            m_arfCPUcoreLoadInPercent) 
          )
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
//            DEBUGN("1 CPU core power plane")
            byCPUcoreWithHighestLoad = GetCPUcoreWithHighestLoad(
              fHighestCPUcoreLoadInPercent ) ;
//            DEBUGN("highest load of a CPU cores:" <<
//                fHighestCPUcoreLoadInPercent )
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
          }
          else
          {
            //DEBUG_COUTN("NOT 1 CPU core power plane")
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
    }//m_bConfirmedYet
    else
      {
      DEBUGN("DynFreqScalingThreadBase::Entry: NOT confirmed yet")
      }
    //LOGN("End of scaling thread loop");
    Sleep(//1000
      //m_wMilliSecondsToWait 
      mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS );
  }//loop
  return 0;
}

//BYTE
//Use DWORD because of GetLastError(...) return code.
DWORD DynFreqScalingThreadBase::Start()
{
  m_vbRun = true ; 
  return 1 ;
}

void DynFreqScalingThreadBase::Stop() { m_vbRun = false ; }
