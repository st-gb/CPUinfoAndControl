// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include <wx/thread.h> //for ExitCode
//#include <math.h> //for log2()
#include "DynFreqScalingThread.hpp"

//Is already declared in class WXDLLIMPEXP_BASE wxThread but
//curiously needs to be defined here.
typedef void * ExitCode;

class CPUcoreData ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;

using namespace wxWidgets ;

//"[...]It does not create or start execution of the real thread -- 
//for this you should use the Create and Run methods.[...]"
//DynFreqScalingThread::DynFreqScalingThread(
//    ICPUcoreUsageGetter * p_icpu
//    , I_CPUcontroller * p_cpucontroller
//    , CPUcoreData & r_cpucoredata
//    )
////    //, m_bCalledInit(false)
////    //, mp_cpucoredata(&r_cpucoredata)
////    //, m_vbRun(true)
////    //, m_wMilliSecondsToWait(600)
////    //, mp_i_cpucontroller ( p_cpucontroller )
//  : DynFreqScalingThreadBase(
//      p_icpu
//      , p_cpucontroller
//      , r_cpucoredata
//      )
//{
////    //DEBUG("constructor of freq scaling thread--begin\n");
////    //mp_icpu = p_icpu ;
////    //LOGN("core usage address: " << p_icpu )
////    ////mp_icpu->Init();
////    //m_wMaxFreqInMHz = r_cpucoredata.m_wMaxFreqInMHz ;
////    //m_fPercentileIncrease = 1.5f ;
////    //mp_i_cpucontroller = p_cpucontroller ;
////    //I_CPUcontroller
////    DEBUG("constructor of freq scaling thread--end\n");
//}

DynFreqScalingThread::DynFreqScalingThread(
  CPUcontrolBase & r_cpucontrolbase
  , CPUcoreData & r_cpucoredata
  )
  : DynFreqScalingThreadBase(
    r_cpucontrolbase
    , r_cpucoredata
  )
{
}

//BYTE
DWORD DynFreqScalingThread::Start()
{
  BYTE byRet = 0 ;
  //Call the base class' method that does the work needed for subclasses that
  //(re-)start a DVFS thread.
  DynFreqScalingThreadBase::Start() ;
  byRet =
    //calls the Run() method of the base class "wxThread".
    //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadrun:
    //"Starts the thread execution. Should be called after Create.
    //This function can only be called from another thread context."
    Run() ;
  switch( byRet )
  {
  case wxTHREAD_NO_ERROR ://= 0,      // No error
    byRet = DynFreqScalingThreadBase::THREAD_START_NO_ERROR ;
    break ;
  case wxTHREAD_NO_RESOURCE : // No resource left to create a new thread
    byRet = DynFreqScalingThreadBase::THREAD_START_NO_RESOURCE ;
    break ;
  case wxTHREAD_RUNNING : // The thread is already running
    byRet = DynFreqScalingThreadBase::THREAD_START_ALREADY_RUNNING ;
    break ;
  case wxTHREAD_NOT_RUNNING: // The thread isn't running
    byRet = DynFreqScalingThreadBase::THREAD_START_NOT_RUNNING ;
    break ;
  case wxTHREAD_KILLED : // Thread we waited for had to be killed
    byRet = DynFreqScalingThreadBase::THREAD_START_KILLED ;
    break ;
  case wxTHREAD_MISC_ERROR :
    byRet = DynFreqScalingThreadBase::THREAD_START_MISC_ERROR ;
    break ;
  }
  return byRet ;
}
  ////http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadentry:
  ////"This is the entry point of the thread. This function is pure virtual and must be implemented by any derived class. The thread execution will start here."
  //ExitCode DynFreqScalingThread::Entry()
  //{

  //}
