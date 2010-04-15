#include "ClocksNotHaltedCPUcoreUsageGetter.hpp"
//#include <ModelData/ModelData.hpp>

//for MSR_TIME_STAMP_COUNTER_REGISTER
#include <Controller/AMD/Griffin/AMD_family17.h>
//#include <Controller/GriffinController.hpp>

#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
ClocksNotHaltedCPUcoreUsageGetter::ClocksNotHaltedCPUcoreUsageGetter(
  //BYTE byCoreID ,
  DWORD dwAffinityMask ,
  GriffinController * p_griffincontroller
  //Model * p_model
  )
  :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_dwAtMask2ndTimeCPUcoreMask ( 0 )
  , mp_griffincontroller (p_griffincontroller)
  //, m_bAtLeastSecondTime (false)
{
  BYTE byNumCPUs = p_griffincontroller->mp_model->GetNumberOfCPUCores() ;
  m_dMaximumCPUcoreFrequency = p_griffincontroller->mp_model->m_cpucoredata.
    m_wMaxFreqInMHz ;
  m_ar_cnh_cpucore_ugpca = new
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts[ byNumCPUs ];
  //m_bAtLeastSecondTime
  m_dwAffinityMask = //1 << byCoreID ;
    dwAffinityMask ;
}
#endif //#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE

