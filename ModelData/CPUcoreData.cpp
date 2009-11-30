#include "CPUcoreData.hpp"
#include <Controller/AMD/Griffin/AMD_family17.h>
#include <wxWidgets/DynFreqScalingThread.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>

extern Logger g_logger ;

PerCPUcoreAttributes::PerCPUcoreAttributes()
    : 
    mp_dynfreqscalingthread(NULL)
    , mp_icpucoreusagegetter(NULL)
    , m_wCurrentFreqInMHz (0)
    , m_fPreviousCPUusage(-1.0)
  {
    //mp_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
    //  m_byCoreID
    //  , 
    //  ) ;
  }
  PerCPUcoreAttributes::~PerCPUcoreAttributes()
  {
    if( mp_dynfreqscalingthread )
      delete mp_dynfreqscalingthread ;
    if( mp_icpucoreusagegetter )
      delete mp_icpucoreusagegetter ;
  }

  void PerCPUcoreAttributes::CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter 
    )
  {
    mp_icpucoreusagegetter = p_icpucoreusagegetter  ;
    if ( ! mp_dynfreqscalingthread )
    {
		//http://docs.wxwidgets.org/2.6/wx_wxthread.html:
		//"[...]"This means, of course, that all detached threads must be 
		//created on the heap because the thread will call delete this; 
		//upon termination.[...]"
      mp_dynfreqscalingthread = new DynFreqScalingThread(
        mp_icpucoreusagegetter
        //, mp_griffincontroller 
        , mp_cpucontroller
        , *mp_cpucoredata
        );
      //mp_dynfreqscalingthread->
	    if( mp_dynfreqscalingthread->Create() == wxTHREAD_NO_ERROR )
      {
        LOGN("DVFS thread succ. created")
		    wxThreadError wxthreaderror = mp_dynfreqscalingthread->Run() ;
        LOGN("after starting DVFS thread result: " << wxthreaderror )
      }
    }
  }
  
  //when this class is an element of an array, the paramless ctor is
  //called?! So do the init with params here.
  void PerCPUcoreAttributes::Create(
    BYTE byCoreID
    //ICPUcoreUsageGetter * p_icpucoreusagegetter 
    //, GriffinController * p_griffincontroller 
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    )
  {
    m_byCoreID = byCoreID ;
    //mp_griffincontroller = p_griffincontroller ;
    mp_cpucontroller = p_cpucontroller ;
    //mp_icpucoreusagegetter = p_icpucoreusagegetter  ;
    mp_cpucoredata = & r_cpucoredata ;
    //mp_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
    //  m_byCoreID
    //  , mp_griffincontroller
    //  ) ;
  }

