#pragma once //include guard

#include "../global.h" //for BYTE, WORD
#include "VoltageAndFreq.hpp" //class MaxVoltageForFreq
#include <set>
//because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208: 
//   'void': Keine Elemente definiert, die diesen Typ verwenden
//http://trac.wxwidgets.org/ticket/11482:
//if you include <windows.h> you must include <wx/msw/winundef.h> after it.
#ifdef _MSC_VER //Microsoft compiler
#include <wx/msw/winundef.h>
#endif
#include <wx/thread.h> //for class wxCriticalSection

#define CPU_CORE_DATA_NOT_SET 255
//#include <Controller/ClocksNotHaltedCPUcoreUsageGetter.hpp>
//#include <Controller/GriffinController.hpp>
//#include <wxWidgets/DynFreqScalingThread.hpp>

class CPUcoreData ;
//class GriffinController ;
class I_CPUcontroller ;
class DynFreqScalingThread ;
class ICPUcoreUsageGetter ;

class PerCPUcoreAttributes
{
private :
  BYTE m_byCoreID ;
  WORD m_wCurrentFreqInMHz ;
  CPUcoreData * mp_cpucoredata ;
  //GriffinController * mp_griffincontroller ;
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ; 
public:
  float m_fPreviousCPUusage ;
  float m_fVoltageInVoltCalculatedFromCPUload ;
  WORD m_wFreqInMHzCalculatedFromCPUload ;
  DynFreqScalingThread * mp_dynfreqscalingthread ;

  PerCPUcoreAttributes() ;
  ~PerCPUcoreAttributes() ;
  void CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter 
    ) ;

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
  float m_fPercentalCPUcoreFreqIncrease ;
  float m_fVoltageForMaxCPUcoreFreq ;
  float m_fThrottleTemp ;
  //GriffinController * mp_griffincontroller ;
  I_CPUcontroller * mp_cpucontroller ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqAvailableFreq ;
  std::set<VoltageAndFreq> m_stdsetvoltageandfreqWanted ;
  std::set<VoltageAndFreq> m_setloweststablevoltageforfreq ;
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
  wxCriticalSection m_wxcriticalsection ;

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
