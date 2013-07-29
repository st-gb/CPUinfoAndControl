/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2012.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include "VoltageAndFreq.hpp" //class VoltageAndFreq

//#include <ModelData/PerCPUcoreAttributes.hpp>
#include <set> //class std::set
#include <vector> //class std::vector
//from http://stackoverflow.com/questions/3243454/what-is-the-linux-equivalent-to-maxdword
#include <limits.h> //UINT_MAX
//GetArrayIndexForClosestValue(...)
#include <algorithms/binary_search/binary_search.hpp>
#include <preprocessor_macros/Windows_compatible_typedefs.h> //WORD etc.
#include <fastest_data_type.h>

#define CPU_CORE_DATA_NOT_SET 255

#if defined(COMPILE_AS_GUI) || defined(COMPILE_AS_SERVICE)
  #define COMPILE_MODEL_ACCESS_THREAD_SAFE
  #define COMPILE_AS_EXECUTABLE
  //Forward declarations (faster than #include)
  class I_CPUcontroller ;
  class Model;
  class MaxVoltageForFreq ;
  class PerCPUcoreAttributes ;

  #include <windef.h> //for BYTE
  #include <map> //class std::map
  #include <string> //std::string
  //CPU controller (dyn libs) do not need thread synchronisation yet.
  #include <Controller/multithread/condition_type.hpp>
  #include <Controller/multithread/criticalsection_type.hpp>
  #include <Controller/multithread/mutex_type.hpp>
#endif

/** Forward decl. */
  class PerCPUcoreAttributes;

/** The member variables can be shared through the subclass of
//ICPUcoreUsageGetter and the user interface class.
//Add new member variables _after_ the last member variable, else
//software using this class may have incongruent declarations of this class:
//a compiled CPU controller dyn lib may have a previous declaration, the GUI
//a newer one. Then when the CPU controller dyn lib tries to write to a member
//variable it may write to the wrong member and the executable the CPU controller
//dyn lib is attached to crashes. */
class CPUcoreData
{
public:
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqWanted ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqLowestStable ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqDefault ;

  bool AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz) ;
  /** Needed by CPU controller dynlibs to insert default voltages */
  bool AddDefaultVoltageForFreq_inline(float fValue,WORD wFreqInMHz)
  {
    bool bInserted = false ;
//  #ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //#ifdef COMPILE_AS_EXECUTABLE
  #ifdef COMPILE_MODEL_ACCESS_THREAD_SAFE
    m_wxcriticalsection.Enter() ;
  #endif
    std::pair <std::set<VoltageAndFreq>::iterator, bool>
      stdpairstdsetvoltageandfreq = m_stdsetvoltageandfreqDefault.insert(
      VoltageAndFreq(fValue,wFreqInMHz) ) ;
    bInserted = stdpairstdsetvoltageandfreq.second ;

  //  //Used by I_CPUcontroller::GetMaximumFrequencyInMHz(),
  //  //I_CPUcontroller::GetMinimumFrequencyInMHz()
  //  m_stdsetvoltageandfreqAvailableFreq.insert(
  //    VoltageAndFreq(fValue,wFreqInMHz) ) ;

  //#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //#ifdef COMPILE_AS_EXECUTABLE
  #ifdef COMPILE_MODEL_ACCESS_THREAD_SAFE
    m_wxcriticalsection.Leave() ;
  #endif
    return bInserted ;
  }
  BYTE m_byNumberOfCPUCores ;
  BYTE GetNumberOfCPUcores()
  {
    return m_byNumberOfCPUCores;
  }
#ifdef COMPILE_AS_EXECUTABLE
  Model & m_r_model;
  std::vector<VoltageAndFreq> m_std_vec_voltageandfreqInsertedByCPUcontroller;
  bool m_b1CPUcorePowerPlane ;
  bool m_bEnableDVFS ;
  bool m_bTooHot ;
  BYTE m_byUpdateViewOnDVFS ;

  BYTE m_byMaxVoltageID ; //=lowest voltage
  BYTE m_byMinVoltageID ;

  BYTE m_byModel ;
  BYTE m_byStepping ;

  //TODO rename to "adjustable..."?! because may be readable but not be
  //writable
  float * m_arfAvailableMultipliers ;
  float * m_arfAvailableThrottleLevels ;
  float * m_arfAvailableVoltagesInVolt ;

  float * m_arfCPUcoreLoadInPercent ;
  float m_fCPUcoreLoadThresholdForIncreaseInPercent;
  float m_fCPUcoreFreqFactor ;
  float m_fCPUcoreFreqIncreaseFactor ;
  float m_fVoltageForMaxCPUcoreFreq ;
  float m_fThrottleTempInDegCelsius ;
