/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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

#include <windef.h> //for BYTE
#include "FreqAndVoltageSettingDlg.hpp" //class FreqAndVoltageSettingDlg
#include <Controller/CPU-related/I_CPUcontroller.hpp>//class I_CPUcontroller
//for GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt
//  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  );
#include <Controller/CPU-related/GetInterpolatedVoltageFromFreq.hpp>

//DISable warning, from
//http://stackoverflow.com/questions/59670/how-to-get-rid-of-deprecated-conversion-from-string-constant-to-char-warning
// : "I believe passing -Wno-write-strings to gcc will suppress this warning."
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <images/cancel16x16.xpm>
#include <images/decrease16x16.xpm>
#include <images/find_lowest_stable_CPU_core_voltage16x16.xpm>
#include <images/increase16x16.xpm>
#include <images/pause_service16x16.xpm>
//#include <images/pause16x16.xpm>
#include <images/prevent_voltage_above_default_voltage16x16.xpm>
#include <images/prevent_voltage_below_lowest_stable_voltage16x16.xpm>
#include <images/select_pstate_via_mouse_position16x16.xpm>
#include <images/set_as_desired_voltage16x16.xpm>
#include <images/set_as_minimal_voltage16x16.xpm>
#include <images/stabilize_voltage.xpm>
#include <images/standby16x16.xpm>
#include <images/stop_finding_lowest_stable_CPU_core_voltage16x16.xpm>
#include <images/auto-configure_CPU_core_voltage_settings16x16.xpm>
//ENable warning
#pragma GCC diagnostic warning "-Wwrite-strings"
#include <ModelData/CPUcoreData.hpp> //class CPUcoreData
//#include <ModelData/ModelData.hpp> //class Model
#include <wxWidgets/App.hpp> //for wxGetApp()
#include <wx/tglbtn.h> //for class wxBitmapToggleButton
#include <wx/button.h> //for class wxButton
#include <wx/checkbox.h> //for class wxCheckBox
#include <wx/choice.h> //class wxChoice
#include <wx/dialog.h> //for base class wxDialog
#include <wx/msgdlg.h> //for ::wxMessageBox(...)
//#include <wx/msw/wx.rc> //for cross cursor
#include <wx/sizer.h> //for class wxBoxSizer
#include <wx/spinbutt.h> //class wxSpinButton
#include <wx/slider.h> //for class wxSlider
//#include <wx/spinbutt.h> //class wxSpinButton
////for a line which may be used in a dialog to separate the groups of controls.
//#include <wx/statline.h> //class wxStaticLine
#include <wx/stattext.h> //for wxStaticText
#include <wx/textctrl.h> //class wxTextCtrl
#include <wx/msw/winundef.h> //aviod "GetClassName" -> "GetClassNameA"

#if (wxMAJOR_VERSION > 1 && wxMINOR_VERSION > 8)
  #define USE_WX_BITMAPTOGGLEBUTTON
#endif
#ifdef wxHAS_BITMAPTOGGLEBUTTON
  #define USE_WX_BITMAPTOGGLEBUTTON
#endif

//#ifdef USE_BITMAP_TOGGLE_BTN
  #ifdef USE_WX_BITMAPTOGGLEBUTTON
    //wxWidgets 2.9.1 has wxBitmapToggleButton but does not have
    // wxHAS_BITMAPTOGGLEBUTTON defined.
    #include <wx/tglbtn.h> //class wxBitmapToggleButton
    #define WX_BITMAP_TOGGLE_BUTTON_EVENT_MACRO EVT_TOGGLEBUTTON
    #define TOGGLE_BUTTON_OR_BUTTON_EVENT_MACRO EVT_TOGGLEBUTTON
  #else
    //class wxBitmapToggleButton
    #include <wxWidgets/UserInterface/wxBitmapToggleButton.hpp>
    #define WX_BITMAP_TOGGLE_BUTTON_EVENT_MACRO EVT_CHECKBOX
    #define TOGGLE_BUTTON_OR_BUTTON_EVENT_MACRO EVT_BUTTON
  #endif
//#endif
#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include <wx/version.h>
#include "MainFrame.hpp" //for class MainFrame

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
  , ID_SelectPstateViaMousePos
  , ID_DecreaseVoltage
  , ID_StabilizeVoltage
  , ID_Panel
  , ID_PreventVoltageBelowLowestStableVoltageCheckbox
  , ID_RestorePerformanceStateAfterResumeCheckbox
  , ID_PreventVoltageAboveDefaultVoltageCheckbox
  , ID_findLowestStableVoltage
  , ID_stopFindingLowestStableVoltage
  , ID_AutoConfigureVoltageSettings
  , VoltageTypeListBox
  , SecondsUntilVoltageDecreaseSpinButton
};

#ifndef MAXWORD
  #define MAXWORD 0xffff
#endif //#ifndef MAXWORD

//Class because of:
//see http://www.wxwidgets.org/docs/faqgtk.htm#charinframe:
//"In wxGTK, the frames never get focus and so can never receive CHAR nor
//KEY events so an EVT_CHAR handler for a frame will be never called.
//To receive these events, you should create a wxPanel inside the frame and
//register the key event handlers for the panel, not the frame."
//But this class is not in use. (using OnChar() for every child of the dialog
//instead.
class KeyEventPanel
  : public wxPanel
{
  FreqAndVoltageSettingDlg & mr_freqandvoltagesettingdlg ;
public:
  KeyEventPanel(FreqAndVoltageSettingDlg & r_freqandvoltagesettingdlg)
    : mr_freqandvoltagesettingdlg(r_freqandvoltagesettingdlg)
  {
  }
  void OnChar(wxKeyEvent & r_wxkeyevent )
  {
    LOGN("KeyEventPanel::OnChar")
    mr_freqandvoltagesettingdlg.HandleKeyEvent_Inline(r_wxkeyevent) ;
  }
  //Necessary in order to get scroll events; to avoid compilation errors.
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(KeyEventPanel, wxPanel)
  EVT_CHAR(KeyEventPanel::OnChar)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(FreqAndVoltageSettingDlg, wxDialog)
  EVT_ACTIVATE(FreqAndVoltageSettingDlg::OnActivate) 
  EVT_SCROLL(FreqAndVoltageSettingDlg::OnScroll)
  EVT_BUTTON(wxID_APPLY, FreqAndVoltageSettingDlg::OnApplyButton)
  EVT_BUTTON(ID_findLowestStableVoltage,
      FreqAndVoltageSettingDlg::OnFindLowestStableVoltageButton)
  EVT_BUTTON(ID_stopFindingLowestStableVoltage,
      FreqAndVoltageSettingDlg::OnStopFindingLowestStableCPUcoreVoltageButton)
  EVT_BUTTON(ID_SetAsMinVoltage, 
    FreqAndVoltageSettingDlg::OnSetAsMinVoltageButton)
  EVT_BUTTON(ID_SetAsWantedVoltage, 
    FreqAndVoltageSettingDlg::OnSetAsWantedVoltageButton)
  EVT_BUTTON(ID_StabilizeVoltage,
    FreqAndVoltageSettingDlg::OnStabilizeVoltageButton)
#if wxUSE_SPINBTN
  //EVT_SPIN_UP(ID_SpinVoltage, FreqAndVoltageSettingDlg::OnSpinVoltageUp)
  //EVT_SPIN_DOWN(ID_SpinVoltage, FreqAndVoltageSettingDlg::OnSpinVoltageDown)
  EVT_SPIN(SecondsUntilVoltageDecreaseSpinButton,
    FreqAndVoltageSettingDlg::OnSpinSecondsUntilVoltageDecreaseSpinButton)
#endif
  EVT_BUTTON( ID_DecreaseVoltage, FreqAndVoltageSettingDlg::OnDecVoltage)
  TOGGLE_BUTTON_OR_BUTTON_EVENT_MACRO( ID_SelectPstateViaMousePos,
    FreqAndVoltageSettingDlg::OnSelectPstateViaMousePos)
  EVT_BUTTON( ID_SpinVoltage, FreqAndVoltageSettingDlg::OnIncVoltage)

  WX_BITMAP_TOGGLE_BUTTON_EVENT_MACRO(
    ID_PreventVoltageAboveDefaultVoltageCheckbox,
    FreqAndVoltageSettingDlg::OnPreventVoltageAboveDefaultVoltageCheckbox)
  WX_BITMAP_TOGGLE_BUTTON_EVENT_MACRO(
    ID_PreventVoltageBelowLowestStableVoltageCheckbox,
    FreqAndVoltageSettingDlg::OnPreventVoltageBelowLowestStableVoltageCheckbox)
  //see http://www.wxwidgets.org/docs/faqgtk.htm#charinframe:
  //"In wxGTK, the frames never get focus and so can never receive CHAR nor
  //KEY events so an EVT_CHAR handler for a frame will be never called.
  //To receive these events, you should create a wxPanel inside the frame and
  //register the key event handlers for the panel, not the frame."
//  EVT_CHAR(FreqAndVoltageSettingDlg::OnChar)
//  EVT_CHAR_HOOK(FreqAndVoltageSettingDlg::OnCharHook)
  EVT_CHOICE( VoltageTypeListBox, FreqAndVoltageSettingDlg::OnSetVoltageType)
  EVT_CLOSE( FreqAndVoltageSettingDlg::OnClose)

#ifdef wxHAS_POWER_EVENTS
  //For function "restore voltage and multiplier after standby".
  EVT_POWER_RESUME(FreqAndVoltageSettingDlg::OnResume)
#endif //#ifdef wxHAS_POWER_EVENTS

END_EVENT_TABLE()

inline void FreqAndVoltageSettingDlg::AddCPUcoreCheckBoxSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  wxSizer * p_wxboxsizerCPUcoreCheckBox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText * p_wxstatictext = new wxStaticText(
    this,
    wxID_ANY,
    wxT("affected\ncores:")
    ) ;
  p_wxstatictext->SetToolTip( wxT("the CPU cores the frequency and voltage "
    "should be applied to ") ) ;
  p_wxboxsizerCPUcoreCheckBox->Add(
    p_wxstatictext
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
      m_ar_p_wxcheckbox[ wCoreID ] = new wxCheckBox(
        this,
        wxID_ANY,
        //const wxString& label,
        wxString::Format( wxT("%u"), wCoreID),
        wxDefaultPosition,
        wxDefaultSize, //const wxSize& size = wxDefaultSize,
        //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
        // "If you need to use this style in order to get the arrows or etc., but
        // would still like to have normal keyboard navigation take place, you
        // should create and send a wxNavigationKeyEvent in response to the key
        // events for Tab and Shift-Tab."
        //To get EVT_CHAR events when the button is focused.
        wxWANTS_CHARS
      ) ;
      if( mp_model->m_userinterfaceattributes.m_bSelectAllCPUcores )
        m_ar_p_wxcheckbox[ wCoreID ]->SetValue(true) ; //Set the check.
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
    wxEXPAND //| //wxALL
//      wxBOTTOM
    ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to
    //include any border flag."
    //10
    2
    );
}

inline void FreqAndVoltageSettingDlg::AddAlsoSetWantedVoltageControls(
  wxSizer * p_wxsizerSuperordinate )
{
  m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton(
    this, wxID_ANY,
    wxBitmap( set_as_desired_voltage16x16_xpm )
    ) ;
  wxString wxstrAlsoSetWantedVoltageToolTip = wxString::Format(
    wxT("If setting a minimum/ lowest stable voltage:\n"
      "adds an operating safety margin of %f Volt\n"
      "and sets the resulting voltage as voltage for the frequency slider's "
      "frequency in MHz to use for\n"
      "Dynamic Voltage and Frequency Scaling"),
    mp_model->m_userinterfaceattributes.m_fOperatingSafetyMarginInVolt
    );
  m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage->SetToolTip(
    wxstrAlsoSetWantedVoltageToolTip) ;
  AddWindowToSizer(
    m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage,
    p_wxsizerSuperordinate
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //[...]can change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable[...]W
    , 1
    , //wxEXPAND |
    wxFIXED_MINSIZE | wxALL
    , 0 //Determines the border width
    );
}

inline void FreqAndVoltageSettingDlg::AddApplyOrCancelSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer * m_p_wxboxsizerOK_Cancel;
  m_p_wxboxsizerOK_Cancel = new //wxBoxSizer(wxHORIZONTAL);
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer(
      //number of columns: zero, it will be calculated to form the total
      //number of children in the sizer
      0
      );

  AddWritePstateButton(m_p_wxboxsizerOK_Cancel);

