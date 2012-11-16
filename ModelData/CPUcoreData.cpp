/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2012.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "CPUcoreData.hpp"
//Must inlude ".cpp" because of "inline" , else if including ".h": g++ error
//"undefined reference".
//GetArrayIndexForClosestValue(...)
#include <algorithms/binary_search/binary_search.cpp>
#include <ModelData/ModelData.hpp> //class Model
#include <Controller/CPU-related/I_CPUcontroller.hpp> //class I_CPUcontroller
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes

void CPUcoreData::AddLowestStableVoltageAndFreq(float fValue,WORD wFreqInMHz)
{
  m_stdsetvoltageandfreqLowestStable.insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
  //mp_setloweststablevoltageforfreq->insert( VoltageAndFreq(fValue,wFreqInMHz) ) ;
}

/** For automatic configuration of lowest CPU core voltage where CPU operation
 * is stable yet. */
float CPUcoreData::AddLowestStableVoltageForLowestAvailableMulti(
  const VoltageAndFreq & lowestStableVoltageForLowestMultiFound,
  const WORD wLowestFreqInMHz,
  VoltageAndFreq & lowestStableVoltageForLowestMulti
  )
{
  float fLowestStableVoltageInVoltForLowestMulti;
  //Extrapolate lowest multi with lowest stable voltage &
  //  highest multi with lowest stable voltage to
  //  lowest stable voltage for lowest multi
  m_stdsetvoltageandfreqLowestStable.rbegin()->
    GetLinearExtrapolatedVoltage(
      lowestStableVoltageForLowestMultiFound,
      wLowestFreqInMHz,
      fLowestStableVoltageInVoltForLowestMulti);
  LOGN( FULL_FUNC_NAME << " extrapolated lowest stable voltage for "
    << wLowestFreqInMHz << " MHz: "
    << fLowestStableVoltageInVoltForLowestMulti << " V")

  lowestStableVoltageForLowestMulti = VoltageAndFreq(
    fLowestStableVoltageInVoltForLowestMulti,
    wLowestFreqInMHz);
  m_stdsetvoltageandfreqLowestStable.insert(
    lowestStableVoltageForLowestMulti
    );
  LOGN( FULL_FUNC_NAME << " inserted extrapolated lowest stable p-state "
    << lowestStableVoltageForLowestMulti)
  return fLowestStableVoltageInVoltForLowestMulti;
}

void CPUcoreData::AddWantedVoltageForMinVoltage(
  WORD wExtrapolatedWantedFreqForMinVoltage)
{
}

/** For automatic configuration of lowest CPU core voltage where CPU operation
 * is stable yet. */
void CPUcoreData::AddMinVoltageForLowestMulti(WORD wLowestFreqInMHz)
{
  m_stdsetvoltageandfreqWanted.insert(
    VoltageAndFreq(
      m_arfAvailableVoltagesInVolt[0],
      wLowestFreqInMHz)
      );
  LOGN( FULL_FUNC_NAME << " added min voltage for min multi "
//    "based on extrapolated wanted voltage frequency [MHz] for lowest settable"
//    " voltage:" << wExtrapolatedWantedFreqForMinVoltage
    )
}

#ifdef COMPILE_AS_GUI //Because margin value is member of class
  // "UserInterfaceAttributes" that is unavailable to the service.
void CPUcoreData::AddWantedVoltageForLowestAvailableMulti(
  const float fLowestStableVoltageInVoltForLowestAvailableMulti,
  const WORD wLowestFreqInMHz)
{
  float fWantedVoltageInVoltForLowestAvailableMulti = m_r_model.
    GetVoltageWithOperatingSafetyMargin(
    fLowestStableVoltageInVoltForLowestAvailableMulti);
  if( fWantedVoltageInVoltForLowestAvailableMulti < m_arfAvailableVoltagesInVolt[0] )
    fWantedVoltageInVoltForLowestAvailableMulti = m_arfAvailableVoltagesInVolt[0];

  ( (VoltageAndFreq &) (* m_stdsetvoltageandfreqWanted.find(
    VoltageAndFreq(fWantedVoltageInVoltForLowestAvailableMulti,
      wLowestFreqInMHz)
    ) ) ).m_fVoltageInVolt = fWantedVoltageInVoltForLowestAvailableMulti;
  LOGN( FULL_FUNC_NAME << " inserted wanted p-state "
    << VoltageAndFreq(fWantedVoltageInVoltForLowestAvailableMulti, wLowestFreqInMHz) )
}
#endif //#ifdef COMPILE_AS_GUI

