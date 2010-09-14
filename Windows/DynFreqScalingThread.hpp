#pragma once //include guard
#ifndef DYNFREQSCALINGTHREAD_HPP
#define DYNFREQSCALINGTHREAD_HPP

//#include "../Controller/ICPUcoreUsageGetter.hpp"
#include <Controller/DynFreqScalingThreadBase.hpp> //Base class for this class.
#include <windef.h> //for DWORD

class ICPUcoreUsageGetter ;

namespace Windows_API
{
  class DynFreqScalingThread
    : public DynFreqScalingThreadBase
  {
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
    //int Run() ;
    DWORD Run() ;
    //void Start() ;
    //BYTE Start() ;
    DWORD Start() ;
    //void SetMembers(
    //  ICPUcoreUsageGetter * p_icpucoreusagegetter,
    //  ) ;
  };
};
#endif //#ifndef DYNFREQSCALINGTHREAD_HPP
