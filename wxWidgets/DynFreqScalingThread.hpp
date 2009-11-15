#pragma once //incl. guard

#include <wx/thread.h>
//#include "../Controller/PumaStateCtrl.h"
//#include "Controller/GriffinController.hpp"
#include <Controller/DynFreqScalingThreadBase.hpp>
//#include <ModelData/CPUcoreData.hpp>
#include "../global.h"
#include <math.h> //for log2()
#include <cmath>
#include <ModelData/CPUcoreData.hpp>
#include <ModelData/PState.hpp>

class CPUcoreData ;

class DynFreqScalingThread
  : public wxThread
  , public DynFreqScalingThreadBase
{
//private:
  ////TODO remove because Griffin/ K10-specfic, an not for e.g. Pentium M
  //PState m_pstateFromMSR ;
public:
  DynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpu
    //, GriffinController * p_pumastatectrl
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
  ExitCode Entry() { return DynFreqScalingThreadBase::Entry(); }
  BYTE Start() ;
};
