#pragma once //include guard

//#include <bits/stl_algobase.h> //for std::max(...). else:522: error: expected unqualified-id before '(' token
#include <vector> //for std::vector
//Must be included here in the header file (else linker error LNK2001)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/event.h>
#include <ModelData/PStates.h> //for class PState

//pre-declarations (to  avoid including the wxWidgets header files HERE)
class wxCheckBox ;
class wxControl ;
class wxDialog ;
class wxStaticText ;
class wxSpinButton ;
class wxSpinEvent ;
//class PumaStateCtrl ;
//class GriffinController ;
class I_CPUcontroller ;
class MainFrame ;

class FreqAndVoltageSettingDlg
  : public wxDialog
{
private:
  //The core ID is needed for setting p-state.
  BYTE m_byCoreID ;
  BYTE m_byPstateID ;
  BYTE m_byPreviousMultiplierValue ;
  float m_fWantedVoltageInVolt ;
  //DIDandFID m_did_and_fid ;
  //Needed for setting p-state.
  //PumaStateCtrl 
  //GriffinController * mp_pumastatectrl ;
  I_CPUcontroller * mp_cpucontroller ;
  //PState m_pstate ;
  MainFrame * mp_mainframe ;
  WORD m_wPreviousFrequencyInMHz ;
  WORD m_wVoltageID ;
  //"When a wxWindow is destroyed, it automatically deletes all its children. These children are all the objects that received the window as the parent-argument in their constructors.  
  //As a consequence, if you're creating a derived class that contains child windows, you should use a pointer to the child windows instead of the objects themself as members of the main window."
  ////This vector contains the dyn. (on the heap) created controls
  ////in order to release the memory later (by storing the pointer
  ////to them).
  //std::vector<wxControl * > m_vecp_wxcontrol ;
  wxButton * mp_wxbuttonApply ;
  wxButton * mp_wxbuttonSetAsMinVoltage ;
  wxButton * mp_wxbuttonSetAsWantedVoltage ;
//  wxCheckBox * mp_wxcheckboxSetAsCurrentAfterApplying ;
//  wxCheckBox * mp_wxcheckboxValidPstate ;
  wxCheckBox * mp_wxcheckboxCOFVIDcontrol ;
  wxCheckBox * mp_wxcheckboxbuttonAlsoSetWantedVoltage ;
  wxSpinButton * mp_wxspinbuttonVoltageInVolt ;
  //wxStaticText * mp_wxstatictextFreqInMHz ;
  wxStaticText * mp_wxstatictextVoltageInVolt ;
  wxStaticText * mp_wxstatictextWantedVoltageInVolt ;
  wxStaticText * mp_wxstatictextExpectedCurrentDissipation ;
  wxStaticText * mp_wxstatictextPercentageOfDefaultVoltage ;
  wxSlider * mp_wxsliderCPUcoreDivisorID ;
  wxSlider * mp_wxsliderCPUcorePstate ;
  wxSlider * mp_wxsliderFreqInMHz ;
  wxSlider * mp_wxsliderCPUcoreFrequencyMultiplier ;
  wxSlider * mp_wxsliderCPUcoreVoltage ;
public:
  FreqAndVoltageSettingDlg(
    wxWindow * parent
    //, PumaStateCtrl * p_pumastatectrl
    //, GriffinController * p_pumastatectrl
    , I_CPUcontroller * p_cpucontroller
    , BYTE byCoreID ) ;
  ~FreqAndVoltageSettingDlg() ;
  void OnActivate(wxActivateEvent & r_activateevent ) ;
  void CreateSliders();
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
