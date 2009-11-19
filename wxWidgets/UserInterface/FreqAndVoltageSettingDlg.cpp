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

#include <wx/slider.h>
#include <wx/dialog.h>
#include <wx/sizer.h> //for class wxBoxSizer
#include <wx/button.h>
#include <global.h> //for BYTE
#include "FreqAndVoltageSettingDlg.hpp"
//#include "../AMD_family17.h"
#include <wx/stattext.h> //for wxStaticText
//#include "../ModelData/PStates.h"
//#include "../Controller/PumaStateCtrl.h"
//#include "Controller/GriffinController.hpp"
#include <wx/msgdlg.h> //for ::wxMessageBox(...)
#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include <wx/checkbox.h>
//for a line which may be used in a dialog to separate the groups of controls. 
#include <wx/statline.h> 
#include "MainFrame.hpp" //for class MyFrame

//An enum guarantees a unique number for each element.
enum
{
  ID_DivisorSlider = 1,
  ID_PstateSlider ,
  ID_MultiplierSlider,
  ID_VoltageSlider
  , ID_FrequencySlider
};

BEGIN_EVENT_TABLE(FreqAndVoltageSettingDlg, wxDialog)
  EVT_ACTIVATE(FreqAndVoltageSettingDlg::OnActivate) 
  EVT_SCROLL(FreqAndVoltageSettingDlg::OnScroll)
  EVT_BUTTON(wxID_APPLY, FreqAndVoltageSettingDlg::OnApplyButton)
END_EVENT_TABLE()

