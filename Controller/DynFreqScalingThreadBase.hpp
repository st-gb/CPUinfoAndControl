#pragma once //Include guard.
#ifndef DYNFREQSCALINGTHREADBASE_HPP
  #define DYNFREQSCALINGTHREADBASE_HPP

//#include <windef.h> //for WORD
//#include <ModelData/PerCPUcoreAttributes.hpp>
#include <preprocessor_macros/Windows_compatible_typedefs.h> //WORD
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
typedef void *ExitCode;

//Forward declarations (faster than #include)
class CPUcontrolBase ;
class CPUcoreData ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;
class PerCPUcoreAttributes ;

//code that both the wxWidgets thread and the WINAPI thread use.
class DynFreqScalingThreadBase
{
//private:
  //"protected" to inherit member attributes
protected:
  bool m_b1stTimeInRowTooHot ;
  bool m_bGotCPUcoreDataAtLeast1Time ;
  CPUcoreData * mp_cpucoredata ;
  CPUcontrolBase & mr_cpucontrolbase ;
  DWORD m_dwBeginInMillisOfTooHot ;
  float * m_ar_fPreviousTemperaturesInDegCelsius ;
  float * m_ar_fPreviousMinusCurrentTemperature ;
  float m_fVoltage ;
  float m_fPercentileIncrease ;
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpu ;
  PerCPUcoreAttributes * m_arp_percpucoreattributes ;
//  bool m_bCalledInit ;
  //DWORD m_dwMSRLow,m_dwMSRHigh ;
  WORD m_wMilliSecondsPassed ;
  WORD m_wMilliSecondsToWaitForCoolDown ;
//  WORD m_wMilliSecondsToWait ;
  WORD m_wNumCPUcores ;
  void ChangeOperatingPointByLoad( 
    BYTE byCoreID 
    , float fLoad 
    ) ;
public:
  enum ThreadStartReturnCode {
    THREAD_START_NO_ERROR = 0,      // No error
    THREAD_START_NO_RESOURCE,       // No resource left to create a new thread
    THREAD_START_ALREADY_RUNNING,           // The thread is already running
    THREAD_START_NOT_RUNNING,       // The thread isn't running
    THREAD_START_KILLED,            // Thread we waited for had to be killed
    THREAD_START_MISC_ERROR
  };
  //volatile is important because more than 1 thread may access this variable
  volatile bool m_vbRun ;
  volatile bool m_vbDVFSthreadStopped ;
//  DynFreqScalingThreadBase(
//    ICPUcoreUsageGetter * p_icpu
//    , I_CPUcontroller * p_cpucontroller
//    , CPUcoreData & r_cpucoredata
//    ) ;
  DynFreqScalingThreadBase(
    CPUcontrolBase & r_cpucontrolbase
    , CPUcoreData & r_cpucoredata
    ) ;
  //"virtual" because else g++: "warning: `class DynFreqScalingThreadBase'
  // has virtual functions but non-virtual destructor"
  virtual ~DynFreqScalingThreadBase()
  {
    if( m_ar_fPreviousTemperaturesInDegCelsius )
      delete m_ar_fPreviousTemperaturesInDegCelsius ;
    if( m_ar_fPreviousMinusCurrentTemperature )
      delete m_ar_fPreviousMinusCurrentTemperature ;
  }
  inline void CalcDiffBetweenCurrentAndPreviousTemperature() ;
  ExitCode Entry() ;
  //implement in the API of the concrete thread class.
  virtual bool IsRunning() const { return false ; }
  //"virtual" because it may be overwritten.
  virtual bool IsStopped() ; //{} ;
  BYTE GetCPUcoreWithHighestLoad( float & fHighestCPUcoreLoadInPercent) ;
  inline void SafePreviousTemperatures() ;
  void SignalCPUdataCanBeSafelyRead() ;
  void SetLowerFrequencyFromAvailableMultipliers(
    BYTE byCoreID ,
    PerCPUcoreAttributes * arp_percpucoreattributes ,
    float ar_fCPUcoreLoadInPercent [] ,
    float fFreqInMHz,
//    WORD wNumCPUcores ,
    float fLowerMultiplier ,
    float fNextMultiplierCalculatedFromCurrentLoad
    ) ;
  //virtual int Run() = 0 ;
  //Return value for the subclass that is also inherited by "wxThread". 
  //So a wxThreadError can be returned to the caller to check for errors.
  virtual //BYTE
    //Use DWORD because of GetLastError(...) return code.
    DWORD Start() ;
  void Stop() ;
  virtual void * WaitForTermination()
  {
    LOGN("DynFreqScalingThreadBase::WaitForTermination")
    return 0 ;
  }
};

namespace DynFreqScalingThreadBaseNameSpace
{
  DWORD //__stdcall is important for Windows' ::CreateThread()
  //Built-in preprocessor macro for MSVC, MinGW (also for 64 bit)
  #ifdef _WIN32 //under Linux g++ error if "__stdcall"
    __stdcall
  #endif
    ThreadFunction( void * pv ) ;
}

#endif //DYNFREQSCALINGTHREADBASE_HPP
