#include "stdafx.h" //for "NULL"

#include "NehalemController.hpp"
//#include <windef.h> //for "NULL"
#include <Controller/Intel_registers.h>
#include <ModelData/ModelData.hpp>
//#include <I_CPUaccess.hpp>
#include <Controller/I_CPUaccess.hpp>

NehalemController::NehalemController()
  //: mp_model(NULL)
{
}

NehalemController::~NehalemController(void)
{
}

//For "increase voltage" in the user interface.
void NehalemController::DecreaseVoltageBy1Step(float & r_fVoltage)
{
  WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
  //if( wVoltageID < GetMaximumVoltageID(
  -- wVoltageID ;
  r_fVoltage = GetVoltageInVolt( wVoltageID ) ;
}

float NehalemController::GetReferenceClock()
{

}

BYTE NehalemController::Init()
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

void NehalemController::GetAllPossibleFrequencies(
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

BYTE NehalemController::GetCurrentVoltageAndFrequency(
  //float because: the FSB*multiplier is not exactly an integer without
  //floating point digits
  float & r_fFreqInMHz
  , float & r_fVoltageInVolt
  , BYTE byCoreID
  )
{
  WORD wFreqInMHz ;
  //float fVoltageInVolt ;
  GetCurrentPstate(
    wFreqInMHz
    , r_fVoltageInVolt
    , byCoreID
    ) ;
  {
//    mp_cpuaccess->ReadTSC() ;
  }
}

BYTE NehalemController::GetCurrentPstate(
  WORD & r_wFreqInMHz 
  , float & r_fVoltageInVolt
  , BYTE byCoreID
  )
{
  BYTE bySuccess = 0 ;
  DWORD dwHigh, dwLow ;
  if( RdmsrEx(
    //See Intel soft.vol. 3B,
    //Table B-5. MSRs in Processors Based on Intel Microarchitecture (Contd.)(Nehalem)
			//IA32_APERF
			IA32_PERF_STATUS
      , dwLow
      , dwHigh
      , //1
      1 << byCoreID
      )
    )
  {
//#ifdef _DEBUG
    //BYTE byFID = dwLow >> 8 ;
		BYTE byFID = ( dwLow & 255 ) ;
//#endif
    r_wFreqInMHz = //( ( dwLow >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ) * 100 ;
      byFID * 133 ;
//		r_wFreqInMHz = 1 ;
#ifdef _DEBUG
    if( r_wFreqInMHz > 1800 )
      r_wFreqInMHz = r_wFreqInMHz ;
#endif
	}
  if( RdmsrEx(
    //See Intel soft.vol. 3B,
    //Table B-5. MSRs in Processors Based on Intel Microarchitecture (Contd.)(Nehalem)
      IA32_PERF_STATUS
      , dwLow
      , dwHigh
      , 1
      )
    )
  {
#ifdef _DEBUG
    BYTE byVoltageID = ( dwLow & 255 ) ;
#endif
    //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
    //63= 0.7V 62=0.716 V,...
    //40=1.340 Volt
    r_fVoltageInVolt = //0.7 + //( 63 - byVoltageID )
      //byVoltageID * 0.016 ;
    //VID6 VID5 VID4 VID3 VID2 VID1 VID0
    //0    0    0    0    0    0    0    = 1.5 Volt 
    //0    0    0    0    0    0    1    = 1.4875 Volt
    //1.5 - 1.4875 = 0.0125 Volt
    //1.5 - ( (float) byVoltageID * 0.0125 ) ;
    -1.0 ;
#ifdef _DEBUG
    byVoltageID = byVoltageID ;
#endif
    bySuccess = 1 ;
  }
#ifdef _DEBUG
  else
  {
    bySuccess = 0 ;
  }
#endif
  //return 1 ;
  return bySuccess ;
}

BYTE NehalemController::GetCurrentPstate(
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
    r_byVoltageID = dwLow >> 8 ;
    r_byFreqID = ( dwLow & 255 ) ;
  }
  return 1 ;
}

WORD NehalemController::GetMaximumFrequencyInMHz()
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

WORD NehalemController::GetMinimumFrequencyInMHz()
{
  if( mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() > 0 
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->size() > 0 
    )
    return mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin()->
    //return mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin()->
      m_wFreqInMHz ;
  return 0 ;
}

WORD NehalemController::GetMaximumVoltageID()
{
  //The maximum voltage is 1.34 V.
  //0.7V + 0.016V * 40 = 0,7 V + 0,64V = 1.34 V
  return 40 ;
}

WORD NehalemController::GetMinimumVoltageID()
{
  //0.7V + 0.016V * 0 = 0.7 V
  return 0 ;
}

