#pragma once //incl. guard

#include "FloatValidator.hpp"

class CPUcoreData ;

class DynFreqScalingDlg
   : public wxDialog
{
private:
  CfloatValidator m_floatvalidator ;
  CPUcoreData * mp_cpucoredata ;
  wxButton * mp_wxbuttonApply ;
  wxTextCtrl * p_wxtextctrlThrottleTemp ;
  wxTextCtrl * p_wxtextctrlMilliSecondsWait ;
  wxTextCtrl * p_wxtextctrlCPUusageForThrottleToPstate1 ;
public:
  DynFreqScalingDlg(
    wxWindow * p_wxwindowParent
    , CPUcoreData & r_cpucoredata
    );
  void OnApplyButton(wxCommandEvent & );
  //Neccessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
} ;
