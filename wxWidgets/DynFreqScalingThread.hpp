#pragma once //incl. guard

#include <wx/thread.h> //because base class wxThread
#include <global.h>
//#include <math.h> //for log2()
//#include <cmath>
#include <Controller/DynFreqScalingThreadBase.hpp>//because base class
//#include <ModelData/CPUcoreData.hpp>

//forward declaration (faster than include)
class CPUcoreData ;

namespace wxWidgets
{
  class DynFreqScalingThread
    : public wxThread
    , public DynFreqScalingThreadBase
  {
  public:
    DynFreqScalingThread(
      ICPUcoreUsageGetter * p_icpu
      , I_CPUcontroller * p_cpucontroller
      , CPUcoreData & r_cpucoredata
      ) ;
    //void ChangeOperatingPointByLoad(
    //  BYTE byCoreID
    //  , float fLoad
    //  ) ;

    //http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadentry:
    //"This is the entry point of the thread. This function is pure virtual
    //and must be implemented by any derived class.
    //The thread execution will start here."
    wxThread::ExitCode Entry() {
      wxThread::ExitCode exitcode = DynFreqScalingThreadBase::Entry() ;
      LOGN("The Dynamic Voltage and Frequency Scaling thread ends now.")
      return exitcode ;
    }
    //BYTE Start() ;
    DWORD Start() ;
  };
}
