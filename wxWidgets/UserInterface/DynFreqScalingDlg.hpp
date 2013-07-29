/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl. guard

#include "FloatValidator.hpp"

class CPUcoreData ;

class DynFreqScalingDlg
   : public wxDialog
{
private:
  CfloatValidator m_floatvalidator ;
  CPUcoreData * mp_cpucoredata ;
  wxButton * mp_wxbuttonOK ;
  wxButton * mp_wxbuttonCancel ;
  wxTextCtrl * p_wxtextctrlThrottleTemp ;
  wxTextCtrl * p_wxtextctrlMilliSecondsWait ;
  wxTextCtrl * p_wxtextctrlCPUusageForThrottleToPstate1 ;
public:
  DynFreqScalingDlg(
    wxWindow * p_wxwindowParent
    , CPUcoreData & r_cpucoredata
    );
  void OnOK_Button(wxCommandEvent & );
  //Necessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
} ;
