/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl. guard

//#include <wx/timer.h>
//#include "CPUcoreUsageGetterIWbemServices.hpp"
#include "../Controller/CPU-related/ICPUcoreUsageGetter.hpp"
#include <Controller/CPU-related/AMD/Griffin/GriffinController.hpp>
#include <global.h>
#include <math.h> //for log2()
#include <cmath>
#include <wx/timer.h>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class UserInterface ;
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
