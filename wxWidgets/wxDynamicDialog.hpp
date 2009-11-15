#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <vector>

//pre-declarations instead of include's(=faster)
//class RegisterData ;
class MSRdata ;
class Model ;
class wxFrame ;
//class wxDialog ;
class wxCommandEvent ;
class wxCheckBox ;
class wxStaticText ;
class wxTimer ;
//class GriffinController ;
class I_CPUcontroller ;

class wxDynamicDialog
  : public wxDialog 
{
  unsigned short m_wControlID ;
  //GriffinController * mp_pumastatecontrol ;
  I_CPUcontroller * mp_cpucontroller ;
  //RegisterData * mp_regdata ;
  //MSRdata * mp_msr_data ;
  Model * mp_modeldata ;
  //std::map<unsigned short, wxStaticText *> m_stdmap_p_wxstatictext ;
  std::vector<wxStaticText *> m_stdvector_p_wxstatictext ;
  std::vector<wxStaticText *>::iterator m_stdvector_p_wxstatictextiter  ;
  wxBoxSizer * mp_sizerTop ;
  wxBoxSizer * mp_sizerLeftColumn ;
  wxBoxSizer * mp_sizerRightColumn ;
  wxCheckBox * mp_wxcheckboxReloadCPUregisterToReadConfig ;
  wxCheckBox * mp_wxcheckboxInterval ;
  wxTextCtrl * mp_wxtextctrlUpdateIntervalInMs ;
  wxTimer m_wxtimer ;
  void DisplayRegisterData() ;
  void DisplayRegisterData(MSRdata & r_msrdata);
public:
  wxDynamicDialog(//RegisterData 
    wxWindow * parent ,
    //MSRdata & r_regdata,
    Model & modeldata ,
    //GriffinController * mp_pumastatecontrol 
    I_CPUcontroller * p_cpucontroller
    ) ;

  void BuildGUI(MSRdata & r_msrdata ) ;
  void BuildGUI() ;
  void OnRuntimeCreatedControls(wxCommandEvent & wxevent) ;
  void OnTimerEvent(wxTimerEvent &event);

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};
