/*
 * VolnFreqSettingDlg_Create.hpp
 *
 *  Created on: Nov 8, 2012
 *      Author: sgebauer
 */

#ifndef VOLNFREQSETTINGDLG_CREATE_HPP_
#define VOLNFREQSETTINGDLG_CREATE_HPP_

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddCPUcoreCheckBoxSizer(
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddAlsoSetWantedVoltageControls(
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddApplyOrCancelSizer(
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
#ifdef wxHAS_POWER_EVENTS
  AddRestorePerformanceStateAfterResumeButton(m_p_wxboxsizerOK_Cancel) ;
#endif
  AddCancelButton(m_p_wxboxsizerOK_Cancel);

#ifdef BUILD_WITH_INSTABLE_CPU_CORE_OPERATION_DETECTION
  AddAutoConfigureVoltageSettingsButton(m_p_wxboxsizerOK_Cancel) ;
  CreateFindLowestStableCPUcoreVoltageButton();
  m_p_wxboxsizerOK_Cancel->Add(m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage);

//  CreateStopFindingLowestStableCPUcoreVoltageButton();
//  m_p_wxboxsizerOK_Cancel->Add(
//    m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage);

  //wxSpinCtrl * p_wx = new

  AddSecondsUntilNextVoltageDecreaseTextControl(m_p_wxboxsizerOK_Cancel);
  AddInstableCPUdetectDynLibCPUcoreUsagePercentageTextControl(m_p_wxboxsizerOK_Cancel);

  m_p_wxstatictextSecondsUntilNextVoltageDecrease = new wxStaticText( this,
    wxID_ANY, wxT("     s") );
  m_p_wxboxsizerOK_Cancel->Add(m_p_wxstatictextSecondsUntilNextVoltageDecrease);
#endif //#ifdef BUILD_WITH_INSTABLE_CPU_CORE_OPERATION_DETECTION

//  m_p_wxspinbuttonSecondsUntilVoltageDecrease = new wxSpinButton (
//      this,
////      NULL,
//      SecondsUntilVoltageDecreaseSpinButton
//      );
  m_p_wxspinbuttonSecondsUntilVoltageDecrease = NULL;
//  m_p_wxboxsizerOK_Cancel->Add(m_p_wxspinbuttonSecondsUntilVoltageDecrease);

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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddCancelButton(
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddCPUcoreFrequencySizer(
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
//  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
//    new wxStaticText(this, wxID_ANY,
//    //_T("core frequency in MHz: ")));
//    wxT("CPU core\nfrequency"//\nin MHz"
//      ":")
//      )
//    , 0 //0=the control should not take more space if the sizer is enlarged
//    , wxLEFT | wxRIGHT
//      //| wxALIGN_TOP
//      | wxALIGN_CENTER_VERTICAL
//    , 5 //Determines the border width
//    );
  p_wxboxsizerCPUcoreFrequencyInMHz->Add(
    mp_wxsliderFreqInMHz
    , 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    , 0 //Determines the border width
    );
  mp_wxsliderFreqInMHz->SetToolTip( wxT("CPU core multiplier/ frequency") );

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
    //proportion: "change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable"
    0 ,
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddCPUcoreThrottlingSizer
  (wxSizer * p_wxsizerSuperordinate)
{
  mp_wxsliderCPUcoreThrottleRatio = new wxSlider(
    this,
    //wxID_ANY//,
    ID_throttleRatio
    //, 2  //value
    , //Initial position for the slider.
      //mp_i_cpucontroller->GetMinimumFrequencyInMHz()
      //voltageandfreq.m_wFreqInMHz
      0
      //Slider minimum value.
    , //lowest adjustable frequency:
    0
    //* mp_model->m_cpucoredata.m_stdset_floatAvailableThrottleLevels.begin()
    //Slider max value.
    , //* mp_model->m_cpucoredata.m_stdset_floatAvailableThrottleLevels.rbegin()
      mp_model->m_cpucoredata.m_stdset_floatAvailableThrottleLevels.size()
    //If the default point (-1, -1) is specified then a default point is chosen.
//    , wxPoint(-1, -1)
    , wxDefaultPosition
    //If the default size (-1, -1) is specified then a default size is chosen.
//    , wxSize(-1, -1)
    , wxDefaultSize
    ,
    //wxSL_AUTOTICKS // Displays tick marks.
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
  m_p_wxboxsizerCPUcoreThrottleRatio = new wxBoxSizer(wxHORIZONTAL);
  m_p_wxboxsizerCPUcoreThrottleRatio->Add(
    mp_wxsliderCPUcoreThrottleRatio
    , 1
    //,wxEXPAND | wxALL
    , wxLEFT | wxRIGHT
      | wxALIGN_CENTER_VERTICAL
    , 0 //Determines the border width
    );
  mp_wxsliderCPUcoreThrottleRatio->SetToolTip( wxT("CPU core throttling ratio") );
  mp_wxstatictextThrottleRatio = new wxStaticText(this, wxID_ANY, wxT("") );
  m_p_wxboxsizerCPUcoreThrottleRatio->Add(mp_wxstatictextThrottleRatio);
  p_wxsizerSuperordinate->Add(//mp_wxsliderFreqInMHz,
    m_p_wxboxsizerCPUcoreThrottleRatio ,
    //proportion: "change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable"
    0 ,
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddCPUcoreVoltageSizer(
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
//  p_wxboxsizerCPUcoreVoltage->Add(
//    new wxStaticText(this, wxID_ANY, wxT("CPU core\nvoltage" //\nIDentifier
//      ":" ))
//    , 0 //0=the control should not take more space if the sizer is enlarged
//    //These flags are used to specify which side(s) of the sizer item the
//    //border width will apply to.
//    , //wxEXPAND |
//    //wxALL,
//    wxLEFT | wxRIGHT
//    | wxALIGN_CENTER_VERTICAL
////    //The label and the adjustable value should be at the same vertical
////    //position, so place at the top.
////    | wxALIGN_TOP
//    , 3 //Determines the border width
//    );
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
  mp_wxsliderCPUcoreVoltage->SetToolTip( wxT("CPU core voltage") );

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
    //proportion: "change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable"
    0 ,
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

void FreqAndVoltageSettingDlg::AddMessageControl(wxSizer * p_wxsizer)
{
  m_p_wxboxsizerMessage = new wxBoxSizer(wxHORIZONTAL);

  mp_wxstatictextMessage = new wxStaticText(this, wxID_ANY, wxT("") );
  m_p_wxboxsizerMessage->Add( mp_wxstatictextMessage ) ;

  p_wxsizer->Add(
    m_p_wxboxsizerMessage,
    0 ,
    wxFIXED_MINSIZE,
    //Determines the border width, if the flag  parameter is set to include
    //any border flag.
    2
    );
}

void FreqAndVoltageSettingDlg::AddPauseServiceCheckbox(wxSizer * p_wxsizer )
{
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  wxString wxstrPauseToolTip =
    wxT("pauses the Dynamic Voltage and Frequency Scaling\n"
    "that is done by the service"
    "after writing CPU core voltage and/or multiplier\n"
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddPerformanceStateSizer(
  wxSizer * p_wxsizerSuperordinate )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  p_wxboxsizerCPUcorePstate = new wxBoxSizer(wxHORIZONTAL);
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
    //proportion: "change its size in the main orientation of the wxBoxSizer -
    //where 0 stands for not changeable"
    0 ,
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

#ifdef wxHAS_POWER_EVENTS
void FreqAndVoltageSettingDlg::AddRestorePerformanceStateAfterResumeButton(
  wxSizer * p_wxsizer )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxString wxstrToolTip = wxT("restore selected voltage and multiplier\n"
    "after resuming from standy or hibernate\n"
    "(else either\n"
    "-the performance state (0) set via ACPI\n"
    "-a CPU core voltage and frequency combination set via the Operating system"
    "may be used)");
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
#endif

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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddVoltageSettingsSizer(
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
  if(mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.size() )
  {
    //AddMultipleConfigurableVoltagesControls();
    AddDecreaseVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
    AddIncreaseVoltageButton(p_wxflexgridsizerSetAsMinVoltage);
    AddStabilizeVoltageButton(p_wxflexgridsizerSetAsMinVoltage) ;
  }
  AddSelectPstateViaMousePositionButton(p_wxflexgridsizerSetAsMinVoltage) ;
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
    0 , //1 ,
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

CREATE_DIALOG_INLINED void FreqAndVoltageSettingDlg::AddSetAsWantedVoltageSizer(
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
//  AddPauseServiceCheckbox(p_wxflexgridsizerSetAsWantedVoltage) ;
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
  //  , wxSL_AUTOTICKS //       Displays tick marks.
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
    //wxSL_AUTOTICKS // Displays tick marks.
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


#endif /* VOLNFREQSETTINGDLG_CREATE_HPP_ */
