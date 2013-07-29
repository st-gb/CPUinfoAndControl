/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
class CPUregisterProperties;
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

class wxExamineCPUregistersDialog
  : public wxDialog 
{
  bool m_bAlignRegisterDataRight;
  unsigned m_horizontalAlignment;
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
  MSRdata * m_p_msrdataCurrent;
  std::string * m_p_stdstrCurrentRegisterDataName;
  const CPUregisterProperties * m_p_cpuregisterproperties;
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
  wxCheckBox * mp_wxcheckboxAlignRegisterDataRight;
  wxCheckBox * mp_wxcheckboxReloadCPUregisterToReadConfig ;
  wxCheckBox * mp_wxcheckboxInterval ;
  wxCheckBox * mp_wxcheckboxPrefixWithRegisterID;
  wxCheckBox * mp_wxcheckboxRebuildGUIonResize ;
  wxStaticText * mp_wxstatictext ;
  //This member variable should be used for reading out CPU register data
  //because this is faster than to create local variables on stack.
  wxString m_wxstrULL ;
  wxTextCtrl * mp_wxtextctrlUpdateIntervalInMs ;
  wxTimer m_wxtimer ;
  wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp ;

  void AddPrefixWithRegisterIDCheckBox();
  void AddIntervalCheckbox();
  void AddUpdateIntervalInMsTextctrl();
  void AddRebuildGUIonResizeCheckbox();
  void DisplayRegisterData() ;
  void DisplayRegisterData(CPUIDdata & r_cpuiddata);
  void DisplayRegisterData(MSRdata & r_msrdata);
  void DisplayTSCvalues() ;
public:
  wxExamineCPUregistersDialog(//RegisterData 
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
  ~wxExamineCPUregistersDialog() ;
  inline void AddCPUIDdataControls();
  inline void AddMSRdataControls();
  inline void AddStatictext( const wxString & cr_wxstr ) ;
  void BuildCPUregisterControls() ;
  void BuildGUI(const MSRdata & r_msrdata ) ;
  void BuildGUI(const CPUIDdata & r_cpuiddata ) ;
  void BuildGUI() ;
  void InsertIntoLeftColumn(const wxString & cr_wxstr);
  void OnClose( wxCloseEvent & wxcmd );
  void OnReloadCPUregisterToReadConfig( wxCommandEvent & r_wxcommandevent ) ;
  void OnRuntimeCreatedControls(wxCommandEvent & wxevent) ;
  void OnSize( wxSizeEvent & sizeevent ) ;
  void OnTimerEvent(wxTimerEvent &event);
  void ReadRegisterDataConfig( std::string & strFamilyAndModelFilePath ) ;
  inline void ReBuildGUI() ;
  inline void ReloadCPUregisterToReadConfig() ;
  bool StartTimerWithIntervalTime();
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};