float NehalemController::GetMinimumVoltageInVolt()
{
  return 
    //Leave an "f" prefix to avoid MSVC warning 
    //  "warning C4305: 'return' : truncation from 'double' to 'float'"
    0.7f ;
}

WORD NehalemController::GetNumberOfPstates()
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
BYTE NehalemController::GetPstate(WORD wPstateID,
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

//bool NehalemController::GetPstateSafefy(
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

WORD NehalemController::GetVoltageID(float fVoltageInVolt )
{
  //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  float fVoltageID = fVoltageMinusLowestVoltage / 0.016 ;
  WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //round up because at 1.18 V the voltage ID in float is 29.999998
    //and would get 29 (1.164 V) when converting to an integer.
    //for 1.164 voltage ID in float is 29.000004
    //ceil( fVoltageID );
	//Cast to WORD to avoid warning "converting to WORD from float.
    (WORD) fVoltageID ;
  if( fVoltageID - (float) wVoltageID >= 0.5 ) 
    ++ wVoltageID ;
  return wVoltageID ;
}

float NehalemController::GetVoltageInVolt(WORD wVoltageID )
{
  return 0.7 + 0.016 * wVoltageID ;
}

//For "increase voltage" in the user interface.
void NehalemController::IncreaseVoltageBy1Step(float & r_fVoltage)
{
  WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
  //if( wVoltageID < GetMaximumVoltageID(
  ++ wVoltageID ;
  r_fVoltage = GetVoltageInVolt( wVoltageID ) ;
}

void NehalemController::IncreaseVoltageForCurrentPstate(BYTE byCoreID)
{
  float fVoltageInVolt ;
  WORD wFreqInMHz ;
  if( GetCurrentPstate(
    wFreqInMHz 
    , fVoltageInVolt
    , byCoreID 
    ) )
  {
    fVoltageInVolt += 
      //Leave an "f" prefix to avoid MSVC warning 
      //  "warning C4305: '+=' : truncation from 'double' to 'float'"
      0.032f ; //increase by 2 voltage ID steps
    SetVoltageAndFrequency(
      fVoltageInVolt
      , wFreqInMHz
      , byCoreID
      ) ;
    fVoltageInVolt += 
      //Leave an "f" prefix to avoid MSVC warning 
      //  "warning C4305: '+=' : truncation from 'double' to 'float'"
      0.32f ; //increase by 2 voltage ID steps
  }
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

//Intel i7 mobile spec update:
//AAP53.       Performance Monitor Counters May Count Incorrectly
//Problem:     Under certain circumstances, a general purpose performance counter, IA32_PMC0-4
//             (C1H - C4H), may count at core frequency or not count at all instead of counting the
//             programmed event.
//Implication: The Performance Monitor Counter IA32_PMCx may not properly count the programmed
//             event. Due to the requirements of the workaround there may be an interruption in the
//             counting of a previously programmed event during the programming of a new event.
//Workaround: Before programming the performance event select registers, IA32_PERFEVTSELx MSR
//             (186H - 189H), the internal monitoring hardware must be cleared. This is accomplished
//             by first disabling, saving valid events and clearing from the select registers, then
//             programming three event values 0x4300D2, 0x4300B1 and 0x4300B5 into the
//             IA32_PERFEVTSELx MSRs, and finally continuing with new event programming and
//             restoring previous programming if necessary. Each performance counter, IA32_PMCx,
//             must have its corresponding IA32_PREFEVTSELx MSR programmed with at least one of
//             the event values and must be enabled in IA32_PERF_GLOBAL_CTRL MSR (38FH) bits
//             [3:0]. All three values must be written to either the same or different
//             IA32_PERFEVTSELx MSRs before programming the performance counters. Note that
//             the performance counter will not increment when its IA32_PERFEVTSELx MSR has a
//             value of 0x4300D2, 0x4300B1 or 0x4300B5 because those values have a zero UMASK
//             field (bits [15:8]).
//Status:      For the steppings affected, see the Summary Tables of Changes.
void NehalemController::AccuratelyStartPerformanceCounting(
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
  bool bInvertCounterMask ,
  BYTE byCounterMask
  )
{
  //Intel i7 mobile spec update AAP53. Workaround step 1: 
  // "[...] disabling, saving valid events and clearing from the select 
  // registers [...]"
  PerformanceEventSelectRegisterWrite(
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
    //bool bEnablePerformanceCounter,
    false , //-> disable PerforManceCounter
    bInvertCounterMask ,
    byCounterMask
    ) ;
  //Intel i7 mobile spec update AAP53. Workaround step 2: 
  // "[...] programming three event values 0x4300D2, 0x4300B1 and 0x4300B5 
  //  into the IA32_PERFEVTSELx MSRs [...]"
  DWORD dwLow = 0x4300D2 ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    dwLow ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    0x4300B1 ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    0x4300B5 ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
  //Intel i7 mobile spec update AAP53. Workaround step 3: 
  // "[...] finally continuing with new event programming and
  // restoring previous programming if necessary [...]"
  PerformanceEventSelectRegisterWrite(
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
    //bool bEnablePerformanceCounter,
    true , //-> enable PerforManceCounter
    bInvertCounterMask ,
    byCounterMask
    ) ;
}

void NehalemController::PerformanceEventSelectRegisterWrite(
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
  //Intel i7 700 spec update:
  // "AAP53. Performance Monitor Counters May Count Incorrectly"
  //-> before selecting the event, the values 
  //  -0x4300D2 (4391122 decimal)
  //  -0x4300B1 (4391089 decimal)
  //  -0x4300B5 (4391093 decimal)
  //  must be written into the Performance Monitor Select registers
  WrmsrEx(
    // MSR index
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber ,
    dwLow ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    // Thread Affinity Mask
    //1	
    dwAffinityBitMask
    ) ;
}

void NehalemController::PrepareForNextPerformanceCounting(
  DWORD dwAffinityBitMask 
  , BYTE byPerformanceEventSelectRegisterNumber
  )
{
  //Intel i7 mobile spec update AAP53. Workaround step 1: 
  // "[...] disabling, saving valid events and clearing from the select 
  // registers [...]"
  PerformanceEventSelectRegisterWrite(
    dwAffinityBitMask ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from 
    //  MSR ... to MSR ...  for 
    // 1 or 2 "Performance Event Counter Registers" from 
    //  ... to ...
    //  that store the 48 bit counter value
    byPerformanceEventSelectRegisterNumber ,
    0 , //byEventSelect , //8 bit
    0 , //byUnitMask , // 8 bit
    0 , //bUserMode,
    0 , //bOSmode,
    0 , //bEdgeDetect,
    0 , //bPINcontrol,
    0 , //bEnableAPICinterrupt,
    //Intel vol. 3B (document # 253659):
    //"When set, performance counting is
    //enabled in the corresponding performance-monitoring counter; when clear, the
    //corresponding counter is disabled. The event logic unit for a UMASK must be
    //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
    //IA32_PMCx."
    //bool bEnablePerformanceCounter,
    false , //-> disable PerforManceCounter
    0 ,//bInvertCounterMask ,
    0 //byCounterMask
    ) ;
  //Intel i7 mobile spec update AAP53. Workaround step 2: 
  // "[...] programming three event values 0x4300D2, 0x4300B1 and 0x4300B5 
  //  into the IA32_PERFEVTSELx MSRs [...]"
  DWORD dwLow = 0x4300D2 ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    dwLow ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    0x4300B1 ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
  WrmsrEx(
    IA32_PERFEVTSEL0 + byPerformanceEventSelectRegisterNumber , // MSR index
    0x4300B5 ,//eax,			// bit  0-31
    0 , //edx,			// bit 32-63
    dwAffinityBitMask 	// Thread Affinity Mask
  ) ;
}

BYTE NehalemController::SetVoltageAndFrequency(
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
  //Intel i7 mobile datasheet Volume 1 (document number 320765):
  //"Frequency selection is software controlled by writing to processor MSRs."

  //Voltage selection does not seem to be possible by writuing to MSRs.
  byRet = WrmsrEx(
    IA32_PERF_CTL
    , dwLow
    , 0
    , 1
    ) ;

  return byRet ;
}

BYTE NehalemController::SetFreqAndVoltageFromFreq(
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

BYTE NehalemController::TooHot()
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
  NehalemController::WrmsrEx(
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
//      WORD wFreqInMHz = ( dwLow >> 8 ) * 100 ;
//      BYTE byVoltageID = ( dwLow & 255 ) ;
//      //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
//      //63= 0.7V 62=0.716 V,...
//      //40=1.340 Volt
//      float fVoltageInVoltFromRegisterValueToWrite = 0.7 + //( 63 - byVoltageID )
//        byVoltageID * 0.016 ;
//      float fMaxVoltageInVolt ;
//      if( GetInterpolatedVoltageFromFreq(
//          wFreqInMHz
//          , fMaxVoltageInVolt
//          , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
//          //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault
//          )
//        )
//      {
//        int i = 0 ;
//        if( fVoltageInVoltFromRegisterValueToWrite <= fMaxVoltageInVolt )
//          mp_cpuaccess->// TRUE: success, FALSE: failure
//            WrmsrEx(
//              index,		// MSR index
//              dwLow ,//eax,			// bit  0-31
//              dwHigh, //edx,			// bit 32-63
//              affinityMask	// Thread Affinity Mask
//            ) ;l
//          //i = 0 ;
//      }
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