FreqAndVoltageSettingDlg::FreqAndVoltageSettingDlg(
  wxWindow * parent
  //, GriffinController * p_pumastatectrl
  , I_CPUcontroller * p_cpucontroller
  , BYTE byCoreID
  )
  : wxDialog( 
      parent, 
      wxID_ANY, 
      wxString::Format( 
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compitable between char and wide char.
      _T("settings for core %u") , 
      byCoreID 
    )
    , wxPoint(30, 30) //A value of (-1, -1) indicates a default size
    , wxSize(400, 400)
    , wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  , m_byCoreID(byCoreID)
  //, mp_pumastatectrl(p_pumastatectrl)
  , mp_cpucontroller (p_cpucontroller)
  , m_byPstateID(0)
  , mp_mainframe( (MainFrame *) parent )
{
  //"When a wxWindow is destroyed, it automatically deletes all its children. 
  //These children are all the objects that received the window as the
  //parent-argument in their constructors.
  //As a consequence, if you're creating a derived class that contains child
  //windows, you should use a pointer to the child windows instead of the
  //objects themself as members of the main window."
  wxBoxSizer * p_wxboxsizerValidPstate = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcorePstate = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcoreDivisor = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcoreFrequencyMultiplier = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcoreVoltage = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcoreFrequencyInMHz = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerCPUcoreVoltageInVolt = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerOK_Cancel = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * sizerTop = new wxBoxSizer(wxVERTICAL);

  //mp_wxbuttonApply = new wxButton(this, wxID_APPLY ) ;
  mp_wxbuttonApply = new wxButton(this, wxID_APPLY, _T("Write p-state") ) ;
  //wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, _T("Apply"));
  CreateSliders();

  mp_wxcheckboxValidPstate = new wxCheckBox(this, wxID_ANY,
      //_T("Set as current after apply") ) ;
      _T("valid p-state") ) ;

  mp_wxcheckboxCOFVIDcontrol = new wxCheckBox(this, wxID_ANY,
      //_T("Set as current after apply") ) ;
      _T("write into COFVID control register") ) ;

  mp_wxstatictextExpectedCurrentDissipation = new wxStaticText(
    this, wxID_ANY, _T("") 
    ) ;

  ////p_wxboxsizerOK_Cancel
  //p_wxboxsizerCPUcoreVoltageInVolt->Add(
  //  mp_wxcheckboxCOFVIDcontrol
  //  , 0 //0=the control should not take more space if the sizer is enlarged
  //  //These flags are used to specify which side(s) of the sizer item the border width will apply to.
  //  , wxLEFT | wxRIGHT |
  //  //wxALIGN_CENTER_VERTICAL
  //  //The label and the adjustable value should be at the same vertical
  //  //position, so place at the top.
  //  wxALIGN_TOP
  //  , 5 //Determines the border width
  //  );

  p_wxboxsizerValidPstate->Add(
    mp_wxcheckboxValidPstate
    , 0 //0=the control should not take more space if the sizer is enlarged
    //These flags are used to specify which side(s) of the sizer item the border width will apply to.
    , wxLEFT | wxRIGHT |
    //wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
    wxALIGN_TOP
    , 5 //Determines the border width
    );

  p_wxboxsizerValidPstate->Add(
    mp_wxstatictextExpectedCurrentDissipation
    , 0 //0=the control should not take more space if the sizer is enlarged
    //These flags are used to specify which side(s) of the sizer item the border width will apply to.
    , wxLEFT | wxRIGHT |
    //wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
    wxALIGN_TOP
    , 5 //Determines the border width
    );

  p_wxboxsizerCPUcorePstate->Add( 
    new wxStaticText(this, wxID_ANY, _T("p-state:"))
    , 0 //0=the control should not take more space if the sizer is enlarged 
    //These flags are used to specify which side(s) of the sizer item the border width will apply to.
    , wxLEFT | wxRIGHT | 
    //wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
    wxALIGN_TOP
    , 5 //Determines the border width
    );
  p_wxboxsizerCPUcorePstate->Add(//p_wxsliderCPUcoreDivisorID
    mp_wxsliderCPUcorePstate 
    , 1
    //, wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5);

  //mp_wxsliderCPUcorePstate->SetBackgroundStyle(wxBG_STYLE_COLOUR);
  //mp_wxsliderCPUcorePstate->SetForegroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnBackgroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnForegroundColour(*wxBLUE);
  //mp_wxsliderCPUcorePstate->SetToolTip("too high");

  //m_vecp_wxcontrol.push_back(p_wxsliderCPUcoreFrequencyMultiplier);
  //p_wxboxsizerCPUcoreDivisor->Add( 
  //  new wxStaticText(this, wxID_ANY, _T("divisor:"))
  //  , 0 //0=the control should not take more space if the sizer is enlarged 
  //  //These flags are used to specify which side(s) of the sizer item the border width will apply to.
  //  , //wxEXPAND | 
  //  //wxALL, 
  //  wxLEFT | wxRIGHT | 
  //  //wxALIGN_CENTER_VERTICAL
  //  //The label and the adjustable value should be at the same vertical
  //  //position, so place at the top.
  //  wxALIGN_TOP
  //  , 5 //Determines the border width
  //  );
  //p_wxboxsizerCPUcoreDivisor->Add(//p_wxsliderCPUcoreDivisorID
  //  mp_wxsliderCPUcoreDivisorID , 1
  //  //, wxEXPAND | wxALL
  //  , wxLEFT | wxRIGHT
  //  , 5);
  //p_wxboxsizerCPUcoreFrequencyMultiplier->Add( 
  //  new wxStaticText(this, wxID_ANY, _T("multiplier:"))
  //  , 0 //0=the control should not take more space if the sizer is enlarged 
  //  //These flags are used to specify which side(s) of the sizer item the border width will apply to. 
  //  , //wxEXPAND | 
  //  //wxALL, 
  //  wxLEFT | wxRIGHT | 
  //  //wxALIGN_CENTER_VERTICAL
  //  //The label and the adjustable value should be at the same vertical
  //  //position, so place at the top.
  //  wxALIGN_TOP
  //  , 5 //Determines the border width
  //  );
  //p_wxboxsizerCPUcoreFrequencyMultiplier->Add(//p_wxsliderCPUcoreFrequencyMultiplier
  //  mp_wxsliderCPUcoreFrequencyMultiplier, 1
  //  //, wxEXPAND | wxALL,
  //  , wxLEFT | wxRIGHT
  //  , 5);
  p_wxboxsizerCPUcoreVoltage->Add( 
    new wxStaticText(this, wxID_ANY, _T("voltage\nIDentifier:"))
    , 0 //0=the control should not take more space if the sizer is enlarged 
    //These flags are used to specify which side(s) of the sizer item the border width will apply to.
    , //wxEXPAND | 
    //wxALL, 
    wxLEFT | wxRIGHT | 
    //wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
    wxALIGN_TOP
    , 5 //Determines the border width
    );
  p_wxboxsizerCPUcoreVoltage->Add(//p_wxsliderCPUcoreVoltage
    mp_wxsliderCPUcoreVoltage, 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5);

  p_wxboxsizerCPUcoreFrequencyInMHz->Add( 
    new wxStaticText(this, wxID_ANY, 
    //_T("core frequency in MHz: ")));
    _T("core\nfrequency\nin MHz:"))
    , 0 //0=the control should not take more space if the sizer is enlarged 
    , wxLEFT | wxRIGHT | 
    wxALIGN_TOP
    , 5 //Determines the border width
    );

  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxsliderFreqInMHz, 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5 );

  sizerTop->Add(
      p_wxboxsizerValidPstate
      //mp_wxcheckboxValidPstate 
      ,
      //"[...] used in wxBoxSizer to indicate if a child of a sizer can 
      //change its size in the main orientation of the wxBoxSizer - where 
      //0 stands for not changeable and 
      //a value of more than zero is interpreted relative to the value of 
      //other children of the same wxBoxSizer. "
      //1 
      0,
      //wxEXPAND | //wxALL
        wxBOTTOM
      ,
      //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
      //"Determines the border width, if the flag  parameter is set to
      //include any border flag."
      //10 
      0
      );

  sizerTop->Add(
      p_wxboxsizerCPUcorePstate, 1 , 
      wxEXPAND | //wxALL 
        wxBOTTOM
      , 
      //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
      //"Determines the border width, if the flag  parameter is set to 
      //include any border flag."
      //10 
      2
      );
  //sizerTop->Add(p_wxboxsizerCPUcoreDivisor, 1 , 
  //    wxEXPAND | //wxALL
  //      wxBOTTOM , 
  //    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
  //    //"Determines the border width, if the flag  parameter is set to 
  //    //include any border flag."
  //    //10 
  //    2
  //    );
  //sizerTop->Add(
  //    p_wxboxsizerCPUcoreFrequencyMultiplier, 1 , 
  //    wxEXPAND | //wxALL
  //      wxBOTTOM
  //    , 
  //    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
  //    //"Determines the border width, if the flag  parameter is set to 
  //    //include any border flag."
  //    //10 
  //    2
  //    );

  sizerTop->Add(//mp_wxsliderFreqInMHz, 
      p_wxboxsizerCPUcoreFrequencyInMHz ,
      1 , 
      wxEXPAND | //wxALL
        wxBOTTOM
      , 
      //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
      //"Determines the border width, if the flag  parameter is set to 
      //include any border flag."
      //10 
      2
      );
  sizerTop->Add( 
    p_wxboxsizerCPUcoreVoltage, 
    1 , 
    wxEXPAND | //wxALL
        wxBOTTOM
    , 
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to 
    //include any border flag."
    //10 
    2
    );
  //p_wxboxsizerCPUcoreFrequencyInMHz->Add(new wxStaticText(this, wxID_ANY, 
  //  //_T("core frequency in MHz: ")));
  //  _T("->resulting core frequency in MHz: ")));
  //mp_wxstatictextFreqInMHz = new wxStaticText(this, wxID_ANY, _T(""));
  //p_wxboxsizerCPUcoreFrequencyInMHz->Add(mp_wxstatictextFreqInMHz, 1 , 
  //  wxEXPAND | wxALL, 0);
  //sizerTop->Add(p_wxboxsizerCPUcoreFrequencyInMHz, 
  //  0 //proportion parameter: if "0" it takes the least space
  //  , wxFIXED_MINSIZE | wxALL, 
  //  //Determines the border width, if the flag  parameter is set to include 
  //  //any border flag.
  //  2);
    //sizerTop->AddSpacer(50) ;
  //sizerTop->Add(new wxStaticLine( this, wxID_ANY ) ) ;

  //mp_wxstatictextPercentageOfDefaultVoltage = new wxStaticText(this, wxID_ANY, _T("") );
  //sizerTop->Add( mp_wxstatictextPercentageOfDefaultVoltage ) ;

  p_wxboxsizerCPUcoreVoltageInVolt->Add(new wxStaticText( this, wxID_ANY, 
    //_T("core voltage in Volt: ") )
    _T("->resulting core voltage in Volt: ") )
    );
  mp_wxstatictextVoltageInVolt = new wxStaticText(this, wxID_ANY, _T("") );
  p_wxboxsizerCPUcoreVoltageInVolt->Add(mp_wxstatictextVoltageInVolt, 1 , 
    wxEXPAND | wxALL, 0);
  sizerTop->Add(
    p_wxboxsizerCPUcoreVoltageInVolt, 
    //proportition parameter: if "0" it takes the least space
    0 , 
    wxFIXED_MINSIZE | wxALL, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2);
  sizerTop->Add( 
    mp_wxcheckboxCOFVIDcontrol ,
    //"[...] used in wxBoxSizer to indicate if a child of a sizer can 
    //change its size in the main orientation of the wxBoxSizer - where 
    //0 stands for not changeable and 
    //a value of more than zero is interpreted relative to the value of 
    //other children of the same wxBoxSizer. "
    //1 
    0 , //0=no additional space between this and neighbour controls
    wxEXPAND | //wxALL
        wxBOTTOM
    , 
    2
    ) ;

  //p_wxboxsizerOK_Cancel->Add(new wxButton(this, wxID_APPLY //, _T("Apply")
  //  ) );
  p_wxboxsizerOK_Cancel->Add( mp_wxbuttonApply ) ;
  mp_wxcheckboxSetAsCurrentAfterApplying = new wxCheckBox(this, wxID_ANY, 
      //_T("Set as current after apply") ) ;
      _T("Set as current p-state after write") ) ;
  p_wxboxsizerOK_Cancel->Add(mp_wxcheckboxSetAsCurrentAfterApplying);
  p_wxboxsizerOK_Cancel->Add(new wxButton(this, wxID_CANCEL) );
  sizerTop->Add(
    p_wxboxsizerOK_Cancel, 
    0 , 
    wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );

  SetSizer(sizerTop);

  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);
  //"[...] wxToolTip::SetDelay can be used to globally alter tooltips behaviour"
  wxToolTip::SetDelay(5000);
  //To show the computed freq and voltage at the beginning yet.
  OutputFreqAndVoltageByControlValues();
}