//  m_p_wxboxsizerOK_Cancel->Add( m_p_wxbuttonFindLowestStableVoltage ) ;

  //  mp_wxcheckboxSetAsCurrentAfterApplying = new wxCheckBox(this, wxID_ANY,
  //      //_T("Set as current after apply") ) ;
  //      _T("Set as current p-state after write") ) ;
  //  m_p_wxboxsizerOK_Cancel->Add(mp_wxcheckboxSetAsCurrentAfterApplying);

  AddPauseServiceCheckbox(m_p_wxboxsizerOK_Cancel) ;
  AddRestorePerformanceStateAfterResumeButton(m_p_wxboxsizerOK_Cancel) ;
  AddCancelButton(m_p_wxboxsizerOK_Cancel);

  CreateFindLowestStableCPUcoreVoltageButton();
  m_p_wxboxsizerOK_Cancel->Add(m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage);

  CreateStopFindingLowestStableCPUcoreVoltageButton();
  m_p_wxboxsizerOK_Cancel->Add(
    m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage);

  //wxSpinCtrl * p_wx = new

  AddSecondsUntilNextVoltageDecreaseTextControl(m_p_wxboxsizerOK_Cancel);

//  m_p_wxspinbuttonSecondsUntilVoltageDecrease = new wxSpinButton (
//      this,
////      NULL,
//      SecondsUntilVoltageDecreaseSpinButton
//      );
  m_p_wxspinbuttonSecondsUntilVoltageDecrease = NULL;
//  m_p_wxboxsizerOK_Cancel->Add(m_p_wxspinbuttonSecondsUntilVoltageDecrease);

  m_p_wxstatictextSecondsUntilNextVoltageDecrease = new wxStaticText( this,
    wxID_ANY, wxT("     s") );
  m_p_wxboxsizerOK_Cancel->Add(m_p_wxstatictextSecondsUntilNextVoltageDecrease);

  //  mp_wxcheckboxOnlySafeRange = new wxCheckBox(this, wxID_ANY,
  //    wxT("only safe range") ) ;
  //  m_p_wxboxsizerOK_Cancel->Add( mp_wxcheckboxOnlySafeRange) ;
  LOGN("before adding the OK etc. sizer")
  p_wxsizerSuperordinate->Add(
    m_p_wxboxsizerOK_Cancel,
    0 ,
    wxFIXED_MINSIZE,
    //Determines the border width, if the flag  parameter is set to include
    //any border flag.
    2
    );
  LOGN( FULL_FUNC_NAME << "--end")
}

inline void FreqAndVoltageSettingDlg::AddAutoConfigureVoltageSettingsButton(
  wxSizer * p_wxsizerSuperordinate )
{
  wxString wxstrToolTip = wxT("auto-configure CPU core voltage settings:\n"
    "find lowest stable CPU core voltage and set the desired voltage based on it");
  m_p_wxbitmapbuttonAutoConfigureVoltageSettings = new wxBitmapButton(
    (wxWindow * ) this , // wxWindow *    parent,
    // wxWindowID id,
    (wxWindowID) ID_AutoConfigureVoltageSettings,
    //const wxBitmap & label,
    wxBitmap( auto_configure_CPU_core_voltage_settings16x16_xpm )
    );
  m_p_wxbitmapbuttonAutoConfigureVoltageSettings->SetToolTip(
    wxstrToolTip ) ;
  p_wxsizerSuperordinate->Add(
    m_p_wxbitmapbuttonAutoConfigureVoltageSettings,
    0 ,//0=the control should not take more space if the sizer is enlarged
    wxLEFT | wxRIGHT,
    0);
}

inline void FreqAndVoltageSettingDlg::AddCancelButton(
  wxSizer * p_wxsizerSuperordinate )
{
//  wxButton * p_wxbuttonCancel = new wxButton(
//    this,
//    wxID_CANCEL //,
//  //    wxEmptyString ,
//  //    wxDefaultPosition ,
//  //    wxDefaultSize ,
//  //    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//  //    // "If you need to use this style in order to get the arrows or etc., but
//  //    // would still like to have normal keyboard navigation take place, you
//  //    // should create and send a wxNavigationKeyEvent in response to the key
//  //    // events for Tab and Shift-Tab."
//  //    //To get EVT_CHAR events when the button is focused.
//  //    wxWANTS_CHARS
//    ) ;

  #ifndef USE_EXTERNAL_ICONS
  wxBitmap wxbitmapCancel( cancel16x16_xpm ) ;
  #else
  wxBitmap wxbitmapCancel ( m_wxstrIconFilesPrefix +
    wxT("cancel16x16.png") ) ;
   if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
       // #b825460a217d250db53df0c9ca293068:
       // "Returns true if bitmap data is present."
     ! wxbitmapCancel.IsOk()
     )
     wxbitmapCancel = wxBitmap( cancel16x16_xpm ) ;
  #endif
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , wxID_CANCEL
  //    , wxT("&-")
    , wxbitmapCancel
    , wxDefaultPosition
  //    , wxsizeMinimal
    , wxDefaultSize
  //    , wxButton::GetDefaultSize()
    , wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  p_wxbitmapbutton->SetToolTip( wxT("cancel/ close dialog") ) ;

//  p_wxbuttonCancel->SetWindowStyle(wxWANTS_CHARS) ;
//  p_wxbitmapbutton->SetWindowStyle(wxWANTS_CHARS) ;

  p_wxsizerSuperordinate->Add( //p_wxbuttonCancel
    p_wxbitmapbutton);
}

inline void FreqAndVoltageSettingDlg::AddCPUcoreFrequencySizer(
  wxSizer * p_wxsizerSuperordinate )
{
  p_wxboxsizerCPUcoreFrequencyInMHz = new wxBoxSizer(wxHORIZONTAL);
  //p_wxboxsizerCPUcoreFrequencyMultiplier->Add(//p_wxsliderCPUcoreFrequencyMultiplier
  //  mp_wxsliderCPUcoreFrequencyMultiplier, 1
  //  //, wxEXPAND | wxALL,
  //  , wxLEFT | wxRIGHT
  //  , 5);

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
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    new wxStaticText(this, wxID_ANY,
    //_T("core frequency in MHz: ")));
    _T("CPU core\nfrequency"//\nin MHz"
      ":")
      )
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
      //| wxALIGN_TOP
      | wxALIGN_CENTER_VERTICAL
    , 5 //Determines the border width
    );
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxsliderFreqInMHz
    , 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    , 0 //Determines the border width
    );
  mp_wxstatictextFreqInMHz = new wxStaticText(this, wxID_ANY, _T("") );
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxstatictextFreqInMHz
//    , 1
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    , 5 //Determines the border width
    );
  p_wxsizerSuperordinate->Add(//mp_wxsliderFreqInMHz,
    p_wxboxsizerCPUcoreFrequencyInMHz ,
    1 ,
    wxEXPAND //| //wxALL
//      wxBOTTOM
    ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to
    //include any border flag."
    //10
    //2
    0
    );
}

inline void FreqAndVoltageSettingDlg::AddCPUcoreVoltageSizer(
  wxSizer * p_wxsizerSuperordinate )
{
//  wxButton * p_wxbuttonDecBy1VoltageStep ;
//  wxButton * p_wxbuttonIncBy1VoltageStep ;
//  wxBitmapButton * p_wxbitmapbutton ;
  wxBoxSizer * p_wxboxsizerCPUcoreVoltage = new wxBoxSizer(wxHORIZONTAL);
  //  wxFlexGridSizer * p_wxflexgridsizerCPUcoreVoltageInVolt = new
  //    wxFlexGridSizer(
  //    //number of columns: zero, it will be calculated to form the total
  //    //number of children in the sizer
  //    0 );
  //p_wxboxsizerCPUcoreVoltageInVolt->Add(
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
  p_wxboxsizerCPUcoreVoltage->Add(
    new wxStaticText(this, wxID_ANY, _T("CPU core\nvoltage" //\nIDentifier
      ":" ))
    , 0 //0=the control should not take more space if the sizer is enlarged
    //These flags are used to specify which side(s) of the sizer item the
    //border width will apply to.
    , //wxEXPAND |
    //wxALL,
    wxLEFT | wxRIGHT
    | wxALIGN_CENTER_VERTICAL
//    //The label and the adjustable value should be at the same vertical
//    //position, so place at the top.
//    | wxALIGN_TOP
    , 3 //Determines the border width
    );
  p_wxboxsizerCPUcoreVoltage->Add(//p_wxsliderCPUcoreVoltage
    mp_wxsliderCPUcoreVoltage,
    1
    //,wxEXPAND | wxALL
    , //wxLEFT | wxRIGHT |
      wxALIGN_CENTER_VERTICAL
    //http://docs.wxwidgets.org/2.8.10/wx_wxsizer.html#wxsizeradd
    // "Determines the border width, if the flag parameter is set to include
    // any border flag."
//    , 2
    );
  mp_wxstatictextVoltageInVolt = new wxStaticText(this, wxID_ANY, wxT("") );
  p_wxboxsizerCPUcoreVoltage->Add(
    mp_wxstatictextVoltageInVolt ,
    0 //0=the control should not take more space if the sizer is enlarged
    //,wxEXPAND | wxALL
    , //wxLEFT | wxRIGHT |
      wxALIGN_CENTER_VERTICAL
    //http://docs.wxwidgets.org/2.8.10/wx_wxsizer.html#wxsizeradd
    // "Determines the border width, if the flag parameter is set to include
    // any border flag."
//    , 5
    );
//  AddIncreaseVoltageButton(p_wxboxsizerCPUcoreVoltage) ;
//  AddDecreaseVoltageButton(p_wxboxsizerCPUcoreVoltage) ;
//  AddPreventVoltageBelowLowestStableVoltageButton(p_wxboxsizerCPUcoreVoltage) ;
//  AddPreventVoltageAboveDefaultVoltageButton(p_wxboxsizerCPUcoreVoltage) ;
//  AddStabilizeVoltageButton(p_wxboxsizerCPUcoreVoltage) ;
  p_wxsizerSuperordinate->Add(
    p_wxboxsizerCPUcoreVoltage,
    1 ,
    wxEXPAND //| //wxALL
      //wxBOTTOM
    ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to
    //include any border flag."
    //10
    //2
    0
    );
}

void FreqAndVoltageSettingDlg::AddDecreaseVoltageButton( wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  //  p_wxbuttonDecBy1VoltageStep = new wxButton(
  //Avoid to show up an error dialog under _Windows_ if the image file could not
  //be load.
  //#ifdef _WIN32
  #ifndef USE_EXTERNAL_ICONS
  wxBitmap wxbitmapDecreaseVoltage( decrease16x16_xpm ) ;
  #else
  wxBitmap wxbitmapDecreaseVoltage ( m_wxstrIconFilesPrefix +
    wxT("decrease16x16.png") ) ;
   if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
       // #b825460a217d250db53df0c9ca293068:
       // "Returns true if bitmap data is present."
     ! wxbitmapDecreaseVoltage.IsOk()
     )
     wxbitmapDecreaseVoltage = wxBitmap( decrease16x16_xpm ) ;
  #endif
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , ID_DecreaseVoltage
  //    , wxT("&-")
    , wxbitmapDecreaseVoltage
    , wxDefaultPosition
  //    , wxsizeMinimal
    , wxDefaultSize
  //    , wxButton::GetDefaultSize()
    , wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  p_wxbitmapbutton->SetToolTip( wxT("decrease voltage by 1 step (press '-')") ) ;
  p_wxsizer->Add( //mp_wxspinbuttonVoltageInVolt
  //    p_wxbuttonDecBy1VoltageStep
    p_wxbitmapbutton
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
}

void FreqAndVoltageSettingDlg::AddIncreaseVoltageButton( wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  //  wxSize wxsizeMinimal(-1,-1) ;
  //  p_wxbuttonIncBy1VoltageStep = new wxButton(
//  wxBitmap wxbitmapIncreaseVoltage( m_wxstrIconFilesPrefix +
//     wxT("increase16x16.png") , wxBITMAP_TYPE_PNG ) ;
  wxBitmap wxbitmapIncreaseVoltage ;
//Avoid to show up an error dialog under _Windows_ if the image file could not
//be load.
//#ifndef _WIN32
#ifdef USE_EXTERNAL_ICONS
   if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
       // #b825460a217d250db53df0c9ca293068:
       // "Returns true if bitmap data is present."
//     ! wxbitmapIncreaseVoltage.IsOk()
     ! wxbitmapIncreaseVoltage.
       //http://docs.wxwidgets.org/2.6/wx_wxbitmap.html#wxbitmaploadfile:
       // "true if the operation succeeded, false otherwise."
       LoadFile(  m_wxstrIconFilesPrefix +
         wxT("increase16x16.png") , wxBITMAP_TYPE_PNG )
     )
