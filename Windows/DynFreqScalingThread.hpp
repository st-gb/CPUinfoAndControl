/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard
#ifndef WINDOWS_API_DYNFREQSCALINGTHREAD_HPP
#define WINDOWS_API_DYNFREQSCALINGTHREAD_HPP

//#include "../Controller/ICPUcoreUsageGetter.hpp"
#include <Controller/DynFreqScalingThreadBase.hpp> //Base class for this class.
#include <windef.h> //for DWORD

class ICPUcoreUsageGetter ;

namespace Windows_API
{
  class DynFreqScalingThread
    : public DynFreqScalingThreadBase
  {
    HANDLE m_handleThread ;
  public:
    //float m_fPercentileIncrease ;
    //WORD m_wCurrentFreqInMHz ;
    //ICPUcoreUsageGetter * mp_icpucoreusagegetter;
    //DynFreqScalingThread( WORD wMaxFreqInMHz )
    //  : m_wMaxFreqInMHz( wMaxFreqInMHz )
    //  , m_fPercentileIncrease(1.5f)
    //  , m_bSuccFullyGotPStateFromMSR(true)
    //{
    //}
//    DynFreqScalingThread(
//      ICPUcoreUsageGetter * p_icpu
//      , I_CPUcontroller * p_cpucontroller
//      , CPUcoreData & r_cpucoredata
//      ) ;
    DynFreqScalingThread(
      CPUcontrolBase & r_cpucontrolbase
      , CPUcoreData & r_cpucoredata
      ) ;
    //"virtual" because else g++ "warning: `class Windows_API::
    // DynFreqScalingThread' has virtual functions but non-virtual destructor"
    virtual ~DynFreqScalingThread() ;
    //int Run() ;
    DWORD Run() ;
    //void Start() ;
    //BYTE Start() ;
    DWORD Start() ;
    void * WaitForTermination() ;
    //void SetMembers(
    //  ICPUcoreUsageGetter * p_icpucoreusagegetter,
    //  ) ;
  };
};
#endif //#ifndef WINDOWS_API_DYNFREQSCALINGTHREAD_HPP
