#include "CPUcoreData.hpp"
//Must inlude ".cpp" because of "inline" , else if including ".h": g++ error
//"undefined reference".
//GetArrayIndexForClosestValue(...)
#include <algorithms/binary_search/binary_search.cpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp> //class I_CPUcontroller
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes

bool CPUcoreData::AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz)
{
  bool bInserted = false ;
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  m_wxcriticalsection.Enter() ;
#endif
  std::pair <std::set<VoltageAndFreq>::iterator, bool> 
    stdpairstdsetvoltageandfreq = m_stdsetvoltageandfreqDefault.insert( 
    VoltageAndFreq(fValue,wFreqInMHz) ) ;
  bInserted = stdpairstdsetvoltageandfreq.second ;
  //Used by I_CPUcontroller::GetMaximumFrequencyInMHz(), 
  //I_CPUcontroller::GetMinimumFrequencyInMHz()
  m_stdsetvoltageandfreqAvailableFreq.insert(
    VoltageAndFreq(fValue,wFreqInMHz) ) ;
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  m_wxcriticalsection.Leave() ;
#endif
  return bInserted ;
}

void CPUcoreData::AddLowestStableVoltageAndFreq(float fValue,WORD wFreqInMHz)
{
  m_stdsetvoltageandfreqLowestStable.insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
  //mp_setloweststablevoltageforfreq->insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

void CPUcoreData::AvailableMultipliersToArray()
{
  if( m_arfAvailableMultipliers )
  {
    delete [ ] m_arfAvailableMultipliers ;
    m_arfAvailableMultipliers = NULL ;
  }
  LOGN("AvailableMultipliersToArray--std::set size: "
    << m_stdset_floatAvailableMultipliers.size() )
  if( ! m_stdset_floatAvailableMultipliers.empty() )
  {
    m_arfAvailableMultipliers = new float [
      m_stdset_floatAvailableMultipliers.size() ] ;
    LOGN("AvailableMultipliersToArray--array address:" <<
      m_arfAvailableMultipliers )
    //If alloc succeeded.
    if( m_arfAvailableMultipliers )
    {
      WORD wArrayIndex = 0 ;
      for( std::set<float>::const_iterator c_iter =
          m_stdset_floatAvailableMultipliers.begin() ;
          c_iter != m_stdset_floatAvailableMultipliers.end() ;
          ++ c_iter , ++ wArrayIndex )
      {
        m_arfAvailableMultipliers[ wArrayIndex ] = *c_iter ;
      }
    }
  }
}

void CPUcoreData::AvailableVoltagesToArray()
{
  if( m_arfAvailableVoltagesInVolt )
  {
    delete [ ] m_arfAvailableVoltagesInVolt ;
    m_arfAvailableVoltagesInVolt = NULL ;
  }
  if( ! m_stdset_floatAvailableVoltagesInVolt.empty() )
  {
    m_arfAvailableVoltagesInVolt = new float [
      m_stdset_floatAvailableVoltagesInVolt.size() ] ;
    //If alloc succeeded.
    if( m_arfAvailableVoltagesInVolt )
    {
      WORD wArrayIndex = 0 ;
      for( std::set<float>::const_iterator c_iter =
          m_stdset_floatAvailableVoltagesInVolt.begin() ;
          c_iter != m_stdset_floatAvailableVoltagesInVolt.end() ;
          ++ c_iter , ++ wArrayIndex )
      {
        m_arfAvailableVoltagesInVolt[ wArrayIndex ] = *c_iter ;
      }
    }
  }
}

//Advantage of this method: if the datastructure is accessed at many 
//code place and is changed then, on needs to change the real impl. only here.
void CPUcoreData::AddPreferredVoltageForFreq(float fValue, WORD wFreqInMHz)
{
  //mp_stdsetvoltageandfreqWanted->insert( 
  m_stdsetvoltageandfreqWanted.insert( 
    VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

void CPUcoreData::ClearCPUcontrollerSpecificAtts()
{
  m_stdsetvoltageandfreqDefault.clear() ;
  m_stdsetvoltageandfreqWanted.clear() ;
  m_stdsetvoltageandfreqLowestStable.clear() ;
  m_stdsetvoltageandfreqAvailableFreq.clear() ;
}

CPUcoreData::CPUcoreData()
  //C++ style initializations:
  :
  m_fCPUcoreLoadThresholdForIncreaseInPercent(80.0f)
  //, m_fCPUcoreFreqIncreaseFactor(1.5f)
  //, m_fVoltageForMaxCPUcoreFreq(1.05f)
  //, m_arfCPUcoreLoadInPercent(NULL)
  //, m_arwCurrentFreqInMHz(NULL)
  //, m_byMaxVoltageID(MAX_VALUE_FOR_VID)
  //, m_byMinVoltageID(0)
  //, m_byMainPLLoperatingFrequencyIDmax(CPU_CORE_DATA_NOT_SET)

//  , m_conditionCPUdataCanBeSafelyRead( m_mutexCPUdataCanBeSafelyRead )
//  , m_conditionDVFSthreadMayChangeData( m_mutexDVFSthreadMayChangeData )
  {
    //LOGN("CPU attributes ctor")
    //TODO get the actual number of CPU cores and set THIS one.
    //SetCPUcoreNumber(2);
    //SetMaxFreqInMHz(2200);
    Init() ;
  }

CPUcoreData::CPUcoreData(
  BYTE byNumberOfCPUcores,
  WORD wMaxFreqInMHz
  )
//  //C++ style initializations:
//  :
  //m_wMaxFreqInMHz(wMaxFreqInMHz)

//  m_conditionCPUdataCanBeSafelyRead( m_mutexCPUdataCanBeSafelyRead )
{
  //LOGN("CPU attributes ctor 2")
  //Call default constructor.
  //CPUcoreData();
  Init() ;
  SetMaxFreqInMHz(wMaxFreqInMHz) ;
  SetCPUcoreNumber(byNumberOfCPUcores);
}

CPUcoreData::~CPUcoreData()
{
  if( m_arfAvailableMultipliers )
  {
    delete [] m_arfAvailableMultipliers ;
    m_arfAvailableMultipliers = NULL ;
  }
  if( m_arfAvailableVoltagesInVolt )
  {
    delete [] m_arfAvailableVoltagesInVolt ;
    m_arfAvailableVoltagesInVolt = NULL ;
  }
  PossiblyReleaseMemForCoreNumAffectedData();
}

//This method should be called by every c'tor.
void CPUcoreData::Init()
{
//  //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionctor:
//  // the mutex should be initially locked
//  m_mutexCPUdataCanBeSafelyRead.Lock();

  m_arfAvailableMultipliers = NULL ;
  m_arfAvailableVoltagesInVolt = NULL ;
  m_b1CPUcorePowerPlane = true ;
  m_fCPUcoreLoadThresholdForIncreaseInPercent =
    //expressed as core usage between [0...1]
      0.9f ;
  m_fCPUcoreFreqIncreaseFactor = 1.5f ;
  m_fThrottleTempInDegCelsius = 90.0 ; //a good default value.
  m_fVoltageForMaxCPUcoreFreq = 1.05f ;
  m_arfCPUcoreLoadInPercent = NULL ;
  m_arwCurrentFreqInMHz = NULL ;
  m_byMaxVoltageID = //MAX_VALUE_FOR_VID ;
      0 ;
  m_byMinVoltageID = 0 ;
  //Important to set to NULL when the CPU controller is NULL. 
  //(creation of menus per CPU core )
  m_byNumberOfCPUCores = 0 ;
//  m_llLastTimeTooHot = 0;
  m_wMaxFreqInMHz = 0 ;
  m_wMilliSecondsWaitToCheckWhetherTemperatureDecreased = 5000 ; //5 seconds.
  m_arp_percpucoreattributes = NULL ;
  mp_cpucontroller = NULL ;
  m_wMilliSecondsWaitBetweenDFVS = 200 ;
  m_byUpdateViewOnDVFS = 0 ;
  m_bEnableDVFS = false ;
  LOGN("Initialisation of CPU attributes")
  //mp_stdsetvoltageandfreqAvailableFreq = new std::set<VoltageAndFreq>() ;
  //mp_stdsetvoltageandfreqWanted = new std::set<VoltageAndFreq> ();
  //mp_setloweststablevoltageforfreq = new std::set<VoltageAndFreq> () ;
  //mp_stdsetvoltageandfreqDefault = new std::set<VoltageAndFreq> () ;
}

void CPUcoreData::InterpolateDefaultVoltages()
{
  LOGN("CPUcoreData::InterpolateDefaultVoltages begin--CPU controller:"
    << mp_cpucontroller)
  if( mp_cpucontroller && m_arfAvailableMultipliers)
  {
    std::set<VoltageAndFreq>::const_iterator c_iter =
      m_stdsetvoltageandfreqDefault.begin() ;
    if( c_iter != m_stdsetvoltageandfreqDefault.end() )
    {
      std::set<VoltageAndFreq>::const_iterator c_iterPrev = c_iter;
      std::set<VoltageAndFreq> stdsetvoltageandfreqDefault;
      ++ c_iter;
      float fReferenceClockInMHz = mp_cpucontroller->m_fReferenceClockInMHz ;
      float fFreqInMHz;
      float fFreqInMHzPrev;
  //      float fFreqInMHzDiff;
      float fFreqInMHzRatio;
      float fVoltageInVolt;
      LOGN("ref clock i MHz:" << fReferenceClockInMHz)
      WORD wArraySize = m_stdset_floatAvailableMultipliers.size();
      float fMultiplier;
      for( ; c_iter != m_stdsetvoltageandfreqDefault.end() ; ++ c_iter )
      {
//        if(  )
        {
          fFreqInMHzPrev = c_iterPrev->m_wFreqInMHz;
          LOGN("Current prev p-state:" << fFreqInMHzPrev << " Volt @ "
            << c_iterPrev->m_fVoltageInVolt << " MHz.")
          for( WORD wArrayIndex = 0 ; wArrayIndex < wArraySize ;
              ++ wArrayIndex )
          {
            fMultiplier = m_arfAvailableMultipliers[ wArrayIndex ];
            fFreqInMHz = fMultiplier * fReferenceClockInMHz;
            LOGN("CPUcoreData::InterpolateDefaultVoltages begin"
              "--" << fFreqInMHz << "in ]" << fFreqInMHzPrev << ";"
              << c_iter->m_wFreqInMHz << "[ ?")
            if( fFreqInMHz > fFreqInMHzPrev && fFreqInMHz <
              c_iter->m_wFreqInMHz )
            {
              fFreqInMHzRatio = fFreqInMHz / fFreqInMHzPrev ; //-> >= 1.0
              fVoltageInVolt = c_iterPrev->m_fVoltageInVolt * fFreqInMHzRatio;
              LOGN("Adding " << fVoltageInVolt << " Volt @ " << fFreqInMHz <<
                " MHz to default voltages.")
              //Warning: if the reference clock is not fixed then it may vary
              //, i.e. more and more frequency-voltage pairs are inserted
              //because the frequency depends on the reference clock.
              stdsetvoltageandfreqDefault.insert(
                VoltageAndFreq(fVoltageInVolt,fFreqInMHz)
                ) ;
            }
          }
        }
        c_iterPrev = c_iter;
      }
      m_stdsetvoltageandfreqDefault.insert(stdsetvoltageandfreqDefault.begin(),
        stdsetvoltageandfreqDefault.end() -- ) ;
    }
  }
  LOGN("CPUcoreData::InterpolateDefaultVoltages end")
}
//float CPUcoreData::GetLowerMultiplier( float fMulti )
//{
//  m_stdset_floatAvailableMultipliers.f
//}

  void CPUcoreData::ThreadFinishedAccess()
  {
//    //Only 1 thread should safely access the var (decrementing a value is
//    //not an atomic operation)
//    m_criticalsectionThreads.Enter() ;
//    -- m_wNumThreadsAccessingThisObject ;
//    if( m_wNumThreadsAccessingThisObject == 0 )
//    {
//
//      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//      // tell the other(s) thread(s) that we're about to terminate: we must
//      // lock the mutex first or we might signal the condition before the
//      // waiting threads start waiting on it!
//
//      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//      //Waits until the mutex is unlocked.
//      //when "condition.Wait()" is called in the other thread then
//      // the mutex is unlocked and so con
//      wxMutexLocker lock(m_mutexDVFSthreadMayChangeData);
//      //All threads finished reading from the data now the DVFS thread can
//      //safely modify the data.
//      m_conditionDVFSthreadMayChangeData.Broadcast() ;
//    }
//    m_criticalsectionThreads.Leave() ;
  }

  WORD CPUcoreData::GetIndexForClosestMultiplier(float fMultiplier)
  {
    //TODO
//    WORD wArrayIndexForClosestLessOrEqual = GetArrayIndexForClosestLessOrEqual(
//      m_arfAvailableMultipliers,
//      m_stdset_floatAvailableMultipliers.size(),
//      fMultiplier
//      ) ;
//    WORD wArrayIndexForClosestGreaterOrEqual =
//      GetArrayIndexForClosestGreaterOrEqual(
//      m_arfAvailableMultipliers,
//      m_stdset_floatAvailableMultipliers.size(),
//      fMultiplier
//      ) ;
    WORD wArrayIndexForClosestValue =
      GetArrayIndexForClosestValue(
        m_arfAvailableMultipliers,
        m_stdset_floatAvailableMultipliers.size(),
        fMultiplier
        ) ;
//    if( w )
    return wArrayIndexForClosestValue ;
//    float fMultiplierClosestGreaterOrEqual =
//        m_arfAvailableMultipliers[ArrayIndexForClosestGreaterOrEqual] ;
//    float fMultiplierClosestLessOrEqual =
//        m_arfAvailableMultipliers[ArrayIndexForClosestLessOrEqual] ;
//    return fMultiplier
  }

  BYTE CPUcoreData::GetIndexForClosestVoltage(float fVoltageInVolt)
  {
    //TODO
    WORD wArrayIndexForClosestValue =
      GetArrayIndexForClosestValue(
        m_arfAvailableVoltagesInVolt,
        m_stdset_floatAvailableVoltagesInVolt.size(),
        fVoltageInVolt
        ) ;
//    if( w )
    return wArrayIndexForClosestValue ;
  }

//  void CPUcoreData::ThreadWantsAccess()
//  {
//    //Only 1 thread should safely access the var (decrementing a value is
//    //not an atomic operation)
//    m_criticalsectionThreads.Enter() ;
//    ++ m_wNumThreadsAccessingThisObject ;
//    if( m_wNumThreadsAccessingThisObject == 0 )
//    {
//
//      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//      // tell the other(s) thread(s) that we're about to terminate: we must
//      // lock the mutex first or we might signal the condition before the
//      // waiting threads start waiting on it!
//
//      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
//      //Waits until the mutex is unlocked.
//      //when "condition.Wait()" is called in the other thread then
//      // the mutex is unlocked and so con
//      wxMutexLocker lock(m_mutexDVFSthreadMayChangeData);
//      //All threads finished reading from the data now the DVFS thread can
//      //safely modify the data.
//      m_conditionDVFSthreadMayChangeData.Broadcast() ;
//    }
//    m_criticalsectionThreads.Leave() ;
//  }

  BYTE CPUcoreData::GetNumberOfCPUcores()
  {
    return m_byNumberOfCPUCores ;
  }

  float CPUcoreData::GetVoltageInVolt(WORD wVoltageInVoltIndex)
  {
    if( m_arfAvailableVoltagesInVolt &&
      wVoltageInVoltIndex < //m_wNumberOfAvailableVoltagesInVolt
      m_stdset_floatAvailableVoltagesInVolt.size()
      )
      return m_arfAvailableVoltagesInVolt[ wVoltageInVoltIndex ] ;
    else
      return -1.0 ;
  }

  //inline
  float CPUcoreData::GetMaximumMultiplier()
  {
    std::set<float>::const_reverse_iterator c_rev_iter_stdset_fMultipliers =
      m_stdset_floatAvailableMultipliers.rbegin() ;
    if( c_rev_iter_stdset_fMultipliers !=
        m_stdset_floatAvailableMultipliers.rend() )
    {
      return *c_rev_iter_stdset_fMultipliers ;
    }
    return 0.0 ;
  }

  //inline
  float CPUcoreData::GetMinimumVoltage()
  {
    std::set<float>::const_iterator c_iter_stdset_fVoltages =
      m_stdset_floatAvailableVoltagesInVolt.begin() ;
    if( c_iter_stdset_fVoltages !=
        m_stdset_floatAvailableVoltagesInVolt.end() )
    {
      return *c_iter_stdset_fVoltages ;
    }
    return 0.0 ;
  }

  //Releases memory that has something to do with the number of CPU cores.
  void CPUcoreData::PossiblyReleaseMemForCoreNumAffectedData()
  {
    if(m_arfCPUcoreLoadInPercent)
    {
      delete [] m_arfCPUcoreLoadInPercent ;
      m_arfCPUcoreLoadInPercent = NULL ;
    }
    if(m_arwCurrentFreqInMHz)
    {
      delete [] m_arwCurrentFreqInMHz ;
      m_arwCurrentFreqInMHz = NULL ;
    }
    if( m_arp_percpucoreattributes )
    {
      delete [] m_arp_percpucoreattributes ;
      m_arp_percpucoreattributes = NULL ;
    }
    //delete mp_stdsetvoltageandfreqAvailableFreq ;
    //delete mp_stdsetvoltageandfreqWanted ;
    //delete mp_setloweststablevoltageforfreq ;
    //delete mp_stdsetvoltageandfreqDefault ;
  }

  void CPUcoreData::SetCPUcoreNumber(BYTE byNumberOfCPUcores)
  {
    LOGN("CPUcoreData::SetCPUcoreNumber("
//#ifdef _DEBUG
      << (WORD) byNumberOfCPUcores
//#endif
      << ")" )
    m_byNumberOfCPUCores = byNumberOfCPUcores ;
    PossiblyReleaseMemForCoreNumAffectedData() ;
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