#endif
     wxbitmapIncreaseVoltage = wxBitmap( increase16x16_xpm ) ;
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , ID_SpinVoltage
//    , wxT("&+")
    , wxbitmapIncreaseVoltage
    , wxDefaultPosition,
    wxDefaultSize,
    wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  p_wxbitmapbutton->SetToolTip( wxT("increase voltage by 1 step (press '+')") ) ;
  p_wxsizer->Add( //mp_wxspinbuttonVoltageInVolt
//    p_wxbuttonIncBy1VoltageStep
    p_wxbitmapbutton
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
}

void FreqAndVoltageSettingDlg::AddSecondsUntilNextVoltageDecreaseTextControl(
  wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxSize wxsize(40, 20);
  m_p_wxtextctrlSecondsUntilNextVoltageDecrease = new wxTextCtrl(
    this,
    wxID_ANY,
    wxString::Format( wxT("%u"), mp_model->
      m_instablecpucorevoltagedetection.
      m_uiNumberOfSecondsToWaitUntilVoltageIsReduced ),
    wxDefaultPosition,
    wxsize
    );
  m_p_wxtextctrlSecondsUntilNextVoltageDecrease->SetToolTip(
    wxT("For detecting an instable CPU core voltage:\n"
      "seconds until the voltage is decreased") );
  p_wxsizer->Add(m_p_wxtextctrlSecondsUntilNextVoltageDecrease//,
    //1
    );
  LOGN( FULL_FUNC_NAME << "--end")
}

void FreqAndVoltageSettingDlg::AddSelectPstateViaMousePositionButton(
  wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxBitmap wxbitmapSelectPstateViaMousePosition ;
  wxString wxstrTooltip = wxT("select voltage&frequency via left mouse "
    "click into mainframe");
  wxbitmapSelectPstateViaMousePosition = wxBitmap(
    select_pstate_via_mouse_position16x16_xpm ) ;
#ifdef wxHAS_BITMAPTOGGLEBUTTON
  m_p_wxbitmaptogglebuttonSelectPstateViaMouseClick = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton( this,
      ID_SelectPstateViaMousePos,
      wxbitmapSelectPstateViaMousePosition
      );
  m_p_wxbitmaptogglebuttonSelectPstateViaMouseClick->SetToolTip( wxstrTooltip ) ;
  AddWindowToSizer( m_p_wxbitmaptogglebuttonSelectPstateViaMouseClick,
    p_wxsizer);
#else
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
   this
   , ID_SelectPstateViaMousePos
  //    , wxT("&+")
   , wxbitmapSelectPstateViaMousePosition
   , wxDefaultPosition,
   wxDefaultSize,
   wxBU_AUTODRAW |
   //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
   // "If you need to use this style in order to get the arrows or etc., but
   // would still like to have normal keyboard navigation take place, you
   // should create and send a wxNavigationKeyEvent in response to the key
   // events for Tab and Shift-Tab."
   //To get EVT_CHAR events when the button is focused.
   wxWANTS_CHARS
   ) ;
  p_wxbitmapbutton->SetToolTip( wxstrTooltip ) ;
  p_wxsizer->Add( //mp_wxspinbuttonVoltageInVolt
  //    p_wxbuttonIncBy1VoltageStep
   p_wxbitmapbutton
   , 0 //0=the control should not take more space if the sizer is enlarged
   , wxLEFT | wxRIGHT
   , 0 );
#endif //wxHAS_BITMAPTOGGLEBUTTON
}

void FreqAndVoltageSettingDlg::AddPauseServiceCheckbox(wxSizer * p_wxsizer )
{
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  wxString wxstrPauseToolTip =
    wxT("pauses the Dynamic Voltage and Frequency Scaling\n"
    "that is done by the service\n"
    "(else the voltage and/ or multiplier may be changed by the service "
    "immediately afterwards)") ;
  m_p_wxbitmaptogglebuttonPauseService = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton(
    this, wxID_ANY,
    wxBitmap( pause_service16x16_xpm )
    ) ;
  m_p_wxbitmaptogglebuttonPauseService->
    //http://docs.wxwidgets.org/2.8/wx_wxcheckbox.html#wxcheckboxsetvalue:
    //"If true, the check is on, otherwise it is off."
    SetValue(true) ;
  m_p_wxbitmaptogglebuttonPauseService->SetToolTip( wxstrPauseToolTip) ;
  AddWindowToSizer(m_p_wxbitmaptogglebuttonPauseService, p_wxsizer);
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
}

inline void FreqAndVoltageSettingDlg::AddPerformanceStateSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxBoxSizer * p_wxboxsizerCPUcorePstate = new wxBoxSizer(wxHORIZONTAL);
//  wxStaticText * p_wxstatictextPerformanceState = new wxStaticText(
//    this,
//    wxID_ANY,
//    wxT("performance\nstate:")
//    ) ;
//  p_wxstatictextPerformanceState->SetToolTip(
//    wxT("combination of CPU core voltage and frequency") ) ;
  p_wxboxsizerCPUcorePstate->Add(
//    p_wxstatictextPerformanceState
    m_p_wxchoiceVoltageType
    , 0 //0=the control should not take more space if the sizer is enlarged
    //"These flags are used to specify which side(s) of the sizer item the
    //border width will apply to."
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    //The label and the adjustable value should be at the same vertical
    //position, so place at the top.
//    | wxALIGN_TOP
    , 5 //Determines the border width
    );
  p_wxboxsizerCPUcorePstate->Add(//p_wxsliderCPUcoreDivisorID
    mp_wxsliderCPUcorePstate
    , 1
    //, wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    , 5);
  p_wxsizerSuperordinate->Add(
    p_wxboxsizerCPUcorePstate,
    1 ,
    wxEXPAND //| //wxALL
//      wxBOTTOM
    ,
    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
    //"Determines the border width, if the flag  parameter is set to
    //include any border flag."
    //10
    2
    );
}

void FreqAndVoltageSettingDlg::AddPreventVoltageAboveDefaultVoltageButton(
  wxSizer * p_wxsizer )
{
  wxString wxstrToolTip = wxT("prevent voltage above\ndefault voltage");
  m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton(
    this ,
    ID_PreventVoltageAboveDefaultVoltageCheckbox ,
    wxBitmap( prevent_voltage_above_default_voltage16x16_xpm )
    ) ;
  m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage->SetToolTip(
    wxstrToolTip);
  //Possibly Check the checkbox.
  m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage->SetValue(
    mp_model->m_userinterfaceattributes.m_bPreventVoltageAboveDefaultVoltage );
  AddWindowToSizer(
    m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage, p_wxsizer);
}

void FreqAndVoltageSettingDlg::AddPreventVoltageBelowLowestStableVoltageButton(
  wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxString wxstrToolTip = wxT("prevent voltage below\nlowest stable voltage");
  //from wxWidgets ver. 2.9.1
//#ifdef wxUSE_TOGGLEBTN
//#ifdef wxHAS_BITMAPTOGGLEBUTTON
#ifdef USE_BITMAP_TOGGLE_BTN
  m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton(
    (wxWindow * ) this , // wxWindow *    parent,
    // wxWindowID id,
    (wxWindowID) ID_PreventVoltageBelowLowestStableVoltageCheckbox,
    //const wxBitmap & label,
    wxBitmap( prevent_voltage_below_lowest_stable_voltage16x16_xpm )
    );
  m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage->SetToolTip(
    wxstrToolTip ) ;
  m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage->SetValue(
    mp_model->
    m_userinterfaceattributes.m_bPreventVoltageBelowLowestStableVoltage ) ;
  AddWindowToSizer(
    m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage, p_wxsizer);
//  m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage->SetText()
#else
//  wxBitmapToggleButton
#endif
  LOGN( FULL_FUNC_NAME << "--end")
}

void FreqAndVoltageSettingDlg::AddRestorePerformanceStateAfterResumeButton(
  wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxString wxstrToolTip = wxT("Restore performance state after resume");
  m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxBitmapToggleButton(
    this ,
    ID_RestorePerformanceStateAfterResumeCheckbox ,
    wxBitmap( standby16x16_xpm )
    ) ;
  m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume->SetToolTip(
    wxstrToolTip);
  //Possibly Check the checkbox.
  m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume->SetValue(
    mp_model->m_userinterfaceattributes.m_bPreventVoltageAboveDefaultVoltage);
  AddWindowToSizer(m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume,
    p_wxsizer);
  LOGN( FULL_FUNC_NAME << "--end")
}

void FreqAndVoltageSettingDlg::AddSetAsDesiredVoltageButton(
  wxSizer * p_wxsizer )
{
  //  wxSize wxsizeMinimal(-1,-1) ;
  //  p_wxbuttonIncBy1VoltageStep = new wxButton(
//  wxBitmap wxbitmapIncreaseVoltage( m_wxstrIconFilesPrefix +
//     wxT("increase16x16.png") , wxBITMAP_TYPE_PNG ) ;
  wxBitmap wxbitmapSetAsDesiredVoltage ;
//Avoid to show up an error dialog under _Windows_ if the image file could not
//be load.
//#ifndef _WIN32
#ifdef USE_EXTERNAL_ICONS
   if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
       // #b825460a217d250db53df0c9ca293068:
       // "Returns true if bitmap data is present."
//     ! wxbitmapIncreaseVoltage.IsOk()
     ! wxbitmapSetAsDesiredVoltage.
       //http://docs.wxwidgets.org/2.6/wx_wxbitmap.html#wxbitmaploadfile:
       // "true if the operation succeeded, false otherwise."
       LoadFile(  m_wxstrIconFilesPrefix +
         wxT("set_as_desired_voltage16x16.png") , wxBITMAP_TYPE_PNG )
     )
#endif
     wxbitmapSetAsDesiredVoltage = wxBitmap( set_as_desired_voltage16x16_xpm ) ;
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , ID_SetAsWantedVoltage
//    , wxT("&+")
    , wxbitmapSetAsDesiredVoltage
    , wxDefaultPosition,
    wxDefaultSize,
    wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  p_wxbitmapbutton->SetToolTip( wxT("Set the voltage sliders's CPU core "
    "voltage\n"
    "as the voltage for the frequency slider's frequency in MHz\n"
    "to use for Dynamic Voltage and Frequency Scaling") ) ;
  p_wxsizer->Add( //mp_wxspinbuttonVoltageInVolt
//    p_wxbuttonIncBy1VoltageStep
    p_wxbitmapbutton
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
}

void FreqAndVoltageSettingDlg::AddSetAsMinVoltageButton( wxSizer * p_wxsizer )
{
  //  wxSize wxsizeMinimal(-1,-1) ;
  //  p_wxbuttonIncBy1VoltageStep = new wxButton(
//  wxBitmap wxbitmapIncreaseVoltage( m_wxstrIconFilesPrefix +
//     wxT("increase16x16.png") , wxBITMAP_TYPE_PNG ) ;
  wxBitmap wxbitmapSetAsMinVoltage ;
//Avoid to show up an error dialog under _Windows_ if the image file could not
//be load.
//#ifndef _WIN32
#ifdef USE_EXTERNAL_ICONS
   if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
       // #b825460a217d250db53df0c9ca293068:
       // "Returns true if bitmap data is present."
//     ! wxbitmapIncreaseVoltage.IsOk()
     ! wxbitmapSetAsMinVoltage.
       //http://docs.wxwidgets.org/2.6/wx_wxbitmap.html#wxbitmaploadfile:
       // "true if the operation succeeded, false otherwise."
       LoadFile(  m_wxstrIconFilesPrefix +
         wxT("set_as_minimal_voltage16x16.png") , wxBITMAP_TYPE_PNG )
     )
#endif
     wxbitmapSetAsMinVoltage = wxBitmap( set_as_minimal_voltage16x16_xpm ) ;
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , ID_SetAsMinVoltage
//    , wxT("&+")
    , wxbitmapSetAsMinVoltage
    , wxDefaultPosition,
    wxDefaultSize,
    wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  p_wxbitmapbutton->SetToolTip( wxT("Set the voltage sliders's CPU core "
    "voltage\n"
    "as the lowest stable voltage\n"
    "for the frequency slider's frequency in MHz\n"
    "to avoid setting a too low voltage") ) ;
  p_wxsizer->Add( //mp_wxspinbuttonVoltageInVolt
//    p_wxbuttonIncBy1VoltageStep
    p_wxbitmapbutton
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
}

