/*
 * PerCPUcoreAttributes.cpp
 *
 *  Created on: May 19, 2010
 *      Author: Stefan
 */

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependency on this class for dyn libs.
  #include <wxWidgets/DynFreqScalingThread.hpp>
#endif
#include <Controller/ICPUcoreUsageGetter.hpp>

#ifdef COMPILE_WITH_LOG
extern Logger g_logger ;
#endif //#ifdef COMPILE_WITH_LOG

PerCPUcoreAttributes::PerCPUcoreAttributes()
    :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
    mp_dynfreqscalingthread(NULL)
  ,
#endif
  m_fPreviousCPUusage(-1.0)
  , mp_icpucoreusagegetter(NULL)
  , m_wCurrentFreqInMHz (0)
  {
    //mp_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
    //  m_byCoreID
    //  ,
    //  ) ;
  }
  PerCPUcoreAttributes::~PerCPUcoreAttributes()
  {
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
    if( mp_dynfreqscalingthread )
      delete mp_dynfreqscalingthread ;
#endif
    if( mp_icpucoreusagegetter )
      delete mp_icpucoreusagegetter ;
  }

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
  void PerCPUcoreAttributes::CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter
    )
  {
    DEBUGN("PerCPUcoreAttributes::CreateDynFreqScalingThread" <<
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
        mp_icpucoreusagegetter
        //, mp_griffincontroller
        , mp_cpucontroller
        , *mp_cpucoredata
        );
      //mp_dynfreqscalingthread->
      if( mp_dynfreqscalingthread->Create() == wxTHREAD_NO_ERROR )
      {
        LOGN("Dynamic Voltage and Frequency Scaling thread successfully "
          "created")
        wxThreadError wxthreaderror = mp_dynfreqscalingthread->Run() ;
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
    //, GriffinController * p_griffincontroller
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    )
  {
    m_byCoreID = byCoreID ;
    //mp_griffincontroller = p_griffincontroller ;
    mp_cpucontroller = p_cpucontroller ;
    //mp_icpucoreusagegetter = p_icpucoreusagegetter  ;
    mp_cpucoredata = & r_cpucoredata ;
    //mp_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
    //  m_byCoreID
    //  , mp_griffincontroller
    //  ) ;
  }
