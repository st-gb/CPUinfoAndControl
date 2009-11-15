#include "stdafx.h" //for "NULL"

#include "PentiumM_Controller.hpp"
//#include <windef.h> //for "NULL"
#include "../Intel_registers.h"
#include <ModelData/ModelData.hpp>
//#include <I_CPUaccess.hpp>
#include <Controller/I_CPUaccess.hpp>

PentiumM_Controller::PentiumM_Controller()
  //: mp_model(NULL)
{
}

PentiumM_Controller::~PentiumM_Controller(void)
{
}

BYTE PentiumM_Controller::Init()
{
  if( mp_cpuaccess )
  {
    bool bSpeedStepIsenabled ;
    DWORD dwLow, dwHigh ;
    mp_cpuaccess->WrmsrEx(
      IA32_MISC_ENABLE
      , dwLow 
      , dwHigh 
      , 1
      ) ;
    bSpeedStepIsenabled = ! (dwHigh & 1 ) ;
    LOGN("SteedStep is " << ( bSpeedStepIsenabled ? "en" : "dis") << " abled" )
    //SpeedStep not activated.
    if( ! (dwHigh & 1 ) )
    {
      LOGN("Enabling SteedStep")
      //Intel PentiumM_SpeedStepDoc.pdf:
      //"Enable Miscellaneous Processor Features. Bit 16 
      //must be set to a 1 in order to implement Enhanced
      //Intel� SpeedStep� Technology features."
      dwHigh |= 1 ;
      mp_cpuaccess->WrmsrEx(
        IA32_MISC_ENABLE
        , dwLow 
        , dwHigh 
        , 1
        ) ;
    }
  }
  return 1 ;
}

BYTE PentiumM_Controller::GetCurrentPstate(
  WORD & r_wFreqInMHz 
  , float & r_fVoltageInVolt
  , BYTE byCoreID
  )
{
  DWORD dwHigh, dwLow ;
  if( RdmsrEx(
      IA32_PERF_STATUS
      , dwLow
      , dwHigh
      , 1
      )
    )
  {
#ifdef _DEBUG
    BYTE byFID = dwLow >> 8 ;
#endif
    r_wFreqInMHz = ( dwLow >> 8 ) * 100 ;
    BYTE byVoltageID = ( dwLow & 255 ) ;
    //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
    //63= 0.7V 62=0.716 V,...
    //40=1.340 Volt
    r_fVoltageInVolt = 0.7 + //( 63 - byVoltageID ) 
      byVoltageID * 0.016 ;
#ifdef _DEBUG
    byVoltageID = byVoltageID ;
#endif
  }
  return 1 ;
}

BYTE PentiumM_Controller::GetCurrentPstate(
  BYTE & r_byFreqID
  , BYTE & r_byVoltageID
  , BYTE byCoreID
  )
{
  DWORD dwHigh, dwLow ;
  if( RdmsrEx(
      IA32_PERF_STATUS
      , dwLow
      , dwHigh
      , 1 << byCoreID
      )
    )
  {
    r_byFreqID = dwLow >> 8 ;
    r_byVoltageID = ( dwLow & 255 ) ;
  }
  return 1 ;
}

WORD PentiumM_Controller::GetMaximumFrequencyInMHz()
{
  //I do not know if there are information about the max. freq. saved 
  //within its registers.
  //So simply return the highest frequency from the 
  //default operating points.
  //MaxVoltageForFreq & r_maxvoltageforfreq =
  std::set<VoltageAndFreq> & r_stdsetvoltageandfreq = 
    mp_model->m_cpucoredata.
    m_stdsetvoltageandfreqAvailableFreq ;

  //std::set<MaxVoltageForFreq>::iterator iter =
  std::set<VoltageAndFreq>::reverse_iterator rev_iter =
    //mp_model->//m_cpucoredata.
    //  //m_setmaxvoltageforfreq.begin() ;
    r_stdsetvoltageandfreq.rbegin() ;
  if( //iter != mp_model->m_setmaxvoltageforfreq.end() 
    rev_iter != r_stdsetvoltageandfreq.rend()  
    )
  {
    mp_model->m_cpucoredata.m_wMaxFreqInMHz = rev_iter->m_wFreqInMHz ;
    return //iter->m_wFreqInMHz;
      rev_iter->m_wFreqInMHz;
  }
  return 0 ;
}

WORD PentiumM_Controller::GetMinimumFrequencyInMHz()
{
  if( mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() > 0 
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->size() > 0 
    )
    return mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin()->
    //return mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin()->
      m_wFreqInMHz ;
  return 0 ;
}

WORD PentiumM_Controller::GetMaximumVoltageID()
{
  //0.7V + 0.016V * 50 = 1.34 V
  return 40 ;
}

WORD PentiumM_Controller::GetMinimumVoltageID()
{
  //0.7V + 0.016V * 0 = 0.7 V
  return 0 ;
}

