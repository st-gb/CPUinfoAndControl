#pragma once //include guard

//#include <global.h> //for BYTE, WORD
#include <windef.h> //for BYTE
#include "VoltageAndFreq.hpp" //class VoltageAndFreq
//CPU controller (dyn libs) do not need thread synchronisation yet.
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  #include <Controller/multithread/condition_type.hpp>
  #include <Controller/multithread/criticalsection_type.hpp>
  #include <Controller/multithread/mutex_type.hpp>
#endif //#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
//#include <ModelData/PerCPUcoreAttributes.hpp>
//CPU controller (dyn libs) do not need thread synchronisation yet.
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
//  #include <Windows/multithread/Win32EventBasedCondition.hpp>
    //Keep away the dependence on mp_dynfreqscalingaccess for dyn libs.
//  #include <wxWidgets/DynFreqScalingThread.hpp>
//  #include <wx/thread.h> //for wxCondition
#endif
#include <map> //class std::map
#include <set> //class std::set
#include <string> //std::string

#define CPU_CORE_DATA_NOT_SET 255
//#include <Controller/ClocksNotHaltedCPUcoreUsageGetter.hpp>
//#include <wxWidgets/DynFreqScalingThread.hpp>

//using namespace wxWidgets ;

//Forward declarations (faster than #include)
class I_CPUcontroller ;
class MaxVoltageForFreq ;
class PerCPUcoreAttributes ;

//The member variables can be shared through the subclass of 
//ICPUcoreUsageGetter and the user interface class.
class CPUcoreData
{
private:
public:
  bool m_b1CPUcorePowerPlane ;
  bool m_bEnableDVFS ;
  bool m_bTooHot ;
  BYTE m_byUpdateViewOnDVFS ;
  BYTE m_byNumberOfCPUCores ;
  BYTE m_byMaxVoltageID ; //=lowest voltage
  BYTE m_byMinVoltageID ;
  BYTE m_byModel ;
  BYTE m_byStepping ;
  float * m_arfAvailableMultipliers ;
  float * m_arfAvailableVoltagesInVolt ;
  float * m_arfCPUcoreLoadInPercent ;
  float m_fCPUcoreLoadThresholdForIncreaseInPercent;
  float m_fCPUcoreFreqFactor ;
  float m_fCPUcoreFreqIncreaseFactor ;
  float m_fVoltageForMaxCPUcoreFreq ;
  float m_fThrottleTempInDegCelsius ;
  I_CPUcontroller * mp_cpucontroller ;
  std::map<float,VoltageAndFreq> m_stdmap_fMultiplier2voltageandfreq ;
  std::set<float> m_stdset_floatAvailableMultipliers ;
  std::set<float> m_stdset_floatAvailableVoltagesInVolt ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqAvailableFreq ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqWanted ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqLowestStable ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqDefault ;
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
  WORD m_wFamily ;
//CPU controller (dyn libs) do not need thread synchronisation yet.
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
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
  bool AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz) ;
  void AddPreferredVoltageForFreq(float fValue,WORD wFreqInMHz) ;
  //void AddFreqAndLowestStableVoltage(float fValue,WORD wFreqInMHz) ;
  void AddLowestStableVoltageAndFreq(float fValue,WORD wFreqInMHz) ;
  void AvailableMultipliersToArray() ;
  void AvailableVoltagesToArray() ;
  void ClearCPUcontrollerSpecificAtts() ;
  CPUcoreData() ;
  CPUcoreData(BYTE byNumberOfCPUcores, WORD wMaxFreqInMHz) ;
  ~CPUcoreData() ;

  WORD GetIndexForClosestMultiplier(float fMultiplier) ;
  BYTE GetIndexForClosestVoltage(float) ;
  float GetLowerMultiplier( float fMulti ) ;
  BYTE GetNumberOfCPUcores() ;
  //Can't be inline, else g++ warning
  // "undefined reference to `CPUcoreData::GetMaximumMultiplier()'"
  // if called from class MainFrame
  //inline
  float GetMaximumMultiplier() ;
  //inline
  float GetMinimumVoltage() ;
  float GetVoltageInVolt(WORD wVoltageInVoltIndex) ;
  void PossiblyReleaseMemForCoreNumAffectedData() ;
  void Init() ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
  void SetCPUcoreNumber(BYTE byNumberOfCPUcores) ;
  void SetMaxFreqInMHz(WORD wMaxFreqInMHz) ;
  void ThreadFinishedAccess() ;
};
