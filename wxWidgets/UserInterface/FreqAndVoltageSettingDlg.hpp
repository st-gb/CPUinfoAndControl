#pragma once //include guard

//#include <bits/stl_algobase.h> //for std::max(...). else:522: error: expected unqualified-id before '(' token
#include <vector> //for std::vector
//Must be included here in the header file (else linker error LNK2001)
//#include <wx/checkbox.h>
//#include <wx/dialog.h>
#include <wx/event.h>
//#include <ModelData/PStates.h> //for class PState

//Forward declarations (to  avoid including the wxWidgets header files HERE)
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
  //Needed for setting p-state.
  I_CPUcontroller * mp_cpucontroller ;
  MainFrame * mp_mainframe ;
  Model * mp_model ;
  WORD m_wPreviousFrequencyInMHz ;
  WORD m_wVoltageID ;
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
  wxButton * mp_wxbuttonApply ;
  wxButton * mp_wxbuttonSetAsMinVoltage ;
  wxButton * mp_wxbuttonSetAsWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxSetAsCurrentAfterApplying ;
//  wxCheckBox * mp_wxcheckboxValidPstate ;
//  wxCheckBox * mp_wxcheckboxCOFVIDcontrol ;
  wxCheckBox * mp_wxcheckboxbuttonAlsoSetWantedVoltage ;
  wxCheckBox * mp_wxcheckboxOnlySafeRange ;
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
//  wxSpinCtrlDouble * mp_wxspinctrldoubleMultiplier ;
private:
  inline void AddCPUcoreFrequencySizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
  inline void AddCPUcoreVoltageSizer(
    wxSizer * p_wxsizerSuperordinate  ) ;
public:
  FreqAndVoltageSettingDlg(
    wxWindow * parent
    , I_CPUcontroller * p_cpucontroller
    , BYTE byCoreID ) ;
  ~FreqAndVoltageSettingDlg() ;
  void OnActivate(wxActivateEvent & r_activateevent ) ;
  void CreateSliders();
  inline float GetCPUcoreFrequencyFromSliderValue() ;
  inline float GetMultiplierFromSliderValue() ;
  inline float GetVoltageInVoltFromSliderValue() ;
  void HandleCPUcoreFrequencyOrVoltageChanged(wxWindow * r_wxwindow) ;
  void HandleMultiplierValueChanged() ;
  void HandlePstateMayHaveChanged() ;
  void OnScroll(wxScrollEvent& WXUNUSED(event) ) ;
  void OnApplyButton(wxCommandEvent & );
  void OnDecVoltage(wxCommandEvent & );
  void OnIncVoltage(wxCommandEvent & );
  void OnSetAsMinVoltageButton(wxCommandEvent & );
  void OnSetAsWantedVoltageButton(wxCommandEvent & );
  void OnSpinVoltageDown(wxSpinEvent & event) ;
  void OnSpinVoltageUp(wxSpinEvent & event) ;
  void OutputFreqAndVoltageByControlValues() ;
  void OutputVoltageByControlValues() ;
  void RemoveAttention(wxWindow * p_wxwindow);
  void SetAttention(wxWindow * p_wxwindow, const wxString & wxstr = _T("")) ;
  WORD SetNearestHigherPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  //void 
  WORD SetNearestLowerPossibleFreqInMHz(
    WORD wFreqInMHz ) ;
  void VoltageIDchanged(int nNewValue) ;
  //Neccessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
};