WORD PentiumM_Controller::GetNumberOfPstates()
{
  return mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() ;
  //return mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->size() ;
}

//P-state 0 is/ must be the highest core frequency 
//(see "Enhanced Intel� SpeedStep� Technology for the Intel� Pentium� M Processor" 
//White Paper from March 2004, page 11).
BYTE PentiumM_Controller::GetPstate(WORD wPstateID, 
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

//bool PentiumM_Controller::GetPstateSafefy(
//  WORD wFreqInMHz
//  , float fVoltageInVolt 
//  )
//{
//  bool bPstateIsSafe = false ;
//  float fMaxVoltageInVolt ;
//  if( GetInterpolatedVoltageFromFreq(
//      wFreqInMHz
//      , fMaxVoltageInVolt 
//      , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
//      )
//    )
//  {
//    int i = 0 ;
//    if( fVoltageInVolt <= fMaxVoltageInVolt )
//      bPstateIsSafe = true ;
//  }
//  return bPstateIsSafe ;
//}

WORD PentiumM_Controller::GetVoltageID(float fVoltageInVolt )
{
  //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  float fVoltageID = fVoltageMinusLowestVoltage / 0.016 ;
  WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //round up because at 1.18 V the voltage ID in float is 29.999998
    //and would get 29 (1.164 V) when converting to an integer.
    //for 1.164 voltage ID in float is 29.000004
    //ceil( fVoltageID );
    fVoltageID ;
  if( fVoltageID - (float) wVoltageID >= 0.5 ) 
    ++ wVoltageID ;
  return wVoltageID ;
}

float PentiumM_Controller::GetVoltageInVolt(WORD wVoltageID )
{
  return 0.7 + 0.016 * wVoltageID ;
}

void PentiumM_Controller::IncreaseVoltageForCurrentPstate(BYTE byCoreID)
{
  float fVoltageInVolt ;
  WORD wFreqInMHz ;
  GetCurrentPstate(
    wFreqInMHz 
    , fVoltageInVolt
    , byCoreID 
    ) ;
  //{
  //  
  //}
  fVoltageInVolt += 0.032 ; //increase by 2 voltage ID steps
  SetVoltageAndFrequency(
    fVoltageInVolt
    , wFreqInMHz
    ) ;
  fVoltageInVolt += 0.32 ; //increase by 2 voltage ID steps

  //BYTE byFreqID ;
  //BYTE byVoltageID ;
  //BYTE byCoreID  ;
  //GetCurrentPstate(
  //  byFreqID
  //  , byVoltageID
  //  , byCoreID
  //  ) ;
  //byVoltageID += 2 ;
  //SetPstate(
  //  byFreqID
  //  , byVoltageID
  //  , byCoreID
  //  ) ;
}

void PentiumM_Controller::PerformanceEventSelectRegisterWrite(
  DWORD dwAffinityBitMask ,
  //Pentium M has 1 or 2 "Performance Event Select Register" from 
  //  MSR ... to MSR ...  for 
  // 1 or 2 "Performance Event Counter Registers" from 
  //  ... to ...
  //  that store the 48 bit counter value
  BYTE byPerformanceEventSelectRegisterNumber ,
  BYTE byEventSelect , //8 bit
  BYTE byUnitMask , // 8 bit
  bool bUserMode,
  bool bOSmode,
  bool bEdgeDetect,
  bool bPINcontrol,
  bool bEnableAPICinterrupt,
  //Intel vol. 3B (document # 253659):
  //"When set, performance counting is
  //enabled in the corresponding performance-monitoring counter; when clear, the
  //corresponding counter is disabled. The event logic unit for a UMASK must be
  //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
  //IA32_PMCx."
  bool bEnablePerformanceCounter,
  bool bInvertCounterMask ,
  BYTE byCounterMask
  )
{
  //DWORD dwLow = byCounterMask ;
  //dwLow <<= 24 ;
  //dwLow = 
  //dwLow <<= 24 ;
  DWORD dwLow = 0 |
    ( byCounterMask << 24 ) |
    ( bInvertCounterMask << 23 ) |
    ( bEnablePerformanceCounter << 22 ) |
    ( bEnableAPICinterrupt << 20 ) |
    ( bPINcontrol << 19 ) |
    ( bEdgeDetect << 18 ) |
    ( bOSmode << 17 ) |
    ( bUserMode << 16 ) |
    ( byUnitMask << 8 ) |
    ( byEventSelect )
    ;
  WrmsrEx(
    // MSR index
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber ,
    dwLow ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    1	// Thread Affinity Mask
    ) ;
}

BYTE PentiumM_Controller::SetVoltageAndFrequency(
  float fVoltageInVolt
  , WORD wFreqInMHz
  )
{
  //byVoltageID: 40 = 1.34 V    27 = 1.1320000
  //float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  //float fVoltageID = fVoltageMinusLowestVoltage / 0.016 ;
  BYTE byVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //round up because at 1.18 V the voltage ID in float is 29.999998
    //and would get 29 (1.164 V) when converting to an integer.
    //ceil( fVoltageID );
    GetVoltageID(fVoltageInVolt) ;
  BYTE byFreqID = wFreqInMHz / 100 ;
  DWORD dwLow = byFreqID << 8 ;
  dwLow |= byVoltageID ;
  //LOGN("P M ctrl: " << fVoltageInVolt << " " << wFreqInMHz )
  WrmsrEx(
    IA32_PERF_CTL
    , dwLow
    , 0
    , 1
    ) ;

  return 0 ;
}

void PentiumM_Controller::SetFreqAndVoltageFromFreq(
  WORD wFreqInMHz 
  , BYTE byCoreID)
{
  float fVoltageInVolt ;
  std::set<VoltageAndFreq> & r_stdsetvoltageandfreq = 
    //mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
    //mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted ;
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqAvailableFreq ;
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
  if( ci_stdsetvoltageandfreqNearestHigherEqual!= 
    r_stdsetvoltageandfreq.end() 
    )
  {
    if( GetInterpolatedVoltageFromFreq(
        //wFreqInMHz
        ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
        , fVoltageInVolt 
        //, r_stdsetvoltageandfreq
        //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
        , mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
        //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqWanted
        ) 
      )
    {
      SetVoltageAndFrequency(//wFreqInMHz
        fVoltageInVolt
        , ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
        ) ;
    }
  }
  else //if e.g. the wanted freq is higher than the max. freq.
  {
    if( ci_stdsetvoltageandfreqNearestLowerEqual  != 
      r_stdsetvoltageandfreq.end() 
      )
    {
      wFreqInMHz = ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
      if( GetInterpolatedVoltageFromFreq(
          //wFreqInMHz
          ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz
          , fVoltageInVolt 
          //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
          //, r_stdsetvoltageandfreq 
          , mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
          //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqWanted
          ) 
        )
      {
        SetVoltageAndFrequency( fVoltageInVolt, wFreqInMHz) ;
      }
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
}

BYTE PentiumM_Controller::TooHot()
{
  DWORD dwEAX ;
  DWORD dwEDX ;
  if( mp_cpuaccess->RdmsrEx(
	  IA32_THERM_STATUS ,		// MSR index
	  & dwEAX ,			// bit  0-31
	  & dwEDX ,			// bit 32-63
    //1bin =core 0; 10bin=2dec= core 1
	  1	// Thread Affinity Mask
      )
    )
  {
    //bool bCriticalTemperatureStatus = dwEAX & ;

    /*if( bCriticalTemperatureStatus  || bPROCHOTorFORCEPR_event )
      return 1 ;*/
  }
  return 0 ;
}

BOOL // TRUE: success, FALSE: failure
  //WINAPI
  //Write through THIS method, not by the CPUaccess class's one.
  PentiumM_Controller::WrmsrEx(
    DWORD index,		// MSR index
    DWORD dwLow ,//eax,			// bit  0-31
    DWORD dwHigh, //edx,			// bit 32-63
    DWORD affinityMask	// Thread Affinity Mask
  )
{
  if( mp_cpuaccess )
  {
    if( //Writing to this register may damage the CPU
      index == IA32_PERF_CTL )
    {
      WORD wFreqInMHz = ( dwLow >> 8 ) * 100 ;
      BYTE byVoltageID = ( dwLow & 255 ) ;
      //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
      //63= 0.7V 62=0.716 V,...
      //40=1.340 Volt
      float fVoltageInVoltFromRegisterValueToWrite = 0.7 + //( 63 - byVoltageID ) 
        byVoltageID * 0.016 ;
      float fMaxVoltageInVolt ;
      if( GetInterpolatedVoltageFromFreq(
          wFreqInMHz
          , fMaxVoltageInVolt 
          , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
          //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault
          )
        )
      {
        int i = 0 ;
        if( fVoltageInVoltFromRegisterValueToWrite <= fMaxVoltageInVolt )
          mp_cpuaccess->// TRUE: success, FALSE: failure
            WrmsrEx(
              index,		// MSR index
              dwLow ,//eax,			// bit  0-31
              dwHigh, //edx,			// bit 32-63
              affinityMask	// Thread Affinity Mask
            ) ;
          //i = 0 ;
      }
    }
    else
      mp_cpuaccess->// TRUE: success, FALSE: failure
        WrmsrEx(
          index,		// MSR index
          dwLow ,//eax,			// bit  0-31
          dwHigh, //edx,			// bit 32-63
          affinityMask	// Thread Affinity Mask
        ) ;
  }
  return true ;
}