FreqAndVoltageSettingDlg::~FreqAndVoltageSettingDlg()
{
  //for(BYTE byControlIndex = 0 ; byControlIndex < //m_byCoreNumber
  //  m_vecp_wxcontrol.size() ; ++ byControlIndex )
  //{
  //  //Release memory from the heap.
  //  delete m_vecp_wxcontrol.at(byControlIndex) ;
  //}
}

void FreqAndVoltageSettingDlg::CreateSliders()
{
    BYTE byPstateIDtoShowSettingsFor = 0 ;
    //DWORD dwLow ;
    //DWORD dwHigh ;
    ////PState pstate ;
    //mp_pumastatectrl->GetPStateFromMSR(
    //    //0, //BYTE byPStateID, 
    //    byPstateIDtoShowSettingsFor ,
    //    dwLow, 
    //    dwHigh, 
    //    //pstate, 
    //    m_pstate ,
    //    m_byCoreID
    //    ) ;

    mp_wxsliderCPUcorePstate  = new wxSlider(
    this
    //wxID_ANY, 
    , ID_PstateSlider 
    //Initial position for the slider.
    , //1 
    byPstateIDtoShowSettingsFor
    //Slider minimum value.
    , 0 
    //Slider maximum value.
    //, NUMBER_OF_PSTATES - 1 
    , mp_cpucontroller->GetNumberOfPstates()
    //, 2
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , wxSL_AUTOTICKS | wxSL_LABELS
    ) ;
  //wxSlider * p_wxsliderCPUcoreDivisorID = new wxSlider(this, 
  //mp_wxsliderCPUcoreDivisorID = new wxSlider(this, 
  //  //wxID_ANY, 
  //  ID_DivisorSlider ,
  //  //Initial position for the slider.
  //  //2 , 
  //  m_pstate.GetDivisorID() ,
  //  0, //slider minimum value
  //  //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
  //  //by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
  //  //the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
  //  HIGHEST_EFFECTIVE_DIVISOR_ID //slider max value
  //  //If the default point (-1, -1) is specified then a default point is chosen.
  //  , wxPoint(-1, -1)
  //  //If the default size (-1, -1) is specified then a default size is chosen.
  //  , wxSize(-1, -1)
  //  , wxSL_AUTOTICKS //	Displays tick marks.
  //  | wxSL_LABELS //Displays minimum, maximum and value labels.
  //  ) ;
  //wxSlider * p_wxsliderCPUcoreFrequencyMultiplier = new wxSlider(this, 
  //m_byPreviousMultiplierValue = //mp_pumastatectrl->mp_model->m_cpucoredata.
  //    //m_byMainPLLoperatingFrequencyIDmax ;
  //    m_pstate.m_byFreqID ;
  //mp_wxsliderCPUcoreFrequencyMultiplier = new wxSlider(
  //  this
  //  //wxID_ANY, 
  //  , ID_MultiplierSlider
  //  //, 2  //value
  //  , m_byPreviousMultiplierValue
  //  //Slider minimum value.
  //      //, 0
  //  //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
  //  //by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
  //  //the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
  //  , mp_pumastatectrl->mp_model->m_cpucoredata.m_byLowestEffectiveFreqID
  //  //Slider max value.
  //      //, MAX_VALUE_FOR_FID 
  //  , mp_pumastatectrl->mp_model->m_cpucoredata.
  //    //m_byMainPLLoperatingFrequencyIDmax
  //    GetMainPLLoperatingFrequencyIDmax()
  //  //If the default point (-1, -1) is specified then a default point is chosen.
  //  , wxPoint(-1, -1)
  //  //If the default size (-1, -1) is specified then a default size is chosen.
  //  , wxSize(-1, -1)
  //  , wxSL_AUTOTICKS //	Displays tick marks.
  //  | wxSL_LABELS //Displays minimum, maximum and value labels.
  //  ) ;

  mp_wxsliderFreqInMHz = new wxSlider(
    this
    //wxID_ANY, 
    , ID_FrequencySlider
    //, 2  //value
    , //pstate.GetFreqInMHz() //Initial position for the slider.
    //m_pstate.GetFreqInMHz() //Initial position for the slider.
    mp_cpucontroller->GetMinimumFrequencyInMHz()
    //Slider minimum value.
    //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
    //by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
    //the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
    , //lowest adjustable frequency:
    //PState::GetFreqInMHz(
    //  //BYTE byFreqID
    //  mp_pumastatectrl->mp_model->m_cpucoredata.
    //    //m_byMainPLLoperatingFrequencyIDmax / 2 
    //    m_byLowestEffectiveFreqID
    //  , 
    //  //BYTE byDivisorID
    //  HIGHEST_EFFECTIVE_DIVISOR_ID
    //  )
    mp_cpucontroller->GetMinimumFrequencyInMHz()
    //Slider max value.
    //, mp_pumastatectrl->GetMaximumFrequencyInMHz()
    , mp_cpucontroller->GetMaximumFrequencyInMHz()
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , 
    //wxSL_AUTOTICKS //	Displays tick marks.
    //| 
    wxSL_LABELS //Displays minimum, maximum and value labels.
    ) ;
  m_wPreviousFrequencyInMHz = mp_wxsliderFreqInMHz->GetValue();
  //HandleMultiplierValueChanged();
  //wxSlider * p_wxsliderCPUcoreVoltage = new wxSlider(this, 
  mp_wxsliderCPUcoreVoltage = new wxSlider(
    this, 
    //wxID_ANY, 
    ID_VoltageSlider ,
    //60 
    //m_pstate.GetVoltageID()
    mp_cpucontroller->GetMinimumVoltageID()
    //slider minimum value
    //, 0 
    //, mp_pumastatectrl->mp_model->m_cpucoredata.m_byMinVoltageID
    , mp_cpucontroller->GetMinimumVoltageID()
    //, MAX_VALUE_FOR_VID //slider max value
    //, mp_pumastatectrl->mp_model->m_cpucoredata.m_byMaxVoltageID
    , mp_cpucontroller->GetMaximumVoltageID()
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , wxSL_AUTOTICKS | wxSL_LABELS
    ) ;
  //m_pstate.SetDivisorID() ;
}

