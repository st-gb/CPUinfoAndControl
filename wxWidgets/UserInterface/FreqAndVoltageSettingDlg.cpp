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

#include <global.h> //for BYTE
#include "FreqAndVoltageSettingDlg.hpp"
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <ModelData/CPUcoreData.hpp>
#include <ModelData/ModelData.hpp>
#include <wx/button.h> //for class wxButton
#include <wx/checkbox.h> //for class wxCheckBox
#include <wx/dialog.h> //for class wxDialog
#include <wx/msgdlg.h> //for ::wxMessageBox(...)
#include <wx/sizer.h> //for class wxBoxSizer
#include <wx/slider.h> //for class wxSlider
#include <wx/spinbutt.h> //class wxSpinButton 
//for a line which may be used in a dialog to separate the groups of controls. 
#include <wx/statline.h> 
#include <wx/stattext.h> //for wxStaticText
#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include "MainFrame.hpp" //for class MyFrame

//An enum guarantees a unique number for each element.
enum
{
  ID_DivisorSlider = 1,
  ID_PstateSlider ,
  ID_MultiplierSlider,
  ID_VoltageSlider
  , ID_FrequencySlider
  , ID_SetAsMinVoltage
  , ID_SetAsWantedVoltage
  //, ID_SpinVoltageUp
  //, ID_SpinVoltageDown
  , ID_SpinVoltage //= 182
  , ID_DecreaseVoltage
};

BEGIN_EVENT_TABLE(FreqAndVoltageSettingDlg, wxDialog)
  EVT_ACTIVATE(FreqAndVoltageSettingDlg::OnActivate) 
  EVT_SCROLL(FreqAndVoltageSettingDlg::OnScroll)
  EVT_BUTTON(wxID_APPLY, FreqAndVoltageSettingDlg::OnApplyButton)
  EVT_BUTTON(ID_SetAsMinVoltage, 
    FreqAndVoltageSettingDlg::OnSetAsMinVoltageButton)
  EVT_BUTTON(ID_SetAsWantedVoltage, 
    FreqAndVoltageSettingDlg::OnSetAsWantedVoltageButton)
#if wxUSE_SPINBTN
  //EVT_SPIN_UP(ID_SpinVoltage, FreqAndVoltageSettingDlg::OnSpinVoltageUp)
  //EVT_SPIN_DOWN(ID_SpinVoltage, FreqAndVoltageSettingDlg::OnSpinVoltageDown)
  //EVT_SPIN(ID_SpinVoltage, FreqAndVoltageSettingDlg::OnSpinVoltageUp)
#endif
  EVT_BUTTON( ID_DecreaseVoltage, FreqAndVoltageSettingDlg::OnDecVoltage)
  EVT_BUTTON( ID_SpinVoltage, FreqAndVoltageSettingDlg::OnIncVoltage)
END_EVENT_TABLE()

inline void FreqAndVoltageSettingDlg::AddCPUcoreCheckBoxSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  wxSizer * p_wxboxsizerCPUcoreCheckBox = new wxBoxSizer(wxHORIZONTAL);
  p_wxboxsizerCPUcoreCheckBox->Add(
    new wxStaticText(this, wxID_ANY,
    //_T("core frequency in MHz: ")));
    _T("affected\ncores"//\nin MHz"
      ":")
      )
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT |
    wxALIGN_TOP
    , 5 //Determines the border width
    );
  WORD wNumCPUcores = mp_model->m_cpucoredata.GetNumberOfCPUcores() ;
  m_ar_p_wxcheckbox = new wxCheckBox * [ wNumCPUcores ] ;
  if( m_ar_p_wxcheckbox ) //Allocating the array succeeded.
    for( WORD wCoreID = 0 ; wCoreID < wNumCPUcores ; ++ wCoreID )
    {
      m_ar_p_wxcheckbox[ wCoreID ] = new wxCheckBox(this,wxID_ANY,
        wxString::Format(wxT("%u"), wCoreID) ) ;
      p_wxboxsizerCPUcoreCheckBox->Add(
        m_ar_p_wxcheckbox[ wCoreID ]
        , 1
        //,wxEXPAND | wxALL
        , wxLEFT | wxRIGHT
        , 5 );
    }
  p_wxsizerSuperordinate->Add(//mp_wxsliderFreqInMHz,
    p_wxboxsizerCPUcoreCheckBox ,
//    1 ,
    0 ,//0=the control should not take more space if the sizer is enlarged
    wxEXPAND | //wxALL
      wxBOTTOM
    ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to
    //include any border flag."
    //10
    2
    );
}

inline void FreqAndVoltageSettingDlg::AddCPUcoreFrequencySizer(
  wxSizer * p_wxsizerSuperordinate )
{
  p_wxboxsizerCPUcoreFrequencyInMHz = new wxBoxSizer(wxHORIZONTAL);
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    new wxStaticText(this, wxID_ANY,
    //_T("core frequency in MHz: ")));
    _T("core\nfrequency"//\nin MHz"
      ":")
      )
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT |
    wxALIGN_TOP
    , 5 //Determines the border width
    );

  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxsliderFreqInMHz
    , 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5 );

  mp_wxstatictextFreqInMHz = new wxStaticText(this, wxID_ANY, _T("") );
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxstatictextFreqInMHz
    , 1
    , wxLEFT | wxRIGHT
    , 5 );
  p_wxsizerSuperordinate->Add(//mp_wxsliderFreqInMHz,
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
}

