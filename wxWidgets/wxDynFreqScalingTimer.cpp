
#include "wxDynFreqScalingTimer.hpp"

#include <ModelData/ModelData.hpp>
#include <UserInterface/UserInterface.hpp> //for UserInterface.m_bConfirmedYet
#include <wx/timer.h>

wxDynFreqScalingTimer::wxDynFreqScalingTimer(
    ICPUcoreUsageGetter * p_icpu,
    GriffinController * p_pumastatectrl
    )
    : m_bSuccFullyGotPStateFromMSR(false)
    , m_bCalledInit(false)
  {
    DEBUG("constructor of timer--begin\n");
    mp_icpu = p_icpu ;
    //mp_icpu->Init();
    m_wMaxFreqInMHz = //2200
      p_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    //wxTimer();
    m_wCurrentFreqInMHz = //550
      m_wMaxFreqInMHz / 4 ;
    m_fPercentileIncrease = 1.5f ;
    m_wAHalfOfMaxFreq = m_wMaxFreqInMHz / 2 ;
    m_wAQuarterOfMaxFreq = m_wMaxFreqInMHz / 4 ;
    mp_pumastatectrl = p_pumastatectrl ;
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
    if( mp_pumastatectrl->GetPStateFromMSR(//p_pstateCurrent->m_byNumber
      2,m_dwMSRLow,m_dwMSRHigh,
      m_pstateFromMSR,0) )
    {
      m_bSuccFullyGotPStateFromMSR = true ;
    }
#else
    m_bSuccFullyGotPStateFromMSR = true ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
  }

  //"This member should be overridden by the user if the default constructor 
  //was used and SetOwner wasn't called.
  //Perform whatever action which is to be taken periodically here."
  void wxDynFreqScalingTimer::Notify()
  {
    DEBUG("Timer notification\n");
    if( ! m_bCalledInit )
    {
      m_bCalledInit = true ;
      //mp_icpu->Init();
    }
    //bool m_wCurrentFreqInMHz = false ;
    if(mp_pumastatectrl->GetUserInterface()->m_bConfirmedYet
      && m_bSuccFullyGotPStateFromMSR)
    {
      bool bChangePstate = false ;
      BYTE byDivisorID, byFrequencyID ;
      //mp_icpu->GetPercentalUsageForBothCores(m_arf);
      //DEBUG("core 0 usage: %lf, core 1 usage: %lf\n",m_arf[0], m_arf[1]);
      LOG("core 0 usage: " << m_arf[0] << ", core 1 usage: " << m_arf[1] << "\n" );
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

      //Percentile usage less than 100%
      if( m_arf[0] < 100.0f )
      {
        bChangePstate = true ;
        m_wCurrentFreqInMHz = (WORD) ( (float) m_wCurrentFreqInMHz * 
          //percentile usage
          m_arf[0] / 100.f
          ) ;
        //if( m_wCurrentFreqInMHz >= 550 )
        if( m_wCurrentFreqInMHz > m_wAQuarterOfMaxFreq )
        {
          //bChangePstate = true ;
          if( m_wCurrentFreqInMHz <= m_wAHalfOfMaxFreq )
          {
            //example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
            //550 MHz would have been encoded as DivisorID:2, FrequencyID: 14:
            //  (14+8)*100/2^2 =2200/4 = 550
            //byDivisorID = 1 ;
          }
        }
        else
        {
          m_wCurrentFreqInMHz = 550 ;
          //mp_pumastatectrl->SetPstate(2,
          //  //1 = 1bin
          //  1);
        }
      }
      else // =100% CPU core load
      {
        //if( m_wCurrentFreqInMHz == m_wMaxFreqInMHz )
        //  bChangePstate = false ;
        //else
        {
          bChangePstate = true ;
          m_wCurrentFreqInMHz *=
            //If no explicit cast: compiler wanring with gcc.
            (WORD)
            m_fPercentileIncrease ;
          if( m_wCurrentFreqInMHz > m_wMaxFreqInMHz )
            m_wCurrentFreqInMHz = m_wMaxFreqInMHz ;
        }
      }
      if( m_wCurrentFreqInMHz > m_wAQuarterOfMaxFreq )
      {
        //bChangePstate = true ;
        if( m_wCurrentFreqInMHz <= m_wAHalfOfMaxFreq )
        {
          //example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
          //550 MHz would have been encoded as DivisorID:2, FrequencyID: 14:
          //  (14+8)*100/2^2 =2200/4 = 550
          byDivisorID = 1 ;
          //Given: freqinMHz: 650, DivisorID:1 -> (FID+8)*100/2^DivisorID = 650
          //(FID+8)*100/2^1 = 650 <=> (FID+8)*100/2 = 650 <=> (FID+8)*50 = 650 |:50
          //FID+8 = 13 |-8 <=> FID=5
          //Test: (FID+8)*100/2^DivisorID = 650 <=> (5+8)*100/2^1 = 650
          //      13*100/2 = 650 <=> 1300/2 = 650 <=> 650 = 650 <=> true
          byFrequencyID = (m_wCurrentFreqInMHz/50) - 8 ;
        }
        else //m_wCurrentFreqInMHz > wAHalfOfMaxFreq 
        {
          //example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
          //1100 MHz would have been encoded as DivisorID:1, FrequencyID: 14:
          //  (14+8)*100/2^1 =2200/2 = 1100
          byDivisorID = 0 ;
          //Given: freqinMHz: 1300, DivisorID:0 -> (FID+8)*100/2^DivisorID = 1300
          //(FID+8)*100/2^0 = 1300 <=> (FID+8)*100/1 = 1300 <=> (FID+8)*100 = 1300 |:100
          //FID+8 = 13 |-8 <=> FID=5
          //Test: (FID+8)*100/2^DivisorID = 1300 <=> (5+8)*100/2^0 = 1300
          //      13*100/1 = 1300 <=> 1300/1 = 1300 <=> 1300 = 1300 <=> true

          //(FID+8)*100=FreqinMHz |:100 <=> (FID+8)=FreqinMHz/100 |-8 <=> 
          //FID=FreqinMHz/100-8
          //===================
          byFrequencyID = (m_wCurrentFreqInMHz/100) - 8 ;
        }
      }
          //if( m_wCurrentFreqInMHz <= 2100 )
          //{
          //  bChangePstate = true ;
          //  m_wCurrentFreqInMHz += 100 ;
          //}
      //  }
      //}
      m_fVoltage = 1.05f - 
        (float) 
        ( 
          //log2f( 
          //MSVC++ has no log2() function (in <math.h>).
          //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
          log10(
            //(double) 
            ( //2200.0f
              m_wMaxFreqInMHz / (float) m_wCurrentFreqInMHz
            )
               )
          / log10(2.0f)
          * 0.25f 
        ) ;
      //DEBUG("timer--change p-state: %u,current freq:%u->optimal voltage:%f\n", 
      //  (WORD) bChangePstate, 
      //  m_wCurrentFreqInMHz,
      //  m_fVoltage);
      LOG("timer--change p-state: " << (WORD) bChangePstate << 
          ",current freq:" << m_wCurrentFreqInMHz << "->optimal voltage:" << 
          m_fVoltage << "\n" );
      //DEBUG("timer--new voltage: %f\n",m_fVoltage);
      if( bChangePstate )
      {
        BYTE byDID, byFID ;
        //PState::GetDIDandFID(m_wCurrentFreqInMHz);
        if( m_wCurrentFreqInMHz >
          //1/2 of maxfreq
          1100)
        {
          byDID = 0 ;
          //freq_in_MHz = ( FreqID + 8 )*100 | :100
          //freq_in_MHz / 100 = FreqID + 8 | -8
          //freq_in_MHz / 100 - 8 = FID
          byFID = 
            ////max. FID for max freq of 2200 MHz
            //14 
            // -
            //m_wCurrentFreqInMHz / 100 - 8 ;
            
            //Just for testing.
            14 ;
            //Just for testing.
          //m_wCurrentFreqInMHz = 2200 ;
          //mp_pumastatectrl->SetPstate(0,
          //  //1 = 1bin
          //  1);
        }
        else
          if(m_wCurrentFreqInMHz >
            //1/4 of maxfreq
            550)
          {
            byDID = 1 ;
            //ex: 1100 MHz: 
            //FID = 1100 * 2 / 100 - 8 
            //= 2200 / 100 - 8
            //= 22 - 8 
            //= 14
            //====
            //check: FreqInMHz = (( FreqID + 8 )*100 ) / (1<<DivisorID) 
            //=(( 14 + 8 )*100 ) / (1<<1) = 22*100 / 2 = 2200 / 2 
            //= 1100
            //======
            //byFID = m_wCurrentFreqInMHz * 2 / 100 - 8 ;

            //Just for testing.
            byFID = 14 ;
            //m_wCurrentFreqInMHz = 1100 ;
            //mp_pumastatectrl->SetPstate(1,
            //  //1 = 1bin
            //  1);
          }
        //m_fVoltage = 1.1f - 
        //  (float) 
        //  ( 
        //    //log2f( 
        //    //MSVC++ has no log2() function (in <math.h>).
        //    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
        //    log10(
        //      //(double) 
        //      ( 2200.0f / (float) m_wCurrentFreqInMHz ) 
        //         )
        //    / log10(2.0f)
        //    * 0.15f 
        //  ) ;
        //DEBUG("timer--new voltage: %f\n",m_fVoltage);
        LOG("timer--new voltage: " << m_fVoltage << "\n" );
        //PState pstate(PState::GetVoltageID(m_fVoltage),byDID,byFID);
        PState pstate(PState::GetVoltageID(m_fVoltage),byDivisorID,byFrequencyID,2);
        //PState pstate2(pstate) ;
        //DEBUG("VID: %u byDID: %u ,byFID: %u\n", 
        //  //PState::GetVoltageID(m_fVoltage), byDID,byFID);
        //  PState::GetVoltageID(m_fVoltage), byDivisorID,byFrequencyID);
        LOG("VID: " << 
            //convert to WORD in order to output as number
            (WORD) PState::GetVoltageID(m_fVoltage) 
            << " byDID: " << 
            //convert to WORD in order to output as number
            (WORD) byDivisorID << " ,byFID: " << 
            //convert to WORD in order to output as number
            (WORD) byFrequencyID << "\n" );
        //DWORD dwHigh = 0, dwLow = 0 ;

        //pstate.AssignChangesToPstateFromMSR(m_pstateFromMSR);

        m_dwMSRHigh = 0 ;
        m_dwMSRLow = 0 ;
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
        mp_pumastatectrl->WriteToPstateOrCOFVIDcontrolRegister(
          //mp_pumastatectrl->GetMSRregisterForPstate(
          ////Use p-state 3 for setting for dyn. freq. scaling
          ////because setting via the MSR register 0xC0010070
          ////did not work.
          //2),
          COFVID_CONTROL_REGISTER,
          pstate,
          //m_pstateFromMSR,
          //pstate2,
          pstate,
          m_dwMSRHigh,
          m_dwMSRLow,
          0) ;
        //Without setting the p-state changes are not applied.
        mp_pumastatectrl->SetPstate(2,//1
          0 //core ID
          ) ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
      }//if( bChangePstate )
    }//m_bConfirmedYet
    DEBUG("End of Timer:Notify()\n");
  }//void Notify()