void FreqAndVoltageSettingDlg::HandleCPUcoreFrequencyOrVoltageChanged(
    wxWindow * p_wxwindow )
{
  BYTE byIsSafe ;
    //mp_wxstatictextPercentageOfDefaultVoltage->SetText( 
    //    wxString("percent of default voltage") + 
    //    mp_pumastatectrl-> ) ;

    //if( m_bPreviousCPUcoreFreqWas
    if( ( 
        byIsSafe = //mp_pumastatectrl->GetPstateSafefy(
          mp_cpucontroller->GetPstateSafefy(
          ////mp_wxsliderCPUcoreVoltage->GetValue()
          //m_pstate.m_byVoltageID
          //, //mp_wxsliderCPUcoreFrequencyMultiplier->GetValue()
          ////m_did_and_fid.m_byFreqID
          //m_pstate.m_byFreqID
          //, //mp_wxsliderCPUcoreDivisorID->GetValue()
          ////m_did_and_fid.m_byDivisorID ;
          //m_pstate.m_byDivisorID
          mp_wxsliderFreqInMHz->GetValue()
          , mp_cpucontroller->GetVoltageInVolt(
            mp_wxsliderCPUcoreVoltage->GetValue() )
          )
        ) == SETTING_VOLTAGE_IS_SAFE
      )
    {
      //RemoveAttention(mp_wxstatictextFreqInMHz);
      RemoveAttention(mp_wxstatictextVoltageInVolt);
      //The button might have been disabled (e.g. because of overvoltage 
      //protection) .
      mp_wxbuttonApply->Enable() ;
      //Remove a possibble tooltip.
      mp_wxbuttonApply->SetToolTip( 
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("") );
    }
    else
    {
      wxString wxstr ;
      switch(byIsSafe)
      {
        case VOLTAGE_IS_TOO_HIGH_FOR_FREQUENCY:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("The voltage is too high for the frequency. So ")
            _T("setting the p-state is unsafe.") ;
          break;
        case NO_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("no appriate voltage-frequency pair was found from the ")
            _T("configuration file. So setting the p-state is unsafe.") ;
          break;
        case NO_VOLTAGE_FREQUENCY_PAIR_EXISTANT:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("There was NOT any voltage-frequency pair found (in the ")
            _T("configuration file ). So setting the p-state is unsafe.") ;
          break;
        case NO_APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND:
         {
           WORD wFreqInMHz = //m_pstate.GetFreqInMHz() ;
             mp_wxsliderFreqInMHz->GetValue() ;
           wxstr = wxString::Format( 
             //Necessary if using ASCII because wxString is wide char.
             wxString(
                //We need a _T() macro (wide char-> L"", char->"") for EACH 
                //line to make it compitable between char and wide char.
                _T("There were no 2 voltage-frequency pairs found (in the ")
                _T("configuration file )\n")
                _T("where\n")
                _T("-1 pair's frequency is above or equal %u and\n")
                _T("-1 pair's frequency is below %u.\n")
                _T("So setting the p-state is unsafe.")
              )
              , wFreqInMHz
              , wFreqInMHz 
              ) ;
         }
          break;
        case NOT_USING_ANY_OVERVOLTING_PROTECTION:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("Warning: The overvoltage protection is disabled. (")
            _T("Look inside the ")
            _T("configuration file ). So setting the p-state is (VERY) risky.\n")
            _T("Before you apply the settings:\n")
            _T("-You have to be sure what you do and \n-you have to pay attention") ;
          break;
      }
      //SetAttention( mp_wxstatictextFreqInMHz, wxstr );
      SetAttention( mp_wxstatictextVoltageInVolt ,
        //_T("the voltage is too high for the freq")
        wxstr );
      if( byIsSafe != NOT_USING_ANY_OVERVOLTING_PROTECTION )
      {
          mp_wxbuttonApply->Disable() ;
          mp_wxbuttonApply->SetToolTip( 
            //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("This button has been disabled ")
            _T("because of overvoltage protection")
            ) ;
      }
    }
    //p_wxwindow->SetFocus();
    mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings = 
      //m_pstate.GetFreqInMHz() ;
      mp_wxsliderFreqInMHz->GetValue() ;
    //Only when true the cross is drawn.
    mp_mainframe->m_bDrawFreqAndVoltagePointForCurrCoreSettings = true ;
    //Force redraw of the client area.
    mp_mainframe->//ReDraw() ;
      Refresh() ;
}