//  long long m_llLastTimeTooHot;
  I_CPUcontroller * mp_cpucontroller ;
  std::map<float,VoltageAndFreq> m_stdmap_fMultiplier2voltageandfreq ;
  std::set<float> m_stdset_floatAvailableMultipliers ;
  std::set<float> m_stdset_floatAvailableThrottleLevels;
  std::set<float> m_stdset_floatAvailableVoltagesInVolt ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqAvailableFreq ;

  ////Make as pointers because shared memories need a fixed size,
  ////else if elements are added.
  //std::set<VoltageAndFreq> * mp_stdsetvoltageandfreqAvailableFreq ;
  //std::set<VoltageAndFreq> * mp_stdsetvoltageandfreqWanted ;
  //std::set<VoltageAndFreq> * mp_setloweststablevoltageforfreq ;
  //std::set<VoltageAndFreq> * mp_stdsetvoltageandfreqDefault ;
  std::string m_strVendorID ;
  WORD * m_arwCurrentFreqInMHz ;
  WORD m_wAQuarterOfMaxFreq ;
  WORD m_wMaxFreqInMHz ;
  WORD m_wAHalfOfMaxFreq ;
  //DFVS = dyn. volt. and freq. scaling
  WORD m_wMilliSecondsWaitBetweenDFVS ;
  WORD m_wMilliSecondsWaitToCheckWhetherTemperatureDecreased ;
  WORD m_wFamily ;
//CPU controller (dyn libs) do not need thread synchronisation yet.
//#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
#ifdef COMPILE_MODEL_ACCESS_THREAD_SAFE
  //wxCriticalSection m_wxcriticalsection ;
  //Purpose: for multiple threads accessing the data:
  //  prevent accessing the data while they are being changed by a thread (->the
  // values could be inconsistent/ not integer ->this could lead to false values)
  criticalsection_type m_wxcriticalsection ;
//  condition_type m_conditionDVFSthreadMayChangeData ;
  condition_type m_conditionCPUdataCanBeSafelyRead ;
  mutex_type m_mutexCPUdataCanBeSafelyRead ;
  mutex_type m_mutexDVFSthreadMayChangeData ;

  wxCriticalSection m_wxcriticalsectionIPCdata ;
  wxCriticalSection wxconditionIPC2InProgramData ;
//  Win32EventBasedCondition m_win32eventCPUdataCanBeSafelyRead ;
  condition_type m_condition_typeCPUdataCanBeSafelyRead ;
#endif
//  condition_type * mp_condition ;

  //Intension: allocate this as an array at runtime. So releasing memory 
  //is easier because only memory of this array must be freed
  // and not such mess arrays for each attributes: 
  //  float * m_arfCPUusage ;
  //  DynFreqScalingThread * m_ardynfreqscalingthread ;
  PerCPUcoreAttributes * m_arp_percpucoreattributes ;

  float m_fMaximumCPUcoreMultiplier;
  float m_fCPUcoreFreqInMHzReserve;

  void AddPreferredVoltageForFreq(float fValue,WORD wFreqInMHz) ;
  //void AddFreqAndLowestStableVoltage(float fValue,WORD wFreqInMHz) ;
  void AddLowestStableVoltageAndFreq(float fValue,WORD wFreqInMHz) ;
  float AddLowestStableVoltageForLowestAvailableMulti(
    const VoltageAndFreq & lowestStableVoltageForLowestMultiFound,
    const WORD wLowestFreqInMHz,
    VoltageAndFreq & lowestStableVoltageForLowestMulti);
  WORD AddWantedVoltageForLowestVoltage(
    const VoltageAndFreq & lowestStableVoltageForLowestMultiFound);
#ifdef COMPILE_AS_GUI //Because margin value is member of class
  // "UserInterfaceAttributes" that is unavailable to the service.
  void AddWantedVoltageForLowestAvailableMulti(
    const float fLowestStableVoltageForLowestAvailableMulti,
    const WORD wLowestFreqInMHz);
#endif //#ifdef COMPILE_AS_GUI
  void AddWantedVoltageForMinVoltage(
    WORD wExtrapolatedWantedFreqForMinVoltage);
  void AddMinVoltageForLowestMulti(WORD wLowestFreqInMHz);
  void AvailableMultipliersToArray() ;
  void AvailableThrottleLevelsToArray();
  void AvailableVoltagesToArray() ;
  void ClearCPUcontrollerSpecificAtts() ;
  void CopyDefaultVoltageToPreferredVoltages()
  {
    for( std::set<VoltageAndFreq>::const_iterator c_iter =
        m_stdsetvoltageandfreqDefault.begin() ;
        c_iter != m_stdsetvoltageandfreqDefault.end() ;
        ++ c_iter
       )
    {
      m_stdsetvoltageandfreqWanted.insert( * c_iter ) ;
    }
  }

  CPUcoreData(Model & r_model)
    : m_r_model (r_model)
    , m_fCPUcoreLoadThresholdForIncreaseInPercent(80.0f)
    //This reserve is e.g. useful for watching TV: so the TV application does not
    // stop showing an image when the time slices for the app wouldn't be
    // sufficient.
    , m_fCPUcoreFreqInMHzReserve(300.0f)
  {
    Init();
  }
  CPUcoreData(Model & r_model,BYTE byNumberOfCPUcores, WORD wMaxFreqInMHz)
    : m_r_model (r_model)
  {
    //LOGN("CPU attributes ctor 2")
    //Call default constructor.
    //CPUcoreData();
    Init() ;
    SetMaxFreqInMHz(wMaxFreqInMHz) ;
    SetCPUcoreNumber(byNumberOfCPUcores);
  }
  ~CPUcoreData() { Destruct(); }
