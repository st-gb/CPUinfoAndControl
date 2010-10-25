#include "stdafx.h" //for "NULL"

#include "PentiumM_Controller.hpp"
//#include <windef.h> //for "NULL"
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <preprocessor_helper_macros.h> //BITMASK_FOR_LOWMOST_3BIT
//for Pentium Ms the perf counter bit width seems to be 40:
//                                    <8 bit > <8 bit > <8 bit > <8 bit > <8 bit >
//previous value: 5497519814387   100 11111111 11111101 10110111 00110110 11110011
//current value:  4398069455221   100 00000000 00000001 01011110 00011001 01110101
//suffix "ULL" is not needed by MS compiler, but by g++
//needed for "PERFORMANCE_COUNTER_VALUE_DIFF"
#define MAXIMUM_PERFORMANCE_COUNTER_VALUE = 0xFFFFFFFFFFULL ; //10xF = 10x 4 bit =40 bit

#include <Controller/CPU-related/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/CPU-related/Intel/PentiumM/PentiumM.hpp>

#include <Controller/CPU-related/PerformanceCounterValueDiff.h>
#include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#ifdef COMPILE_WITH_ATTRIBUTES
#include <ModelData/ModelData.hpp>
#endif
//#include <I_CPUaccess.hpp>
#include <Controller/I_CPUaccess.hpp>
#include <windows.h> //::GetTickCount()

#define PERFORMANCE_COUNTER_FOR_FID_CHANGE IA32_PMC0

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  #define MONITOR_NUMBER_OF_FREQUENCY_ID_TRANSITIONS \
    MonitorNumberOfFrequencyIDtransitions() ;
#else
  #define MONITOR_NUMBER_OF_FREQUENCY_ID_TRANSITIONS
#endif

PentiumM_Controller::PentiumM_Controller()
  :
  //mp_model(NULL)
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  m_b2ndTimeOrLaterReadTSCandFIDchange(false)
  , m_dCurrentTSCclockInHz (0.0)
  , m_dwMinimumTimeSpanInMilliseconds ( 100 )
  ,
#endif
  m_fCurrentReferenceClockInMHz ( 0.0 )
{
  gp_cpuaccess = this ;
}

PentiumM_Controller::~PentiumM_Controller(void)
{
}

//For "increase voltage" in the user interface.
void PentiumM_Controller::DecreaseVoltageBy1Step(float & r_fVoltage)
{
  WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
  //if( wVoltageID < GetMaximumVoltageID(
  -- wVoltageID ;
  r_fVoltage = GetVoltageInVolt( wVoltageID ) ;
}