inline void FreqAndVoltageSettingDlg::AddCPUcoreVoltageSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  wxButton * p_wxbuttonDecBy1VoltageStep ;
  wxButton * p_wxbuttonIncBy1VoltageStep ;
  wxBoxSizer * p_wxboxsizerCPUcoreVoltage = new wxBoxSizer(wxHORIZONTAL);
  p_wxboxsizerCPUcoreVoltage->Add(
    new wxStaticText(this, wxID_ANY, _T("voltage" //\nIDentifier
      ":" ))
    , 0 //0=the control should not take more space if the sizer is enlarged
    //These flags are used to specify which side(s) of the sizer item the border width will apply to.
    , //wxEXPAND |
    //wxALL,
    wxLEFT | wxRIGHT |
    //wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
    wxALIGN_TOP
    , 3 //Determines the border width
    );
  p_wxboxsizerCPUcoreVoltage->Add(//p_wxsliderCPUcoreVoltage
    mp_wxsliderCPUcoreVoltage,
    1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5);
  mp_wxstatictextVoltageInVolt = new wxStaticText(this, wxID_ANY, _T("") );
  p_wxboxsizerCPUcoreVoltage->Add(
    mp_wxstatictextVoltageInVolt ,
    0 //0=the control should not take more space if the sizer is enlarged
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
    , 5);
  wxSize wxsizeMinimal(-1,-1) ;
  p_wxbuttonIncBy1VoltageStep = new wxButton( this
    , ID_SpinVoltage
    , wxT("&+")
    ) ;
  p_wxboxsizerCPUcoreVoltage->Add( //mp_wxspinbuttonVoltageInVolt
    p_wxbuttonIncBy1VoltageStep
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
  p_wxbuttonDecBy1VoltageStep = new wxButton( this
    , ID_DecreaseVoltage
    , wxT("&-")
    , wxDefaultPosition
    , wxsizeMinimal
    ) ;
  p_wxboxsizerCPUcoreVoltage->Add( //mp_wxspinbuttonVoltageInVolt
    p_wxbuttonDecBy1VoltageStep
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
  p_wxsizerSuperordinate->Add(
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
}

inline void FreqAndVoltageSettingDlg::AddPerformanceStateSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  wxBoxSizer * p_wxboxsizerCPUcorePstate = new wxBoxSizer(wxHORIZONTAL);
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
  p_wxsizerSuperordinate->Add(
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
}

FreqAndVoltageSettingDlg::FreqAndVoltageSettingDlg(
  wxWindow * parent
  , I_CPUcontroller * p_cpucontroller
  , BYTE byCoreID
  )
  : wxDialog( 
      parent, 
      wxID_ANY, 
#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
      wxString::Format( 
      //We need a wxT() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      wxT("settings for core %u") ,
      byCoreID 
    )
#else //#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
    wxT("voltage and frequency settings")
#endif //#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
    , wxPoint(30, 30) //A value of (-1, -1) indicates a default size
    , wxSize(400, 400)
    , wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  , m_byCoreID(byCoreID)
  , m_byPstateID(0)
  , mp_cpucontroller (p_cpucontroller)
  , mp_mainframe( (MainFrame *) parent )
  , mp_model ( p_cpucontroller->mp_model )
{
  LOGN("voltage and freq dialog begin")
  //http://wiki.wxwidgets.org/Avoiding_Memory_Leaks#Child_windows:
  //"When a wxWindow is destroyed, it automatically deletes all its children. 
  //These children are all the objects that received the window as the
  //parent-argument in their constructors.
  //As a consequence, if you're creating a derived class that contains child
  //windows, you should use a pointer to the child windows instead of the
  //objects themself as members of the main window."
//  wxBoxSizer * p_wxboxsizerValidPstate = new wxBoxSizer(wxHORIZONTAL);
//  wxBoxSizer * p_wxboxsizerCPUcoreDivisor = new wxBoxSizer(wxHORIZONTAL);
//  wxBoxSizer * p_wxboxsizerCPUcoreFrequencyMultiplier = new wxBoxSizer(wxHORIZONTAL);
  //wxBoxSizer *
//  wxBoxSizer * p_wxboxsizerCPUcoreVoltageInVolt = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * p_wxboxsizerOK_Cancel = new wxBoxSizer(wxHORIZONTAL);
//  wxBoxSizer * p_wxboxsizerSetAsMinVoltage = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer * sizerTop = new wxBoxSizer(wxVERTICAL);
//  wxButton * p_wxbuttonDecBy1VoltageStep ;
//  wxButton * p_wxbuttonIncBy1VoltageStep ;
  wxFlexGridSizer * p_wxflexgridsizerSetAsMinVoltage = new wxFlexGridSizer(
    //number of columns: zero, it will be calculated to form the total 
    //number of children in the sizer
    0 );
//  wxFlexGridSizer * p_wxflexgridsizerCPUcoreVoltageInVolt = new
//    wxFlexGridSizer(
//    //number of columns: zero, it will be calculated to form the total
//    //number of children in the sizer
//    0 );

  //mp_wxbuttonApply = new wxButton(this, wxID_APPLY ) ;
  mp_wxbuttonApply = new wxButton(this, wxID_APPLY, _T("&Write p-state") ) ;

  wxString wxstrToolTip = wxT("mustn't write because ");
  bool bSetToolTip = false ;
  if( ! mp_model->m_cpucoredata.m_arfAvailableMultipliers )
  {
    wxstrToolTip += wxT("no multipliers are available") ;
    bSetToolTip = true ;
  }
  if( ! mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt )
  {
    bSetToolTip = true ;
    if( //! wxstrToolTip.Empty()
        bSetToolTip )
      wxstrToolTip += wxT(" and ") ;
    wxstrToolTip += wxT("no voltages are available") ;
  }
  if( bSetToolTip )
  {
    mp_wxbuttonApply->Enable(false) ;
    //TODO tooltip is not permanent
    mp_wxbuttonApply->SetToolTip( wxstrToolTip ) ;
  }
  mp_wxbuttonSetAsWantedVoltage = new wxButton( this, 
    ID_SetAsWantedVoltage, wxT("set as wanted voltage") ) ;
  mp_wxbuttonSetAsMinVoltage = new wxButton( this, 
    ID_SetAsMinVoltage, wxT("set as minimum voltage") ) ;
  //wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, _T("Apply"));
  CreateSliders();

//  mp_wxcheckboxValidPstate = new wxCheckBox(this, wxID_ANY,
//      //_T("Set as current after apply") ) ;
//      _T("valid p-state") ) ;

//  mp_wxcheckboxCOFVIDcontrol = new wxCheckBox(this, wxID_ANY,
//      //_T("Set as current after apply") ) ;
//      _T("write into COFVID control register") ) ;
  mp_wxcheckboxbuttonAlsoSetWantedVoltage = new wxCheckBox(this, wxID_ANY,
    wxT("also set wanted voltage:") ) ;

  mp_wxstatictextExpectedCurrentDissipation = new wxStaticText(
    this, wxID_ANY, _T("") 
    ) ;
  mp_wxstatictextWantedVoltageInVolt = new wxStaticText(
    this, wxID_ANY, _T("") 
    ) ;

  //mp_wxsliderCPUcorePstate->SetBackgroundStyle(wxBG_STYLE_COLOUR);
  //mp_wxsliderCPUcorePstate->SetForegroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnBackgroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnForegroundColour(*wxBLUE);
  //mp_wxsliderCPUcorePstate->SetToolTip("too high");

  //m_vecp_wxcontrol.push_back(p_wxsliderCPUcoreFrequencyMultiplier);

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

  //Only display the checkbox options if more than 1 logical CPU core exists.
  //(else if 1 CPU core then it is clear that the 1st and only core should be
  // used)
  if( mp_model->m_cpucoredata.GetNumberOfCPUcores() > 1 )
    AddCPUcoreCheckBoxSizer(sizerTop) ;
  AddPerformanceStateSizer(sizerTop) ;

  AddCPUcoreFrequencySizer(sizerTop) ;

  AddCPUcoreVoltageSizer(sizerTop) ;
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

  //p_wxboxsizerCPUcoreVoltageInVolt->Add(
//  p_wxflexgridsizerCPUcoreVoltageInVolt->Add(
//    new wxStaticText( this, wxID_ANY,
//      //_T("core voltage in Volt: ") )
//      _T("->resulting core voltage in Volt: ")
//      )
//    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
//    //[...]can change its size in the main orientation of the wxBoxSizer -
//    //where 0 stands for not changeable[...]
//    , 1
//    , //wxEXPAND |
//    wxFIXED_MINSIZE | wxALL
//    , 0
//    );
  //p_wxboxsizerCPUcoreVoltageInVolt->Add( mp_wxstatictextVoltageInVolt
//  p_wxflexgridsizerCPUcoreVoltageInVolt->Add( mp_wxstatictextVoltageInVolt
//    , 1
//    , wxEXPAND | wxALL
//    , 0 );
  //mp_wxspinbuttonVoltageInVolt = new wxSpinButton(this, //wxID_ANY
  //  ID_SpinVoltage ) ;
  //mp_wxspinbuttonVoltageInVolt->SetRange( 
  //  mp_i_cpucontroller->GetMinimumVoltageInVolt() //-> Millivolt 
  //    * 1000 , 
  //  mp_i_cpucontroller->GetMaximumVoltageInVolt() //-> Millivolt 
  //    * 1000 
  //  );
  //mp_wxspinbuttonVoltageInVolt->SetValue(
  //    mp_i_cpucontroller->GetMinimumVoltageInVolt() * 1000 );

//  p_wxbuttonIncBy1VoltageStep = new wxButton( this
//    , ID_SpinVoltage
//    , wxT("&+")
//    ) ;
//  p_wxflexgridsizerCPUcoreVoltageInVolt->Add( //mp_wxspinbuttonVoltageInVolt
//    p_wxbuttonIncBy1VoltageStep
//    , 0
//    , //wxEXPAND |
//    wxALL
//    , 0 );
//  p_wxbuttonDecBy1VoltageStep = new wxButton( this
//    , ID_DecreaseVoltage
//    , wxT("&-")
//    ) ;
//  p_wxflexgridsizerCPUcoreVoltageInVolt->Add( //mp_wxspinbuttonVoltageInVolt
//    p_wxbuttonDecBy1VoltageStep
//    , 1
//    , wxEXPAND | wxALL
//    , 0 );
//  sizerTop->Add(
//    //p_wxboxsizerCPUcoreVoltageInVolt,
//    p_wxflexgridsizerCPUcoreVoltageInVolt ,
//    //proportion parameter: if "0" it takes the least space
//    0 ,
//    //1 ,
//    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
//    //"If you would rather have a window item stay the size it started with
//    //then use wxFIXED_MINSIZE. "
//    wxEXPAND |
//    //wxFIXED_MINSIZE |
//    wxALL,
//    //Determines the border width, if the flag  parameter is set to include
//    //any border flag.
//    2);

  //p_wxboxsizerSetAsMinVoltage->Add(
  p_wxflexgridsizerSetAsMinVoltage->Add(
    mp_wxbuttonSetAsMinVoltage
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //[...]can change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable[...]
    , 1
    , //wxEXPAND |
    wxFIXED_MINSIZE | wxALL
    , 0
    );
  //p_wxboxsizerSetAsMinVoltage->Add(
  p_wxflexgridsizerSetAsMinVoltage->Add(
    mp_wxcheckboxbuttonAlsoSetWantedVoltage
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //[...]can change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable[...]
    , 1
    , //wxEXPAND |
    wxFIXED_MINSIZE | wxALL
    , 0 //Determines the border width
    );
//  p_wxboxsizerSetAsMinVoltage->Add(
  p_wxflexgridsizerSetAsMinVoltage->Add(
    mp_wxstatictextWantedVoltageInVolt
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //[...]can change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable[...]
    , 1
    , //wxEXPAND |
    wxFIXED_MINSIZE | wxALL
    , 0
    );
  LOGN("before adding the set as min voltage sizer")
  sizerTop->Add(
    //mp_wxbuttonSetAsMinVoltage, 
    //p_wxboxsizerSetAsMinVoltage ,
    p_wxflexgridsizerSetAsMinVoltage ,
    //proportion parameter: if "0" it takes the least space
    0 ,
    //1 ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"If you would rather have a window item stay the size it started with
    //then use wxFIXED_MINSIZE. "
    //wxEXPAND |
    wxFIXED_MINSIZE |
    wxALL,
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2);
  
  sizerTop->Add(
    mp_wxbuttonSetAsWantedVoltage, 
    //proportion parameter: if "0" it takes the least space
    0 ,
    //1 ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"If you would rather have a window item stay the size it started with
    //then use wxFIXED_MINSIZE. "
    //wxEXPAND |
    wxFIXED_MINSIZE |
    wxALL,
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 );
  //p_wxboxsizerOK_Cancel->Add(new wxButton(this, wxID_APPLY //, _T("Apply")
  //  ) );
  p_wxboxsizerOK_Cancel->Add( mp_wxbuttonApply ) ;
//  mp_wxcheckboxSetAsCurrentAfterApplying = new wxCheckBox(this, wxID_ANY,
//      //_T("Set as current after apply") ) ;
//      _T("Set as current p-state after write") ) ;
//  p_wxboxsizerOK_Cancel->Add(mp_wxcheckboxSetAsCurrentAfterApplying);
  p_wxboxsizerOK_Cancel->Add( new wxButton(this, wxID_CANCEL) );
  mp_wxcheckboxOnlySafeRange = new wxCheckBox(this, wxID_ANY,
    wxT("only safe range") ) ;
  p_wxboxsizerOK_Cancel->Add( mp_wxcheckboxOnlySafeRange) ;
  LOGN("before adding the OK etc. sizer")
  sizerTop->Add(
    p_wxboxsizerOK_Cancel, 
    0 , 
    wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );

  LOGN("before adding to the outer sizer")
  SetSizer(sizerTop);

  sizerTop->SetSizeHints(this);
  sizerTop->Fit(this);
  //"[...] wxToolTip::SetDelay can be used to globally alter tooltips behaviour"
  wxToolTip::SetDelay(5000);
  //To show the computed freq and voltage at the beginning yet.
  //OutputFreqAndVoltageByControlValues();
  //OutputVoltageByControlValues();
//  VoltageIDchanged(m_wVoltageID) ;
  //Force the neighbour controls of "voltage in volt" to be resized.
  Layout() ;
  HandlePstateMayHaveChanged() ;
  LOGN("end of voltage and freq dialog creation")
}

FreqAndVoltageSettingDlg::~FreqAndVoltageSettingDlg()
{
  //for(BYTE byControlIndex = 0 ; byControlIndex < //m_byCoreNumber
  //  m_vecp_wxcontrol.size() ; ++ byControlIndex )
  //{
  //  //Release memory from the heap.
  //  delete m_vecp_wxcontrol.at(byControlIndex) ;
  //}
  if( m_ar_p_wxcheckbox )
  {
//    WORD wNumCPUcores = mp_model->m_cpucoredata.GetNumberOfCPUcores() ;
//    if( m_ar_p_wxcheckbox )
//      for( WORD wCoreID = 0 ; wCoreID < wNumCPUcores ; ++ wCoreID )
//        delete m_ar_p_wxcheckbox [ wCoreID] ;
//    for( )
    delete [] m_ar_p_wxcheckbox ;
  }
}

void FreqAndVoltageSettingDlg::CreateSliders()
{
  BYTE byPstateIDtoShowSettingsFor = 0 ;
  VoltageAndFreq voltageandfreq ;
  LOGN("voltage and freq dialog creating sliders")
  mp_wxsliderCPUcorePstate  = new wxSlider(
    this
    //wxID_ANY, 
    , ID_PstateSlider 
    //Initial position for the slider.
    , //1 
    byPstateIDtoShowSettingsFor
    //Slider minimum value.
    , 0 
    //Slider maximum value. If e.g. 2 p-states: 1st is "0", 2nd & last is "1"
    //, NUMBER_OF_PSTATES - 1 
    , mp_cpucontroller->GetNumberOfPstates() - 1
    //, 2
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , wxSL_AUTOTICKS | wxSL_LABELS
    ) ;
  //wxSlider * p_wxsliderCPUcoreFrequencyMultiplier = new wxSlider(this, 
  //m_byPreviousMultiplierValue =
  //    m_pstate.m_byFreqID ;
  //mp_wxsliderCPUcoreFrequencyMultiplier = new wxSlider(
  //  this
  //  //wxID_ANY, 
  //  , ID_MultiplierSlider
  //  //, 2  //value
  //  , m_byPreviousMultiplierValue
  //  //Slider minimum value.
  //      //, 0
  //  //Slider max value.
  //      //, MAX_VALUE_FOR_FID 
  //  //If the default point (-1, -1) is specified then a default point is chosen.
  //  , wxPoint(-1, -1)
  //  //If the default size (-1, -1) is specified then a default size is chosen.
  //  , wxSize(-1, -1)
  //  , wxSL_AUTOTICKS //	Displays tick marks.
  //  | wxSL_LABELS //Displays minimum, maximum and value labels.
  //  ) ;
  BYTE byPstateExists = mp_cpucontroller->GetPstate(0,voltageandfreq) ;
  //If the p-state "0" does not exist.
  if( ! byPstateExists )
  {
    //TODO reactivate
//    //Disable the write button.
//    mp_wxbuttonApply->Enable( //p_cpucontroller->GetPstateSafefy() ==
//      //SETTING_VOLTAGE_IS_SAFE
//      false
//      ) ;
  }
  WORD wMaxValue = mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.
      size() ;
  if( wMaxValue )
    //If just 1 element, min and max should be "0".
    -- wMaxValue ;
  LOGN("voltage and freq dialog before creating CPU core frequency slider")
  mp_wxsliderFreqInMHz = new wxSlider(
    this
    //wxID_ANY, 
    , ID_FrequencySlider
    //, 2  //value
    , //Initial position for the slider.
      //mp_i_cpucontroller->GetMinimumFrequencyInMHz()
      //voltageandfreq.m_wFreqInMHz
      0
      //Slider minimum value.
    , //lowest adjustable frequency:
//    mp_cpucontroller->GetMinimumFrequencyInMHz()
    0
    //Slider max value.
//    , mp_cpucontroller->GetMaximumFrequencyInMHz()
    , wMaxValue
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , 
    //wxSL_AUTOTICKS //	Displays tick marks.
    //| 
//    wxSL_LABELS //Displays minimum, maximum and value labels.
    wxSL_TICKS
    ) ;
  LOGN("voltage and freq dialog after creating CPU core frequency slider")
//  m_wPreviousFrequencyInMHz = //mp_wxsliderFreqInMHz->GetValue();
//      GetCPUcoreFrequencyFromSliderValue() ;
  //HandleMultiplierValueChanged();
  //HandlePstateMayHaveChanged() ;
  //WORD wVoltageID

//  m_wVoltageID = mp_cpucontroller->GetVoltageID(
//    voltageandfreq.m_fVoltageInVolt ) ;
  //wxSlider * p_wxsliderCPUcoreVoltage = new wxSlider(this, 

  wMaxValue = mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.
      size() ;
  //Prevent value wrap to 65535
  if( wMaxValue )
    //If just 1 element, min and max should be "0".
    -- wMaxValue ;

  LOGN("voltage and freq dialog before creating voltage slider")
  mp_wxsliderCPUcoreVoltage = new wxSlider(
    this, 
    //wxID_ANY, 
    ID_VoltageSlider 
    , //Initial position for the slider.
      //mp_i_cpucontroller->GetMinimumVoltageID()
      //wVoltageID
      //m_wVoltageID
      0
      //slider minimum value
      //, 0
//    , mp_cpucontroller->GetMinimumVoltageID()
    , 0
    //, MAX_VALUE_FOR_VID //slider max value
//    , mp_cpucontroller->GetMaximumVoltageID()
    , wMaxValue
    //If the default point (-1, -1) is specified then a default point is chosen.
    , wxPoint(-1, -1)
    //If the default size (-1, -1) is specified then a default size is chosen.
    , wxSize(-1, -1)
    , wxSL_AUTOTICKS //| wxSL_LABELS
    ) ;
  ////Possibly force calc of wanted voltage.
  //VoltageIDchanged(wVoltageID) ;
  LOGN("voltage and freq dialog end of creating sliders")
}

float FreqAndVoltageSettingDlg::GetCPUcoreFrequencyFromSliderValue()
{
  float fMultiplier = GetMultiplierFromSliderValue() ;
  if( fMultiplier > 0.0 )
  {
    return fMultiplier * mp_cpucontroller->m_fReferenceClockInMHz ;
  }
  return 0.0 ;
}

float FreqAndVoltageSettingDlg::GetMultiplierFromSliderValue()
{
  WORD wMultiplierIndex = mp_wxsliderFreqInMHz->GetValue() ;
  CPUcoreData & r_cpucoredata = mp_model->m_cpucoredata ;
  LOGN("freq slider value:" << wMultiplierIndex
    << " reference clock:" << mp_cpucontroller->m_fReferenceClockInMHz
    << " number of multis in attribute data:" <<
    r_cpucoredata.m_stdset_floatAvailableMultipliers.size()
    << " address of avail. multis array: "
    << r_cpucoredata.m_arfAvailableMultipliers
    )
  if( // m_fReferenceClockInMHz <> 0
//      mp_cpucontroller->m_fReferenceClockInMHz &&
      wMultiplierIndex <
      r_cpucoredata.m_stdset_floatAvailableMultipliers.size()
      && r_cpucoredata.m_arfAvailableMultipliers
    )
  {
    float fMultiplier = r_cpucoredata.m_arfAvailableMultipliers[ wMultiplierIndex ];
    LOGN("multiplier corresponding to slider value:" << fMultiplier )
    return fMultiplier ;
  }
  return 0.0 ;
}

void FreqAndVoltageSettingDlg::HandleCPUcoreFrequencyOrVoltageChanged(
    wxWindow * p_wxwindow )
{
  LOGN("HandleCPUcoreFrequencyOrVoltageChanged")
    //mp_wxstatictextPercentageOfDefaultVoltage->SetText( 
    //    wxString("percent of default voltage") + ) ;

//  std::set<float>::const_iterator c_iter_stdset_fMultiplier =
//    mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.find(
//      mp_wxsliderFreqInMHz->GetValue()
//      ) ;
  float fFreq = GetCPUcoreFrequencyFromSliderValue() ;
  LOGN("CPU core freq from slider :" << fFreq )
  if( //c_iter_stdset_fMultiplier != mp_model->m_cpucoredata.
      //m_stdset_floatAvailableMultipliers.end() &&
      fFreq &&
      mp_wxcheckboxOnlySafeRange->IsChecked()
    //false
      )
  {
    BYTE byIsSafe = 0 ;
    //if( m_bPreviousCPUcoreFreqWas
    if(         (
        byIsSafe = mp_cpucontroller->GetPstateSafefy(
          ////mp_wxsliderCPUcoreVoltage->GetValue()
          //, //mp_wxsliderCPUcoreFrequencyMultiplier->GetValue()
          (WORD) //( *c_iter_stdset_fMultiplier *
          //mp_cpucontroller->m_fReferenceClockInMHz )
            fFreq
          , //mp_cpucontroller->GetVoltageInVolt(
            //mp_wxsliderCPUcoreVoltage->GetValue() )
          GetVoltageInVoltFromSliderValue()
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
        //line to make it compatible between char and wide char.
        _T("") );
    }
    else
    {
      wxString wxstr ;
      switch(byIsSafe)
      {
        case VOLTAGE_IS_TOO_HIGH_FOR_FREQUENCY:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compatible between char and wide char.
            _T("The voltage is too high for the frequency. So ")
            _T("setting the p-state is unsafe.") ;
          break;
        case NO_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compatible between char and wide char.
            _T("no appriate voltage-frequency pair was found from the ")
            _T("configuration file. So setting the p-state is unsafe.") ;
          break;
        case NO_VOLTAGE_FREQUENCY_PAIR_EXISTANT:
          wxstr = //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compatible between char and wide char.
            _T("There was NOT any voltage-frequency pair found (in the ")
            _T("configuration file ). So setting the p-state is unsafe.") ;
          break;
        case NO_APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND:
         {
           WORD wFreqInMHz = //m_pstate.GetFreqInMHz() ;
//             mp_wxsliderFreqInMHz->GetValue() ;
             //"(WORD)" to avoid g++ compiler warning
             // "converting to `WORD' from `float'"
             (WORD) GetCPUcoreFrequencyFromSliderValue() ;
           wxstr = wxString::Format( 
             //Necessary if using ASCII because wxString is wide char.
             wxString(
                //We need a _T() macro (wide char-> L"", char->"") for EACH 
                //line to make it compatible between char and wide char.
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
            //line to make it compatible between char and wide char.
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
            //line to make it compatible between char and wide char.
            _T("This button has been disabled ")
            _T("because of overvoltage protection")
            ) ;
      }
    }
  }
  else //The button may be disabled before. So enable it.
    mp_wxbuttonApply->Enable() ;

    //p_wxwindow->SetFocus();
    mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings = 
//      mp_wxsliderFreqInMHz->GetValue() ;
      //"(WORD)" to avoid g++ compiler warning
      // "converting to `WORD' from `float'"
      (WORD) GetCPUcoreFrequencyFromSliderValue() ;
    //Only when true the cross is drawn.
    mp_mainframe->m_bDrawFreqAndVoltagePointForCurrCoreSettings = true ;
    //Force redraw of the client area.
    mp_mainframe->//ReDraw() ;
      Refresh() ;
}

float FreqAndVoltageSettingDlg::GetVoltageInVoltFromSliderValue()
{
  LOGN("voltage slider value:" << mp_wxsliderCPUcoreVoltage->GetValue() )
  if( ! mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.empty() )
    return mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt[
      mp_wxsliderCPUcoreVoltage->GetValue() ] ;
  return 0.0 ;
}

void FreqAndVoltageSettingDlg::HandleMultiplierValueChanged()
{
//  BYTE byCurrentCPUcoreFrequencyMultiplierValue =
//      mp_wxsliderCPUcoreFrequencyMultiplier->GetValue() ;
  //Commented for Pentium M
  //if( byCurrentCPUcoreFrequencyMultiplierValue >
  //  )
  //{
  //  //To reduce flickering.
  //  if( m_byPreviousMultiplierValue
  //      <=
  //    )
  //      SetAttention(mp_wxsliderCPUcoreFrequencyMultiplier,
  //        _T("The multiplier is above the specification.\r\n"
  //        "You will surely not get the expected frequency.\n"
  //        "You can verify this with a benchmark.")
  //        );
  //}
  //else if(
  //    byCurrentCPUcoreFrequencyMultiplierValue <=
  //    (
  //  )
  //{
  //  //To reduce flickering.
  //    if(
  //        m_byPreviousMultiplierValue + 8 > 
  //        (
  //      )
  //      SetAttention(mp_wxsliderCPUcoreFrequencyMultiplier,
  //        _T("The multiplier is below the specification."
  //        "You may not get the expected freqeuency."
  //        "You can verify this with a benchmark.")
  //        );
  //}
  //else
  //    if( m_byPreviousMultiplierValue >
  //        ||
  //        m_byPreviousMultiplierValue + 8 <= 
  //        (
  //      )
  //      RemoveAttention(mp_wxsliderCPUcoreFrequencyMultiplier);
  //HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreFrequencyMultiplier);
  float fFreq //= //mp_cpucontroller->GetCPUcoreFrequencyInMHz(
    //wSliderValue ) ;
    //GetCPUcoreFrequencyFromSliderValue() ;
    ;
  float fMultiplier = GetMultiplierFromSliderValue() ;
  fFreq = fMultiplier * mp_cpucontroller->m_fReferenceClockInMHz ;
  m_wPreviousFrequencyInMHz =
    //"(WORD)" to avoid g++ compiler warning
    // "converting to `WORD' from `float'"
    (WORD) fFreq ;
  mp_wxstatictextFreqInMHz->SetLabel(
    wxString::Format(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      wxT("multi:%.3f\n%.3f MHz"), //PState::GetVoltageInVolt(
      fMultiplier, fFreq
      )
    );
  //The freq label may need more space now, so recalc its size.
  p_wxboxsizerCPUcoreFrequencyInMHz->RecalcSizes() ;
  mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings =
      m_wPreviousFrequencyInMHz ;
//  m_byPreviousMultiplierValue = byCurrentCPUcoreFrequencyMultiplierValue ;
}

void FreqAndVoltageSettingDlg::HandlePstateMayHaveChanged()
{
  VoltageAndFreq voltageandfreq ;
  if( mp_cpucontroller->GetPstate(
    mp_wxsliderCPUcorePstate->GetValue() , voltageandfreq ) 
    )
  {
    m_wVoltageID = mp_wxsliderCPUcoreVoltage->GetValue() ;
  //mp_wxcheckboxValidPstate->SetValue() ;

  //mp_wxstatictextExpectedCurrentDissipation->SetLabel( 
  //  wxString::Format("expected current dissipation: %f Ampere", 
  //    )
  //  ) ;
    //mp_wxsliderCPUcoreFrequencyMultiplier->SetValue(m_pstate.m_byFreqID) ;

//    //TODO
    //Avoid div by zero.
    if( mp_cpucontroller->m_fReferenceClockInMHz
        //Array may be NULL.
        && mp_model->m_cpucoredata.m_arfAvailableMultipliers
        && mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt
      )
    {
      float fMultiplier = voltageandfreq.m_wFreqInMHz /
          mp_cpucontroller->m_fReferenceClockInMHz ;
      BYTE byMultiIndex = mp_model->m_cpucoredata.GetIndexForClosestMultiplier(
        fMultiplier ) ;
      float fVoltageInVolt ;
      WORD wFreqInMHz =
        (WORD)
        ( (float) mp_model->m_cpucoredata.m_arfAvailableMultipliers[
        byMultiIndex ] * mp_cpucontroller->m_fReferenceClockInMHz ) ;
      //The multiplier for the calculated multiplier can be far away
      //from the closest available multiplier. E.g. maybe 1.0 V for 1600 MHz
      //for AMD Griffin where only 1100 and 2200 MHz are possible. So calc.
      //the voltage for 1100 MHz:
//      mp_cpucontroller->GetVoltageFromMultiplier(byMultiIndex) ;
      if( mp_cpucontroller->GetInterpolatedVoltageFromFreq(
          wFreqInMHz
          //ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
          , fVoltageInVolt
          , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
//          , cr_stdsetvoltageandfreqForInterpolation
          )
        )
      {
        BYTE by = mp_model->m_cpucoredata.GetIndexForClosestVoltage(
          fVoltageInVolt ) ;
        mp_wxsliderFreqInMHz->SetValue( //m_pstate.GetFreqInMHz()
          byMultiIndex ) ;
        HandleMultiplierValueChanged() ;
        mp_wxsliderCPUcoreVoltage->SetValue(by) ;
        VoltageIDchanged( by) ;
      }
      HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcorePstate);
    }
//    //TODO
//    mp_wxsliderCPUcoreVoltage->SetValue( //m_pstate.GetVoltageID()
//      //mp_cpucontroller->GetVoltageID(voltageandfreq.m_fVoltageInVolt)
//      mp_model->m_cpucoredata.GetIndexForClosestVoltage(
//        voltageandfreq.m_fVoltageInVolt )
//      ) ;
//    VoltageIDchanged(//nValue
//      m_wVoltageID ) ;
  }
}

void FreqAndVoltageSettingDlg::OnDecVoltage( wxCommandEvent & wxcmd )
{
//  float fMinVoltageInVolt = mp_cpucontroller->GetMinimumVoltageInVolt() ;
//  float fVoltageInVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
//  //Min voltage ID is not always the max voltage: with AMD Griffin this is
//  //exactly the opposite.
//  WORD wMinVoltageAsVoltageID = mp_cpucontroller->GetVoltageID(fMinVoltageInVolt ) ;
//  if( //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) >
//    //fVolt >
//    //mp_i_cpucontroller->GetMinimumVoltageInVolt()
//
//    //Use a function because: the calculated voltage sometimes varies from
//    //the real voltage because the computer uses disrecte values.
//    //E.g. the value read from the config. file is 1.1000000 and the
//    //calculated value is 1.0999999.
//    mp_cpucontroller->VIDisLowerVoltageThan( wMinVoltageAsVoltageID, m_wVoltageID )
//    )
//  {
//    //-- m_wVoltageID ;
//    mp_cpucontroller->DecreaseVoltageBy1Step( fVoltageInVolt ) ;
//    m_wVoltageID = mp_cpucontroller->GetVoltageID( fVoltageInVolt ) ;
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//    //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID )
//    VoltageIDchanged(m_wVoltageID) ;
//  }
//  WORD wNewVoltageID = m_wVoltageID - 1 ;
  //->lower voltage ID = lower voltage
  //TODO
//  if( mp_cpucontroller->GetVoltageInVolt(wNewVoltageID) < fVoltageInVolt )
//  {
//    m_wVoltageID = wNewVoltageID ;
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//    VoltageIDchanged(m_wVoltageID) ;
//  }
//  else //lower voltage ID = higher voltage (AMD Griffin)
//  {
//    m_wVoltageID = m_wVoltageID + 1 ;
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//    VoltageIDchanged(m_wVoltageID) ;
//  }

  //If the current voltage is NOT the first/ lowest element.
  if( m_wVoltageID > 0 )
  {
    -- m_wVoltageID ;
    VoltageIDchanged(m_wVoltageID) ;
  }
}

void FreqAndVoltageSettingDlg::OnIncVoltage( wxCommandEvent & wxcmd )
{
  //int i ;
//  float fVoltageInVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
//  float fMaxVoltageInVolt = mp_cpucontroller->GetMaximumVoltageInVolt() ;
//  //Max voltage ID is not always the max voltage: with AMD Griffin this is
//  //exactly the opposite.
//  WORD wMaxVoltageAsVoltageID = mp_cpucontroller->GetVoltageID(fMaxVoltageInVolt ) ;
//  if( //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) <
//    //fVoltageInVolt <
//    //mp_i_cpucontroller->GetMaximumVoltageInVolt()
//    //fMaxVoltageInVolt
//    //Use a function because: the calculated voltage sometimes varies from
//    //the real voltage because the computer uses discrete values.
//    //E.g. the value read from the config. file is 1.1000000 and the
//    //calculated value is 1.0999999.
//    //mp_i_cpucontroller.IsLessVoltageThan(fVoltageInVolt,fMaxVoltageInVolt)
//    mp_cpucontroller->VIDisLowerVoltageThan( m_wVoltageID, wMaxVoltageAsVoltageID )
//    )
//  {
//  //++ m_wVoltageID ;
//  mp_cpucontroller->IncreaseVoltageBy1Step( fVoltageInVolt ) ;
//  m_wVoltageID = mp_cpucontroller->GetVoltageID( fVoltageInVolt ) ;
//  mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//  VoltageIDchanged(m_wVoltageID) ;
//}
//  WORD wNewVoltageID = m_wVoltageID + 1 ;
  //->higher voltage ID = higher voltage
  //TODO
//  if( mp_cpucontroller->GetVoltageInVolt(wNewVoltageID) > fVoltageInVolt )
//  {
//    m_wVoltageID = wNewVoltageID ;
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//    VoltageIDchanged(m_wVoltageID) ;
//  }
//  else //higher voltage ID = lower voltage (AMD Griffin)
//  {
//    m_wVoltageID = m_wVoltageID - 1 ;
//    mp_wxsliderCPUcoreVoltage->SetValue(m_wVoltageID) ;
//    VoltageIDchanged(m_wVoltageID) ;
//  }
  //If the current voltage is NOT the last/ highest element.
  if( ( m_wVoltageID + 1 ) < (WORD) mp_model->m_cpucoredata.
      m_stdset_floatAvailableVoltagesInVolt.size()
    )
  {
    ++ m_wVoltageID ;
    VoltageIDchanged(m_wVoltageID) ;
  }
}

void FreqAndVoltageSettingDlg::OnScroll(wxScrollEvent & //WXUNUSED(wxscrollevent) 
  wxscrollevent )
{
//  int i = 0 ;
  //int nValue = mp_wxsliderCPUcoreVoltage->GetValue() ;
  switch(wxscrollevent.GetId())
  {
    case ID_PstateSlider :
      {
      HandlePstateMayHaveChanged() ;
      }
      break;
  //case ID_DivisorSlider:
  //    HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreDivisorID);
  //    mp_wxsliderFreqInMHz->SetValue(m_pstate.GetFreqInMHz() ) ;
  //  break;
  //case ID_MultiplierSlider:
  //    mp_wxsliderFreqInMHz->SetValue(m_pstate.GetFreqInMHz() ) ;
  //    //calls HandleCPUcoreFrequencyOrVoltageChanged(...)
  //    HandleMultiplierValueChanged();
  //    break;
  case ID_FrequencySlider:
      {
//      WORD wSliderValue = //mp_wxsliderFreqInMHz->GetValue() ;
//      if( wSliderValue <
//          m_wPreviousFrequencyInMHz )
//          m_wPreviousFrequencyInMHz = SetNearestLowerPossibleFreqInMHz(
//            wSliderValue );
//      else if( wSliderValue > m_wPreviousFrequencyInMHz )
//          m_wPreviousFrequencyInMHz = SetNearestHigherPossibleFreqInMHz(
//            wSliderValue );
      //else
      //    if( mp_wxsliderFreqInMHz->GetValue() > 
      //    m_wPreviousFrequencyInMHz )
      //    SetNearestHigherPossibleFreqInMHz();
      //m_wPreviousFrequencyInMHz = wSliderValue ;
      }
      HandleMultiplierValueChanged() ;
      HandleCPUcoreFrequencyOrVoltageChanged(
        mp_wxsliderCPUcoreFrequencyMultiplier ) ;
      break ;
  case ID_VoltageSlider:
    m_wVoltageID = mp_wxsliderCPUcoreVoltage->GetValue() ;
//    WORD wVoltageInVoltIndex = mp_wxsliderCPUcoreVoltage->GetValue() ;
//    mp_model->m_cpucoredata.GetVoltageInVolt(wVoltageInVoltIndex) ;
    VoltageIDchanged(m_wVoltageID) ;
    break;
  }
  //OutputFreqAndVoltageByControlValues() ;
}

void FreqAndVoltageSettingDlg::OnSpinVoltageDown( wxSpinEvent & event)
{
  float fVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
  if( //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) > 
    fVolt >
    mp_cpucontroller->GetMinimumVoltageInVolt() 
    )
  {
    //-- m_wVoltageID ;
    //mp_i_cpucontroller->DecreaseVoltageBy1Step( fVolt ) ;
    //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID )
    //VoltageIDchanged(m_wVoltageID) ;
  }
}

void FreqAndVoltageSettingDlg::OnSpinVoltageUp( wxSpinEvent & event)
{
  float fVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
  if( mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) < 
    mp_cpucontroller->GetMaximumVoltageInVolt() 
    )
  {
    //++ m_wVoltageID ;
    mp_cpucontroller->IncreaseVoltageBy1Step( fVolt ) ;
    m_wVoltageID = mp_cpucontroller->GetVoltageID( fVolt ) ;
    VoltageIDchanged(m_wVoltageID) ;
  }
}

void FreqAndVoltageSettingDlg::OnSetAsMinVoltageButton(
  wxCommandEvent & wxcmd )
{
  //TODO maybe disable the button if the ref. clock is 0
  // Reference clock <> 0
  if( mp_cpucontroller->m_fReferenceClockInMHz )
  {
    CPUcoreData & r_cpucoredata = mp_cpucontroller->mp_model->m_cpucoredata ;
    WORD wMultiplierIndex = mp_wxsliderFreqInMHz->GetValue() ;
    WORD wFreq = //Explicit cast to WORD to avoid g++ warning.
     (WORD) ( r_cpucoredata.m_arfAvailableMultipliers[ wMultiplierIndex ] *
      mp_cpucontroller->m_fReferenceClockInMHz ) ;
    WORD wVoltageIndex = mp_wxsliderCPUcoreVoltage->GetValue() ;
    float fVoltage = //mp_cpucontroller->GetVoltageInVolt(
      //mp_wxsliderCPUcoreVoltage->GetValue() ) ;
      r_cpucoredata.m_arfAvailableVoltagesInVolt[ wVoltageIndex ] ;
    r_cpucoredata.m_wxcriticalsection.Enter() ;

//  float fVoltage = mp_cpucontroller->GetVoltageInVolt(
//     ) ;
//  WORD wFreq = mp_wxsliderFreqInMHz->GetValue() ;
//  mp_cpucontroller->mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable.
//    insert(
//      VoltageAndFreq( fVoltage
//        , wFreq
//        )
//    ) ;
    VoltageAndFreq voltageandfreq( fVoltage , wFreq) ;
    std::set<VoltageAndFreq> & r_stdsetvoltageandfreq =
        r_cpucoredata.m_stdsetvoltageandfreqLowestStable ;
    std::set<VoltageAndFreq>::iterator iter =
        r_stdsetvoltageandfreq.find( voltageandfreq ) ;
    //VoltageAndFreq exists in set yet.
    if( iter != r_stdsetvoltageandfreq.end() )
      r_stdsetvoltageandfreq.erase(iter) ;
  //    std::pair <std::set<VoltageAndFreq>::iterator, bool> stdpair =
      r_stdsetvoltageandfreq.
        insert(
//          VoltageAndFreq( fVoltage
//            , wFreq
//            )
          voltageandfreq
        ) ;
    if( mp_wxcheckboxbuttonAlsoSetWantedVoltage->IsChecked() )
    {
  //    mp_cpucontroller->mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.
  //      insert(
  //        VoltageAndFreq( m_fWantedVoltageInVolt
  //          , wFreq
  //          )
  //      ) ;
      VoltageAndFreq voltageandfreq( fVoltage + 0.07 , wFreq) ;
      std::set<VoltageAndFreq> & r_stdsetvoltageandfreq =
          r_cpucoredata.m_stdsetvoltageandfreqWanted ;
      std::set<VoltageAndFreq>::iterator iter =
          r_stdsetvoltageandfreq.find( voltageandfreq ) ;
      //VoltageAndFreq exists in set yet.
      if( iter != r_stdsetvoltageandfreq.end() )
        r_stdsetvoltageandfreq.erase(iter) ;
    //    std::pair <std::set<VoltageAndFreq>::iterator, bool> stdpair =
        r_stdsetvoltageandfreq.
          insert(
//            VoltageAndFreq( fVoltage
//              , wFreq
//              )
            voltageandfreq
          ) ;
    }
    r_cpucoredata.m_wxcriticalsection.Leave() ;
    mp_mainframe->RedrawEverything() ;
    mp_mainframe->Refresh() ;
  }
  else
    ::wxMessageBox(wxT("the voltage can not be set because the reference "
      "clock and so the CPU core frequency is is 0 MHz/ unknown"),
      wxT("error")) ;
}

void FreqAndVoltageSettingDlg::OnSetAsWantedVoltageButton(
  wxCommandEvent & wxcmd )
{
  //TODO maybe disable the button if the ref. clock is 0
//  WORD wFreq = mp_wxsliderFreqInMHz->GetValue() ;
  // Reference clock <> 0
  if( mp_cpucontroller->m_fReferenceClockInMHz )
  {
    CPUcoreData & r_cpucoredata = mp_cpucontroller->mp_model->m_cpucoredata ;
    WORD wMultiplierIndex = mp_wxsliderFreqInMHz->GetValue() ;
    r_cpucoredata.m_wxcriticalsection.Enter() ;

    WORD wFreq = //Explicit cast to WORD to avoid g++ warning.
     (WORD) ( r_cpucoredata.m_arfAvailableMultipliers[ wMultiplierIndex ] *
      mp_cpucontroller->m_fReferenceClockInMHz ) ;
    WORD wVoltageIndex = mp_wxsliderCPUcoreVoltage->GetValue() ;
    float fVoltage = //mp_cpucontroller->GetVoltageInVolt(
      //mp_wxsliderCPUcoreVoltage->GetValue() ) ;
      r_cpucoredata.m_arfAvailableVoltagesInVolt[ wVoltageIndex ] ;

    VoltageAndFreq voltageandfreq( fVoltage, wFreq) ;
    std::set<VoltageAndFreq>::iterator iter =
      r_cpucoredata.m_stdsetvoltageandfreqWanted.find( voltageandfreq ) ;
    //VoltageAndFreq exists in set yet.
    if( iter != r_cpucoredata.m_stdsetvoltageandfreqWanted.end() )
      r_cpucoredata.m_stdsetvoltageandfreqWanted.erase(iter) ;
  //    std::pair <std::set<VoltageAndFreq>::iterator, bool> stdpair =
      r_cpucoredata.m_stdsetvoltageandfreqWanted.
        insert(
          VoltageAndFreq( fVoltage
            , wFreq
            )
        ) ;
  //    if( //http://www.cplusplus.com/reference/stl/set/insert/:
  //        //"false if an element with the same value existed."
  //        ! stdpair.second )
  //      *stdpair.first = VoltageAndFreq( fVoltage, wFreq ) ;
    r_cpucoredata.m_wxcriticalsection.Leave() ;
    mp_mainframe->RedrawEverything() ;
    mp_mainframe->Refresh() ;
  }
  else
    ::wxMessageBox(wxT("the voltage can not be set because the reference "
      "clock and so the CPU core frequency is is 0 MHz/ unknown"),
      wxT("error")) ;
}
void FreqAndVoltageSettingDlg::VoltageIDchanged(int nNewValue)
{
  LOGN("VoltageIDchanged()")
  mp_wxsliderCPUcoreVoltage->SetValue(nNewValue) ;
//  float fMinVoltage = mp_cpucontroller->GetMinimumVoltageInVolt() ;
//  float fMaxVoltage = mp_cpucontroller->GetMaximumVoltageInVolt() ;
//  //Max voltage ID is not always the max voltage: with AMD Griffin this is
//  //exactly the opposite.
//  WORD wMaxVoltageAsVoltageID = mp_cpucontroller->GetVoltageID(fMaxVoltage ) ;
//  WORD wMinVoltageAsVoltageID = mp_cpucontroller->GetVoltageID(fMinVoltage ) ;

  //TODO the following shows "the voltage is above the specification for
  //Griffin even if it's within the specs.
////  float fVoltage = mp_i_cpucontroller->GetVoltageInVolt( nNewValue ) ;
//  if( //nNewValue < //mp_pumastatectrl->mp_model->m_cpucoredata.m_byMinVoltageID
//    //mp_i_cpucontroller->GetMinimumVoltageID()
//    //mp_i_cpucontroller->IsLowerVoltageThan(fMaxVoltage, fVoltage )
//    mp_cpucontroller->VIDisLowerVoltageThan(wMaxVoltageAsVoltageID, nNewValue )
//    //wMaxVoltageAsVoltageID
//    )
//    //mp_wxsliderCPUcoreVoltage->SetToolTip(_T("the voltage is above the spec."));
//    SetAttention(mp_wxsliderCPUcoreVoltage,
//    _T("the voltage is above the specification."));
//  else if( //nNewValue > //mp_pumastatectrl->mp_model->m_cpucoredata.m_byMaxVoltageID
//    //mp_i_cpucontroller->GetMaximumVoltageID()
//    //mp_i_cpucontroller->IsLowerVoltageThan(fVoltage , fMinVoltage)
//    mp_cpucontroller->VIDisLowerVoltageThan( nNewValue , wMinVoltageAsVoltageID)
//    )
//    //mp_wxsliderCPUcoreVoltage->SetToolTip(
//    SetAttention( mp_wxsliderCPUcoreVoltage,
//      //We need a _T() macro (wide char-> L"", char->"") for EACH
//      //line to make it compatible between char and wide char.
//      _T("the voltage is below the specification.")
//      _T("->this will surely have no effect"));
  //else
  //  ;
  mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings =
//    mp_cpucontroller->GetVoltageInVolt(nNewValue) ;
//      mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt[ nNewValue] ;
      GetVoltageInVoltFromSliderValue() ;
  if( mp_wxcheckboxbuttonAlsoSetWantedVoltage->IsChecked() )
  {
    float fStableVoltageToSet = mp_cpucontroller->GetVoltageInVolt(
        nNewValue ) 
        //Add 0.07 V for stability safety
        + 0.07 ;
    WORD wVoltageID = //In order to get the voltage in Volt that corresponds
      //to a voltage ID boundary first convert to the corresponding 
      //(possibly rounded) voltage ID.
      mp_cpucontroller->GetVoltageID(
      fStableVoltageToSet ) ;
    //Store value in member for OnSetAsMinimumVoltage().
    m_fWantedVoltageInVolt = mp_cpucontroller->GetVoltageInVolt( 
      wVoltageID ) ;
    mp_wxstatictextWantedVoltageInVolt->SetLabel( 
      wxString::Format(
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compatible between char and wide char.
        _T("%f"), //PState::GetVoltageInVolt(
        m_fWantedVoltageInVolt
        )
      ) ;
  }
  OutputVoltageByControlValues() ;
  HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreVoltage);
  //mp_mainframe->Refresh() ;//rm_timer.
  mp_mainframe->m_wxtimer.Stop() ;
  mp_mainframe->m_wxtimer.Start(mp_mainframe->m_dwTimerIntervalInMilliseconds);
}

void FreqAndVoltageSettingDlg::OutputFreqAndVoltageByControlValues()
{
}

void FreqAndVoltageSettingDlg::OutputVoltageByControlValues()
{
  mp_wxstatictextVoltageInVolt->SetLabel(//_T("0.8")
    wxString::Format(
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compatible between char and wide char.
      _T("%.3f V"), //PState::GetVoltageInVolt(
//      mp_cpucontroller->GetVoltageInVolt(
//      mp_wxsliderCPUcoreVoltage->GetValue() )
      GetVoltageInVoltFromSliderValue()
      )
    ) ;
  ////Force the neighbour controls to be resized.
  //Layout() ;
}

void FreqAndVoltageSettingDlg::OnActivate(wxActivateEvent & r_activateevent )
{
  if( r_activateevent.GetActive() )
  {
    //mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings = 
    //  mp_i_cpucontroller->GetVoltageInVolt( 
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
//  int i = 0 ;
  BYTE byPstateNumber = 0 ;
  byPstateNumber = mp_wxsliderCPUcorePstate->GetValue() ;
  float fVoltageInVolt = GetVoltageInVoltFromSliderValue() ;
  if( fVoltageInVolt > 0.0 )
  {
    mp_mainframe->PossiblyAskForOSdynFreqScalingDisabling() ;
    WORD wNumCPUcores = mp_model->m_cpucoredata.GetNumberOfCPUcores() ;
    if( wNumCPUcores > 1 )
    {
      for( WORD wCPUcore = 0 ; wCPUcore < wNumCPUcores ; ++ wCPUcore )
        if( m_ar_p_wxcheckbox[wCPUcore]->IsChecked() )
          //TODO the confirmation of e.g. wxWidgets seems to happen in
          //ANOTHER thread->synchronize here (by e.g. using critical sections)
          //TODO change to "I_CPUcontroller::SetCurrentVoltageAndFrequency(...)"
          mp_cpucontroller->SetCurrentVoltageAndMultiplier(
            fVoltageInVolt ,
            GetMultiplierFromSliderValue() ,
            wCPUcore
            ) ;
    }
    else
      mp_cpucontroller->SetCurrentVoltageAndMultiplier(
        fVoltageInVolt ,
        GetMultiplierFromSliderValue() ,
        //Only 1 logical CPU core-> use CPU core ID "0"
        0
        ) ;
  //  if( mp_wxcheckboxSetAsCurrentAfterApplying->IsChecked() )
    {
//      mp_mainframe->PossiblyAskForOSdynFreqScalingDisabling() ;
      //mp_i_cpucontroller->SetPstate( byPstateNumber,
      //  //1 = 1bin
      //  //1 << m_byCoreID
      //  m_byCoreID
      //  );
    }
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

    wNewFreqInMhz =
        //GetNearestHigherPossibleFreqInMHzAsDIDnFID(wFreqInMHz)
        mp_cpucontroller->GetNearestHigherPossibleFreqInMHz(wFreqInMHz)
        //.GetFreqInMHz() 
        ;

    if( wNewFreqInMhz != 0 )
    {
      HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderFreqInMHz);

//      mp_wxsliderFreqInMHz->SetValue( //PState::GetFreqInMHz(byFID,byDivisorID)
//          wNewFreqInMhz
//          ) ;
    }
    return wNewFreqInMhz ;
}

//void 
WORD FreqAndVoltageSettingDlg::SetNearestLowerPossibleFreqInMHz(
    WORD wFreqInMHz
    )
{
    WORD wNewFreqInMhz ;

    wNewFreqInMhz =
      mp_cpucontroller->GetNearestLowerPossibleFreqInMHz(wFreqInMHz) ;

    if( wNewFreqInMhz != 0 )
    {
      HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderFreqInMHz);

//      mp_wxsliderFreqInMHz->SetValue( //PState::GetFreqInMHz(byFID,byDivisorID)
//          wNewFreqInMhz
//          ) ;
    }
    return wNewFreqInMhz ;
}