/** For automatic configuration of lowest CPU core voltage where CPU operation
 * is stable yet. */
WORD CPUcoreData::AddWantedVoltageForLowestVoltage(
  const VoltageAndFreq & lowestStableVoltageForLowestMultiFound)
{
  //Wanted voltage for lowest multi may have been set before to mark
  //the lowest voltage is above an unstable voltage and additionally
  //to use it for interpolating the lowest stable voltage in addition
  //with the lowest stable voltage for a higher multiplier.
  //So do not extrapolate but instead use extrapolated lowest stable
  //voltage and add safety margin voltage.
  //TODO important for Intel Core 2 duo (because the lowest possible
  // voltage is reached event with high frequencies)
  // get Schnittpunkt from line from wanted voltage line
  //and lowest possible voltage:

  //             *<-wanted voltage
  //            /
  //      -----+--- <-lowest possible voltage
  //           ^
  //         Schnittpunkt
  //From the schnittpunkt insert to the wanted voltages.
  WORD wExtrapolatedWantedFreqForMinVoltage;
  m_stdsetvoltageandfreqWanted.rbegin()-> //last entry
    GetLinearExtrapolatedFrequency(
      * m_stdsetvoltageandfreqWanted.find(
        //E.g. lowest stable voltage was found for lowest freq of 800 MHz,
        //but lowest freq is 600 MHz.
        lowestStableVoltageForLowestMultiFound),
      m_arfAvailableVoltagesInVolt[0],//Lowest possible voltage.
      wExtrapolatedWantedFreqForMinVoltage);
  LOGN( FULL_FUNC_NAME << " extrapolated frequency [MHz] for wanted voltages "
    "for lowest settable"
    " voltage:" << wExtrapolatedWantedFreqForMinVoltage )

  if( wExtrapolatedWantedFreqForMinVoltage >
    lowestStableVoltageForLowestMultiFound.m_wFreqInMHz)
  {
    LOGN( FULL_FUNC_NAME << " extrapolated frequency [MHz] is > lowest frequency")
    VoltageAndFreq voltageandfreq = VoltageAndFreq(
      m_arfAvailableVoltagesInVolt[0],
      wExtrapolatedWantedFreqForMinVoltage);
    m_stdsetvoltageandfreqWanted.insert( voltageandfreq);
    LOGN( FULL_FUNC_NAME << " added wanted voltage based on extrapolated "
      "frequency [MHz] for for wanted voltages for lowest settable"
      " voltage:" << voltageandfreq )
  }
  return wExtrapolatedWantedFreqForMinVoltage;
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
      std::set<float>::const_iterator c_iter_std_set_float;
      for( c_iter_std_set_float =
          m_stdset_floatAvailableMultipliers.begin() ;
          c_iter_std_set_float != m_stdset_floatAvailableMultipliers.end() ;
          ++ c_iter_std_set_float , ++ wArrayIndex )
      {
        m_arfAvailableMultipliers[ wArrayIndex ] = *c_iter_std_set_float ;
      }
      m_fMaximumCPUcoreMultiplier =
        //Highest multiplier.
        * (-- c_iter_std_set_float);
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

//CPUcoreData::CPUcoreData(Model & r_model)
//  //C++ style initializations:
//  : m_r_model (r_model),
//  m_fCPUcoreLoadThresholdForIncreaseInPercent(80.0f)
//  //This reserve is e.g. useful for watching TV: so the TV application does not
//  // stop showing an image when the time slices for the app wouldn't be
//  // sufficient.
//  , m_fCPUcoreFreqInMHzReserve(300.0f)
//  //, m_fCPUcoreFreqIncreaseFactor(1.5f)
//  //, m_fVoltageForMaxCPUcoreFreq(1.05f)
//  //, m_arfCPUcoreLoadInPercent(NULL)
//  //, m_arwCurrentFreqInMHz(NULL)
//  //, m_byMaxVoltageID(MAX_VALUE_FOR_VID)
//  //, m_byMinVoltageID(0)
//  //, m_byMainPLLoperatingFrequencyIDmax(CPU_CORE_DATA_NOT_SET)
//
////  , m_conditionCPUdataCanBeSafelyRead( m_mutexCPUdataCanBeSafelyRead )
////  , m_conditionDVFSthreadMayChangeData( m_mutexDVFSthreadMayChangeData )
//  {
//    //LOGN("CPU attributes ctor")
//    //TODO get the actual number of CPU cores and set THIS one.
//    //SetCPUcoreNumber(2);
//    //SetMaxFreqInMHz(2200);
//    Init() ;
//  }

//CPUcoreData::CPUcoreData(
//  Model & r_model,
//  BYTE byNumberOfCPUcores,
//  WORD wMaxFreqInMHz
//  )
//  //C++ style initializations:
//  :
//    m_r_model (r_model)
//  //m_wMaxFreqInMHz(wMaxFreqInMHz)
//
////  m_conditionCPUdataCanBeSafelyRead( m_mutexCPUdataCanBeSafelyRead )
//{
//  Construct_Inline();
//}

//CPUcoreData::~CPUcoreData()
//{
//  Destruct();
//}

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

  WORD CPUcoreData::GetIndexForClosestMultiplier(float fMultiplier) const
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

  BYTE CPUcoreData::GetIndexForClosestVoltage(float fVoltageInVolt) const
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

  const VoltageAndFreq * CPUcoreData::GetClosestHigherVoltageAndFreq(
    const std::set<VoltageAndFreq> & stdsetvoltageandfreq,
    float fMultiplier) const
  {
    std::set<VoltageAndFreq>::const_iterator c_iter =
      stdsetvoltageandfreq.upper_bound( VoltageAndFreq(0.0f,
      (WORD) (fMultiplier * mp_cpucontroller->m_fReferenceClockInMHz)) );
    if( c_iter != stdsetvoltageandfreq.end() )
      return & * c_iter;
    return NULL;
  }

  const VoltageAndFreq * CPUcoreData::GetClosestLowerVoltageAndFreq(
    const std::set<VoltageAndFreq> & stdsetvoltageandfreq,
    float fMultiplier) const
  {
    LOGN( FULL_FUNC_NAME << " begin multi:" << fMultiplier)
    std::set<VoltageAndFreq>::const_iterator c_iter = stdsetvoltageandfreq.
      begin();
    const VoltageAndFreq * p_voltageandfreqLower = NULL;
    while( c_iter != stdsetvoltageandfreq.end() )
    {
      if( c_iter->m_wFreqInMHz / mp_cpucontroller->m_fReferenceClockInMHz
          >= fMultiplier )
        break;
      p_voltageandfreqLower = & * c_iter;
      ++ c_iter;
    }
    if( p_voltageandfreqLower)
      LOGN( FULL_FUNC_NAME << " return " << * p_voltageandfreqLower)
    return p_voltageandfreqLower;
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

  float CPUcoreData::GetMinimumMultiplier()
  {
    std::set<float>::const_iterator c_iter_stdset_fMultipliers =
      m_stdset_floatAvailableMultipliers.begin() ;
    if( c_iter_stdset_fMultipliers !=
        m_stdset_floatAvailableMultipliers.end() )
    {
      //Return first element.
      return * c_iter_stdset_fMultipliers ;
    }
    return 0.0 ;
  }

  //inline
  float CPUcoreData::GetMaximumMultiplier()
  {
    std::set<float>::const_reverse_iterator c_rev_iter_stdset_fMultipliers =
      m_stdset_floatAvailableMultipliers.rbegin() ;
    if( c_rev_iter_stdset_fMultipliers !=
        m_stdset_floatAvailableMultipliers.rend() )
    {
      //Return last element.
      return * c_rev_iter_stdset_fMultipliers ;
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

  float CPUcoreData::GetNextVoltageAbove(float fVoltageInVolt)
  {
    WORD wArrayIndex = ::GetArrayIndexForClosestGreaterOrEqual(
      m_arfAvailableVoltagesInVolt,
      m_stdset_floatAvailableVoltagesInVolt.size(),
      fVoltageInVolt);
    if( wArrayIndex < m_stdset_floatAvailableVoltagesInVolt.size() - 1)
    {
        return m_arfAvailableVoltagesInVolt[wArrayIndex + 1];
    }
    return 0.0;
  }

  float CPUcoreData::
    GetStartVoltageForHighestMultiplierForFindingLowestCPUcoreVoltage(
      float & fMultiplier) const
  {
    LOGN( FULL_FUNC_NAME << " begin")
    float fVoltageInVolt = 0.0f;//, fMultiplier;
    const std::vector<VoltageAndFreq> & voltageandfreqInsertedByCPUcontroller =
      m_std_vec_voltageandfreqInsertedByCPUcontroller;

    std::vector<VoltageAndFreq>::const_reverse_iterator
      c_rev_iter_voltageandfreqInsertedByCPUcontroller =
      voltageandfreqInsertedByCPUcontroller.rbegin();

    std::set<float>::const_reverse_iterator c_rev_iterAvailableMultipliers =
      //Start with the highest frequency/ multiplier inserted by the CPU controller.
      m_stdset_floatAvailableMultipliers.rbegin();
    if( c_rev_iterAvailableMultipliers !=
        m_stdset_floatAvailableMultipliers.rend() )
    {
      const float fHighestMultiplier = * c_rev_iterAvailableMultipliers;
      LOGN( FULL_FUNC_NAME << " highest multiplier:" << fHighestMultiplier)

      if( c_rev_iter_voltageandfreqInsertedByCPUcontroller !=
          voltageandfreqInsertedByCPUcontroller.rend()
        )
      {
        LOGN( FULL_FUNC_NAME << " last p-state inserted by CPU controller:"
          << * c_rev_iter_voltageandfreqInsertedByCPUcontroller)
        fMultiplier = mp_cpucontroller->GetClosestMultiplier(
          (* c_rev_iter_voltageandfreqInsertedByCPUcontroller).m_wFreqInMHz);
        if( fMultiplier == fHighestMultiplier
          )
        {
          LOGN( FULL_FUNC_NAME << "last p-state inserted by CPU controller has"
            "highest CPU core multiplier " << fMultiplier)
          fVoltageInVolt =
            c_rev_iter_voltageandfreqInsertedByCPUcontroller->
            m_fVoltageInVolt;
        }
      }
      if( fVoltageInVolt == 0.0f // voltage was not assigned yet
          && ! m_stdset_floatAvailableVoltagesInVolt.empty() )
      {
        //highest voltage,
        fVoltageInVolt = m_arfAvailableVoltagesInVolt[
          m_stdset_floatAvailableVoltagesInVolt.size() - 1 ];
        //highest multiplier,
        fMultiplier = m_arfAvailableMultipliers[
          m_stdset_floatAvailableMultipliers.size() - 1 ];
        LOGN( FULL_FUNC_NAME << " using p-state with max voltage and max "
          "multi: ("
          << fVoltageInVolt << "V," << fMultiplier << ")")
      }
    }
    return fVoltageInVolt;
  }

  float CPUcoreData::
    GetStartVoltageForLowestMultiplierForFindingLowestCPUcoreVoltage(
      float & fMultiplier) const
  {
    LOGN( FULL_FUNC_NAME << " begin")
    float fVoltageInVolt = 0.0f;//, fMultiplier;
    const std::vector<VoltageAndFreq> & voltageandfreqInsertedByCPUcontroller =
      m_std_vec_voltageandfreqInsertedByCPUcontroller;

    if( ! m_stdset_floatAvailableMultipliers.empty() )
    {
      const float fLowestMultiplier = * m_stdset_floatAvailableMultipliers.
        begin();
      LOGN( FULL_FUNC_NAME << " lowest multiplier:" << fLowestMultiplier)

      LOGN( FULL_FUNC_NAME << " # volt and freq inserted by CPU controller:"
        << voltageandfreqInsertedByCPUcontroller.size() )
      if( ! voltageandfreqInsertedByCPUcontroller.empty() )
      {
        const VoltageAndFreq &
          voltageandfreq1stMultiplierInsertedByCPUcontroller =
          voltageandfreqInsertedByCPUcontroller.front();
        fMultiplier = mp_cpucontroller->GetClosestMultiplier(
          voltageandfreq1stMultiplierInsertedByCPUcontroller.m_wFreqInMHz);
        LOGN( FULL_FUNC_NAME << " 1st multiplier inserted by CPU controller:"
          << fMultiplier)
        if( fMultiplier == fLowestMultiplier
          )
        {
          fVoltageInVolt = voltageandfreq1stMultiplierInsertedByCPUcontroller.
            m_fVoltageInVolt;
          LOGN( FULL_FUNC_NAME << " 1st multiplier inserted by CPU controller "
            "is lowest multiplier--so using " << fVoltageInVolt << "V")
        }
      }
      else if( ! m_stdset_floatAvailableVoltagesInVolt.empty() )
      {
//        fVoltageInVolt = m_arfAvailableVoltagesInVolt[
//          m_stdset_floatAvailableVoltagesInVolt.size() - 1 ];
        //Lowest multiplier
        fMultiplier = m_arfAvailableMultipliers[ 0];
      }
    }
    return fVoltageInVolt;
  }

  bool CPUcoreData::ArentDirectlyNeighbouredMultipliers(
    const float fLowestMultiplierWhereInstabilityWasReached,
    const float fHighestMultiplierWhereInstabilityCouldntBeReached)
  {
    bool bRet = true;
    LOGN( FULL_FUNC_NAME << "--begin--LowestMultiplierWhereInstabilityWasReached:" <<
      fLowestMultiplierWhereInstabilityWasReached
      << " HighestMultiplierWhereInstabilityCouldntBeReached:"
      << fHighestMultiplierWhereInstabilityCouldntBeReached )
    WORD wIndex = GetIndexForClosestMultiplier(
      fLowestMultiplierWhereInstabilityWasReached);
    WORD wIndex2 = GetIndexForClosestMultiplier(
      fHighestMultiplierWhereInstabilityCouldntBeReached);
    WORD wDiff = wIndex - wIndex2;
    if( wDiff == 1 || wDiff == MAXWORD )
      bRet = false;
    LOGN( FULL_FUNC_NAME << "--return " << bRet)
    return bRet;
  }

  const VoltageAndFreq * CPUcoreData::
    GetClosestHigherVoltageAndFreqInsertedByCPUcontroller(
    float fMultiplier) const
  {
    std::vector<VoltageAndFreq>::const_iterator c_iter =
      m_std_vec_voltageandfreqInsertedByCPUcontroller.begin();
    float fReferenceClockInMHz = mp_cpucontroller->m_fReferenceClockInMHz;
    if( fReferenceClockInMHz > 0.0f )
      while( c_iter != m_std_vec_voltageandfreqInsertedByCPUcontroller.end() )
      {
        if( (c_iter->m_wFreqInMHz / fReferenceClockInMHz) > fMultiplier )
          return & *c_iter;
        ++ c_iter;
      }
    return NULL;
  }

  /** Gets start (interpolated) default/ desired voltage for CPU core
   * multiplier in center of a low and a high CPU core multiplier */
  float CPUcoreData::
    GetStartVoltageForMultiplierForFindingLowestCPUcoreVoltage(
//      float & fPercentOfMinAndMaxMultiplier
      const float fHighestMultiplierWhereInstabilityCouldntBeReached,
      const float fLowestMultiplierWhereInstabilityWasReached,
      float & fMultiplier
      ) const
  {
    LOGN( FULL_FUNC_NAME << "--begin--"
      //"highest multi where instability couldn't be reached "
      "lowest multi with instable voltage:"
      << fLowestMultiplierWhereInstabilityWasReached
      << " highest multi where instability not reached:"
      << fHighestMultiplierWhereInstabilityCouldntBeReached )
    float fVoltageInVolt = 0.0f;//, fMultiplier;
    WORD wArrayIndexForClosestCPUcoreMultiplier = GetIndexForClosestMultiplier(
      (fHighestMultiplierWhereInstabilityCouldntBeReached +
      fLowestMultiplierWhereInstabilityWasReached) / 2.0f);
    LOGN( FULL_FUNC_NAME << " wArrayIndexForClosestCPUcoreMultiplier:"
      << wArrayIndexForClosestCPUcoreMultiplier)

    if( wArrayIndexForClosestCPUcoreMultiplier != MAXWORD)
    {
      fMultiplier = m_arfAvailableMultipliers[
        wArrayIndexForClosestCPUcoreMultiplier];
      LOGN( FULL_FUNC_NAME << " CPU core multiplier to get voltage for:"
        << fMultiplier)

      //TODO first search for wanted voltage closest to multiplier
      // if no
      const VoltageAndFreq * p_voltageandfreqHigher;
      p_voltageandfreqHigher = GetClosestHigherVoltageAndFreq(
        m_stdsetvoltageandfreqLowestStable, fMultiplier);
      const VoltageAndFreq * p_voltageandfreqFromLowestStableVoltages =
        p_voltageandfreqHigher;
      if( p_voltageandfreqHigher )
      {
        LOGN( FULL_FUNC_NAME << " lowest stable p-state closest to > "
          "multiplier " << fMultiplier << ":"
          << * p_voltageandfreqFromLowestStableVoltages)
      }
      else
        p_voltageandfreqHigher = GetClosestHigherVoltageAndFreq(
          m_stdsetvoltageandfreqWanted, fMultiplier);
      if( ! p_voltageandfreqHigher)
        p_voltageandfreqHigher = GetClosestHigherVoltageAndFreq(
          m_stdsetvoltageandfreqDefault, fMultiplier);
      const float maxVoltage = m_arfAvailableVoltagesInVolt[
        m_stdset_floatAvailableVoltagesInVolt.size() - 1];
      const float maxMulti = m_arfAvailableMultipliers[
        m_stdset_floatAvailableMultipliers.size() - 1];
      const VoltageAndFreq voltageandfreqMaxMultAndMaxVoltage(
        maxVoltage, maxMulti * mp_cpucontroller->m_fReferenceClockInMHz);
      if( ! p_voltageandfreqHigher)
        p_voltageandfreqHigher = & voltageandfreqMaxMultAndMaxVoltage;
      LOGN( FULL_FUNC_NAME << "--higher volt & freq:" << * p_voltageandfreqHigher)

      const VoltageAndFreq * p_voltageandfreqLower =
        GetClosestLowerVoltageAndFreq(
        fMultiplier);
      if( p_voltageandfreqLower)
      {
        LOGN( FULL_FUNC_NAME << "--lower volt & freq:" << * p_voltageandfreqLower)
        p_voltageandfreqHigher->GetInterpolatedVoltage(
          * p_voltageandfreqLower,
          (WORD) (fMultiplier * mp_cpucontroller->m_fReferenceClockInMHz),
          fVoltageInVolt);
        LOGN( FULL_FUNC_NAME << " interpolated voltage of lower and higher "
          "volt & freq:" << fVoltageInVolt)
        BYTE arrayIndex = GetIndexForClosestVoltage(fVoltageInVolt);
        float closestHigherVoltage = m_arfAvailableVoltagesInVolt[arrayIndex];
        float fVoltageInVoltPlusSafetyMargin = closestHigherVoltage;
//        if( closestHigherVoltage < fVoltageInVolt )
        //The calculated interpolated voltage may be < the lowest stable voltage,
        // so add an extra of ~ 0.04V.
        while( fVoltageInVoltPlusSafetyMargin < fVoltageInVolt + 0.04f)
        {
          ++ arrayIndex;
          fVoltageInVoltPlusSafetyMargin = m_arfAvailableVoltagesInVolt[
            arrayIndex];
        }
//        BYTE arrayIndex = GetIndexForClosestVoltage(fVoltageInVolt);
        LOGN( FULL_FUNC_NAME << " interpolated voltage with margin:"
          << fVoltageInVoltPlusSafetyMargin)
        fVoltageInVolt = fVoltageInVoltPlusSafetyMargin;
//        fVoltageInVolt = p_voltageandfreqHigher->m_fVoltageInVolt;
      }
      else
      {
        fVoltageInVolt = p_voltageandfreqHigher->m_fVoltageInVolt;
        LOGN( FULL_FUNC_NAME << " no lower volt & freq")
      }
//      const std::vector<VoltageAndFreq> & voltageandfreqInsertedByCPUcontroller
//        = m_std_vec_voltageandfreqInsertedByCPUcontroller;
//
//      const VoltageAndFreq * p_voltageandfreqFromLowestStableVoltages = NULL,
//        * p_voltageandfreqForMultiplier = NULL;
//
//      if( voltageandfreqInsertedByCPUcontroller.empty() )
//      { //No voltages inserted by CPU controller existent.
//        p_voltageandfreqForMultiplier = p_voltageandfreqFromLowestStableVoltages;
//      }
//      else //Voltages inserted by CPU controller are existent.
//      {
//        p_voltageandfreqForMultiplier =
//          GetClosestHigherVoltageAndFreqInsertedByCPUcontroller(fMultiplier);
//        LOGN( FULL_FUNC_NAME << " pointer to p-state inserted by CPU "
//          "controller closest to > multiplier: " << fMultiplier << ":"
//          << p_voltageandfreqForMultiplier)
//        if( p_voltageandfreqForMultiplier )
//        {
//          LOGN( FULL_FUNC_NAME << " p-state inserted by CPU controller closest "
//            "to > multiplier: " << fMultiplier << ":"
//            << * p_voltageandfreqForMultiplier)
//          if( * p_voltageandfreqFromLowestStableVoltages <
//              * p_voltageandfreqForMultiplier )
//            p_voltageandfreqForMultiplier =
//              p_voltageandfreqFromLowestStableVoltages;
//        }
//      }
//      if(p_voltageandfreqForMultiplier)
//        fVoltageInVolt = p_voltageandfreqForMultiplier->m_fVoltageInVolt;
    }
    LOGN( FULL_FUNC_NAME << " return " << fVoltageInVolt)
//    g_logger.Log(<< fVoltageInVolt);
    return fVoltageInVolt;
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
