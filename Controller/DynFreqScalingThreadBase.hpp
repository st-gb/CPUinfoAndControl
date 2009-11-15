#pragma once

//#include <windef.h> //WORD
#include <Windows.h> //for DWORD, WINAPI etc.
typedef void *ExitCode;

class I_CPUcontroller ;
class CPUcoreData ;
class ICPUcoreUsageGetter ;

//code that both the wxWidgets thread and the WINAPI thread use.
class DynFreqScalingThreadBase
{
//private:
  //"protected" to inherit member atts
protected:
  CPUcoreData * mp_cpucoredata ;
  //GriffinController * mp_pumastatectrl ;
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpu ;
  //bool m_bSuccFullyGotPStateFromMSR ;
  bool m_bCalledInit ;
  //DWORD m_dwMSRLow,m_dwMSRHigh ;
  float m_arf[2] ;
  float m_fVoltage ;
  float m_fPercentileIncrease ;
  WORD m_wCurrentFreqInMHz ;
  WORD m_wAHalfOfMaxFreq ;
  WORD m_wAQuarterOfMaxFreq ;
  WORD m_wMaxFreqInMHz ;
  WORD m_wMilliSecondsToWait ;
  void ChangeOperatingPointByLoad( 
    BYTE byCoreID 
    , float fLoad 
    ) ;
public:
  //volatile is important because more than 1 thread may access this variable
  volatile bool m_vbRun ;
  DynFreqScalingThreadBase(  
    ICPUcoreUsageGetter * p_icpu
    //, GriffinController * p_pumastatectrl
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
  ExitCode Entry() ;
  //virtual int Run() = 0 ;
  virtual BYTE Start() = 0 ;
  void Stop() ;
};
