#pragma once //include guard

//#ifndef COMPILE_WITHOUT_IWBEMSERVICES
//#include "../Controller/ICPUcoreUsageGetter.hpp"
//#include "../Controller/PumaStateCtrl.h"
//#include "../Controller/GriffinController.hpp"
#include <Controller/DynFreqScalingThreadBase.hpp>
#include "../global.h" //for WORD

class ICPUcoreUsageGetter ;
//class PumaStateCtrl ;
class GriffinController ;

namespace Windows_API
{
  class DynFreqScalingThread
    : public DynFreqScalingThreadBase
  {
  public:
    //bool m_bSuccFullyGotPStateFromMSR ;
    //float m_fPercentileIncrease ;
    //float m_arfCPUcorePercentage[2] ;
    //WORD m_wCurrentFreqInMHz ;
    //WORD m_wAQuarterOfMaxFreq ;
    //WORD m_wAHalfOfMaxFreq ;
    //WORD m_wMaxFreqInMHz ;
    //ICPUcoreUsageGetter * mp_icpucoreusagegetter;
    ////PumaStateCtrl * mp_pumastatectrl ;
    //GriffinController * mp_pumastatectrl ;
    //DynFreqScalingThread( WORD wMaxFreqInMHz )
    //  : m_wMaxFreqInMHz(//2200
    //    wMaxFreqInMHz )
    //  , m_fPercentileIncrease(1.5f)
    //  , m_bSuccFullyGotPStateFromMSR(true)
    //{
    //  m_wAQuarterOfMaxFreq = m_wMaxFreqInMHz / 4 ;
    //  m_wAHalfOfMaxFreq = m_wMaxFreqInMHz / 2 ;
    //}
    DynFreqScalingThread(
      ICPUcoreUsageGetter * p_icpu
      //, GriffinController * p_pumastatectrl
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
    //  //PumaStateCtrl * p_pumastatectrl
    //  GriffinController * p_pumastatectrl
    //  ) ;
  };
};
//#endif //#ifndef COMPILE_WITHOUT_IWBEMSERVICES