void FreqAndVoltageSettingDlg::HandleMultiplierValueChanged()
{
  BYTE byCurrentCPUcoreFrequencyMultiplierValue =
      mp_wxsliderCPUcoreFrequencyMultiplier->GetValue() ;
  //Commented for Pentium M
  //if( byCurrentCPUcoreFrequencyMultiplierValue > //14 
  //    mp_pumastatectrl->mp_model->m_cpucoredata.
  //    //m_byMainPLLoperatingFrequencyIDmax 
  //    GetMainPLLoperatingFrequencyIDmax()
  //  )
  //{
  //  //To reduce flickering.
  //  if( m_byPreviousMultiplierValue //< 15 
  //      <= mp_pumastatectrl->mp_model->m_cpucoredata.
  //        //m_byMainPLLoperatingFrequencyIDmax 
  //        GetMainPLLoperatingFrequencyIDmax() 
  //    )
  //      SetAttention(mp_wxsliderCPUcoreFrequencyMultiplier,
  //        _T("The multiplier is above the specification.\r\n"
  //        "You will surely not get the expected freqeuency.\n"
  //        "And even AMD power monitor showed wrong frequencies.\n"
  //        "You can verify this with a benchmark.")
  //        );
  //}
  //else if( //byCurrentCPUcoreFrequencyMultiplierValue < //14/2
  //    //mp_pumastatectrl->mp_model->m_cpucoredata.
  //    //  m_byMainPLLoperatingFrequencyIDmax/2
  //    //AMD spec says: FID + 8 must be > 50% of max FID + 8 
  //    byCurrentCPUcoreFrequencyMultiplierValue + 8 <= 
  //    ( mp_pumastatectrl->mp_model->m_cpucoredata.
  //      //m_byMainPLLoperatingFrequencyIDmax
  //      GetMainPLLoperatingFrequencyIDmax() + 8 ) / 2
  //  )
  //{
  //  //To reduce flickering.
  //    if( //m_byPreviousMultiplierValue >= //14/2
  //        //mp_pumastatectrl->mp_model->m_cpucoredata.
  //        //m_byMainPLLoperatingFrequencyIDmax/2 
  //        m_byPreviousMultiplierValue + 8 > 
  //        ( mp_pumastatectrl->mp_model->m_cpucoredata.
  //          //m_byMainPLLoperatingFrequencyIDmax 
  //          GetMainPLLoperatingFrequencyIDmax() + 8 ) / 2 
  //      )
  //      SetAttention(mp_wxsliderCPUcoreFrequencyMultiplier,
  //        _T("The multiplier is below the specification."
  //        "You may not get the expected freqeuency."
  //        "Even AMD power monitor showed wrong frequencies if the multioplier "
  //        "was too high."
  //        "You can verify this with a benchmark.")
  //        );
  //}
  //else
  //    if( m_byPreviousMultiplierValue > //14 
  //        mp_pumastatectrl->mp_model->m_cpucoredata.
  //          //m_byMainPLLoperatingFrequencyIDmax
  //          GetMainPLLoperatingFrequencyIDmax()
  //        || 
  //        //m_byPreviousMultiplierValue < //14/2
  //        //mp_pumastatectrl->mp_model->m_cpucoredata.
  //        //m_byMainPLLoperatingFrequencyIDmax/2 
  //        m_byPreviousMultiplierValue + 8 <= 
  //        ( mp_pumastatectrl->mp_model->m_cpucoredata.
  //          //m_byMainPLLoperatingFrequencyIDmax 
  //          GetMainPLLoperatingFrequencyIDmax() + 8 ) / 2
  //      )
  //      RemoveAttention(mp_wxsliderCPUcoreFrequencyMultiplier);
  //HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreFrequencyMultiplier);
  m_byPreviousMultiplierValue = byCurrentCPUcoreFrequencyMultiplierValue ;
}

