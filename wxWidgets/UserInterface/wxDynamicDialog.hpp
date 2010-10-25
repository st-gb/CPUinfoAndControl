#pragma once

// "For compilers that support precompilation, includes "wx/wx.h". "
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// "for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)"
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <windef.h> //for DWORD
#include <winnt.h> //for ULONGLONG
#include <vector>

//Forward declaration instead of include's(=faster)
//class RegisterData ;
class CPUIDdata ;
class MSRdata ;
class Model ;
class wxFrame ;
//class wxDialog ;
class wxCommandEvent ;
class wxCheckBox ;
class wxStaticText ;
class wxTimer ;
//class I_CPUcontroller ;
class I_CPUaccess ;
class wxX86InfoAndControlApp ;

class wxDynamicDialog
  : public wxDialog 
{
  unsigned short m_wControlID ;
  //These member variables should be used for reading out CPU register data
  //because this is faster than to create local variables on stack.
  DWORD m_dwEAX ;
  DWORD m_dwEBX ;
  DWORD m_dwECX ;
  DWORD m_dwEDX ;
  //I_CPUcontroller * mp_i_cpucontroller ;
  I_CPUaccess * mp_cpuaccess ;
  //RegisterData * mp_regdata ;
  //MSRdata * mp_msr_data ;
  Model * mp_modeldata ;
  //std::map<unsigned short, wxStaticText *> m_stdmap_p_wxstatictext ;
  std::vector<wxStaticText *> m_stdvector_p_wxstatictext ;
  std::vector<wxStaticText *>::iterator m_stdvector_p_wxstatictextiter  ;
  //This member variable should be used for reading out CPU register data
  //because this is faster than to create local variables on stack.
  ULONGLONG m_ullPrevTSCvalue ;
  ULONGLONG m_ullValue ;
  ULONGLONG m_ullValue2 ;
  wxBoxSizer * p_wxboxsizerOptions ;
  wxBoxSizer * mp_wxboxsizerOutmost ;
  wxBoxSizer * mp_sizerTop ;
  wxBoxSizer * mp_sizerLeftColumn ;
  wxBoxSizer * mp_sizerRightColumn ;
  wxCheckBox * mp_wxcheckboxReloadCPUregisterToReadConfig ;
  wxCheckBox * mp_wxcheckboxInterval ;
  wxCheckBox * mp_wxcheckboxRebuildGUIonResize ;
  wxStaticText * mp_wxstatictext ;
  //This member variable should be used for reading out CPU register data
  //because this is faster than to create local variables on stack.
  wxString m_wxstrULL ;
  wxTextCtrl * mp_wxtextctrlUpdateIntervalInMs ;
  wxTimer m_wxtimer ;
  wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp ;
  void DisplayRegisterData() ;
  void DisplayRegisterData(CPUIDdata & r_cpuiddata);
  void DisplayRegisterData(MSRdata & r_msrdata);
  void DisplayTSCvalues() ;
public:
  wxDynamicDialog(//RegisterData 
    wxWindow * parent ,
    //MSRdata & r_regdata,
    //Model & modeldata ,
    //I_CPUcontroller * p_cpucontroller
    //The CPU access is independant from the CPU controller and usually does 
    //not change during runtime in contract to the CPU controller that may be
    //exchanged during runtime.
    //I_CPUaccess * p_cpuaccess
    //, 
    wxX86InfoAndControlApp * p_wxx86infoandcontrolapp
    ) ;
  ~wxDynamicDialog() ;
  inline void AddStatictext( const wxString & cr_wxstr ) ;
  void BuildCPUregisterControls() ;
  void BuildGUI(MSRdata & r_msrdata ) ;
  void BuildGUI(CPUIDdata & r_cpuiddata ) ;
  void BuildGUI() ;
  void OnReloadCPUregisterToReadConfig( wxCommandEvent & r_wxcommandevent ) ;
  void OnRuntimeCreatedControls(wxCommandEvent & wxevent) ;
  void OnSize( wxSizeEvent & sizeevent ) ;
  void OnTimerEvent(wxTimerEvent &event);
  void ReadRegisterDataConfig( std::string & strFamilyAndModelFilePath ) ;
  inline void ReBuildGUI() ;
  inline void ReloadCPUregisterToReadConfig() ;
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};
