#pragma once //incl. guard

//#include <wx/timer.h>
//#include "CPUcoreUsageGetterIWbemServices.hpp"
#include "../Controller/ICPUcoreUsageGetter.hpp"
//#include "../Controller/PumaStateCtrl.h"
#include <Controller/AMD/Griffin/GriffinController.hpp>
#include "../global.h"
#include <math.h> //for log2()
#include <cmath>
#include <wx/timer.h>

class UserInterface ;
#include "UserInterface.hpp" //for UserInterface.m_bConfirmedYet

class wxTimer ;

class wxDynFreqScalingTimer
  :public wxTimer
{
private:
  GriffinController * mp_pumastatectrl ;
  ICPUcoreUsageGetter * mp_icpu ;
  bool m_bSuccFullyGotPStateFromMSR ;
  bool m_bCalledInit ;
  DWORD m_dwMSRLow,m_dwMSRHigh ;
  float m_arf[2] ;
  float m_fVoltage ;
  float m_fPercentileIncrease ;
  WORD m_wCurrentFreqInMHz ;
  WORD m_wAHalfOfMaxFreq ;
  WORD m_wAQuarterOfMaxFreq ;
  WORD m_wMaxFreqInMHz ;
  PState m_pstateFromMSR ;
public:
  wxDynFreqScalingTimer(
    ICPUcoreUsageGetter * p_icpu,
    GriffinController * p_pumastatectrl
    ) ;

  //"This member should be overridden by the user if the default constructor 
  //was used and SetOwner wasn't called.
  //Perform whatever action which is to be taken periodically here."
  void Notify() ;
};//end class