void FreqAndVoltageSettingDlg::OnScroll(wxScrollEvent & //WXUNUSED(wxscrollevent) 
  wxscrollevent )
{
  int i = 0 ;
  int nValue = mp_wxsliderCPUcoreVoltage->GetValue() ;
  switch(wxscrollevent.GetId())
  {
    case ID_PstateSlider :
      //DWORD dwLow, dwHigh;
      {
//      PState pstate ;
      //mp_pumastatectrl->GetPStateFromMSR(
        //mp_wxsliderCPUcorePstate->GetValue() , dwLow, dwHigh , m_pstate, m_byCoreID ) ;
      //mp_pumastatectrl->GetPstate(
      VoltageAndFreq voltageandfreq ;
      if( mp_cpucontroller->GetPstate(
        mp_wxsliderCPUcorePstate->GetValue() , voltageandfreq ) 
        )
      {
//      m_pstate = pstate ;
      //mp_wxcheckboxValidPstate->SetValue(
      //  //"Bit 63 PstateEn"
      //  dwHigh >> 31 ) ;

      //mp_wxstatictextExpectedCurrentDissipation->SetLabel( 
      //  wxString::Format("expected current dissipation: %f Ampere", 
      //    PState::GetExpectedPowerDissipation(dwHigh) 
      //    )
      //  ) ;
        HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcorePstate);
        //mp_wxsliderCPUcoreFrequencyMultiplier->SetValue(m_pstate.m_byFreqID) ;
        //mp_wxsliderCPUcoreDivisorID->SetValue(m_pstate.m_byDivisorID) ;
        mp_wxsliderFreqInMHz->SetValue( //m_pstate.GetFreqInMHz() 
          voltageandfreq.m_wFreqInMHz ) ;
        mp_wxsliderCPUcoreVoltage->SetValue( //m_pstate.GetVoltageID() 
          mp_cpucontroller->GetVoltageID(voltageandfreq.m_fVoltageInVolt) ) ;
        VoltageIDchanged(nValue) ;
      }
      }
      break;
  //case ID_DivisorSlider:
  //    //m_did_and_fid.SetDivisorID( mp_wxsliderCPUcoreDivisorID->GetValue() ) ;
  //    m_pstate.SetDivisorID( mp_wxsliderCPUcoreDivisorID->GetValue() ) ;

  //    HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreDivisorID);
  //    mp_wxsliderFreqInMHz->SetValue(m_pstate.GetFreqInMHz() ) ;
  //  break;
  //case ID_MultiplierSlider:
  //    //m_did_and_fid.SetDivisorID( mp_wxsliderCPUcoreDivisorID->GetValue() ) ;
  //    m_pstate.SetFrequencyID( mp_wxsliderCPUcoreFrequencyMultiplier->GetValue() ) ;
  //    mp_wxsliderFreqInMHz->SetValue(m_pstate.GetFreqInMHz() ) ;
  //    //calls HandleCPUcoreFrequencyOrVoltageChanged(...)
  //    HandleMultiplierValueChanged();
  //    break;
  case ID_FrequencySlider:
      //mp_wxsliderFreqInMHz->SetLabel(wxString::Format("%u", 
      //  PState::GetFreqInMHz(
      //    mp_wxsliderCPUcoreFrequencyMultiplier->GetValue(),
      //    mp_wxsliderCPUcoreDivisorID->GetValue() 
      //    )
      //    )
      //  );
      //Just for testing:
      //mp_wxsliderFreqInMHz->SetValue(1100) ;
      {
      WORD wSliderValue = mp_wxsliderFreqInMHz->GetValue() ;
      if( wSliderValue < 
          m_wPreviousFrequencyInMHz )
          m_wPreviousFrequencyInMHz = SetNearestLowerPossibleFreqInMHz( 
            wSliderValue );
      else if( wSliderValue > m_wPreviousFrequencyInMHz )
          m_wPreviousFrequencyInMHz = SetNearestHigherPossibleFreqInMHz( 
            wSliderValue );

      mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings = 
          m_wPreviousFrequencyInMHz ;
      //else
      //    if( mp_wxsliderFreqInMHz->GetValue() > 
      //    m_wPreviousFrequencyInMHz )
      //    SetNearestHigherPossibleFreqInMHz();
      //m_wPreviousFrequencyInMHz = wSliderValue ;
      }
      break ;
  case ID_VoltageSlider:
    VoltageIDchanged(nValue) ;
      //m_pstate.m_byVoltageID = nValue ;
    break;
  }
  OutputFreqAndVoltageByControlValues() ;
}