void ClocksNotHaltedCPUcoreUsageGetter::ProgramNecessaryPermanceCounters(
    BYTE byCoreID)
{
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
  mp_griffincontroller->AccuratelyStartPerformanceCounting(
#else
  AccuratelyStartPerformanceCounting(
#endif
    1 << byCoreID ,
    //m_dwAffinityMask ,
    //2
    //1 +
    //byCoreID //performance counter ID/ number
    0
    , CPU_CLOCKS_NOT_HALTED
    , false //InvertCounterMask
    ) ;
//for( BYTE byCoreID = 0 ; byCoreID < byNumCPUs ; ++ byCoreID )
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
  mp_griffincontroller->AccuratelyStartPerformanceCounting(
#else
  AccuratelyStartPerformanceCounting(
#endif
    1 << byCoreID ,
    //m_dwAffinityMask ,
    //2
    //1 +
    //byCoreID //performance counter ID/ number
    1
    , CPU_CLOCKS_NOT_HALTED
    , true //InvertCounterMask
    ) ;
}

BYTE ClocksNotHaltedCPUcoreUsageGetter::Init(
  //DWORD dwCPUcoreAffinityBitMask 
  )
{
  DEBUGN("ClocksNotHaltedCPUcoreUsageGetter::Init")
  //m_dwAffinityMask = dwCPUcoreAffinityBitMask ;
  if( //get for load all cores 
    //dwCPUcoreAffinityBitMask 
    m_dwAffinityMask == 0 
    )
  {
    BYTE byNumCPUs =
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
        mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
#else
      m_wNumLogicalCPUcores ;
#endif
    DEBUG_COUTN("ClocksNotHaltedCPUcoreUsageGetter # CPU cores" <<
      m_wNumLogicalCPUcores )
    for( BYTE byCoreID = 0 ; byCoreID < byNumCPUs ; ++ byCoreID )
      ProgramNecessaryPermanceCounters(byCoreID) ;
  }
  else
  {
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
    mp_griffincontroller->AccuratelyStartPerformanceCounting(
#else
    AccuratelyStartPerformanceCounting(
#endif
      //1 << byCoreID ,
      m_dwAffinityMask ,
      0, //performance counter ID/ number
      CPU_CLOCKS_NOT_HALTED
      , false //InvertCounterMask
      ) ;
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
    mp_griffincontroller->AccuratelyStartPerformanceCounting( 
#else
    AccuratelyStartPerformanceCounting(
#endif
      //1 << byCoreID ,
      m_dwAffinityMask ,
      1, //performance counter ID/ number
      CPU_CLOCKS_NOT_HALTED
      , true //InvertCounterMask
      ) ;
  }
  return 0 ;
}

//As a convention for all CPU controllers, this method should return the 
//core usage related to the CURRENT (and not 
//to maximum) the CPU frequency.
  //double 
float ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForCore(
  BYTE byCoreID
  )
{
	double dClocksNotHaltedDiffDivTCSdiff = -1.0 ;
  //The Griffin TimeStampCounter runs CONSTANTLY at the highest possible 
  //core frequency.
    //mp_griffincontroller->RdmsrEx(
    //  MSR_TIME_STAMP_COUNTER_REGISTER,
    //  m_dwLowmostBits,// bit  0-31 (register "EAX")
    //  m_dwHighmostBits, 
    //  //m_dwAffinityMask
	   // 1 << byCoreID
    //  ) ;

    ////Use member variable to NOT create a variable for each method call (->faster).
    //m_ull = m_dwHighmostBits ;
    //m_ull <<= 32 ;
    //m_ull |= m_dwLowmostBits ;

#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
	mp_griffincontroller->ReadPerformanceEventCounterRegister(
#else
	ReadPerformanceEventCounterRegister(
#endif
      //2
      //1 + 
      //byCoreID //performance counter ID/ number
      0
      , m_ullPerformanceEventCounter3 ,
      //m_dwAffinityMask 
	    1 << byCoreID
      ) ;
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
	mp_griffincontroller->ReadPerformanceEventCounterRegister(
#else
	ReadPerformanceEventCounterRegister(
#endif
    //2
    //1 + 
    //byCoreID //performance counter ID/ number
    1
    , m_ull
    //m_dwAffinityMask 
    , 1 << byCoreID
    ) ;
  //For the first time there are no previous values for difference .
  if( //m_bAtLeastSecondTime 
	  (m_dwAtMask2ndTimeCPUcoreMask >> byCoreID ) & 1 )
  {

    //ULONGLONG ullTimeStampCounterValueDiff 
  	//m_ullTimeStampCounterValueDiff  = //ull - 
   // //  m_ullPreviousTimeStampCounterValue;
   //   ULONGLONG_VALUE_DIFF( m_ull , //m_ullPreviousTimeStampCounterValue
   //     m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
   //     ) ;
  	m_ullTimeStampCounterValueDiff  = //ull - 
    //  m_ullPreviousTimeStampCounterValue;
      PERFORMANCE_COUNTER_VALUE_DIFF( m_ull , //m_ullPreviousTimeStampCounterValue
        m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue
        ) ;
    //ULONGLONG ullPerformanceEventCounter3Diff = 
    m_ullPerformanceEventCounter3Diff = 
      PERFORMANCE_COUNTER_VALUE_DIFF( m_ullPerformanceEventCounter3 , 
      //m_ullPreviousPerformanceEventCounter3 
      m_ar_cnh_cpucore_ugpca[ byCoreID ].
        m_ullPreviousPerformanceEventCounter3
      ) ;
    //TODO this is pretty sure, but might be tested:
    //After an ACPI hibernation or standby the Performance
    //event counter registers are static/ constant, so the
    //differences have the value 0.
    if( m_ullPerformanceEventCounter3Diff == 0
        || m_ullTimeStampCounterValueDiff == 0 )
    {
      DEBUGN("m_ullPerformanceEventCounter3 == 0 && m_ull == 0")
      //Reprogram performance event counter registers.
//      Init() ;
      ProgramNecessaryPermanceCounters(byCoreID) ;
    }

    //This is the CPU core load/ usage expressed as clocks not halted from 
    //max. possible clocks. 
    //example:
    // If the current freq is 1100 MHz and the 
      //max. freq is 2200 MHz:
      // 1 s time diff
    //if 100% load: "clocks not halted" is 1100 million or nearly 1100 million.
    // so the quotient of "CnH diff/TSC diff" is ca. 1100 M / 2200 M =~ 0.5 
    //if the frequency was constantly 1100 MHz the quotient is 1100 M / 1100 M = 1.0 = 100%
    //If the frequency was NOT constantly the same between the times of 
    //taking differences, the exact usage can not be calculated:
    // if the last set frequency is e.g. 1100 MHz and other software changes it
    //  immediately to 550 MHz after this software did it and the load is 100% the quotient is
    //  ca. 550 M / 1100 M = 0.5 = 50% although the usage was 100%.
    //  This is a problem because
    //  other software may manipulate the current frequency.
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
    float fVoltageInVolt ;
    WORD wFreqInMHz ;
    mp_griffincontroller->GetCurrentPstate( wFreqInMHz, fVoltageInVolt , 
      byCoreID ) ;
    m_dMaximumPossibleCPUclocksNotHalted = (double) m_ullTimeStampCounterValueDiff
      * ( (double) wFreqInMHz / m_dMaximumCPUcoreFrequency ) ;
#endif
    // 
    //double 

	  //dClocksNotHaltedDiffDivTCSdiff =
      //(double) m_ullPerformanceEventCounter3Diff /
      ////The timestamp counter runs at a CONSTANT frequency (same as the 
      ////maximum core frequency) on AMD Griffin CPUs. 
      ////So in order to get the current CPU usage we need the last set 
      ////core frequency.
      ////(double) m_ullTimeStampCounterValueDiff ;
      //m_dMaximumPossibleCPUclocksNotHalted ;
 
    if( m_ullPerformanceEventCounter3Diff )
    {
      dClocksNotHaltedDiffDivTCSdiff = 
        (double) m_ullPerformanceEventCounter3Diff //clocks NOT halted (=CPU busy)
        / 
        //clocks NOT halted + clocks halted = clocks passed
        (double) ( m_ullPerformanceEventCounter3Diff //clocks NOT halted
          + m_ullTimeStampCounterValueDiff ) //clocks halted
        ;
    }
    //If Clocks not halted = 0 the CPU was 100% busy
    else
      dClocksNotHaltedDiffDivTCSdiff = 1.0 ;
#ifdef _DEBUG
	if( //dClocksNotHaltedDiffDivTCSdiff > 1.0 || 
    dClocksNotHaltedDiffDivTCSdiff < 0.02 )
	{
//    //Breakpoint possibility
//		int i = 0 ;
	}
#endif
    //return (float) dClocksNotHaltedDiffDivTCSdiff ;
  }
  else
    //m_bAtLeastSecondTime = true ;
	m_dwAtMask2ndTimeCPUcoreMask |= ( 1 << byCoreID ) ;

      //m_ullPreviousTimeStampCounterValue 
    m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousTimeStampCounterValue 
      = m_ull ;
    //m_ullPreviousPerformanceEventCounter3 
    m_ar_cnh_cpucore_ugpca[ byCoreID ].m_ullPreviousPerformanceEventCounter3
      = m_ullPerformanceEventCounter3 ;
  return //-1.0 ;
	(float) dClocksNotHaltedDiffDivTCSdiff ;
}

BYTE ClocksNotHaltedCPUcoreUsageGetter::GetPercentalUsageForAllCores(
  float arf[] 
  )
{
  BYTE byReturn = 1 ;
  BYTE byNumCPUs =
#ifdef COMPILE_WITH_GRIFFINCONTROLLER_DEPENDANCE
      mp_griffincontroller->mp_model->GetNumberOfCPUCores() ;
#else
    m_wNumLogicalCPUcores ;
#endif
  for( BYTE byCoreID = 0 ; byCoreID < byNumCPUs ; ++ byCoreID )
  {
    arf[ byCoreID ] = GetPercentalUsageForCore( byCoreID ) ;
    if( arf[ byCoreID ] == 
      //-1.0 = error
      -1.0 
      )
	{
      byReturn = 0 ;
	  //break ;
	}
  }
  return byReturn ;
}

BYTE ClocksNotHaltedCPUcoreUsageGetter::SetNumberOfLogicalCPUcores(
  WORD wNumLogicalCPUcores )
{
  m_wNumLogicalCPUcores = wNumLogicalCPUcores ;
  m_ar_cnh_cpucore_ugpca = new
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts[ wNumLogicalCPUcores ];
  return m_ar_cnh_cpucore_ugpca != NULL ;
}