//  void Construct_Inline()
//  {
//    //LOGN("CPU attributes ctor 2")
//    //Call default constructor.
//    //CPUcoreData();
//    Init() ;
//    SetMaxFreqInMHz(wMaxFreqInMHz) ;
//    SetCPUcoreNumber(byNumberOfCPUcores);
//  }
  void Destruct()
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

  bool ArentDirectlyNeighbouredMultipliers(
    const float fLowestMultiplierWhereInstabilityWasReached,
    const float fHighestMultiplierWhereInstabilityCouldntBeReached);
  const VoltageAndFreq * GetClosestHigherVoltageAndFreqInsertedByCPUcontroller(
    float fMultiplier) const;
  fastestUnsignedDataType GetIndexForClosestMultiplier(const float fMultiplier) const;
  WORD GetIndexForClosestGreaterEqualMultiplier(float fMultiplier) const
  {
    return MANUFACTURER_ID_NAMESPACE::BinarySearch::
      GetArrayIndexForClosestGreaterOrEqual(
        m_arfAvailableMultipliers,
        m_stdset_floatAvailableMultipliers.size(),
        fMultiplier);
  }

  /** Define a function because of this problem:
  * the calculated float value was 1.0999999
  * the compared float value was 1.1000000
  * both values belong the SAME voltage ID, just a little rounding error. */
  float GetClosestVoltage(float fVoltageInVolt)
  {
    WORD index = GetIndexForClosestVoltage(fVoltageInVolt);
    if( index != //MAXWORD
        USHRT_MAX)
      return m_arfAvailableVoltagesInVolt[index];
    return 0.0f;
  }
  /** unsigned=same bit width as CPU arch=fastest data type */
  fastestUnsignedDataType GetIndexForClosestVoltage(const float) const;
  const VoltageAndFreq * GetClosestHigherVoltageAndFreq(
    const std::set<VoltageAndFreq> & stdsetvoltageandfreq,
    float fMultiplier) const;
  const VoltageAndFreq * GetClosestLowerVoltageAndFreq(
    const std::set<VoltageAndFreq> & stdsetvoltageandfreq,
    float fMultiplier) const;
  const VoltageAndFreq * GetClosestLowerVoltageAndFreq(
    float fMultiplier) const
  {
    const VoltageAndFreq * p_voltageandfreqLower =
      GetClosestLowerVoltageAndFreq(
      m_stdsetvoltageandfreqLowestStable, fMultiplier);
    if( ! p_voltageandfreqLower)
      p_voltageandfreqLower = GetClosestLowerVoltageAndFreq(
        m_stdsetvoltageandfreqWanted, fMultiplier);
    if( ! p_voltageandfreqLower)
      p_voltageandfreqLower = GetClosestLowerVoltageAndFreq(
        m_stdsetvoltageandfreqDefault, fMultiplier);
    return p_voltageandfreqLower;
  }
  float GetLowerMultiplier( float fMulti ) ;
  float GetNextLowerThrottleLevel(unsigned coreID);
  float GetNextVoltageAbove(float fVoltageInVolt);
  float GetMinimumMultiplier();
  //Can't be inline, else g++ warning
  // "undefined reference to `CPUcoreData::GetMaximumMultiplier()'"
  // if called from class MainFrame
  //inline
  float GetMaximumMultiplier() ;
  //inline
  float GetMinimumVoltage() ;
  float GetStartVoltageForHighestMultiplierForFindingLowestCPUcoreVoltage(
    float & fMultiplier) const;
  float GetStartVoltageForLowestMultiplierForFindingLowestCPUcoreVoltage(
    float & fMultiplier) const;
  float GetStartVoltageForMultiplierForFindingLowestCPUcoreVoltage(
  //      float & fPercentOfMinAndMaxMultiplier
    const float fHighestMultiplierWhereInstabilityCouldntBeReached,
    const float fLowestMultiplierWhereInstabilityWasReached,
    float & fMultiplier
    ) const;
  float GetVoltageInVolt(WORD wVoltageInVoltIndex) ;
  void PossiblyReleaseMemForCoreNumAffectedData() ;
  void Init() ;
  void InterpolateDefaultVoltages();
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
  void SetCPUcoreNumber(BYTE byNumberOfCPUcores) ;
  void SetMaxFreqInMHz(WORD wMaxFreqInMHz) ;
  void ThreadFinishedAccess() ;
#endif //#ifdef COMPILE_AS_EXECUTABLE
};