inline void FreqAndVoltageSettingDlg::AddVoltageSettingsSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  LOGN( FULL_FUNC_NAME << "--begin")
//  wxFlexGridSizer *
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer *
    p_wxflexgridsizerSetAsMinVoltage = new //wxFlexGridSizer(
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer(
    //number of columns: zero, it will be calculated to form the total
    //number of children in the sizer
    0 );
  AddIncreaseVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  AddSelectPstateViaMousePositionButton(p_wxflexgridsizerSetAsMinVoltage) ;
  AddDecreaseVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  mp_wxstatictextWantedVoltageInVolt = new wxStaticText(
    this, wxID_ANY, //_T("")
    //Insert spaces so that there is enough room for displaying the "wanted
    //voltage in Volt" if the checkbox "also set wanted voltage in Volt" is
    //checked. a label text may have 5 characters, e.g. "1.0370"
    wxT("         ")
    ) ;
//  mp_wxbuttonSetAsMinVoltage = new wxButton(
//    this,
//    ID_SetAsMinVoltage,
//    wxT("set as minimum voltage") ,
//    wxDefaultPosition ,
//    wxDefaultSize ,
//    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//    // "If you need to use this style in order to get the arrows or etc., but
//    // would still like to have normal keyboard navigation take place, you
//    // should create and send a wxNavigationKeyEvent in response to the key
//    // events for Tab and Shift-Tab."
//    //To get EVT_CHAR events when the button is focused.
//    wxWANTS_CHARS //| wxTAB_TRAVERSAL
//    ) ;
//  mp_wxbuttonSetAsMinVoltage->SetToolTip( wxT("This should be the lowest "
//    "stable voltage for a specific frequency to avoid setting a too low "
//    "voltage") ) ;
  AddSetAsDesiredVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  AddSetAsMinVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  AddAlsoSetWantedVoltageControls(p_wxflexgridsizerSetAsMinVoltage);

  //p_wxboxsizerSetAsMinVoltage->Add(
//  p_wxflexgridsizerSetAsMinVoltage->Add(
//    mp_wxbuttonSetAsMinVoltage
//    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
//    //[...]can change its size in the main orientation of the wxBoxSizer -
//    //where 0 stands for not changeable[...]
//    , 1
//    , //wxEXPAND |
//    wxFIXED_MINSIZE | wxALL
//    , 0
//    );
  //p_wxboxsizerSetAsMinVoltage->Add(
  AddPreventVoltageBelowLowestStableVoltageButton(
    p_wxflexgridsizerSetAsMinVoltage) ;
  AddPreventVoltageAboveDefaultVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  AddStabilizeVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
//  AddAutoConfigureVoltageSettingsButton(p_wxflexgridsizerSetAsMinVoltage);

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
  p_wxsizerSuperordinate->Add(
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
}

// The "Add" function of a "wxSizer"-derived class object is not called
// when it was casted to the base class "wxSizer")--possibly because the
// wxSizer::"Add" function is not declared "virtual" but as "inline"
// (for polymorphism "Virtual method tables" are needed which seems to be
// hardly possible with "inline"--inlining avoids functions).
// So this member function is provided
void FreqAndVoltageSettingDlg::AddWindowToSizer(
  wxWindow * p_window,
  wxSizer * p_wxsizer,
  int proportion, int flag, int border, wxObject* userData
  )
{
  LOGN( FULL_FUNC_NAME << "--class name of window to add: \"" << GetStdString(
    std::tstring(p_window->GetClassInfo()->GetClassName() )) << "\"")
#ifndef USE_WX_BITMAPTOGGLEBUTTON
  //from http://en.wikipedia.org/wiki/RTTI#C.2B.2B_Example:
  // to find whether abc_pointer is pointing to xyz type of object
  wx2_9compatibility::wxFlexGridSizer * p_wx2_9compatibility_wxFlexGridSizer =
    dynamic_cast<wx2_9compatibility::wxFlexGridSizer *> (p_wxsizer);
  if( p_wx2_9compatibility_wxFlexGridSizer )
  {
    LOGN( FULL_FUNC_NAME << "--sizer is wx2_9compatibility::wxFlexGridSizer)")
//    p_wxsizer = (wxSizer *) p_wx2_9compatibility_wxFlexGridSizer;
    p_wx2_9compatibility_wxFlexGridSizer->Add(
      p_window
      , proportion //0=the control should not take more space if the sizer is enlarged
      , flag //wxLEFT | wxRIGHT
      , border
      );
  }
  else
#endif //USE_WX_BITMAPTOGGLEBUTTON
  {
    LOGN( FULL_FUNC_NAME << "--sizer <> wx2_9compatibility::wxFlexGridSizer)")
    p_wxsizer->Add(
      p_window
      , proportion //0=the control should not take more space if the sizer is enlarged
      , flag //wxLEFT | wxRIGHT
      , border
      );
  }
}

