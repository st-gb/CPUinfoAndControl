/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#ifndef WXWIDGETS_DYNFREQSCALINGTHREAD_HPP
#define WXWIDGETS_DYNFREQSCALINGTHREAD_HPP

#include <wx/thread.h> //because base class wxThread
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <windef.h> //DWORD
//#include <math.h> //for log2()
//#include <cmath>
#include <Controller/DynFreqScalingThreadBase.hpp>//because base class
//#include <ModelData/CPUcoreData.hpp>

//forward declaration (faster than #include)
class CPUcoreData ;

namespace wxWidgets
{
  class DynFreqScalingThread
    : public wxThread
    , public DynFreqScalingThreadBase
  {
  public:
//    DynFreqScalingThread(
//      ICPUcoreUsageGetter * p_icpu
//      , I_CPUcontroller * p_cpucontroller
//      , CPUcoreData & r_cpucoredata
//      ) ;
    DynFreqScalingThread(
      CPUcontrolBase & r_cpucontrolbase
      , CPUcoreData & r_cpucoredata
      , wxThreadKind kind = wxTHREAD_DETACHED
      ) ;
    //void ChangeOperatingPointByLoad(
    //  BYTE byCoreID
    //  , float fLoad
    //  ) ;

    //http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadentry:
    //"This is the entry point of the thread. This function is pure virtual
    //and must be implemented by any derived class.
    //The thread execution will start here."
    wxThread::ExitCode Entry()
    {
      wxThread::ExitCode exitcode = DynFreqScalingThreadBase::Entry() ;
      LOGN("The Dynamic Voltage and Frequency Scaling thread ends now.")
      return exitcode ;
    }
    //BYTE Start() ;
    DWORD Start() ;
    void * WaitForTermination()
    {
      return Wait() ;
    }
  };
}
#endif //#ifndef WXWIDGETS_DYNFREQSCALINGTHREAD_HPP