void CPUcoreData::AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz)
{
  m_stdsetvoltageandfreqDefault.insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

void CPUcoreData::AddLowestStableVoltageAndFreq(float fValue,WORD wFreqInMHz)
{
  m_setloweststablevoltageforfreq.insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
  //mp_setloweststablevoltageforfreq->insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

//Advantage of this method: if the datastructure is accessed at many 
//code place and is changed then, on needs to change the real impl. only here.
void CPUcoreData::AddPreferredVoltageForFreq(float fValue, WORD wFreqInMHz)
{
  //mp_stdsetvoltageandfreqWanted->insert( 
  m_stdsetvoltageandfreqWanted.insert( 
    VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

CPUcoreData::CPUcoreData()
    //C++ style initialisations:
    //: m_fCPUcoreLoadThresholdForIncreaseInPercent(80.0f)
    //, m_fPercentalCPUcoreFreqIncrease(1.5f)
    //, m_fVoltageForMaxCPUcoreFreq(1.05f)
    //, m_arfCPUcoreLoadInPercent(NULL)
    //, m_arwCurrentFreqInMHz(NULL)
    //, m_byMaxVoltageID(MAX_VALUE_FOR_VID)
    //, m_byMinVoltageID(0)
    //, m_byMainPLLoperatingFrequencyIDmax(CPU_CORE_DATA_NOT_SET)
  {
    LOGN("CPU attributes ctor")
    //TODO get the actual number of CPU cores and set THIS one.
    //SetCPUcoreNumber(2);
    //SetMaxFreqInMHz(2200);
    Init() ;
  }

void CPUcoreData::Init()
{
  m_fCPUcoreLoadThresholdForIncreaseInPercent = 80.0f ;
  m_fPercentalCPUcoreFreqIncrease = 1.5f ;
  m_fThrottleTemp = 90.0 ; //a good default value.
  m_fVoltageForMaxCPUcoreFreq = 1.05f ;
  m_arfCPUcoreLoadInPercent = NULL ;
  m_arwCurrentFreqInMHz = NULL ;
  m_byMaxVoltageID = MAX_VALUE_FOR_VID ;
  m_byMinVoltageID = 0 ;
  m_wMaxFreqInMHz = 0 ;
  m_byMainPLLoperatingFrequencyIDmax = CPU_CORE_DATA_NOT_SET ;
  m_arp_percpucoreattributes = NULL ;
  //mp_griffincontroller = NULL ;
  mp_cpucontroller = NULL ;
  m_wMilliSecondsWaitBetweenDFVS = 200 ;
  m_byUpdateViewOnDVFS = 0 ;
  m_bEnableDVFS = false ;
  LOGN("Init. of CPU attributes")
  //mp_stdsetvoltageandfreqAvailableFreq = new std::set<VoltageAndFreq>() ;
  //mp_stdsetvoltageandfreqWanted = new std::set<VoltageAndFreq> ();
  //mp_setloweststablevoltageforfreq = new std::set<VoltageAndFreq> () ;
  //mp_stdsetvoltageandfreqDefault = new std::set<VoltageAndFreq> () ;
}

  BYTE CPUcoreData::GetMainPLLoperatingFrequencyIDmax()
  {
    return m_byMainPLLoperatingFrequencyIDmax ;
  }

  void CPUcoreData::SetMainPLLoperatingFrequencyIDmax(
    BYTE byMainPLLoperatingFrequencyIDmax)
  {
    m_byMainPLLoperatingFrequencyIDmax = byMainPLLoperatingFrequencyIDmax;
        //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
        //by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
        //the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
        //"F3xD4[MainPllOpFreqId] is defined as follows: 100 MHz *
        //(F3xD4[MainPllOpFreqId] + 08h)"
        //(100 MHz * (CpuFid + 08h)) > 100 MHz * (MainPllOpFreqId] + 08h) | : 100 MHz
        //(100 MHz * (CpuFid + 08h)) > (100 MHz * (MainPllOpFreqId] + 08h))/2
        //<=> (100 MHz * (CpuFid + 08h)) > 50 MHz * (MainPllOpFreqId] + 08h) | : 50 MHz
        //<=> 2 * (CpuFid + 08h)) > MainPllOpFreqId + 08h
        //<=> 2 * CpuFid + 2 * 08h > MainPllOpFreqId + 08h
        //<=> 2 * CpuFid + 16 > MainPllOpFreqId + 08 | - 16
        //<=> 2 * CpuFid > MainPllOpFreqId - 08 | : 2
        //<=> CpuFid > ( MainPllOpFreqId - 08 ) : 2
     m_byLowestEffectiveFreqID = ( m_byMainPLLoperatingFrequencyIDmax - 8 ) 
       / 2 + 1 ;
     SetMaxFreqInMHz( ( byMainPLLoperatingFrequencyIDmax + 8 ) * 100 ) ;
  }

  CPUcoreData::CPUcoreData(
    BYTE byNumberOfCPUcores, 
    WORD wMaxFreqInMHz
    )
    //: m_wMaxFreqInMHz(wMaxFreqInMHz)
  {
    LOGN("CPU attributes ctor 2")
    //Call default constructor.
    //CPUcoreData();
    Init() ;
    SetMaxFreqInMHz(wMaxFreqInMHz) ;
    SetCPUcoreNumber(byNumberOfCPUcores);
  }
  
  CPUcoreData::~CPUcoreData()
  {
    PossiblyReleaseMem();
  }

  BYTE CPUcoreData::GetNumberOfCPUcores()
  {
    return m_byNumberOfCPUCores ;
  }

  void CPUcoreData::PossiblyReleaseMem()
  {
    if(m_arfCPUcoreLoadInPercent)
      delete [] m_arfCPUcoreLoadInPercent ;
    if(m_arwCurrentFreqInMHz)
      delete [] m_arwCurrentFreqInMHz ;
    if( m_arp_percpucoreattributes )
      delete [] m_arp_percpucoreattributes ;
    //delete mp_stdsetvoltageandfreqAvailableFreq ;
    //delete mp_stdsetvoltageandfreqWanted ;
    //delete mp_setloweststablevoltageforfreq ;
    //delete mp_stdsetvoltageandfreqDefault ;
  }

  void CPUcoreData::SetCPUcoreNumber(BYTE byNumberOfCPUcores)
  {
    m_byNumberOfCPUCores = byNumberOfCPUcores ;
    PossiblyReleaseMem() ;
    //if( m_arfCPUcoreLoadInPercent )
    //  delete [] m_arfCPUcoreLoadInPercent ;
    m_arfCPUcoreLoadInPercent = new float[byNumberOfCPUcores];
    m_arwCurrentFreqInMHz = new WORD[byNumberOfCPUcores];
    m_arp_percpucoreattributes = new PerCPUcoreAttributes[ 
      byNumberOfCPUcores ] ;
    if( m_arp_percpucoreattributes )
      for(BYTE byCoreID = 0 ; byCoreID < byNumberOfCPUcores ; ++ byCoreID )
        m_arp_percpucoreattributes[ byCoreID ].Create(
          byCoreID
          //, mp_griffincontroller 
          , mp_cpucontroller
          , *this
          ) ;
  }

  void CPUcoreData::SetMaxFreqInMHz(WORD wMaxFreqInMHz)
  {
    m_wMaxFreqInMHz = wMaxFreqInMHz ;
    m_wAQuarterOfMaxFreq = wMaxFreqInMHz / 4 ;
    m_wAHalfOfMaxFreq = wMaxFreqInMHz / 2 ;
  }

