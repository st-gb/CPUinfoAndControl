#pragma once //include guard

//#include "../Controller/ICPUcoreUsageGetter.hpp"
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <global.h> //for WORD

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
    //  : m_wMaxFreqInMHz(//2200
    //    wMaxFreqInMHz )
    //  , m_fPercentileIncrease(1.5f)
    //  , m_bSuccFullyGotPStateFromMSR(true)
    //{
    //}
    DynFreqScalingThread(
      ICPUcoreUsageGetter * p_icpu
      , I_CPUcontroller * p_cpucontroller
      , CPUcoreData & r_cpucoredata
      ) ;
    //void Start() ;
    //int Run() ;
    DWORD Run() ;
    //BYTE Start() ;
    DWORD Start() ;
    //void SetMembers(
    //  ICPUcoreUsageGetter * p_icpucoreusagegetter,
    //  GriffinController * p_pumastatectrl
    //  ) ;
  };
};
