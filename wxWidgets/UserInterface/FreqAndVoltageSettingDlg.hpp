/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include <set> //class std::set
//#include <bits/stl_algobase.h> //for std::max(...). else:522: error:
// expected unqualified-id before '(' token
#include <vector> //for std::vector
//Must be included here in the header file (else linker error LNK2001)
//#include <wx/checkbox.h>
//#include <wx/dialog.h>
#include <wx/event.h>
#include "wx/power.h" //for power mgmt notification (wxPowerType etc.)
#include <wx/tglbtn.h> //wxHAS_BITMAPTOGGLEBUTTON

#define USE_BITMAP_TOGGLE_BTN

#ifndef wxHAS_BITMAPTOGGLEBUTTON
  //#if wxMAJOR_VERSION > 1
    //#if wxMAJOR_VERSION == 2 && wxMINOR_VERSION > 8 && wxRELEASE_NUMBER > 0
    #if wxCHECK_VERSION(2,9,0)
      //For wx2.9.1 wxHAS_BITMAPTOGGLEBUTTON is not defined although
      //there class wxBitmapToggleButton exists.
      #define wxHAS_BITMAPTOGGLEBUTTON
    #else
      //#define wxHAS_BITMAPTOGGLEBUTTON
    #endif
  //#endif
#endif
//#ifdef USE_BITMAP_TOGGLE_BTN
//  //Forward declarations (to  avoid including the wxWidgets header files HERE)
#ifdef wxHAS_BITMAPTOGGLEBUTTON
  #define WX_BITMAP_TOGGLE_BUTTON_NAMESPACE /* empty string->global namespace*/
#else
  #define WX_BITMAP_TOGGLE_BUTTON_NAMESPACE wx2_9compatibility
#endif //#ifdef wxHAS_BITMAPTOGGLEBUTTON

namespace WX_BITMAP_TOGGLE_BUTTON_NAMESPACE
{
  class wxBitmapToggleButton;
};
//  using namespace wx2_9compatibility;


class wxBoxSizer ;
class wxCheckBox ;
class wxChoice;
class wxControl ;
class wxDialog ;
class wxStaticText ;
class wxSpinButton ;
class wxSpinEvent ;
class wxTextCtrl;
class I_CPUcontroller ;
class MainFrame ;
class Model ;
class VoltageAndFreq;
//namespace std
//{
//  template<typename _Key, typename _Compare = std::less<_Key>,
//   typename _Alloc = std::allocator<_Key> > class set;
//}

