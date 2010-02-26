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
#include <math.h> //for log2()
#include "DynFreqScalingThread.hpp"
//#include <Controller/GriffinController.hpp>

//Is already deklared in class WXDLLIMPEXP_BASE wxThread but 
//curiosly needs to be defined here.
typedef void *ExitCode;

class CPUcoreData ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;

//"[...]It does not create or start execution of the real thread -- 
//for this you should use the Create and Run methods.[...]"
DynFreqScalingThread::DynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpu
    //, GriffinController * p_pumastatectrl
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    )
//    //: m_bSuccFullyGotPStateFromMSR(false)
//    //, m_bCalledInit(false)
//    //, mp_cpucoredata(&r_cpucoredata)
//    //, m_vbRun(true)
//    //, m_wMilliSecondsToWait(600)
//    //, mp_cpucontroller ( p_cpucontroller )
  : DynFreqScalingThreadBase(
      p_icpu
      //, GriffinController * p_pumastatectrl
      , p_cpucontroller
      , r_cpucoredata
      )
{
//    //DEBUG("constructor of freq scaling thread--begin\n");
//    //mp_icpu = p_icpu ;
//    //LOGN("core usage address: " << p_icpu )
//    ////mp_icpu->Init();
//    //m_wMaxFreqInMHz = //2200
//    //  r_cpucoredata.m_wMaxFreqInMHz ;
//    ////wxTimer();
//    //m_wCurrentFreqInMHz = //550
//    //  m_wMaxFreqInMHz / 4;
//    //m_fPercentileIncrease = 1.5f ;
//    //m_wAHalfOfMaxFreq = m_wMaxFreqInMHz / 2 ;
//    //m_wAQuarterOfMaxFreq = m_wMaxFreqInMHz / 4 ;
//    ////mp_pumastatectrl = p_pumastatectrl ;
//    //mp_cpucontroller = p_cpucontroller ;
//    //I_CPUcontroller
////#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
////    if( mp_pumastatectrl->GetPStateFromMSR(//p_pstateCurrent->m_byNumber
////      2,m_dwMSRLow,m_dwMSRHigh,
////      m_pstateFromMSR,0) )
////    {
////      m_bSuccFullyGotPStateFromMSR = true ;
////    }
////#else
////    m_bSuccFullyGotPStateFromMSR = true ;
////#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
//    DEBUG("constructor of freq scaling thread--end\n");
}

BYTE DynFreqScalingThread::Start()
{
  DynFreqScalingThreadBase::Start() ;
  return 
    //calls the Run() method of the base class "wxThread".
    Run() ;
}
  ////http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadentry:
  ////"This is the entry point of the thread. This function is pure virtual and must be implemented by any derived class. The thread execution will start here."
  //ExitCode DynFreqScalingThread::Entry()
  //{

  //}