void FreqAndVoltageSettingDlg::VoltageIDchanged(int nNewValue)
{
  if( nNewValue < //mp_pumastatectrl->mp_model->m_cpucoredata.m_byMinVoltageID 
    mp_cpucontroller->GetMinimumVoltageID() )
    //mp_wxsliderCPUcoreVoltage->SetToolTip(_T("the voltage is above the spec."));
    SetAttention(mp_wxsliderCPUcoreVoltage,
    _T("the voltage is above the specification."));
  else if( nNewValue > //mp_pumastatectrl->mp_model->m_cpucoredata.m_byMaxVoltageID 
    mp_cpucontroller->GetMaximumVoltageID()
    )
    //mp_wxsliderCPUcoreVoltage->SetToolTip(
    SetAttention( mp_wxsliderCPUcoreVoltage,
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compitable between char and wide char.
      _T("the voltage is below the specification.")
      _T("->this will surely have no effect"));
  //else
  //  ;
  mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings =
    //PState::GetVoltageInVolt(nNewValue) ;
    mp_cpucontroller->GetVoltageInVolt(nNewValue) ;
  HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreVoltage);
  mp_mainframe->Refresh() ;//rm_timer.
}

void FreqAndVoltageSettingDlg::OutputFreqAndVoltageByControlValues()
{
  //mp_wxstatictextFreqInMHz->SetLabel(wxString::Format("%u", 
  //  PState::GetFreqInMHz(
  //    mp_wxsliderCPUcoreFrequencyMultiplier->GetValue(),
  //    mp_wxsliderCPUcoreDivisorID->GetValue() 
  //    )
  //    )
  //  );
  mp_wxstatictextVoltageInVolt->SetLabel(//_T("0.8")
    wxString::Format(
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compitable between char and wide char.
      _T("%f"), //PState::GetVoltageInVolt(
      mp_cpucontroller->GetVoltageInVolt(
      mp_wxsliderCPUcoreVoltage->GetValue() )
      )
    ) ;
}

void FreqAndVoltageSettingDlg::OnActivate(wxActivateEvent & r_activateevent )
{
  if( r_activateevent.GetActive() )
  {
    //mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings = 
    //  //PState::GetVoltageInVolt(m_pstate.m_byVoltageID) ;
    //  mp_cpucontroller->GetVoltageInVolt( 
    //  ) ;
    mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings =
      m_wPreviousFrequencyInMHz ;
    mp_mainframe->m_vbAnotherWindowIsActive = true ;
  }
  else
    mp_mainframe->m_vbAnotherWindowIsActive = false ;
}

void FreqAndVoltageSettingDlg::OnApplyButton(wxCommandEvent & //WXUNUSED(event) 
  r_wxcommandevent )
{
  int i = 0 ;
  BYTE byPstateNumber = 0 ;
  byPstateNumber = mp_wxsliderCPUcorePstate->GetValue() ;
  //PState pstate(mp_wxsliderCPUcoreVoltage->GetValue(),
  //  mp_wxsliderCPUcoreDivisorID->GetValue(),
  //  mp_wxsliderCPUcoreFrequencyMultiplier->GetValue(),//byPstateID
  //  //2
  //  //0
  //  ////Alternate the p-state ID because of that: 
  //  ////"The PstateId field must be updated to cause a new CpuFid value to take effect."
  //  //m_byPstateID = //(~m_byPstateID) & 1 
  //  //! m_byPstateID
  //  byPstateNumber
  //  );
  if( //byPstateNumber == 7 
    mp_wxcheckboxCOFVIDcontrol->GetValue() )
  {
//    mp_pumastatectrl->GetAsLowWord 
    //mp_pumastatectrl->WriteToCOFVID(pstate,m_byCoreID) ;
  }
  else
  {
  //setVidAndFrequencyForPState_Puma();

  DWORD dwMSRHigh = 0 ;
  DWORD dwMSRLow = 0 ;
  //::wxMessage
  //::wxMessageBox(wxString::Format("VID:%u FID:%u DID:%u", 
  //  pstate.m_byVoltageID
  //  ,pstate.m_byFreqID
  //  ,pstate.m_byDivisorID )
  //  ) ;
  //TODO the confirmation of e.g. wxWidgets seems to happen in 
  //ANOTHER thread->synchronize here (by e.g. using critical sections)
  //mp_pumastatectrl->setVidAndFrequencyForPState_Puma(
  mp_cpucontroller->SetVoltageAndFrequency(
    //mp_pumastatectrl->GetMSRregisterForPstate(
    //  //Use p-state 3 for setting for dyn. freq. scaling
    //  //because setting via the MSR register 0xC0010070
    //  //did not work.
    //  //2
    //  byPstateNumber
    //  )
    //byPstateNumber
    mp_cpucontroller->GetVoltageInVolt(
      mp_wxsliderCPUcoreVoltage->GetValue() )
    //////Setzen im COFVID_CONTROL_REGISTER hat NICHT funktioniert:
    //////es wurde nur die Spannung gesenkt, nicht aber die Takfrequenz
    ////COFVID_CONTROL_REGISTER
    //, pstate
    ////m_pstateFromMSR,
    ////pstate2,
    //, pstate
    //, dwMSRHigh
    //, dwMSRLow
    , 
    //m_byCoreID
    mp_wxsliderFreqInMHz->GetValue()
    ) ;
  }
  if( mp_wxcheckboxSetAsCurrentAfterApplying->IsChecked() )
  {
    mp_mainframe->PossiblyAskForOSdynFreqScalingDisabling() ;
    //mp_pumastatectrl->SetPstate( byPstateNumber,
    //mp_cpucontroller->SetPstate( byPstateNumber,
    //  //1 = 1bin
    //  //1 << m_byCoreID 
    //  m_byCoreID
    //  );
  }
  //mp_mainframe->SetMenuItemLabel(m_byCoreID, byPstateNumber, pstate ) ; 
}

