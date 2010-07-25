#pragma once

//#include <windef.h> //WORD
//#include <Windows.h> //for DWORD, WINAPI etc.
#include <ModelData/PerCPUcoreAttributes.hpp>
#include <Windows_compatible_typedefs.h>
typedef void *ExitCode;

class CPUcoreData ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;
class PerCPUcoreAttributes ;

//code that both the wxWidgets thread and the WINAPI thread use.
class DynFreqScalingThreadBase
{
//private:
  //"protected" to inherit member atts
protected:
  CPUcoreData * mp_cpucoredata ;
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
  DynFreqScalingThreadBase(  
    ICPUcoreUsageGetter * p_icpu
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
  ExitCode Entry() ;
  //implement in the API of the concrete thread class.
  virtual bool IsRunning() const { return false ; }
  //"virtual" because it may be overwritten.
  virtual bool IsStopped() ; //{} ;
  BYTE GetCPUcoreWithHighestLoad( float & fHighestCPUcoreLoadInPercent) ;
  void SignalCPUdataCanBeSafelyRead() ;
  void SetLowerFrequencyFromAvailableMultipliers(
    BYTE byCoreID ,
    PerCPUcoreAttributes * arp_percpucoreattributes ,
    float ar_fCPUcoreLoadInPercent [] ,
    float fFreqInMHz,
    WORD wNumCPUcores ,
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
};