class FreqAndVoltageSettingDlg
  : public wxDialog
{
private:
  bool m_bAllowWritingVoltageAndFrequency;
  //The core ID is needed for setting p-state.
  BYTE m_byCoreID ;
  BYTE m_byPstateID ;
  BYTE m_byPreviousMultiplierValue ;
  float m_fWantedVoltageInVolt ;
public:
  static wxString s_wxstrInstableCPUcoreVoltageWarning;
  enum voltageTypes
  {
    minimum_voltage = 0,
    wanted_voltage,
    maximum_voltage
  };
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
  wxBoxSizer * p_wxboxsizerCPUcoreFrequencyInMHz ;
  wxBoxSizer * p_wxboxsizerTop ;
  wxButton * mp_wxbuttonApply ;
public:
  wxButton * m_p_wxbuttonFindLowestStableVoltage;
  wxBitmapButton * m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage;
  wxSpinButton * m_p_wxspinbuttonSecondsUntilVoltageDecrease;
  wxStaticText * m_p_wxstatictextSecondsUntilNextVoltageDecrease;
//  wxStaticText * m_p_wxstatictextInstableCPUcoreVoltageWarning;
  wxTextCtrl * m_p_wxtextctrlSecondsUntilNextVoltageDecrease;
//  wxBitmapButton * m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage;
private:
//  wxBoxSizer * m_p_wxboxsizerOK_Cancel;
//  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer * m_p_wxboxsizerOK_Cancel;
    //wx2_9compatibility::wxFlexGridSizer * m_p_wxboxsizerOK_Cancel;
  wxBoxSizer * m_p_wxboxsizerMessage;
//  wxButton * mp_wxbuttonSetAsMinVoltage ;
  wxButton * mp_wxbuttonSetAsWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxSetAsCurrentAfterApplying ;
  wxBoxSizer * p_wxboxsizerCPUcorePstate;

  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton *
    m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxOnlySafeRange ;
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton * 
    m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage ;
public:
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton *
    m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage;
  wxBitmapButton *
    m_p_wxbitmapbuttonAutoConfigureVoltageSettings;
private:
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton *
    m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume;
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton *
    m_p_wxbitmaptogglebuttonSelectPstateViaMouseClick;
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton *
    m_p_wxbitmaptogglebuttonPauseService ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  //Array of pointers to checkbox.
  wxCheckBox ** m_ar_p_wxcheckbox ;
  wxChoice * m_p_wxchoiceVoltageType;
  wxSlider * mp_wxsliderCPUcoreDivisorID ;
  wxSlider * mp_wxsliderCPUcorePstate ;
  wxSlider * mp_wxsliderFreqInMHz ;
  wxSlider * mp_wxsliderCPUcoreFrequencyMultiplier ;
  wxSlider * mp_wxsliderCPUcoreVoltage ;
  wxSpinButton * mp_wxspinbuttonVoltageInVolt ;
  wxStaticText * mp_wxstatictextFreqInMHz ;
  wxStaticText * mp_wxstatictextVoltageInVolt ;
  wxStaticText * mp_wxstatictextMessage;
  wxStaticText * mp_wxstatictextWantedVoltageInVolt ;
  wxStaticText * mp_wxstatictextExpectedCurrentDissipation ;
  wxStaticText * mp_wxstatictextPercentageOfDefaultVoltage ;
  wxString m_wxstrIconFilesPrefix ; //= wxT("icons/") ;
  wxString m_wxstrWriteVoltageAndMultiplierToolTip ;
  wxTextCtrl * m_p_wxtextctrlInstableCPUcoreVoltageWarning;
//  wxSpinCtrlDouble * mp_wxspinctrldoubleMultiplier ;
private:
  inline void AddAlsoSetWantedVoltageControls(
    wxSizer * p_wxsizerSuperordinate );
  inline void AddApplyOrCancelSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddAutoConfigureVoltageSettingsButton(
    wxSizer * p_wxsizerSuperordinate );
  inline void AddCancelButton(wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddCPUcoreCheckBoxSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddCPUcoreFrequencySizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
  inline void AddCPUcoreVoltageSizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
  inline void AddDecreaseVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddIncreaseVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddSecondsUntilNextVoltageDecreaseTextControl(
    wxSizer * p_wxsizer );
  inline void AddSelectPstateViaMousePositionButton(
    wxSizer * p_wxsizer );
  inline void AddPauseServiceCheckbox(wxSizer * p_wxsizer ) ;
  inline void AddPerformanceStateSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddPreventVoltageAboveDefaultVoltageButton(
    wxSizer * p_wxsizer ) ;
  inline void AddPreventVoltageBelowLowestStableVoltageButton(
    wxSizer * p_wxsizer ) ;
  inline void AddRestorePerformanceStateAfterResumeButton(wxSizer * p_wxsizer ) ;
  inline void AddSetAsDesiredVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddSetAsMinVoltageButton( wxSizer * p_wxsizer ) ;
  inline void AddVoltageSettingsSizer( wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddWritePstateButton(wxSizer * p_wxsizer);
  inline void AddSetAsWantedVoltageSizer(
    wxSizer * p_wxsizerSuperordinate ) ;
  inline void AddStabilizeVoltageButton( wxSizer * p_wxsizer ) ;
  void AddWindowToSizer(wxWindow * p_window, wxSizer * p_sizer,
    ////0=the control should not take more space if the sizer is enlarged
    int proportion = 0,
    int flag = 0, int border = 0, wxObject* userData = 0);
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
  void CreateFindLowestStableCPUcoreVoltageButton();
  void CreateStopFindingLowestStableCPUcoreVoltageButton();
  void CreateSliders();
  void DisableOSesDVFSandServiceDVFS(wxString & r_wxstrMessageFromService);
  void DisableWritingVoltageAndMultiplier(const wxString & c_r_wxstrToolTip)
  {
    m_bAllowWritingVoltageAndFrequency = false;

    //TODO tooltip is not permanent
  //    mp_wxbuttonApply->SetToolTip( m_wxstrWriteVoltageAndMultiplierToolTip ) ;
    SetAttention(mp_wxbuttonApply, c_r_wxstrToolTip);
  }
  void EnableWritingVoltageAndMultiplier(const wxString & c_r_wxstrToolTip)
  {
    m_bAllowWritingVoltageAndFrequency = true;
    mp_wxbuttonApply->Enable() ;
    //TODO tooltip is not permanent
  //    mp_wxbuttonApply->SetToolTip( m_wxstrWriteVoltageAndMultiplierToolTip ) ;
    RemoveAttention(mp_wxbuttonApply);
    //Remove a possible tooltip.
    mp_wxbuttonApply->SetToolTip(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
//        _T("")
        c_r_wxstrToolTip
      );
  }
  void FindLowestStableVoltage();
  void FindLowestStableVoltage(
    const float fVoltageInVolt,
    const float fMultiplier
    );
  inline float GetCPUcoreFrequencyFromSliderValue() ;
  uint32_t GetCPUcoreMask();
  inline void GetPstateUnsafetyDescription(BYTE byIsSafe, wxString & wxstr) ;
  //inline
    float GetMultiplierFromSliderValue() ;
  const std::set<VoltageAndFreq> & GetSelectedVoltageTypeStdSet();
  //inline
    float GetVoltageInVoltFromSliderValue() ;
  void HandleCPUcoreFrequencyOrVoltageChanged(wxWindow * r_wxwindow) ;
  inline void HandleKeyEvent_Inline( wxKeyEvent & r_wxkeyevent) ;
  void HandleMultiplierValueChanged() ;
  void HandlePstateMayHaveChanged() ;
  void OnActivate(wxActivateEvent & r_activateevent ) ;
  void OnApplyButton(wxCommandEvent & );
  void OnAutoConfigureVoltageSettingsButton(wxCommandEvent & r_wxcommandevent );
  void OnChar( wxKeyEvent & event) ;
  void OnCharHook( wxKeyEvent & r_wxkeyevent) ;
  void OnClose( wxCloseEvent & wxcmd );
  void OnDecVoltage(wxCommandEvent & );
  void OnFindLowestStableVoltageButton(wxCommandEvent & );
  void OnStopFindingLowestStableCPUcoreVoltageButton(wxCommandEvent & );
  void OnIncVoltage(wxCommandEvent & );
  void OnPreventVoltageAboveDefaultVoltageCheckbox(wxCommandEvent & ) ;
  void OnPreventVoltageBelowLowestStableVoltageCheckbox(wxCommandEvent & ) ;
#ifdef wxHAS_POWER_EVENTS
  void OnResume(wxPowerEvent & WXUNUSED(event) );
#endif //#ifdef wxHAS_POWER_EVENTS
  void OnScroll(wxScrollEvent& WXUNUSED(event) ) ;
  void OnSelectPstateViaMousePos(wxCommandEvent & );
  void OnSetAsMinVoltageButton(wxCommandEvent & );
  void OnSetAsWantedVoltageButton(wxCommandEvent & );
//  void OnSpinVoltageDown(wxSpinEvent & event) ;
//  void OnSpinVoltageUp(wxSpinEvent & event) ;
  void OnSpinSecondsUntilVoltageDecreaseSpinButton(
    wxSpinEvent & event);
  void OnStabilizeVoltageButton(wxCommandEvent & wxcmd ) ;
  void OnSetSecondsCountDownLabel( wxCommandEvent
    //wxEvent
    & event );
  void OnSetVoltageType(wxCommandEvent & wxcmd);
  void OutputFrequencyByControlValues() ;
  void OutputVoltageByControlValues() ;
  inline void PossiblyWriteVoltageAndMultiplier_Inline(
    float fVoltageInVolt) ;
  void RemoveAttention(wxWindow * p_wxwindow);
  void ResumendFromStandByOrHibernate();
  void SetAttention(wxWindow * p_wxwindow, const wxString & wxstr = _T("")) ;
  void HideInstableCPUcoreVoltageWarning();
  void ShowInstableCPUcoreVoltageWarning();
  WORD SetNearestHigherPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  //void 
  WORD SetNearestLowerPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  inline void SetMessage(const wxString & c_r_wxstrMessage);
  void SetSecondsUntilNextVoltageDecreaseLabel(
    unsigned seconds);
  void ChangeToStopFindingLowestStableCPUcoreVoltageButton();
  void ChangeVoltageSliderValue(int nNewValue) ;
  void SetStartFindingLowestStableVoltageButton();
  void SetMultiplierSliderToClosestValue(float fMultiplier);
  BYTE SetVoltageSliderToClosestValue(float fVoltageInVolt) ;
  void StartedInstableCPUcoreVoltageDetection();
  void StopFindingLowestStableCPUcoreVoltage();
  inline BYTE VoltageIsWithinValidRange(float fVoltageInVolt
    , float fCPUcoreFrequencyinMHz) ;
  //Necessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
};
  BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE( wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE, wxNewEventType() )
  //"EVT_BUTTON" also expands to "DECLARE_EVENT_TYPE"
  //  DECLARE_EVENT_TYPE(wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE, 7777)
  END_DECLARE_EVENT_TYPES()