void FreqAndVoltageSettingDlg::RemoveAttention(wxWindow * p_wxwindow)
{
  //p_wxwindow->ClearBackground();
  p_wxwindow->SetOwnBackgroundColour(
    //http://docs.wxwidgets.org/stable/wx_wxwindow.html#wxwindowsetbackgroundcolour:
    //"[...]pass wxNullColour to reset to the default colour."
    wxNullColour);
  //force repaint the "dirty" way: hide and show immediately.
  //because ClearBackground(),Refresh() and Update() didn't bring the 
  //desired IMMEDIATE FULL repaint.
      //p_wxwindow->Show(false);
      //p_wxwindow->Show(true);
      ////After hiding a window it looses the focus.
      //p_wxwindow->SetFocus();
  //p_wxwindow->ClearBackground();
  //p_wxwindow->Refresh();
  //p_wxwindow->Update();
  p_wxwindow->SetToolTip(_T(""));
}

void FreqAndVoltageSettingDlg::SetAttention(
  wxWindow * p_wxwindow, 
  const wxString & wxstr // = _T("")
  )
{
  //p_wxwindow->ClearBackground();
  p_wxwindow->SetOwnBackgroundColour(*wxRED);
  //force repaint the "dirty" way: hide and show immediately.
  //because ClearBackground(),Refresh() and Update() didn't bring the 
  //desired IMMEDIATE FULL repaint.
      //p_wxwindow->Show(false);
      //p_wxwindow->Show(true);
      ////After hiding a window it looses the focus.
      //p_wxwindow->SetFocus();
  //p_wxwindow->Refresh();
  //p_wxwindow->Update();
  p_wxwindow->SetToolTip(wxstr);
}

WORD FreqAndVoltageSettingDlg::SetNearestHigherPossibleFreqInMHz(
    WORD wFreqInMHz
    )
{
    WORD wNewFreqInMhz ;
    //DIDandFID didandfid ;
    //PState pstate ;
    //The FID and the DID may have changes in the above lines close to here, 
    //so recalculate.
    //wNewFreqInMhz = PState::GetFreqInMHz(byFID,byDivisorID) ;
    //pstate 
    //DIDandFID didandfid = //mp_pumastatectrl->

    wNewFreqInMhz =
        //GetNearestHigherPossibleFreqInMHzAsDIDnFID(wFreqInMHz)
        mp_cpucontroller->GetNearestHigherPossibleFreqInMHz(wFreqInMHz)
        //.GetFreqInMHz() 
        ;

    //m_pstate.SetDIDandFID(didandfid);
    //wNewFreqInMhz = didandfid.GetFreqInMHz() ;

    //mp_wxsliderCPUcoreFrequencyMultiplier->SetValue(didandfid.m_byFreqID) ;
    //mp_wxsliderCPUcoreDivisorID->SetValue(didandfid.m_byDivisorID) ;
    if( wNewFreqInMhz != 0 )
    {
      HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderFreqInMHz);

      mp_wxsliderFreqInMHz->SetValue( //PState::GetFreqInMHz(byFID,byDivisorID) 
          wNewFreqInMhz
          ) ;
    }
    return wNewFreqInMhz ;
}

//TODO move to class PumaStateCtrl.
//void 
WORD FreqAndVoltageSettingDlg::SetNearestLowerPossibleFreqInMHz(
    WORD wFreqInMHz
    )
{
    WORD wNewFreqInMhz ;

    //wNewFreqInMhz = PState::GetFreqInMHz(byFID,byDivisorID) ;
    //DIDandFID didandfid = //mp_pumastatectrl->
        //GetNearestLowerPossibleFreqInMHzAsDIDnFID(wFreqInMHz) ;

    wNewFreqInMhz =
      mp_cpucontroller->GetNearestLowerPossibleFreqInMHz(wFreqInMHz) ;
    //m_pstate.SetDIDandFID(didandfid);
    //wNewFreqInMhz = didandfid.GetFreqInMHz() ;

    //mp_wxsliderCPUcoreFrequencyMultiplier->SetValue(didandfid.m_byFreqID) ;
    //mp_wxsliderCPUcoreDivisorID->SetValue(didandfid.m_byDivisorID) ;
    if( wNewFreqInMhz != 0 )
    {
      HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderFreqInMHz);

      mp_wxsliderFreqInMHz->SetValue( //PState::GetFreqInMHz(byFID,byDivisorID) 
          wNewFreqInMhz
          ) ;
    }
    return wNewFreqInMhz ;
}
