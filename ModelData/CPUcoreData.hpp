#pragma once //include guard

#include "../global.h" //for BYTE, WORD
#include "VoltageAndFreq.hpp" //class MaxVoltageForFreq
#include <set>
#include <string> //std::string
//#ifdef __WXMSW__
////because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208:
////   'void': Keine Elemente definiert, die diesen Typ verwenden
////http://trac.wxwidgets.org/ticket/11482:
////if you include <windows.h> you must include <wx/msw/winundef.h> after it.
//#ifdef _MSC_VER //Microsoft compiler
//#include <wx/msw/winundef.h>
//#endif
//#include <wx/thread.h> //for class wxCriticalSection
//typedef wxCriticalSection criticalsection_type ;
//#else
#include <criticalsection_type.hpp>
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on mp_dynfreqscalingaccess for dyn libs.
#include <wxWidgets/DynFreqScalingThread.hpp>
#endif
//#endif

#define CPU_CORE_DATA_NOT_SET 255
//#include <Controller/ClocksNotHaltedCPUcoreUsageGetter.hpp>
//#include <Controller/GriffinController.hpp>
//#include <wxWidgets/DynFreqScalingThread.hpp>

//using namespace wxWidgets ;

class CPUcoreData ;
//class GriffinController ;
class I_CPUcontroller ;
//class wxWidgets::DynFreqScalingThread ;
class ICPUcoreUsageGetter ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
namespace wxWidgets
{
  class DynFreqScalingThread ;
}
#endif

class PerCPUcoreAttributes
{
//Sort the attributes  lexicographically for simplification of:
//Initialize in the same order as textual in the declaration?
//(to avoid g++ warnings)
private :
  BYTE m_byCoreID ;
  CPUcoreData * mp_cpucoredata ;
public:
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
  wxWidgets::DynFreqScalingThread * mp_dynfreqscalingthread ;
#endif
  float m_fPreviousCPUusage ;
  float m_fVoltageInVoltCalculatedFromCPUload ;
private:
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ;
  WORD m_wCurrentFreqInMHz ;
public:
  WORD m_wFreqInMHzCalculatedFromCPUload ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
  void CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter 
    ) ;
#endif

  PerCPUcoreAttributes() ;
  ~PerCPUcoreAttributes() ;

  //when this class is an element of an array, the paramless ctor is
  //called?! So do the init with params here.
  void Create(
    BYTE byCoreID
    //ICPUcoreUsageGetter * p_icpucoreusagegetter 
    //, GriffinController * p_griffincontroller 
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
} ;

//class GriffinController ;
class I_CPUcontroller ;
class MaxVoltageForFreq ;

//The member variables can be shared through the subclass of 
//ICPUcoreUsageGetter and the user interface class.
class CPUcoreData
{
private:
  //this Frequency IDentifier determines the max frequency (at Divisor ID 0)
  BYTE m_byMainPLLoperatingFrequencyIDmax ; 
public:
  bool m_b1CPUcorePowerPlane ;
  bool m_bEnableDVFS ;
  BYTE m_byUpdateViewOnDVFS ;
  BYTE m_byLowestEffectiveFreqID ;
  BYTE m_byNumberOfCPUCores ;
  BYTE m_byMaxVoltageID ; //=lowest voltage
  BYTE m_byMinVoltageID ;
  BYTE m_byModel ;
  BYTE m_byStepping ;
  float * m_arfCPUcoreLoadInPercent ;
  float m_fCPUcoreLoadThresholdForIncreaseInPercent;
  float m_fCPUcoreFreqIncreaseFactor ;
  float m_fVoltageForMaxCPUcoreFreq ;
  float m_fThrottleTempInDegCelsius ;
  //GriffinController * mp_griffincontroller ;
  I_CPUcontroller * mp_cpucontroller ;
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
  //wxCriticalSection m_wxcriticalsection ;
  criticalsection_type m_wxcriticalsection ;

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
  void ClearCPUcontrollerSpecificAtts() ;
  CPUcoreData() ;
  CPUcoreData(BYTE byNumberOfCPUcores, WORD wMaxFreqInMHz) ;
  ~CPUcoreData() ;

  BYTE GetNumberOfCPUcores() ;
  void PossiblyReleaseMem() ;
  BYTE GetMainPLLoperatingFrequencyIDmax(
    ) ;
  void Init() ;
  //void SetGriffinController(
  //  GriffinController * p_griffincontroller )
  //{
  //  mp_griffincontroller = p_griffincontroller ;
  //}
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
  void SetMainPLLoperatingFrequencyIDmax(
    BYTE byMainPLLoperatingFrequencyIDmax) ;
  void SetCPUcoreNumber(BYTE byNumberOfCPUcores) ;
  void SetMaxFreqInMHz(WORD wMaxFreqInMHz) ;
};