BYTE PentiumM_Controller::Init()
{
  if( mp_cpuaccess )
  {
    bool bSpeedStepIsenabled ;
    //Initialize just to avoid (g++) compiler warning.
    DWORD dwLow = 0 , dwHigh = 0 ;
    mp_cpuaccess->WrmsrEx(
      IA32_MISC_ENABLE
      , dwLow 
      , dwHigh 
      , 1
      ) ;

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
    MonitorNumberOfFrequencyIDtransitions() ;
#else
//    ReferenceClockAccordingToStepping() ;
    GetReferenceClockAccordingToStepping() ;
#endif

    bSpeedStepIsenabled = ! (dwHigh & 1 ) ;
    LOGN("SteedStep is " << ( bSpeedStepIsenabled ? "en" : "dis") << 
      " abled" )
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

#ifdef COMPILE_WITH_ATTRIBUTES
void PentiumM_Controller::GetAllPossibleFrequencies(
  std::set<VoltageAndFreq> & r_stdsetvoltageandfreq )
{
  r_stdsetvoltageandfreq.clear() ;
  //If set.
  if( mp_model->m_cpucoredata.m_wMaxFreqInMHz )
  for( WORD wFreqInMHz = 600 ; 
    wFreqInMHz <= mp_model->m_cpucoredata.m_wMaxFreqInMHz 
    ; wFreqInMHz += 100 )
    r_stdsetvoltageandfreq.insert( VoltageAndFreq( 0.0 , wFreqInMHz ) ) ;
}
#endif //#ifdef COMPILE_WITH_ATTRIBUTES

//BYTE PentiumM_Controller:://GetCurrentPstate(
//  WORD & r_wFreqInMHz
//  , float & r_fVoltageInVolt
//  , BYTE byCoreID
BYTE PentiumM_Controller::GetCurrentVoltageAndFrequency(
  float & r_fVoltageInVolt
  , float & r_fMultiplier
  , float & r_fReferencClockInMHz
  )
{
  return GetCurrentVoltageAndFrequencyPentium_M(
    r_fVoltageInVolt
    , r_fMultiplier
    , r_fReferencClockInMHz
    ) ;
}

BYTE PentiumM_Controller::GetCurrentPstate(
  BYTE & r_byFreqID
  , BYTE & r_byVoltageID
  , BYTE byCoreID
  )
{
  DWORD dwHigh, dwLow ;
  if( mp_cpuaccess->RdmsrEx(
      IA32_PERF_STATUS
      , & dwLow
      , & dwHigh
      , 1 << byCoreID
      )
    )
  {
    r_byFreqID = dwLow >> 8 ;
    r_byVoltageID = ( dwLow & 255 ) ;
  }
  return 1 ;
}

#ifdef COMPILE_WITH_ATTRIBUTES
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
#endif //#ifdef COMPILE_WITH_ATTRIBUTES

WORD PentiumM_Controller::GetMaximumVoltageID()
{
  //The maximum voltage is 1.34 V.
  //0.7V + 0.016V * 40 = 0,7 V + 0,64V = 1.34 V
  return 40 ;
}

WORD PentiumM_Controller::GetMinimumVoltageID()
{
  //0.7V + 0.016V * 0 = 0.7 V
  return 0 ;
}

float PentiumM_Controller::GetMinimumVoltageInVolt()
{
  return 0.7 ;
}

#ifdef COMPILE_WITH_ATTRIBUTES
WORD PentiumM_Controller::GetNumberOfPstates()
{
  return 
    //"(WORD)" because of MSVC's "warning C4267: 'return' : conversion from 
    //'size_t' to 'WORD', possible loss of data"
    (WORD) mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() ;
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
#endif //#ifdef COMPILE_WITH_ATTRIBUTES

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
    //Avoid g++ warning "warning: converting to `WORD' from `float'"
    (WORD) fVoltageID ;
  if( fVoltageID - (float) wVoltageID >= 0.5 ) 
    ++ wVoltageID ;
  return wVoltageID ;
}

float PentiumM_Controller::GetVoltageInVolt(WORD wVoltageID )
{
  return 0.7 + 0.016 * wVoltageID ;
}

//For "increase voltage" in the user interface.
void PentiumM_Controller::IncreaseVoltageBy1Step(float & r_fVoltage)
{
  WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
  //if( wVoltageID < GetMaximumVoltageID(
  ++ wVoltageID ;
  r_fVoltage = GetVoltageInVolt( wVoltageID ) ;
}

//void PentiumM_Controller::IncreaseVoltageForCurrentPstate(BYTE byCoreID)
//{
//  float fVoltageInVolt ;
//  WORD wFreqInMHz ;
//  if( GetCurrentPstate(
//    wFreqInMHz
//    , fVoltageInVolt
//    , byCoreID
//    ) )
//  {
//    fVoltageInVolt +=
//      //Suffix "f" to avoid MSVC's
//      //"warning C4305: '+=' : truncation from 'double' to 'float'"
//      0.032f ; //increase by 2 voltage ID steps
//    SetVoltageAndFrequency(
//      fVoltageInVolt
//      , wFreqInMHz
//      , byCoreID
//      ) ;
//    fVoltageInVolt +=
//      //Suffix "f" to avoid MSVC's
//      //"warning C4305: '+=' : truncation from 'double' to 'float'"
//      0.32f ; //increase by 2 voltage ID steps
//  }
//  //BYTE byFreqID ;
//  //BYTE byVoltageID ;
//  //BYTE byCoreID  ;
//  //GetCurrentPstate(
//  //  byFreqID
//  //  , byVoltageID
//  //  , byCoreID
//  //  ) ;
//  //byVoltageID += 2 ;
//  //SetPstate(
//  //  byFreqID
//  //  , byVoltageID
//  //  , byCoreID
//  //  ) ;
//}

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
void PentiumM_Controller::MonitorNumberOfFrequencyIDtransitions()
{
  PerformanceEventSelectRegisterWrite(
    1 //<< byCoreID ,
    ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from
    //  MSR ... to MSR ...  for
    // 1 or 2 "Performance Event Counter Registers" from
    //  ... to ...
    //  that store the 48 bit counter value
    //Performance Event Counter number
//    1 ,
    0 ,
    EMON_EST_TRANS ,
    //LAST_LEVEL_CACHE_MISSES_UMASK , // 8 bit unit mask
    OnlyFrequencytransitionsMask ,
    1, //User Mode
    1, //OS mode
    0, //edge
    0, //pin control
    0, //APIC
    1, //enable counters
    0 , //invert counter mask
    0 //counter mask
    ) ;
  mp_cpuaccess->WrmsrEx( //IA32_PMC1
//    IA32_PMC0
    PERFORMANCE_COUNTER_FOR_FID_CHANGE
    //Set to 1 so that next time no more 0 so that this function is called again.
    , 1
    , 0
    , 1 ) ;
  //Cause to fetch 2 values for taking a diff for _each_ value type (TSC, time,
  //  FID changes)
  m_b2ndTimeOrLaterReadTSCandFIDchange = false ;
}
#endif

//AFAIK performance counter for counter other than 0 also needs counter 0
// to have the enable flag set to "1".
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
  PerformanceEventSelectRegisterWrite_PentiumM(
    dwAffinityBitMask ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from
    //  MSR ... to MSR ...  for
    // 1 or 2 "Performance Event Counter Registers" from
    //  ... to ...
    //  that store the 48 bit counter value
    byPerformanceEventSelectRegisterNumber ,
    byEventSelect , //8 bit
    byUnitMask , // 8 bit
    bUserMode,
    bOSmode,
    bEdgeDetect,
    bPINcontrol,
    bEnableAPICinterrupt,
    //Intel vol. 3B (document # 253659):
    //"When set, performance counting is
    //enabled in the corresponding performance-monitoring counter; when clear, the
    //corresponding counter is disabled. The event logic unit for a UMASK must be
    //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
    //IA32_PMCx."
    bEnablePerformanceCounter,
    bInvertCounterMask ,
    byCounterMask
    ) ;
}

BYTE PentiumM_Controller::SetVoltageAndFrequency(
  float fVoltageInVolt
  , WORD wFreqInMHz
  //The Pentium has just 1 core but this param is for compatibility to the 
  //base class.
  , BYTE byCoreID
  )
{
  BYTE byRet ;
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
  byRet = WrmsrEx(
    IA32_PERF_CTL
    , dwLow
    , 0
    , 1
    ) ;

  return byRet ;
}

#ifdef COMPILE_WITH_ATTRIBUTES
BYTE PentiumM_Controller::SetFreqAndVoltageFromFreq(
  WORD wFreqInMHz 
  , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
  , BYTE byCoreID )
{
  BYTE byRet = 0 ;
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
        wFreqInMHz
        //ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
        , fVoltageInVolt 
        //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
        , cr_stdsetvoltageandfreqForInterpolation
        ) 
      )
    {
      byRet = SetVoltageAndFrequency(//wFreqInMHz
        fVoltageInVolt
        //, ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
        , wFreqInMHz
        , byCoreID
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
          //, mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
          , cr_stdsetvoltageandfreqForInterpolation
          ) 
        )
      {
        byRet = SetVoltageAndFrequency( fVoltageInVolt, wFreqInMHz, byCoreID ) ;
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
  return byRet ;
}
#endif //#ifdef COMPILE_WITH_ATTRIBUTES

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
#ifdef COMPILE_WITH_ATTRIBUTES
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
//        //Breakpoint possibility
//        int i = 0 ;
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
#endif //#ifdef COMPILE_WITH_ATTRIBUTES
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