void FreqAndVoltageSettingDlg::AddWritePstateButton(wxSizer * p_wxsizer)
{
  mp_wxbuttonApply = new wxButton(
    this,
    wxID_APPLY,
    wxT("&Write"
      //" p-state"
      ) ,
    wxDefaultPosition ,
    wxDefaultSize ,
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;

  bool bSetToolTip = false ;
  if( ! mp_model->m_cpucoredata.m_arfAvailableMultipliers )
  {
    m_wxstrWriteVoltageAndMultiplierToolTip += wxT("no multipliers are available") ;
    bSetToolTip = true ;
  }
  if( ! mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt )
  {
    bSetToolTip = true ;
    if( //! wxstrToolTip.Empty()
        bSetToolTip )
      m_wxstrWriteVoltageAndMultiplierToolTip += wxT(" and ") ;
    m_wxstrWriteVoltageAndMultiplierToolTip += wxT("no voltages are available") ;
  }
  if( bSetToolTip )
  {
    //If disabled then no tooltip is visible.
    //mp_wxbuttonApply->Enable(false) ;

    DisableWritingVoltageAndMultiplier(m_wxstrWriteVoltageAndMultiplierToolTip);
  }
  else
  {
    m_bAllowWritingVoltageAndFrequency = true;
    m_wxstrWriteVoltageAndMultiplierToolTip = wxT(">>w<<rite/ set (both) the "
      "configured multiplier/ frequency AND voltage") ;
    mp_wxbuttonApply->SetToolTip( m_wxstrWriteVoltageAndMultiplierToolTip ) ;
  }

  p_wxsizer->Add( mp_wxbuttonApply ) ;
}

inline void FreqAndVoltageSettingDlg::AddSetAsWantedVoltageSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer *
    p_wxflexgridsizerSetAsWantedVoltage = new
    WX_BITMAP_TOGGLE_BUTTON_NAMESPACE::wxFlexGridSizer(
    //number of columns: zero, it will be calculated to form the total
    //number of children in the sizer
    0 );
//  mp_wxbuttonSetAsWantedVoltage = new wxButton(
//    this,
//    ID_SetAsWantedVoltage,
//    wxT("set as wanted voltage") ,
//    wxDefaultPosition ,
//    wxDefaultSize ,
//    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//    // "If you need to use this style in order to get the arrows or etc., but
//    // would still like to have normal keyboard navigation take place, you
//    // should create and send a wxNavigationKeyEvent in response to the key
//    // events for Tab and Shift-Tab."
//    //To get EVT_CHAR events when the button is focused.
//    wxWANTS_CHARS
//    ) ;
//  mp_wxbuttonSetAsWantedVoltage->SetToolTip(
//    wxT("this is the voltage that should be used for Dynamic Voltage and "
//      "Frequency Scaling") ) ;
//  p_wxflexgridsizerSetAsWantedVoltage->Add(
//    mp_wxbuttonSetAsWantedVoltage
//    //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
//    //[...]can change its size in the main orientation of the wxBoxSizer -
//    //where 0 stands for not changeable[...]
//    , 1
//    , //wxEXPAND |
//    wxFIXED_MINSIZE | wxALL
//    , 0
//    );
  AddPauseServiceCheckbox(p_wxflexgridsizerSetAsWantedVoltage) ;
  p_wxsizerSuperordinate->Add(
    p_wxflexgridsizerSetAsWantedVoltage,
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
}

void FreqAndVoltageSettingDlg::AddStabilizeVoltageButton( wxSizer * p_wxsizer )
{
  //  wxButton * p_wxbutton = new wxButton(
  //Avoid to show up an error dialog under _Windows_ if the image file could not
  //be load.
//  #ifdef _WIN32
#ifndef USE_EXTERNAL_ICONS
  wxBitmap wxbitmapStabilizeVoltage( stabilize_voltage_xpm ) ;
#else
  wxBitmap wxbitmapStabilizeVoltage( m_wxstrIconFilesPrefix +
    wxT("stabilize_voltage.png") ) ;
  if( //http://docs.wxwidgets.org/trunk/classwx_bitmap.html
      // #b825460a217d250db53df0c9ca293068:
      // "Returns true if bitmap data is present."
    ! wxbitmapStabilizeVoltage.IsOk()
    )
    wxbitmapStabilizeVoltage = wxBitmap( stabilize_voltage_xpm ) ;
#endif
  wxBitmapButton * p_wxbitmapbutton = new wxBitmapButton(
    this
    , ID_StabilizeVoltage
  //   , wxT("stabilize voltage")
    , wxbitmapStabilizeVoltage
    , wxDefaultPosition,
    wxDefaultSize,
    wxBU_AUTODRAW |
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    ) ;
  //p_wxbutton->SetToolTip(
  p_wxbitmapbutton->SetToolTip( wxT(">>s<<tabilize voltage / ") +
    wxString::Format(
    wxT("add %f Volt to the current "
      "voltage slider voltage"),
      mp_model->m_userinterfaceattributes.m_fOperatingSafetyMarginInVolt
      )
    ) ;
  p_wxsizer->Add(
  //    p_wxbutton
    p_wxbitmapbutton
    , 0 //0=the control should not take more space if the sizer is enlarged
    , wxLEFT | wxRIGHT
    , 0 );
}

FreqAndVoltageSettingDlg::FreqAndVoltageSettingDlg(
  wxWindow * p_wxwindowParent
  , I_CPUcontroller * p_cpucontroller
  , BYTE byCoreID
  , wxWindowID wxwindow_id ,
  const wxString & cr_wxstrTitle ,
  const wxPoint & cr_wxpointWindowPosition ,
  const wxSize & cr_wxsizeWindow ,
  long lStyle
  )
  : wxDialog( 
      p_wxwindowParent, 
      wxwindow_id //wxID_ANY,
#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
      wxString::Format( 
      //We need a wxT() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      wxT("settings for core %u") ,
      byCoreID 
    )
#else //#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
    , cr_wxstrTitle //wxT("voltage and frequency settings")
#endif //#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
    //A value of (-1, -1) indicates a default size.
    , cr_wxpointWindowPosition //wxPoint(30, 30)
    , cr_wxsizeWindow //wxSize(600, 300)
    , lStyle //wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  , m_byCoreID(byCoreID)
  , m_byPstateID(0)
  , mp_cpucontroller (p_cpucontroller)
  , mp_mainframe( (MainFrame *) p_wxwindowParent )
  , mp_model ( p_cpucontroller->mp_model )
  , m_wxstrIconFilesPrefix( wxT("icons/") )
{
  LOGN("voltage and freq dialog begin")
//  //http://docs.wxwidgets.org/trunk/classwx_tool_tip.html
//  // #527208d12631dfb3e6dea71011b887d6:
//  // "Enable or disable tooltips globally."
//  wxToolTip::Enable( true) ;
//  //http://docs.wxwidgets.org/trunk/classwx_tool_tip.html
//  // #5db5357f084afef9c4602baf0cb89105
//  //"Set the delay after which the tooltip appears."
//  wxToolTip::SetDelay (//long msecs
//    0 ) ;
//  //"Set the delay after which the tooltip disappears or how long a tooltip
//  // remains visible."
//  wxToolTip::SetAutoPop (//long msecs
//    5000) ;
//  //"Set tooltip maximal width in pixels. "
//  wxToolTip::SetMaxWidth (//int width
//    1000) ;
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
//  wxBoxSizer * p_wxboxsizerCPUcoreVoltageInVolt = new wxBoxSizer(wxHORIZONTAL);
//  wxBoxSizer * p_wxboxsizerSetAsMinVoltage = new wxBoxSizer(wxHORIZONTAL);
//  wxBoxSizer * p_wxboxsizerTop = new wxBoxSizer(wxVERTICAL);
  p_wxboxsizerTop = new wxBoxSizer(wxVERTICAL);
//  wxButton * p_wxbuttonDecBy1VoltageStep ;
//  wxButton * p_wxbuttonIncBy1VoltageStep ;

  //mp_wxbuttonApply = new wxButton(this, wxID_APPLY ) ;
//  m_p_wxbuttonFindLowestStableVoltage = new wxButton(
//    this,
//    ID_findLowestStableVoltage,
//    wxT("find &lowest stable voltage") ,
//    wxDefaultPosition ,
//    wxDefaultSize ,
//    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//    // "If you need to use this style in order to get the arrows or etc., but
//    // would still like to have normal keyboard navigation take place, you
//    // should create and send a wxNavigationKeyEvent in response to the key
//    // events for Tab and Shift-Tab."
//    //To get EVT_CHAR events when the button is focused.
//    wxWANTS_CHARS
//    ) ;

  m_wxstrWriteVoltageAndMultiplierToolTip = wxT("mustn't write because ");
  //wxButton *btn = new wxButton(this, DIALOGS_MODELESS_BTN, _T("Apply"));

//  mp_wxcheckboxValidPstate = new wxCheckBox(this, wxID_ANY,
//      //_T("Set as current after apply") ) ;
//      _T("valid p-state") ) ;

//  mp_wxcheckboxCOFVIDcontrol = new wxCheckBox(this, wxID_ANY,
//      //_T("Set as current after apply") ) ;
//      _T("write into COFVID control register") ) ;

//  mp_wxstatictextExpectedCurrentDissipation = new wxStaticText(
//    this, wxID_ANY, _T("")
//    ) ;

  //mp_wxsliderCPUcorePstate->SetBackgroundStyle(wxBG_STYLE_COLOUR);
  //mp_wxsliderCPUcorePstate->SetForegroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnBackgroundColour(*wxRED);
  //mp_wxsliderCPUcorePstate->SetOwnForegroundColour(*wxBLUE);
  //mp_wxsliderCPUcorePstate->SetToolTip("too high");

  //m_vecp_wxcontrol.push_back(p_wxsliderCPUcoreFrequencyMultiplier);

  //Only display the checkbox options if more than 1 logical CPU core exists.
  //(else if 1 CPU core then it is clear that the 1st and only core should be
  // used)
  if( mp_model->m_cpucoredata.GetNumberOfCPUcores() > 1 )
    AddCPUcoreCheckBoxSizer(p_wxboxsizerTop) ;
  CreateSliders();
  AddPerformanceStateSizer(p_wxboxsizerTop) ;

  AddCPUcoreFrequencySizer(p_wxboxsizerTop) ;

  AddCPUcoreVoltageSizer(p_wxboxsizerTop) ;

  AddVoltageSettingsSizer(p_wxboxsizerTop );

//  AddSetAsWantedVoltageSizer(p_wxboxsizerTop );
    //sizerTop->AddSpacer(50) ;
  //sizerTop->Add(new wxStaticLine( this, wxID_ANY ) ) ;

  //mp_wxstatictextPercentageOfDefaultVoltage = new wxStaticText(this,
  // wxID_ANY, _T("") );
  //sizerTop->Add( mp_wxstatictextPercentageOfDefaultVoltage ) ;
  
  //p_wxboxsizerOK_Cancel->Add(new wxButton(this, wxID_APPLY //, _T("Apply")
  //  ) );
  AddApplyOrCancelSizer(p_wxboxsizerTop) ;

  m_p_wxboxsizerMessage = new wxBoxSizer(wxHORIZONTAL);

  mp_wxstatictextMessage = new wxStaticText(this, wxID_ANY, wxT("") );
  m_p_wxboxsizerMessage->Add( mp_wxstatictextMessage ) ;

  p_wxboxsizerTop->Add(
      m_p_wxboxsizerMessage,
      0 ,
      wxFIXED_MINSIZE,
      //Determines the border width, if the flag  parameter is set to include
      //any border flag.
      2
      );

  LOGN("before adding to the outer sizer")
//  wxPanel * p_wxpanel = new wxPanel( this, ID_Panel ) ;
  KeyEventPanel * p_wxpanel = new KeyEventPanel( * this ) ;
  SetSizer(p_wxboxsizerTop);
//  p_wxpanel->SetSizer(p_wxboxsizerTop);
  AddChild( p_wxpanel ) ;
//  Connect(
////    ID_Panel ,
//    ID_SetAsMinVoltage ,
////    wxID_ANY ,
//    wxEVT_CHAR ,
//    //wxEventFunction //)
//    wxKeyEventHandler( FreqAndVoltageSettingDlg::OnChar )
//    );
//  p_wxpanel->Layout() ;
  p_wxboxsizerTop->SetSizeHints(this);
  p_wxboxsizerTop->Fit(this);
  //To show the computed freq and voltage at the beginning yet.
  //OutputFreqAndVoltageByControlValues();
  //OutputVoltageByControlValues();
//  VoltageIDchanged(m_wVoltageID) ;
  //Force the neighbour controls of "voltage in volt" to be resized.
  Layout() ;
  if( mp_model->m_userinterfaceattributes.m_wToolTipDelay != MAXWORD )
    //http://docs.wxwidgets.org/trunk/classwx_tool_tip.html
    // #5db5357f084afef9c4602baf0cb89105
    //"Set the delay after which the tooltip appears."
    //"[...] wxToolTip::SetDelay can be used to globally alter tooltips behaviour"
    //wxToolTip::SetDelay(...) must be called here. When it was called in
    //App::OnInit() or in MainFrame's constructor it did not work.
    wxToolTip::SetDelay(mp_model->m_userinterfaceattributes.m_wToolTipDelay);
//  wxToolTip::SetDelay(5000);
  HandlePstateMayHaveChanged() ;
  OutputVoltageByControlValues() ;
  OutputFrequencyByControlValues() ;
  //From http://wiki.wxwidgets.org/Catching_key_events_globally
  // #Recursive_connect
  ConnectCharEvent(this);
  //For possibly disabling the "write p-state" button at the beginning.
  HandleCPUcoreFrequencyOrVoltageChanged(NULL) ;
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

//Adapted from http://wiki.wxwidgets.org/Catching_key_events_globally
// #Recursive_connect:
// ( This function should be called _after_ creation of children of the
// windows as parameter )
//http://docs.wxwidgets.org/2.6/wx_eventhandlingoverview.html#eventprocessing:
//"As mentioned above, only command events are recursively applied to the
//parents event handler in the library itself. As this quite often causes
//confusion for users, here is a list of system events which will NOT get sent
//to the parent's event handler: [...]
//wxKeyEvent [...]"
void FreqAndVoltageSettingDlg::ConnectCharEvent(
  //The hierarchy of children is a tree: a window may have at least 1 child,
  // this also applies to each of the child windows.
  wxWindow * p_wxWindowThatMayContainChildren
  )
{
  LOGN("FreqAndVoltageSettingDlg::ConnectCharEvent("
    << p_wxWindowThatMayContainChildren << ")" )
  if(p_wxWindowThatMayContainChildren )
  {
//    switch( p_wxWindowThatMayContainChildren->GetId() )
//    {
//    case ID_PstateSlider :
//    case ID_MultiplierSlider :
//    case ID_VoltageSlider :
//    case ID_FrequencySlider :
//      break ;
//    default:
      p_wxWindowThatMayContainChildren->Connect(
  //      wxID_ANY,
        p_wxWindowThatMayContainChildren->GetId() ,
        wxEVT_CHAR,
        wxKeyEventHandler(FreqAndVoltageSettingDlg::OnChar),
        (wxObject *) NULL,
        this
        );
      wxWindowListNode * p_wxwindow_list_node =
        p_wxWindowThatMayContainChildren->GetChildren().GetFirst();
      //Traverse all children.
      while(p_wxwindow_list_node)
      {
        wxWindow * p_wxwindowChild = p_wxwindow_list_node->GetData();
        //TODO possibly eliminate the recursive call of _this_ function here.
        ConnectCharEvent(p_wxwindowChild);
        p_wxwindow_list_node = p_wxwindow_list_node->GetNext();
      }
//    } //switch( p_wxWindowThatMayContainChildren->GetId() )
  } // if(p_wxWindowThatMayContainChildren )
}

void FreqAndVoltageSettingDlg::CPUcoreVoltageChanged()
{
  mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings =
  //    mp_cpucontroller->GetVoltageInVolt(nNewValue) ;
  //      mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt[ nNewValue] ;
    GetVoltageInVoltFromSliderValue() ;
  if( m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage->GetValue() )
  {
    //float fStableVoltageToSet = mp_cpucontroller->GetVoltageInVolt(
      //nNewSliderValue )
    m_fWantedVoltageInVolt =
      mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings
      //Add 0.07 V for stability safety
      //+ 0.07 ;
      + mp_model->m_userinterfaceattributes.m_fOperatingSafetyMarginInVolt ;
  //    WORD wVoltageID = //In order to get the voltage in Volt that corresponds
  //      //to a voltage ID boundary first convert to the corresponding
  //      //(possibly rounded) voltage ID.
  //      mp_cpucontroller->GetVoltageID(
  //      fStableVoltageToSet ) ;
  //    //Store value in member for OnSetAsMinimumVoltage().
  //    m_fWantedVoltageInVolt = mp_cpucontroller->GetVoltageInVolt(
  //      wVoltageID ) ;
    mp_wxstatictextWantedVoltageInVolt->SetLabel(
      wxString::Format(
        //We need a _T() macro (wide char-> L"", char->"") for EACH
        //line to make it compatible between char and wide char.
        _T("%f"), //PState::GetVoltageInVolt(
        m_fWantedVoltageInVolt
        )
      ) ;
  }
//  else
//    mp_wxstatictextWantedVoltageInVolt->SetLabel( wxT("bla")) ;
  OutputVoltageByControlValues() ;
  HandleCPUcoreFrequencyOrVoltageChanged(mp_wxsliderCPUcoreVoltage);

  //To possibly enlarge the window because of a wider "wanted voltage in Volt"
  // static text
//  SetSizeHints(this);
//  Fit(); // "set window size to wrap around its children"
//  FitInside() ;// "set virtual size to satisfy children"
  //http://docs.wxwidgets.org/trunk/classwx_window.html
  // #55aca401aab29d59f7cc53f89ba2e38d:
  //"Also, if the window has exactly one subwindow it is better (faster and
  //the result is more precise as Fit() adds some margin to account for
  //fuzziness of its calculations) to call:
  //window->SetClientSize(child->GetSize());
  //instead of calling Fit()."
//  SetClientSize( p_wxboxsizerTop->GetSize() );
  //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizerfit:
  //"Tell the sizer to resize the window to match the sizer's minimal size."
//  p_wxboxsizerTop->Fit(this) ;

//  //Force the neighbour controls of "voltage in volt" to be resized.
//  Layout() ; // "lay out the window and its children
  //mp_mainframe->Refresh() ;//rm_timer.
  mp_mainframe->m_wxtimer.Stop() ;
  mp_mainframe->m_wxtimer.Start(mp_mainframe->m_dwTimerIntervalInMilliseconds);
}

void FreqAndVoltageSettingDlg::CreateFindLowestStableCPUcoreVoltageButton()
{
  wxBitmap wxbitmapFindLowestStableCPUcoreVoltage = wxBitmap(
    find_lowest_stable_CPU_core_voltage16x16_xpm
//    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm
    ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage =
    new wxBitmapButton(
   this
   , ID_findLowestStableVoltage
  //    , wxT("&+")
   , wxbitmapFindLowestStableCPUcoreVoltage
   , wxDefaultPosition,
   wxDefaultSize,
   wxBU_AUTODRAW |
   //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
   // "If you need to use this style in order to get the arrows or etc., but
   // would still like to have normal keyboard navigation take place, you
   // should create and send a wxNavigationKeyEvent in response to the key
   // events for Tab and Shift-Tab."
   //To get EVT_CHAR events when the button is focused.
   wxWANTS_CHARS
   ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip( wxT("start "
    "finding the lowest stable CPU core voltage") );
}

void FreqAndVoltageSettingDlg::CreateStopFindingLowestStableCPUcoreVoltageButton()
{
  //Setting an bitmap button after creation failed with wxWidgets 2.8.10.
  //So create a second button for stopping the low CPU core voltage detection.
  wxBitmap wxbitmapStopFindingLowestStableCPUcoreVoltage = wxBitmap(
    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm
    ) ;
  m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage =
    new wxBitmapButton(
   this
   , ID_stopFindingLowestStableVoltage
  //    , wxT("&+")
   , wxbitmapStopFindingLowestStableCPUcoreVoltage
   , wxDefaultPosition,
   wxDefaultSize,
   wxBU_AUTODRAW |
   //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
   // "If you need to use this style in order to get the arrows or etc., but
   // would still like to have normal keyboard navigation take place, you
   // should create and send a wxNavigationKeyEvent in response to the key
   // events for Tab and Shift-Tab."
   //To get EVT_CHAR events when the button is focused.
   wxWANTS_CHARS
   ) ;
  m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage->SetToolTip(
    wxT("stop finding the lowest stable CPU core voltage") );
}

void FreqAndVoltageSettingDlg::CreateSliders()
{
  BYTE byPstateIDtoShowSettingsFor = 0 ;
  VoltageAndFreq voltageandfreq ;
  LOGN("voltage and freq dialog creating sliders")

  wxString ar_wxstrVoltageTypes [] = { wxT("minimum"), wxT("wanted"),
    wxT("maximum") };
  m_p_wxchoiceVoltageType = new wxChoice(
    this,
    //wxID_ANY,
    VoltageTypeListBox,
    wxDefaultPosition, //const wxPoint& pos = wxDefaultPosition,
    wxDefaultSize, //const wxSize& size = wxDefaultSize,
    3, //int n = 0,
    ar_wxstrVoltageTypes, //const wxString choices[] = NULL
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    wxWANTS_CHARS
    );
  m_p_wxchoiceVoltageType->Select(maximum_voltage);
  m_p_wxchoiceVoltageType->SetToolTip( wxT("type of voltage for the voltage/"
    "frequency pair slider") );

  mp_wxsliderCPUcorePstate = new wxSlider(
    this
    //wxID_ANY, 
    , ID_PstateSlider 
    //Initial position for the slider.
    , //1 
    byPstateIDtoShowSettingsFor
    //see
    //http://en.wikipedia.org/wiki/Advanced_Configuration_and_Power_Interface
    // #Performance_states: "[...]P0 max power and frequency"
    //Slider minimum value.
    , 0 
    //Slider maximum value. If e.g. 2 p-states: 1st is "0", 2nd & last is "1"
    //, NUMBER_OF_PSTATES - 1 
    , mp_cpucontroller->GetNumberOfPstates() - 1
    //, 2
    //If the default point (-1, -1) is specified then a default point is chosen.
//    , wxPoint(-1, -1)
    , wxDefaultPosition
    //If the default size (-1, -1) is specified then a default size is chosen.
//    , wxSize(-1, -1)
    , wxDefaultSize
    , wxSL_AUTOTICKS | wxSL_LABELS
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    | wxWANTS_CHARS
    ) ;
  mp_wxsliderCPUcorePstate->SetToolTip(
    wxT("voltage/frequency pair slider: combination of CPU core voltage and "
      "frequency") ) ;
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
  BYTE byPstateExists = mp_cpucontroller->GetPstate(
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault, 0, voltageandfreq);
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
//    , wxPoint(-1, -1)
    , wxDefaultPosition
    //If the default size (-1, -1) is specified then a default size is chosen.
//    , wxSize(-1, -1)
    , wxDefaultSize
    , 
    //wxSL_AUTOTICKS //	Displays tick marks.
    //| 
//    wxSL_LABELS //Displays minimum, maximum and value labels.
    wxSL_TICKS
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    | wxWANTS_CHARS
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
    //slider max value
    //, MAX_VALUE_FOR_VID
//    , mp_cpucontroller->GetMaximumVoltageID()
    , wMaxValue
    //If the default point (-1, -1) is specified then a default point is chosen.
//    , wxPoint(-1, -1)
    , wxDefaultPosition
//    //If the default size (-1, -1) is specified then a default size is chosen.
    // "wxSize(-1, -1)" causes assertion 'width >= -1' failed ?!
//    , wxSize(-1, -1)
    , wxDefaultSize
    , wxSL_AUTOTICKS //| wxSL_LABELS
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    // "If you need to use this style in order to get the arrows or etc., but
    // would still like to have normal keyboard navigation take place, you
    // should create and send a wxNavigationKeyEvent in response to the key
    // events for Tab and Shift-Tab."
    //To get EVT_CHAR events when the button is focused.
    | wxWANTS_CHARS
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

uint32_t FreqAndVoltageSettingDlg::GetCPUcoreMask()
{
  uint32_t ui32CPUcoreMask = 0;
  WORD wNumCPUcores = mp_model->m_cpucoredata.GetNumberOfCPUcores() ;
  if( wNumCPUcores > 1 )
  {
    for( WORD wCPUcore = 0 ; wCPUcore < wNumCPUcores ; ++ wCPUcore )
      if( m_ar_p_wxcheckbox[wCPUcore]->IsChecked() )
        ui32CPUcoreMask |= ( 1 << wCPUcore);
  }
  else
    ui32CPUcoreMask = 1;
  return ui32CPUcoreMask;
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
  float fVoltage = GetVoltageInVoltFromSliderValue() ;
  LOGN("CPU core freq from slider :" << fFreq )
  BYTE byVoltageIsWithinValidRangeRetVal = VoltageIsWithinValidRange(
    fVoltage,fFreq) ;
  if( //success
      ! byVoltageIsWithinValidRangeRetVal )
  {
    if(
      //c_iter_stdset_fMultiplier != mp_model->m_cpucoredata.
      //m_stdset_floatAvailableMultipliers.end() &&
      fFreq
//      mp_wxcheckboxOnlySafeRange->IsChecked()
    //false
      )
    {
//      BYTE byIsSafe = 0 ;
//      //if( m_bPreviousCPUcoreFreqWas
//      if(
//          (
//          byIsSafe = mp_cpucontroller->GetPstateSafefy(
//            ////mp_wxsliderCPUcoreVoltage->GetValue()
//            //, //mp_wxsliderCPUcoreFrequencyMultiplier->GetValue()
//            (WORD) //( *c_iter_stdset_fMultiplier *
//            //mp_cpucontroller->m_fReferenceClockInMHz )
//              fFreq
//            , //mp_cpucontroller->GetVoltageInVolt(
//              //mp_wxsliderCPUcoreVoltage->GetValue() )
//            GetVoltageInVoltFromSliderValue()
//            )
//          ) == SETTING_VOLTAGE_IS_SAFE
//        )
//      {
        //RemoveAttention(mp_wxstatictextFreqInMHz);
        RemoveAttention(mp_wxstatictextVoltageInVolt);
        //The button might have been disabled (e.g. because of overvoltage
        //protection) .
        EnableWritingVoltageAndMultiplier(
          m_wxstrWriteVoltageAndMultiplierToolTip);
//      }
//      else
//      {
//        wxString wxstr ;
//        GetPstateUnsafetyDescription(byIsSafe,wxstr) ;
//        //SetAttention( mp_wxstatictextFreqInMHz, wxstr );
//        SetAttention( mp_wxstatictextVoltageInVolt ,
//          //_T("the voltage is too high for the freq")
//          wxstr );
//        if( byIsSafe != NOT_USING_ANY_OVERVOLTING_PROTECTION )
//        {
//            mp_wxbuttonApply->Disable() ;
//            mp_wxbuttonApply->SetToolTip(
//              //We need a _T() macro (wide char-> L"", char->"") for EACH
//              //line to make it compatible between char and wide char.
//              _T("This button has been disabled ")
//              _T("because of overvoltage protection")
//              ) ;
//        }
//      }
    }
    else //The button may be disabled before. So enable it.
    {
      wxString wxstrToolTip = wxT(" has been disabled because "
        "the frequency is 0 (maybe because the reference clock is 0).\n"
        "This may be the case because the GUI is connected to the service.") ;
//      mp_wxbuttonApply->Enable() ;

      //If disabled then no tooltip is visible.
//      mp_wxbuttonApply->Enable(false) ;
      DisableWritingVoltageAndMultiplier(
        //m_wxstrWriteVoltageAndMultiplierToolTip
        wxstrToolTip);

      //Showing a tool tip is not possible while the button is disabled (at least
      //under Windows).
//      mp_wxbuttonApply->SetToolTip( wxT("This button") + wxstrToolTip) ;
      SetAttention( mp_wxstatictextVoltageInVolt ,
        wxT("The \"") + mp_wxbuttonApply->GetLabel() + wxT("\" button") +
        wxstrToolTip );
    }
  } //if( VoltageIsWithinValidRange )
  else
  {
    wxString wxstrToolTip = wxT(" has been disabled because "
      "the voltage is not in a safe range.\nChange the voltage or frequency "
      "or uncheck "
      "1 or all checkboxes that prevent the voltage from being outside the "
      "safe range.") ;

    //Showing a tool tip is not possible while the button is disabled (at least
    //under Windows).
//    mp_wxbuttonApply->Enable(false) ;
    DisableWritingVoltageAndMultiplier(
      //m_wxstrWriteVoltageAndMultiplierToolTip
      wxstrToolTip);

//    mp_wxbuttonApply->SetToolTip( wxT("This button ") + wxstrToolTip ) ;
    SetAttention( mp_wxstatictextVoltageInVolt , wxT("The \"") +
      mp_wxbuttonApply->GetLabel() + wxT("\" button") +wxstrToolTip );
  }
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

const std::set<VoltageAndFreq> & FreqAndVoltageSettingDlg::
  GetSelectedVoltageTypeStdSet()
{
  switch( m_p_wxchoiceVoltageType->//GetCurrentSelection()
      GetSelection() )
  {
  case minimum_voltage:
    return mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable;
    break;
  case wanted_voltage:
    return mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted;
    break;
  }

  return mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault;
}

float FreqAndVoltageSettingDlg::GetVoltageInVoltFromSliderValue()
{
  LOGN("voltage slider value:" << mp_wxsliderCPUcoreVoltage->GetValue() )
  if( ! mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.empty() )
    return mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt[
      mp_wxsliderCPUcoreVoltage->GetValue() ] ;
  return 0.0 ;
}

void FreqAndVoltageSettingDlg::GetPstateUnsafetyDescription(
  BYTE byIsSafe, wxString & wxstr)
{
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
        _T("no appropriate voltage-frequency pair was found from the ")
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
  OutputFrequencyByControlValues() ;
  mp_mainframe->m_wFreqInMHzOfCurrentActiveCoreSettings =
      m_wPreviousFrequencyInMHz ;
//  m_byPreviousMultiplierValue = byCurrentCPUcoreFrequencyMultiplierValue ;
}

void FreqAndVoltageSettingDlg::HandlePstateMayHaveChanged()
{
  VoltageAndFreq voltageandfreq ;
  const std::set<VoltageAndFreq> & c_r_std_set_voltages =
    GetSelectedVoltageTypeStdSet();
  if( mp_cpucontroller->GetPstate( c_r_std_set_voltages,
      mp_wxsliderCPUcorePstate->GetValue() , voltageandfreq )
    )
  {
    m_wVoltageSliderValue = mp_wxsliderCPUcoreVoltage->GetValue() ;
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
      if( //mp_cpucontroller->
          GetInterpolatedVoltageFromFreq(
          wFreqInMHz
          //ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz
          , fVoltageInVolt
          , c_r_std_set_voltages
//          , cr_stdsetvoltageandfreqForInterpolation
          )
        )
      {
        mp_wxsliderFreqInMHz->SetValue( //m_pstate.GetFreqInMHz()
          byMultiIndex ) ;
        HandleMultiplierValueChanged() ;
        BYTE byIndexForClosestVoltage = SetVoltageSliderToClosestValue(
          fVoltageInVolt) ;
        ChangeVoltageSliderValue( byIndexForClosestVoltage) ;
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

//from http://docs.wxwidgets.org/trunk/classwx_key_event.html
// #3dccc5a254770931e5d8066ef47e7fb0
void FreqAndVoltageSettingDlg::HandleKeyEvent_Inline( wxKeyEvent & r_wxkeyevent)
{
//  wxChar wxch = r_wxkeyevent.GetUnicodeKey() ;
  int nKeyEvent = r_wxkeyevent.GetKeyCode() ;
  LOGN("FreqAndVoltageSettingDlg::HandleKeyEvent_Inline--key pressed:"
    << (char) //wxch
    nKeyEvent )
  switch( //wxch
    nKeyEvent
    )
  {
  case wxT('+'):
    LOGN("OnChar--\'+\' key pressed")
    {
      wxCommandEvent wxcommandevent ;
      OnIncVoltage( wxcommandevent ) ;
    }
    break ;
  case wxT('-'):
    LOGN("OnChar--\'+\' key pressed")
    {
      wxCommandEvent wxcommandevent ;
      OnDecVoltage( wxcommandevent ) ;
    }
    break ;
  case wxT('s'):
  case wxT('S'):
    LOGN("OnChar--\'s\' or \'S\' key pressed")
    {
      wxCommandEvent wxcommandevent ;
      OnStabilizeVoltageButton( wxcommandevent ) ;
    }
    break ;
  case wxT('w'):
  case wxT('W'):
    LOGN("OnChar--\'w\' or \'W\' key pressed")
    {
      wxCommandEvent wxcommandevent ;
      OnApplyButton( wxcommandevent ) ;
    }
    break ;
  case wxT('\t'): //tabulator
    LOGN("OnChar--tab key pressed")

//    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//    // "If you need to use this style in order to get the arrows or etc., but
//    // would still like to have normal keyboard navigation take place, you
//    // should create and send a wxNavigationKeyEvent in response to the key
//    // events for Tab and Shift-Tab."
//    //Sending/ processing a navigation event is needed to change the input
//    //focus.
//    wxNavigationKeyEvent wxnavigationkeyevent ;
//    if( r_wxkeyevent.ShiftDown() )
//      wxnavigationkeyevent.SetDirection(//bool bForward
//        false ) ;
//    ProcessEvent( wxnavigationkeyevent ) ;

    //http://docs.wxwidgets.org/2.9.1/overview_changes_since28.html
    // #overview_changes_other
//    //http://docs.wxwidgets.org/2.9.1/classwx_window.html
//    // #53ba9981a718e971db096ab733852486
    wxWindow * p_wxwindow = (wxWindow *) r_wxkeyevent.GetEventObject() ;
    p_wxwindow->Navigate(   //int      flags = IsForward
      r_wxkeyevent.ShiftDown() ? wxNavigationKeyEvent::IsBackward :
      wxNavigationKeyEvent::IsForward
      ) ;
//    //wxWindow::NavigateIn(): since wxWidgets 2.9.1
//    NavigateIn    (   //int      flags = IsForward
//      r_wxkeyevent.ShiftDown() ? wxNavigationKeyEvent::IsBackward :
//      wxNavigationKeyEvent::IsForward ) ;
//    this->Set
    //For setting the focus to the next window control.
//    wxDialog::OnChar(r_wxkeyevent) ;
//    wxDialog::ProcessEvent( r_wxkeyevent) ;
    break ;
  }
  //Let the default actions happen.
  //http://docs.wxwidgets.org/2.6/wx_wxevent.html#wxeventskip:
  // "Called by an event handler, it controls whether additional event
  // handlers bound to this event will be called after the current event
  //handler returns. Skip(false) (the default behavior) will prevent
  //additional event handlers from being called and control will be returned
  //to the sender of the event immediately after the current handler has
  //finished. Skip(true) will cause the event processing system to continue
  //searching for a handler function for this event."
  r_wxkeyevent.Skip();
}

void FreqAndVoltageSettingDlg::OnChar( wxKeyEvent & r_wxkeyevent)
{
  LOGN("FreqAndVoltageSettingDlg::OnChar")
  HandleKeyEvent_Inline(r_wxkeyevent) ;
}

void FreqAndVoltageSettingDlg::OnCharHook( wxKeyEvent & r_wxkeyevent)
{
  LOGN("FreqAndVoltageSettingDlg::OnCharHook")
//  ProcessPendingEvents()
}

void FreqAndVoltageSettingDlg::OnClose( wxCloseEvent & wxcmd )
{
  DWORD dwExitCode;

  if( mp_mainframe->m_p_freqandvoltagesettingsdialog == this)
  {
    mp_mainframe->m_p_freqandvoltagesettingsdialog = NULL;
    mp_mainframe->SetCursor(wxNullCursor);
  }
  //TODO change to ~ m_x86iandc_threadFindLowestStableVoltage.IsRunning()
  ::GetExitCodeThread( wxGetApp().
    m_x86iandc_threadFindLowestStableVoltage.
    m_handleThread, & dwExitCode ) ;
   if(
  //        ::WaitForSingleObject( wxGetApp().
  //          m_x86iandc_threadFindLowestStableVoltage.m_handleThread,
  //          //wxGetApp().m_x86iandc_threadFindLowestStableVoltage
  //          0)
  //          ==
       dwExitCode == STILL_ACTIVE
     )
   {
     wxGetApp().MessageWithTimeStamp(
//     if(
         //::wxMessageBox( wxT(
         L"The thread for detecting an "
           "instable CPU core voltage is still running.\n"
           //"Close the dialog anyway?"),
             "You should stop the instable CPU core voltage detection before "
             "in order to close the dialog."//)
//           wxT("question")//,
  //         wxYES_NO
           );
         //== wxYES
//       )
   }
   else
   {
//     mp_mainframe->m_crit_secVoltAndFreqDlgs.Enter();
//     mp_mainframe->m_stdvec_p_freqandvoltagesettingdlg.delete(this);
//     mp_mainframe->m_crit_secVoltAndFreqDlgs.Leave();

     Destroy();
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

  //Safer if the _current_ value is fetched ("stabilize voltage" may have
  //been done before).
  m_wVoltageSliderValue = mp_wxsliderCPUcoreVoltage->GetValue();
  //If the current voltage is NOT the first/ lowest element.
  if( m_wVoltageSliderValue > 0 )
  {
    -- m_wVoltageSliderValue ;
    ChangeVoltageSliderValue(m_wVoltageSliderValue) ;
  }
}

void FreqAndVoltageSettingDlg::OnSelectPstateViaMousePos( wxCommandEvent & wxcmd )
{
  if( mp_mainframe->m_p_freqandvoltagesettingsdialog == this)
  {
    mp_mainframe->m_p_freqandvoltagesettingsdialog = NULL;
    mp_mainframe->SetCursor(wxNullCursor);
  }
  else
  {
    wxCursor wxcursorCross(wxCURSOR_CROSS);
    mp_mainframe->SetCursor( wxcursorCross
      //* wxCROSS_CURSOR
      );
    //::wxSetCursor( * wxCROSS_CURSOR);
    mp_mainframe->m_p_freqandvoltagesettingsdialog = this;
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
  if( ( m_wVoltageSliderValue + 1 ) < (WORD) mp_model->m_cpucoredata.
      m_stdset_floatAvailableVoltagesInVolt.size()
    )
  {
    ++ m_wVoltageSliderValue ;
    ChangeVoltageSliderValue(m_wVoltageSliderValue) ;
  }
}

void FreqAndVoltageSettingDlg::
  OnPreventVoltageAboveDefaultVoltageCheckbox(
  wxCommandEvent & r_wxcommandevent )
{
  HandleCPUcoreFrequencyOrVoltageChanged( NULL) ;
}

void FreqAndVoltageSettingDlg::
  OnPreventVoltageBelowLowestStableVoltageCheckbox(
  wxCommandEvent & r_wxcommandevent )
{
//  if( m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage-> )
  HandleCPUcoreFrequencyOrVoltageChanged( NULL) ;
}

#ifdef wxHAS_POWER_EVENTS
  void FreqAndVoltageSettingDlg::OnResume(wxPowerEvent & WXUNUSED(event) )
  {
    LOGN( FULL_FUNC_NAME << "--(object:" << this <<
        ")--resumed from standby/ hibernate")
    ResumendFromStandByOrHibernate();
  }
#endif // wxHAS_POWER_EVENTS

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
    m_wVoltageSliderValue = mp_wxsliderCPUcoreVoltage->GetValue() ;
//    WORD wVoltageInVoltIndex = mp_wxsliderCPUcoreVoltage->GetValue() ;
//    mp_model->m_cpucoredata.GetVoltageInVolt(wVoltageInVoltIndex) ;
//    VoltageIDchanged(m_wVoltageSliderValue) ;
    CPUcoreVoltageChanged() ;
    break;
  }
  //OutputFreqAndVoltageByControlValues() ;
}

//void FreqAndVoltageSettingDlg::OnSpinVoltageDown( wxSpinEvent & event)
//{
//  float fVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
//  if( //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID ) >
//    fVolt >
//    mp_cpucontroller->GetMinimumVoltageInVolt()
//    )
//  {
//    //-- m_wVoltageID ;
//    //mp_i_cpucontroller->DecreaseVoltageBy1Step( fVolt ) ;
//    //mp_i_cpucontroller->GetVoltageInVolt(m_wVoltageID )
//    //VoltageIDchanged(m_wVoltageID) ;
//  }
//}
//
//void FreqAndVoltageSettingDlg::OnSpinVoltageUp( wxSpinEvent & event)
//{
//  float fVolt = mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) ;
//  if( mp_cpucontroller->GetVoltageInVolt(m_wVoltageID ) <
//    mp_cpucontroller->GetMaximumVoltageInVolt()
//    )
//  {
//    //++ m_wVoltageID ;
//    mp_cpucontroller->IncreaseVoltageBy1Step( fVolt ) ;
//    m_wVoltageID = mp_cpucontroller->GetVoltageID( fVolt ) ;
//    VoltageIDchanged(m_wVoltageID) ;
//  }
//}

void FreqAndVoltageSettingDlg::OnSetAsMinVoltageButton(
  wxCommandEvent & wxcmd )
{
  //TODO maybe disable the button if the ref. clock is 0
  // Reference clock <> 0
  if( mp_cpucontroller->m_fReferenceClockInMHz )
  {
    CPUcoreData & r_cpucoredata = mp_cpucontroller->mp_model->m_cpucoredata ;
    WORD wMultiplierIndex = mp_wxsliderFreqInMHz->GetValue() ;
    WORD wFrequencyInMHz = //Explicit cast to WORD to avoid g++ warning.
     (WORD) ( r_cpucoredata.m_arfAvailableMultipliers[ wMultiplierIndex ] *
      mp_cpucontroller->m_fReferenceClockInMHz ) ;
    WORD wVoltageIndex = mp_wxsliderCPUcoreVoltage->GetValue() ;
    float fVoltage = //mp_cpucontroller->GetVoltageInVolt(
      //mp_wxsliderCPUcoreVoltage->GetValue() ) ;
      r_cpucoredata.m_arfAvailableVoltagesInVolt[ wVoltageIndex ] ;
    r_cpucoredata.m_wxcriticalsection.Enter() ;

    wxGetApp().SetAsMinimumVoltage(fVoltage, wFrequencyInMHz);

    if( m_p_wxbitmaptogglebuttonAlsoSetWantedVoltage->GetValue() )
    {
      wxGetApp().SetAsWantedVoltage(fVoltage + //0.07
        mp_model->m_userinterfaceattributes.m_fOperatingSafetyMarginInVolt ,
        wFrequencyInMHz) ;
//      if( m_p_wxchoiceVoltageType->GetSelection() == wanted_voltage)
//        RecreatePstateSlider();
    }
    r_cpucoredata.m_wxcriticalsection.Leave() ;
    mp_mainframe->RedrawEverything() ;
    mp_mainframe->Refresh() ; //force paint event/ call of "OnPaint()".
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

void FreqAndVoltageSettingDlg::OnSetVoltageType(wxCommandEvent & wxcmd)
{
//  switch( m_p_wxchoiceVoltageType->GetCurrentSelection() )
//  {
//  case minimum_voltage:
//    mp_wxsliderCPUcorePstate->SetMax(
//      mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable.size() );
//    break;
//  case maximum_voltage:
//    mp_wxsliderCPUcorePstate->SetMax(
//      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() );
//    break;
//  case wanted_voltage:
//    mp_wxsliderCPUcorePstate->SetMax(
//      mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.size() );
//    break;
//  }
  const std::set<VoltageAndFreq> & c_r_std_set_voltages =
    GetSelectedVoltageTypeStdSet();
  mp_wxsliderCPUcorePstate->SetMax( c_r_std_set_voltages.size() - 1 );
  mp_wxsliderCPUcorePstate->SetValue(0);
  //Set voltage slider and multiplier slider value.
  HandlePstateMayHaveChanged();
}

void FreqAndVoltageSettingDlg::OnSpinSecondsUntilVoltageDecreaseSpinButton(
  wxSpinEvent & event)
{
  LOGN( FULL_FUNC_NAME << "--begin")
//  if( event.GetInt() )
//  m_p_wxtextctrlSecondsUntilNextVoltageDecrease->SetLabel( //wxString::Format(
//    //wxT("%u"), event.GetPosition() )
//    wxT("200")
//    );
}

void FreqAndVoltageSettingDlg::OnStabilizeVoltageButton(
  wxCommandEvent & wxcmd )
{
  float fVoltageInVolt = GetVoltageInVoltFromSliderValue() ;
  fVoltageInVolt += mp_model->m_userinterfaceattributes.
    m_fOperatingSafetyMarginInVolt ;
  SetVoltageSliderToClosestValue(fVoltageInVolt) ;
  CPUcoreVoltageChanged() ;
  PossiblyWriteVoltageAndMultiplier_Inline(fVoltageInVolt) ;
}

void FreqAndVoltageSettingDlg::
  ChangeToStopFindingLowestStableCPUcoreVoltageButton()
{
  wxBitmap wxbitmapStopFindingLowestStableCPUcoreVoltage = wxBitmap(
    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetImageLabel(
    wxbitmapStopFindingLowestStableCPUcoreVoltage);
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
    wxT("stop finding the lowest stable CPU core voltage") );
}

void FreqAndVoltageSettingDlg::ChangeVoltageSliderValue(int nNewSliderValue)
{
  LOGN("VoltageIDchanged()")
  mp_wxsliderCPUcoreVoltage->SetValue(nNewSliderValue) ;
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
//  LOGN("voltage in Volt corresponding to the slider value " << nNewSliderValue
//    << " :" << mp_mainframe->m_fVoltageInVoltOfCurrentActiveCoreSettings )
  CPUcoreVoltageChanged() ;
}

void FreqAndVoltageSettingDlg::OutputFrequencyByControlValues()
{
  //TODO Does not show the "... MHz" when the multiplier has at leat 1 digit
  // more than the previous multiplier AND the value come from the service
  if( mp_cpucontroller )
  {
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
  }
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
  if( m_bAllowWritingVoltageAndFrequency)
  {
  //  int i = 0 ;
  //  BYTE byPstateNumber = 0 ;
  //  byPstateNumber = mp_wxsliderCPUcorePstate->GetValue() ;
    float fVoltageInVolt = GetVoltageInVoltFromSliderValue() ;
    PossiblyWriteVoltageAndMultiplier_Inline(fVoltageInVolt) ;
    //mp_mainframe->SetMenuItemLabel(m_byCoreID, byPstateNumber, pstate ) ;
  }
  else
  {
    ::wxMessageBox( wxT("Writing the voltage and/or multiplier is disabled.\n"
        "To enable it unchecking the "
        "\"prevent voltage ...\" checkboxes may help") );
  }
}

#include <preprocessor_macros/thread_proc_calling_convention.h>

void FreqAndVoltageSettingDlg::OnStopFindingLowestStableCPUcoreVoltageButton(
    wxCommandEvent & //WXUNUSED(event)
    r_wxcommandevent )
{
  wxGetApp().StopInstableCPUcoreVoltageDetection();
}

void FreqAndVoltageSettingDlg::OnFindLowestStableVoltageButton(
    wxCommandEvent & //WXUNUSED(event)
    r_wxcommandevent )
{
  wxString wxstr = m_p_wxtextctrlSecondsUntilNextVoltageDecrease->GetLabel();
  unsigned long ul;
  if( wxstr.ToULong( & ul, 10) )
  {
    mp_model->m_instablecpucorevoltagedetection.
      m_uiNumberOfSecondsToWaitUntilVoltageIsReduced = ul;

//      wxGetApp().StopInstableCPUcoreVoltageDetection();
//      }
//      else
//      {
        wxGetApp().StartInstableCPUcoreVoltageDetection(this);
//      }
      //FindLowestStableVoltage();
      //Sleep(10000);
      //(* wxGetApp().m_pfnStopInstableCPUcoreVoltageDetection)();

//  m_p_wxboxsizerOK_Cancel->Replace(
//    m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage,
//    m_p_wxstatictextSecondsUntilNextVoltageDecrease );
//  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->Hide();
//  m_p_wxstatictextSecondsUntilNextVoltageDecrease->Show();
//  //http://docs.wxwidgets.org/stable/wx_wxsizer.html#wxsizerreplace:
//  //"[...] call wxSizer::Layout to update the layout "on screen" after
//  //replacing a child from the sizer."
//  m_p_wxboxsizerOK_Cancel->Layout();
//  Layout();
  }
  else
  {
      std::wstring std_wstr = L"can't convert \"" +
        ::GetStdWstring( wxstr) + L"\" to int";
      wxGetApp().MessageWithTimeStamp( std_wstr);
  }
}

void FreqAndVoltageSettingDlg::DisableOSesDVFSandServiceDVFS(
  wxString & r_wxstrMessageFromService)
{
  mp_mainframe->PossiblyAskForOSdynFreqScalingDisabling() ;
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  if( m_p_wxbitmaptogglebuttonPauseService->GetValue() )
    wxGetApp().PauseService( r_wxstrMessageFromService,
      mp_model->m_userinterfaceattributes.
      m_bTryToPauseViaServiceControlManagerIfViaIPCfails
      ) ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION

}

inline void FreqAndVoltageSettingDlg::SetMessage(const wxString &
  c_r_wxstrMessage)
{
  mp_wxstatictextMessage->SetLabel( ::wxNow() + wxT(" ") + c_r_wxstrMessage);
  mp_wxstatictextMessage->SetToolTip(c_r_wxstrMessage);
}

//Must provide a voltage parameter because this function is called from
// -OnStabilizeVoltageButton(...)
// -OnApplyButton(...)
void FreqAndVoltageSettingDlg::PossiblyWriteVoltageAndMultiplier_Inline(
  float fVoltageInVolt)
{
//  float fFreq = GetCPUcoreFrequencyFromSliderValue() ;
  float fMultiplierFromSliderValue = GetMultiplierFromSliderValue();
//  if( //fVoltageInVolt > 0.0
  //The voltage check is/ should be done when changing the voltage or
  //(un)checking the check boxes for preventing a voltage outside of the safe
  // range.
//      VoltageIsWithinValidRange( fVoltageInVolt , fFreq ) )
  {
    LOGN("settings dialog--voltage to set:"
      << fVoltageInVolt
      << "multiplier to set:" << fMultiplierFromSliderValue
      )
    wxString wxstrMessageFromService;
    DisableOSesDVFSandServiceDVFS(wxstrMessageFromService);
    SetMessage(wxstrMessageFromService);

    //Use the app's CPU controller because it may have changed:
    // e.g. at first the IPC CPU controller object, then this dialog was
    //opened -> pointer assigned to it and afterwards a dynamic link
    //library (file's) CPU controller.
    I_CPUcontroller * p_cpucontroller = wxGetApp().mp_cpucontroller;
    LOGN( FULL_FUNC_NAME << "--CPU controller:" << p_cpucontroller )
    if( p_cpucontroller )
    {
      WORD wNumCPUcores = mp_model->m_cpucoredata.GetNumberOfCPUcores() ;
      if( wNumCPUcores > 1 )
      {
        for( WORD wCPUcore = 0 ; wCPUcore < wNumCPUcores ; ++ wCPUcore )
          if( m_ar_p_wxcheckbox[wCPUcore]->IsChecked() )
            //TODO the confirmation of e.g. wxWidgets seems to happen in
            //ANOTHER thread->synchronize here (by e.g. using critical sections)
            //TODO change to "I_CPUcontroller::SetCurrentVoltageAndFrequency(...)"

            //mp_cpucontroller->SetCurrentVoltageAndMultiplier(

            p_cpucontroller->SetCurrentVoltageAndMultiplier(
              fVoltageInVolt ,
              fMultiplierFromSliderValue ,
              wCPUcore
              ) ;
      }
      else
  //      mp_cpucontroller->SetCurrentVoltageAndMultiplier(
        p_cpucontroller->SetCurrentVoltageAndMultiplier(
          fVoltageInVolt ,
          fMultiplierFromSliderValue ,
          //Only 1 logical CPU core-> use CPU core ID "0"
          0
          ) ;
    }
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
  p_wxwindow->Show(false);
  p_wxwindow->Show(true);
      ////After hiding a window it looses the focus.
      //p_wxwindow->SetFocus();
  //p_wxwindow->ClearBackground();
  //p_wxwindow->Refresh();
  //p_wxwindow->Update();
  p_wxwindow->SetToolTip( wxT("") );
}

void FreqAndVoltageSettingDlg::ResumendFromStandByOrHibernate()
{
  LOGN( FULL_FUNC_NAME << "--this object:" << this )
  bool bRestorePerformanceStateAfterResume =
    m_p_wxbitmaptogglebuttonRestorePerformanceStateAfterResume->GetValue();
  LOGN( FULL_FUNC_NAME << "(" << this << ")--should restore after standby?:"
    << ( bRestorePerformanceStateAfterResume ? "yes" : "no") )
  if( bRestorePerformanceStateAfterResume )
  {
    LOGN( FULL_FUNC_NAME << "(" << this << ")--writing voltage and multiplier "
      "is allowed?:" << (m_bAllowWritingVoltageAndFrequency ? "yes" : "no") )
    if( m_bAllowWritingVoltageAndFrequency)
    {
    //  BYTE byPstateNumber = 0 ;
    //  byPstateNumber = mp_wxsliderCPUcorePstate->GetValue() ;
      float fVoltageInVolt = GetVoltageInVoltFromSliderValue() ;
      PossiblyWriteVoltageAndMultiplier_Inline(fVoltageInVolt) ;
      //mp_mainframe->SetMenuItemLabel(m_byCoreID, byPstateNumber, pstate ) ;
    }
  } // if( mp_i_cpucontroller )
}

void FreqAndVoltageSettingDlg::SetAttention(
  wxWindow * p_wxwindow, 
  const wxString & wxstr // = _T("")
  )
{
  //p_wxwindow->ClearBackground();
  p_wxwindow->SetOwnBackgroundColour(*wxRED);
  //http://docs.wxwidgets.org/2.9/classwx_window.html
  // #37219df52734626e23401fd83b25d8a0:
  //"Note that setting the background colour does not cause an immediate
  //refresh, so you may wish to call wxWindow::ClearBackground or
  //wxWindow::Refresh after calling this function."
  //ClearBackground did not draw the foreground text under Windows 7 (64 bit)!
//  p_wxwindow->ClearBackground () ;
  //force repaint the "dirty" way: hide and show immediately.
  //because ClearBackground(),Refresh() and Update() didn't bring the 
  //desired IMMEDIATE FULL repaint.
  p_wxwindow->Show(false);
  p_wxwindow->Show(true);
      ////After hiding a window it looses the focus.
      //p_wxwindow->SetFocus();
  //p_wxwindow->Refresh();
//  p_wxwindow->Update();
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

void FreqAndVoltageSettingDlg::SetStartFindingLowestStableVoltageButton()
{
//  m_p_wxboxsizerOK_Cancel->Replace(
//    m_p_wxstatictextSecondsUntilNextVoltageDecrease,
//    m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage);
//  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->Show();
//  m_p_wxstatictextSecondsUntilNextVoltageDecrease->Hide();
//  //http://docs.wxwidgets.org/stable/wx_wxsizer.html#wxsizerreplace:
//  //"[...] call wxSizer::Layout to update the layout "on screen" after
//  //replacing a child from the sizer."
//  m_p_wxboxsizerOK_Cancel->Layout();
}

void FreqAndVoltageSettingDlg::SetMultiplierSliderToClosestValue(
    float fMultiplier)
{
  BYTE byMultiIndex = mp_model->m_cpucoredata.GetIndexForClosestMultiplier(
    fMultiplier ) ;
  mp_wxsliderFreqInMHz->SetValue( //m_pstate.GetFreqInMHz()
    byMultiIndex ) ;
}

BYTE FreqAndVoltageSettingDlg::SetVoltageSliderToClosestValue(
  float fVoltageInVolt)
{
  BYTE byIndexForClosestVoltage = mp_model->m_cpucoredata.
    GetIndexForClosestVoltage( fVoltageInVolt ) ;
  mp_wxsliderCPUcoreVoltage->SetValue(byIndexForClosestVoltage) ;
  return byIndexForClosestVoltage ;
}

BYTE FreqAndVoltageSettingDlg::VoltageIsWithinValidRange(
  float fVoltageInVolt, float fCPUcoreFrequencyinMHz)
{
  BYTE bVoltageIsValid = 0 ;
  LOGN("VoltageIsValid(" << fVoltageInVolt << ")--begin")
  if( fVoltageInVolt > 0.0 )
  {
    BYTE byRet = 0 ;
    if( m_p_wxbitmaptogglebuttonPreventVoltageAboveDefaultVoltage->GetValue() )
    {
//      float fCPUcoreFrequencyinMHz = GetCPUcoreFrequencyFromSliderValue() ;
      LOGN("VoltageIsValid--fCPUcoreFrequencyinMHz:" << fCPUcoreFrequencyinMHz)
      byRet = mp_cpucontroller->CheckWhetherVoltageIsAboveDefaultVoltage(
        fVoltageInVolt, fCPUcoreFrequencyinMHz) ;
      bVoltageIsValid = byRet ;
    }
    if( //mp_wxcheckboxPreventVoltageBelowLowestStableVoltage->IsChecked()
        m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage->
        GetValue()
      )
    {
//      float fCPUcoreFrequencyinMHz = GetCPUcoreFrequencyFromSliderValue() ;
      LOGN("VoltageIsValid--fCPUcoreFrequencyinMHz:" << fCPUcoreFrequencyinMHz)
      byRet = mp_cpucontroller->CheckWhetherVoltageIsBelowLowestStableVoltage(
        fVoltageInVolt, fCPUcoreFrequencyinMHz) ;
      if( byRet ) //error
        bVoltageIsValid = byRet ;
    }
  }
  else
  {
    LOGN("voltage in Volt is <= 0.0 ")
    bVoltageIsValid = false ;
  }
  return bVoltageIsValid ;
}
