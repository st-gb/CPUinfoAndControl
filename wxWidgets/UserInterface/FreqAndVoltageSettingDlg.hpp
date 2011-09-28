/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

//#include <bits/stl_algobase.h> //for std::max(...). else:522: error:
// expected unqualified-id before '(' token
#include <vector> //for std::vector
//Must be included here in the header file (else linker error LNK2001)
//#include <wx/checkbox.h>
//#include <wx/dialog.h>
#include <wx/event.h>

//Forward declarations (to  avoid including the wxWidgets header files HERE)
class wxBitmapToggleButton ;
class wxBoxSizer ;
class wxCheckBox ;
class wxControl ;
class wxDialog ;
class wxStaticText ;
class wxSpinButton ;
class wxSpinEvent ;
class I_CPUcontroller ;
class MainFrame ;
class Model ;

class FreqAndVoltageSettingDlg
  : public wxDialog
{
private:
  //The core ID is needed for setting p-state.
  BYTE m_byCoreID ;
  BYTE m_byPstateID ;
  BYTE m_byPreviousMultiplierValue ;
  float m_fWantedVoltageInVolt ;
public:
  //Needed for setting p-state.
  I_CPUcontroller * mp_cpucontroller ;
private:
  MainFrame * mp_mainframe ;
public:
  Model * mp_model ;
private:
  WORD m_wPreviousFrequencyInMHz ;
  WORD m_wVoltageSliderValue ;
  //http://wiki.wxwidgets.org/Avoiding_Memory_Leaks#Child_windows:
  //"When a wxWindow is destroyed, it automatically deletes all its children.
  //These children are all the objects that received the window as the
  //parent-argument in their constructors.
  //As a consequence, if you're creating a derived class that contains child
  //windows, you should use a pointer to the child windows instead of the
  //objects themself as members of the main window."
  ////This vector contains the dyn. (on the heap) created controls
  ////in order to release the memory later (by storing the pointer
  ////to them).
  //std::vector<wxControl * > m_vecp_wxcontrol ;
#ifdef wxUSE_BITMAPTOGGLEBTN
  wxBitmapToggleButton * m_p_wxbitmapToggleButtonPause ;
#endif //#ifdef wxUSE_BITMAPTOGGLEBTN
  wxBoxSizer * p_wxboxsizerCPUcoreFrequencyInMHz ;
  wxBoxSizer * p_wxboxsizerTop ;
  wxButton * mp_wxbuttonApply ;
public:
  wxButton * m_p_wxbuttonFindLowestStableVoltage;
private:
//  wxButton * mp_wxbuttonSetAsMinVoltage ;
  wxButton * mp_wxbuttonSetAsWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxSetAsCurrentAfterApplying ;
//  wxCheckBox * mp_wxcheckboxValidPstate ;
//  wxCheckBox * mp_wxcheckboxCOFVIDcontrol ;
  wxCheckBox * mp_wxcheckboxAlsoSetWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxOnlySafeRange ;
  wxCheckBox * mp_wxcheckboxPreventVoltageAboveDefaultVoltage ;
  wxCheckBox * mp_wxcheckboxPreventVoltageBelowLowestStableVoltage ;
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  wxCheckBox * mp_wxcheckboxPauseService ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  //Array of pointers to checkbox.
  wxCheckBox ** m_ar_p_wxcheckbox ;
  wxSlider * mp_wxsliderCPUcoreDivisorID ;
  wxSlider * mp_wxsliderCPUcorePstate ;
  wxSlider * mp_wxsliderFreqInMHz ;
  wxSlider * mp_wxsliderCPUcoreFrequencyMultiplier ;
  wxSlider * mp_wxsliderCPUcoreVoltage ;
  wxSpinButton * mp_wxspinbuttonVoltageInVolt ;
  wxStaticText * mp_wxstatictextFreqInMHz ;
  wxStaticText * mp_wxstatictextVoltageInVolt ;
  wxStaticText * mp_wxstatictextWantedVoltageInVolt ;
  wxStaticText * mp_wxstatictextExpectedCurrentDissipation ;
  wxStaticText * mp_wxstatictextPercentageOfDefaultVoltage ;
  wxString m_wxstrIconFilesPrefix ; //= wxT("icons/") ;
  wxString m_wxstrWriteVoltageAndMultiplierToolTip ;
//  wxSpinCtrlDouble * mp_wxspinctrldoubleMultiplier ;
private:
  inline void AddApplyOrCancelSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddCPUcoreCheckBoxSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddCPUcoreFrequencySizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
  inline void AddCPUcoreVoltageSizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
  inline void AddDecreaseVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddIncreaseVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddPauseServiceCheckbox(wxSizer * p_wxsizer ) ;
  inline void AddPerformanceStateSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddPreventVoltageAboveDefaultVoltageButton(
    wxSizer * p_wxsizer ) ;
  inline void AddPreventVoltageBelowLowestStableVoltageButton(
    wxSizer * p_wxsizer ) ;
  inline void AddSetAsDesiredVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddSetAsMinVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddSetAsMinVoltageSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddSetAsWantedVoltageSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddStabilizeVoltageButton( wxSizer * p_wxsizer ) ;
public:
  FreqAndVoltageSettingDlg(
    wxWindow * parent
    , I_CPUcontroller * p_cpucontroller
    , BYTE byCoreID
    , wxWindowID wxwindow_id = wxID_ANY,
    const wxString & cr_wxstrTitle = wxT("voltage and frequency settings"),
    const wxPoint & cr_wxpointWindowPosition = wxDefaultPosition,
    const wxSize & cr_wxsizeWindow = wxSize( 600,300 ),
    long lStyle = wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    ) ;
  ~FreqAndVoltageSettingDlg() ;
  void ConnectCharEvent( wxWindow * p_wxWindow ) ;
  inline void CPUcoreVoltageChanged() ;
  void CreateSliders();
  void DisableOSesDVFSandServiceDVFS();
  inline float GetCPUcoreFrequencyFromSliderValue() ;
  inline void GetPstateUnsafetyDescription(BYTE byIsSafe, wxString & wxstr) ;
  inline float GetMultiplierFromSliderValue() ;
  inline float GetVoltageInVoltFromSliderValue() ;
  void HandleCPUcoreFrequencyOrVoltageChanged(wxWindow * r_wxwindow) ;
  inline void HandleKeyEvent_Inline( wxKeyEvent & r_wxkeyevent) ;
  void HandleMultiplierValueChanged() ;
  void HandlePstateMayHaveChanged() ;
  void OnActivate(wxActivateEvent & r_activateevent ) ;
  void OnApplyButton(wxCommandEvent & );
  void OnChar( wxKeyEvent & event) ;
  void OnCharHook( wxKeyEvent & r_wxkeyevent) ;
  void OnDecVoltage(wxCommandEvent & );
  void OnFindLowestStableVoltageButton(wxCommandEvent & );
  void OnIncVoltage(wxCommandEvent & );
  void OnPreventVoltageAboveDefaultVoltageCheckbox(wxCommandEvent & ) ;
  void OnPreventVoltageBelowLowestStableVoltageCheckbox(wxCommandEvent & ) ;
  void OnScroll(wxScrollEvent& WXUNUSED(event) ) ;
  void OnSetAsMinVoltageButton(wxCommandEvent & );
  void OnSetAsWantedVoltageButton(wxCommandEvent & );
//  void OnSpinVoltageDown(wxSpinEvent & event) ;
//  void OnSpinVoltageUp(wxSpinEvent & event) ;
  void OnStabilizeVoltageButton(wxCommandEvent & wxcmd ) ;
  void OutputFrequencyByControlValues() ;
  void OutputVoltageByControlValues() ;
  inline void PossiblyWriteVoltageAndMultiplier_Inline(
    float fVoltageInVolt) ;
  void RemoveAttention(wxWindow * p_wxwindow);
  void SetAttention(wxWindow * p_wxwindow, const wxString & wxstr = _T("")) ;
  WORD SetNearestHigherPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  //void 
  WORD SetNearestLowerPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  void ChangeVoltageSliderValue(int nNewValue) ;
  inline BYTE SetVoltageSliderToClosestValue(float fVoltageInVolt) ;
  inline BYTE VoltageIsWithinValidRange(float fVoltageInVolt
    , float fCPUcoreFrequencyinMHz) ;
  //Necessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
};
