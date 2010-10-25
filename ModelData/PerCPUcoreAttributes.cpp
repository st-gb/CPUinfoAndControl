/*
 * PerCPUcoreAttributes.cpp
 *
 *  Created on: May 19, 2010
 *      Author: Stefan
 */

#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependency on this class for dyn libs.
  #include <wxWidgets/DynFreqScalingThread.hpp>
#endif
#include "PerCPUcoreAttributes.hpp"

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  extern CPUcontrolBase * gp_cpucontrolbase ;
#endif //#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB

#ifdef COMPILE_WITH_LOG
extern Logger g_logger ;
#endif //#ifdef COMPILE_WITH_LOG

PerCPUcoreAttributes::PerCPUcoreAttributes()
    :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependence on this class for dyn libs.
    mp_dynfreqscalingthread(NULL)
  ,
#endif
  m_fPreviousCPUusage(-1.0)
  , mp_icpucoreusagegetter(NULL)
  , m_wCurrentFreqInMHz (0)
  {
  }
  PerCPUcoreAttributes::~PerCPUcoreAttributes()
  {
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependence on this class for dyn libs.
    if( mp_dynfreqscalingthread )
      delete mp_dynfreqscalingthread ;
#endif
    if( mp_icpucoreusagegetter )
      delete mp_icpucoreusagegetter ;
  }

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependence on this class for dyn libs.
  void PerCPUcoreAttributes::CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter
    )
  {
//    DEBUGN
    LOGN("PerCPUcoreAttributes::CreateDynFreqScalingThread" <<
      "mp_dynfreqscalingthread:" << mp_dynfreqscalingthread )
    mp_icpucoreusagegetter = p_icpucoreusagegetter  ;
    if ( ! mp_dynfreqscalingthread )
    {
    //http://docs.wxwidgets.org/2.6/wx_wxthread.html:
    //"[...]"This means, of course, that all detached threads must be
    //created on the heap because the thread will call delete this;
    //upon termination.[...]"
      mp_dynfreqscalingthread = new //DynFreqScalingThread(
        wxWidgets::DynFreqScalingThread(
//        mp_icpucoreusagegetter
//        , mp_cpucontroller
        * gp_cpucontrolbase
        , * mp_cpucoredata
        //DETACHED is default, worked with GUI, not with daemon
        , wxTHREAD_JOINABLE
        );
      //mp_dynfreqscalingthread->
      if( mp_dynfreqscalingthread->Create() == wxTHREAD_NO_ERROR )
      {
        LOGN("Dynamic Voltage and Frequency Scaling thread successfully "
          "created")
//Prevent unused variable error for "wxthreaderror" if not compiled
//with logging.
#ifdef COMPILE_WITH_LOG
        wxThreadError wxthreaderror = mp_dynfreqscalingthread->Run() ;
#endif //#ifdef COMPILE_WITH_LOG
        LOGN("after starting Dynamic Voltage and Frequency Scaling thread"
          "--result: " << wxthreaderror )
      }
      else
      {
        LOGN("creating Dynamic Voltage and Frequency Scaling thread failed")
      }
    }
  }
#endif

  //when this class is an element of an array, the paramless ctor is
  //called?! So do the init with params here.
  void PerCPUcoreAttributes::Create(
    BYTE byCoreID
    //ICPUcoreUsageGetter * p_icpucoreusagegetter
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    )
  {
    m_byCoreID = byCoreID ;
    mp_cpucontroller = p_cpucontroller ;
    //mp_icpucoreusagegetter = p_icpucoreusagegetter  ;
    mp_cpucoredata = & r_cpucoredata ;
  }

  float PerCPUcoreAttributes::GetFreqInMHz()
  {
    return m_fMultiplier * m_fReferenceClockInMhz ;
  }
