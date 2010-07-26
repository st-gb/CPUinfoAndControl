#include "wxDynamicDialog.hpp"
#include <wx/button.h>
#include <wx/checkbox.h>
//#include <wx/dialog.h>
#include <wx/stattext.h> //for wxStaticText
#include <wx/sizer.h> //wxBoxSizer
#include <wx/timer.h> //wxTimer 
//#include <wx/wrapsizer.h>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/I_CPUaccess.hpp>
#include "ModelData/ModelData.hpp"
//#include <ModelData/RegisterData.hpp>
#include <wxWidgets/App.hpp> //for wxGetApp() / DECLARE_APP
//#include <wxWidgets/wxStringHelper.h>
#include <wxWidgets/Controller/wxStringHelper.h>

#include <vector>

//An enum guarantees a unique number for each element.
enum
{
  ID_IntervalCheckbox = 1,
  ID_ReloadCPUregisterToReadConfig
  , ID_Timer
  , ID_Dialog
};

//// declare a custom event type
////
//// note that in wxWin 2.3+ these macros expand simply into the following code:
////
////  extern const wxEventType wxEVT_MY_CUSTOM_COMMAND;
////
////  const wxEventType wxEVT_MY_CUSTOM_COMMAND = wxNewEventType();
////
//// and you may use this code directly if you don't care about 2.2 compatibility
//BEGIN_DECLARE_EVENT_TYPES()
//    DECLARE_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND, 7777)
//END_DECLARE_EVENT_TYPES()
//
//DEFINE_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND)
//
BEGIN_EVENT_TABLE(wxDynamicDialog, wxDialog)
//    EVT_MY_CUSTOM_COMMAND(wxID_ANY, wxDynamicDialog::OnRuntimeCreatedControls)
  EVT_TIMER(ID_Timer,wxDynamicDialog::OnTimerEvent)
  EVT_SIZE(wxDynamicDialog::OnSize)
  EVT_BUTTON(ID_ReloadCPUregisterToReadConfig,
    wxDynamicDialog::OnReloadCPUregisterToReadConfig )
END_EVENT_TABLE()

wxDynamicDialog::wxDynamicDialog(//RegisterData 
  wxWindow * parent ,
  //MSRdata & r_regdata ,
  //Model & r_modeldata ,
  //, 
  wxX86InfoAndControlApp * p_wxx86infoandcontrolapp
  )
  : wxDialog( 
    parent, //wxID_ANY, //
    ID_Dialog ,
    //wxString::Format("MSR register index: %x",
    //r_regdata.m_dwIndex)
    wxT("")
    , wxPoint(30, 30) //A value of (-1, -1) indicates a default size
    , wxSize(400, 200)
    , wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  //, mp_i_cpucontroller ( p_cpucontroller )
  //The CPU access is independent from the CPU controller and usually does
  //not change during runtime in contract to the CPU controller that may be
  //exchanged during runtime.
  , mp_cpuaccess ( p_wxx86infoandcontrolapp->GetCPUaccess() )
  , mp_modeldata ( //& r_modeldata
    p_wxx86infoandcontrolapp->GetModel() )
  , mp_wxx86infoandcontrolapp ( p_wxx86infoandcontrolapp )
{
  m_wControlID = 1 ;
  m_wxtimer.SetOwner(this, ID_Timer) ;
  //mp_regdata = & r_regdata ;
  //mp_msr_data = & r_regdata ;
  BuildGUI() ;

//  Connect(
//    //m_wControlID //++
//    ID_ReloadCPUregisterToReadConfig ,
//    //,
//    wxID_ANY,
//    wxEVT_COMMAND_BUTTON_CLICKED,
//    wxCommandEventHandler(wxDynamicDialog::OnRuntimeCreatedControls)
//    );
  Connect( 
    ID_IntervalCheckbox ,
    wxEVT_COMMAND_CHECKBOX_CLICKED, 
    wxCommandEventHandler(wxDynamicDialog::OnRuntimeCreatedControls) 
    );
}

wxDynamicDialog::~wxDynamicDialog()
{
  //TODO prog crash in wxSizerItem::~wxSizerItem() (a children of a sizer
  //at "delete m_sizer" m_sizer.m_containingWindow  is 0xfeeefeee
  LOGN("wxDynamicDialog's d'tor")
}

inline void wxDynamicDialog::AddStatictext( const wxString & cr_wxstr )
{
//  wxStaticText *
  mp_wxstatictext = new wxStaticText(
    this
    , wxID_ANY
    //(*iter).m_str
    , wxString(wxT("") )
    ) ;
  wxRect wxrect = mp_wxstatictext->GetRect() ;
#ifdef _DEBUG
  wxrect = mp_wxstatictext->GetRect() ;
  wxPoint wxpoint = mp_wxstatictext->GetPosition() ;
  wxpoint = mp_wxboxsizerOutmost->GetPosition() ;
  wxSize wxsize = mp_wxboxsizerOutmost->GetSize() ;
  wxsize = mp_wxboxsizerOutmost->GetMinSize() ;
  wxpoint = mp_sizerLeftColumn->GetPosition() ;
  wxsize = mp_sizerLeftColumn->GetSize() ;
  wxsize = mp_sizerLeftColumn->GetMinSize() ;
  wxrect = GetClientRect() ;
//  if( //mp_wxstatictext->GetRect().GetBottom()
//      mp_sizerLeftColumn->GetMinSize().GetHeight() > GetClientRect().height )
//    ::wxMessageBox(wxT( "mp_sizerLeftColumn->GetSize().GetHeight() > "
//      "GetClientRect().height") ) ;
#endif
  if( p_wxboxsizerOptions->GetMinSize().GetHeight() +
      mp_sizerLeftColumn->GetMinSize().GetHeight() + wxrect.height
      > GetClientRect().height )
  {
    mp_sizerLeftColumn = new wxBoxSizer(wxVERTICAL);
    mp_sizerRightColumn = new wxBoxSizer(wxVERTICAL);
    mp_sizerTop->Add( mp_sizerLeftColumn );
    mp_sizerTop->Add( mp_sizerRightColumn );
  }
//  if( //mp_sizerLeftColumn->GetSize().GetHeight() > GetClientRect().height
//      mp_sizerTop->GetSize().GetHeight() > GetClientRect().height )
//    ::wxMessageBox(wxT( "mp_sizerLeftColumn->GetSize().GetHeight() > "
//      "GetClientRect().height") ) ;
  mp_sizerLeftColumn->Add( new wxStaticText(
    this,
    wxID_ANY,
    cr_wxstr
//#ifdef _DEBUG
//      + wxString::Format("%x",p_wxstatictext)
//#endif //#ifdef _DEBUG
    + wxString(wxT(": ") )
    )
    //0=the control should not take more space if the sizer is enlarged
    , 0
    //, wxFIXED_MINSIZE,
    , wxLEFT | wxRIGHT |
      //wxALIGN_CENTER_VERTICAL
      //The label and the adjustable value should be at the same vertical
      //position, so place at the top.
      wxALIGN_TOP
      | wxALIGN_RIGHT //| wxALIGN_CENTER_VERTICAL
    //Determines the border width, if the flag parameter is set to include
    //any border flag.
    , 2
    );
  //m_stdmap_p_wxstatictext
  m_stdvector_p_wxstatictext.push_back(mp_wxstatictext) ;
  //p_sizerHorizontal->Add(
  mp_sizerRightColumn->Add(
   mp_wxstatictext
   , 0
   //, wxFIXED_MINSIZE,
   , wxLEFT | wxRIGHT |
     //wxALIGN_CENTER_VERTICAL
     //The label and the adjustable value should be at the same vertical
     //position, so place at the top.
     wxALIGN_TOP
   //Determines the border width, if the flag  parameter is set to include
   //any border flag.
   , 2
   );
}

void wxDynamicDialog::BuildCPUregisterControls()
{
  std::vector<MSRdata>::iterator itermsrdata =
    mp_modeldata->m_stdvector_msrdata.begin() ;
  std::vector<CPUIDdata>::iterator itercpuiddata =
    mp_modeldata->m_stdvector_cpuiddata.begin() ;

  mp_sizerLeftColumn = new wxBoxSizer(wxVERTICAL);
  mp_sizerRightColumn = new wxBoxSizer(wxVERTICAL);
  mp_sizerTop->Add( mp_sizerLeftColumn );
  mp_sizerTop->Add( mp_sizerRightColumn );
  //  AddTimeStampCounter() ;
  AddStatictext( wxT("TSC") ) ;
  AddStatictext( wxT("TSC diff") ) ;

  while( itercpuiddata != mp_modeldata->m_stdvector_cpuiddata.end() )
  {
    BuildGUI( *itercpuiddata ) ,
    ++ itercpuiddata ;
  }
  while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
  {
    BuildGUI( *itermsrdata ) ,
    ++ itermsrdata ;
  }
}

void wxDynamicDialog::BuildGUI()
{
  mp_wxboxsizerOutmost = new wxBoxSizer( wxVERTICAL ) ;
//  wxWrapSizer * p_wxwrapsizer = new wxWrapSizer() ;
  p_wxboxsizerOptions = new wxBoxSizer( wxHORIZONTAL) ;
//  mp_wxboxsizerOutmost->Add( //p_wxwrapsizer
//    p_wxboxsizerOptions ) ;
  mp_sizerTop = new wxBoxSizer(//wxVERTICAL
    wxHORIZONTAL );
//  mp_wxcheckboxReloadCPUregisterToReadConfig = new wxCheckBox(
//    this
//    , ID_ReloadCPUregisterToReadConfig
//    , _T("reload CPU register to read configuration")
//    ) ;
//  p_wxboxsizerOptions->Add( mp_wxcheckboxReloadCPUregisterToReadConfig ) ;
  wxButton * p_wxbuttonReloadCPUregisterToReadConfig = new wxButton(
    this
    , ID_ReloadCPUregisterToReadConfig
    , _T("reload CPU register to read configuration")
    ) ;
  p_wxboxsizerOptions->Add( p_wxbuttonReloadCPUregisterToReadConfig ) ;
  mp_wxcheckboxInterval = new wxCheckBox(
    this 
    , ID_IntervalCheckbox
    , _T("interval in ms:")
    ) ;
  mp_wxtextctrlUpdateIntervalInMs = new wxTextCtrl(
    this
    , wxID_ANY
//    , _T("interval in ms")
    , wxT("1000")
    ) ;

//  wxButton * p_wxbuttonMSR = new wxButton(
//      this
//      , m_wControlID
//      , _T("read from MSR register")
//    ) ;
//  mp_sizerLeftColumn->Add(
//    mp_wxcheckboxReloadCPUregisterToReadConfig
//    , 0
//    , wxFIXED_MINSIZE,
//    //Determines the border width, if the flag  parameter is set to include
//    //any border flag.
//    2
//    );
//  mp_sizerLeftColumn->Add(
  p_wxboxsizerOptions->Add(
    mp_wxcheckboxInterval
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );
//  mp_sizerRightColumn->Add(
  p_wxboxsizerOptions->Add(
    mp_wxtextctrlUpdateIntervalInMs
    , 0
    , wxFIXED_MINSIZE,
    //Determines the border width, if the flag  parameter is set to include
    //any border flag.
    2
    );
  mp_wxcheckboxRebuildGUIonResize = new wxCheckBox(
    this
//    , ID_RebuildGUIonResizeCheckbox
    , wxID_ANY
    , _T("RebuildGUIonResize")
    ) ;
  p_wxboxsizerOptions->Add(
    mp_wxcheckboxRebuildGUIonResize
    , 0
    , wxFIXED_MINSIZE,
    //Determines the border width, if the flag  parameter is set to include
    //any border flag.
    2
    );
//  mp_sizerLeftColumn->Add(
//    p_wxbuttonMSR
//    , 0
//    , wxFIXED_MINSIZE,
//    //Determines the border width, if the flag  parameter is set to include
//    //any border flag.
//    2
//    );
//  mp_sizerRightColumn->Add(
//    //p_wxbuttonMSR
//    //new wxStaticText(
//    //  this,
//    //  wxID_ANY,
//    //  ""
//    //  )
//    new wxButton(
//      this
//      , wxID_ANY
//      , _T("dummy button")
//    )
//    , 0
//    , wxFIXED_MINSIZE,
//    //Determines the border width, if the flag  parameter is set to include
//    //any border flag.
//    2
//    );

  BuildCPUregisterControls() ;

  //mp_sizerTop->Add( new wxButton(
  //    this
  //    , m_wControlID 
  //    , _T("read from MSR register") 
  //  ) 
  //  , 0 
  //  , wxFIXED_MINSIZE, 
  //  //Determines the border width, if the flag  parameter is set to include 
  //  //any border flag.
  //  2 
  //  );
  //mp_sizerTop->Add( new wxButton(
  //    this
  //    , ++ m_wControlID 
  //    , _T("reload CPU register config file") 
  //  ) 
  //  , 0 
  //  , wxFIXED_MINSIZE, 
  //  //Determines the border width, if the flag  parameter is set to include 
  //  //any border flag.
  //  2 
  //  );
  mp_wxboxsizerOutmost->Add( //p_wxwrapsizer
    p_wxboxsizerOptions ) ;
  mp_wxboxsizerOutmost->Add( mp_sizerTop ) ;
  SetSizer( //mp_sizerTop
    mp_wxboxsizerOutmost );
//  mp_sizerTop->SetSizeHints(this);
//  mp_sizerTop->Fit(this);
//  mp_wxboxsizerOutmost->Fit(this);
}

void wxDynamicDialog::BuildGUI(MSRdata & r_msrdata )
{
  //wxBoxSizer * p_sizerTop = new wxBoxSizer(wxVERTICAL);
  std::vector<RegisterData>::iterator iter_registerdata = 
    //mp_msr_data->m_stdvec_registerdata.begin() ;
    r_msrdata.m_stdvec_registerdata.begin() ;

  while( iter_registerdata != //mp_msr_data->
    r_msrdata.m_stdvec_registerdata.end() )
  {
    //wxBoxSizer * p_sizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
    AddStatictext(getwxString( (*iter_registerdata).m_strDataName )) ;
     ++ iter_registerdata ;
  }
  //std::vector<RegisterDataTable>::iterator iter_registerdatatable =
  //  //mp_msr_data->
  //  r_msrdata.m_stdvector_registerdatatable.begin() ;

  ////Now add controls for values dependent on register values.
  //while( iter_registerdatatable != //mp_msr_data->
  //  r_msrdata.m_stdvector_registerdatatable.end() )
  //{
  //  wxBoxSizer * p_sizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
  //   p_sizerHorizontal->Add( new wxStaticText(this, wxID_ANY, 
  //     //&iter_registerdatatable->m_ararstdstr[0][0] 
  //     iter_registerdatatable->m_ararstdstr[0] + ": "
  //    ) 
  //     , 0 , wxFIXED_MINSIZE, 
  //    //Determines the border width, if the flag  parameter is set to include 
  //    //any border flag.
  //    2 
  //    );
  //   wxStaticText * p_wxstatictext = new wxStaticText(this, wxID_ANY, 
  //     //iter_registerdatatable->m_ararstdstr[0][0]
  //     ""
  //     ) ;
  //     
  //   //m_stdmap_p_wxstatictext
  //   m_stdvector_p_wxstatictext.push_back(p_wxstatictext) ;
  //  p_sizerHorizontal->Add( p_wxstatictext, 0 , wxFIXED_MINSIZE, 
  //    //Determines the border width, if the flag  parameter is set to include 
  //    //any border flag.
  //    2 
  //    );
  //   p_sizerTop->Add(p_sizerHorizontal);
  //  ++ iter_registerdatatable ;
  //}
  //Create(this,wxID_ANY, _T("gg"));
   //p_wxdlg->AddChild(new wxStaticText(p_wxdlg, wxID_ANY, _T("sdsd") ) );
   //p_wxdlg->Add(new wxStaticText(p_wxdlg, wxID_ANY, _T("sdsd") ) );
}

void wxDynamicDialog::BuildGUI(CPUIDdata & r_cpuiddata )
{
  //wxBoxSizer * p_sizerTop = new wxBoxSizer(wxVERTICAL);
  wxString wxstrDataName ;
  std::vector<RegisterData>::iterator iter_registerdata = 
    //mp_msr_data->m_stdvec_registerdata.begin() ;
    r_cpuiddata.m_stdvec_registerdata.begin() ;

  while( iter_registerdata != //mp_msr_data->
    r_cpuiddata.m_stdvec_registerdata.end() )
  {
    //wxBoxSizer * p_sizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
    wxstrDataName = getwxString( (*iter_registerdata).m_strDataName ) ;
    AddStatictext(wxstrDataName ) ;
     ++ iter_registerdata ;
  }
}

void wxDynamicDialog::DisplayTSCvalues()
{
  if( m_stdvector_p_wxstatictextiter != m_stdvector_p_wxstatictext.end()
    )
  {
//    DWORD dwHigh, dwLow ;
    //http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf:
    //"force all previous instructions to complete"
//    mp_cpuaccess->CpuidEx( 1, & m_dwEAX, & m_dwEBX, & m_dwECX, & m_dwEDX, 1 ) ;
//    mp_cpuaccess->ReadTSC(m_dwEAX,m_dwEDX) ;
    mp_cpuaccess->ReadTSCinOrder(m_dwEAX,m_dwEDX,1) ;
    //http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf:
    //"force all previous instructions to complete"
//    mp_cpuaccess->CpuidEx( 1, m_dwEAX, m_dwEBX, m_dwECX, m_dwEDX, 1 ) ;
    m_ullValue = m_dwEDX ;
    m_ullValue <<= 32 ;
    m_ullValue |= m_dwEAX ;
#ifdef __CYGWIN__
    m_wxstrULL = wxString::Format( wxString( wxT("%llu") ), m_ullValue) ;
#else
    m_wxstrULL = wxString::Format( wxString( wxT("%I64u") ), m_ullValue) ;
#endif
  #ifdef _DEBUG
    //wxstrULL = wxString::Format("%x", *m_stdvector_p_wxstatictextiter );
    //(*m_stdvector_p_wxstatictextiter)->SetLabel() ;
  #endif
    (*m_stdvector_p_wxstatictextiter)->SetLabel(m_wxstrULL) ;
    ++ m_stdvector_p_wxstatictextiter ;
  }
  if( m_stdvector_p_wxstatictextiter != m_stdvector_p_wxstatictext.end()
    )
  {
    m_ullValue2 = m_ullValue - m_ullPrevTSCvalue ;
    m_ullPrevTSCvalue = m_ullValue ;
#ifdef __CYGWIN__
    m_wxstrULL = wxString::Format( wxString( wxT("%llu") ), m_ullValue2) ;
#else
    m_wxstrULL = wxString::Format( wxString( wxT("%I64u") ), m_ullValue2) ;
#endif
    (*m_stdvector_p_wxstatictextiter)->SetLabel(m_wxstrULL) ;
    ++ m_stdvector_p_wxstatictextiter ;
  }
}

void wxDynamicDialog::DisplayRegisterData()
{
  m_stdvector_p_wxstatictextiter = m_stdvector_p_wxstatictext.begin() ;
  std::vector<MSRdata>::iterator itermsrdata = 
    mp_modeldata->m_stdvector_msrdata.begin() ;
  std::vector<CPUIDdata>::iterator iter_cpuiddata = 
    mp_modeldata->m_stdvector_cpuiddata.begin() ;
  DisplayTSCvalues() ;
  while( iter_cpuiddata != mp_modeldata->m_stdvector_cpuiddata.end() )
  {
    DisplayRegisterData(*iter_cpuiddata) ;

    ++ iter_cpuiddata ;
  }
  while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
  {
    DisplayRegisterData(*itermsrdata) ;

    ++ itermsrdata ;
  }
}

void wxDynamicDialog::DisplayRegisterData(CPUIDdata & r_cpuiddata)
{
  std::vector<RegisterData>::iterator iter_registerdata = 
    r_cpuiddata.m_stdvec_registerdata.begin() ;
  std::vector<wxStaticText *>::iterator iterp_wxstatictext = 
    m_stdvector_p_wxstatictext.begin() ;

  DWORD dwValue ; 
  try
  {
    mp_cpuaccess->CpuidEx(
      r_cpuiddata.m_dwIndex,
      & m_dwEAX,
      & m_dwEBX,
      & m_dwECX,
      & m_dwEDX,
      //dwAffMask
			1 << r_cpuiddata.m_byCoreID
      );
    //This vector data is needed to get the replacement value for a table:
    //e.g. a value depends on more than 1 other value:
    //MHz | FID | DID
    //2200|  14 | 0
    //1100|  14 | 1
    //so both FID and DID are needed in order to get the value for "MHz"
    std::vector<std::string> stdvector_stdstringAttributeValue ;
//    wxString wxstrULL ;
    //In this loop: get every single data at first.
    while( 
      iter_registerdata != r_cpuiddata.m_stdvec_registerdata.end() && 
      iterp_wxstatictext != m_stdvector_p_wxstatictext.end() )
    {
      if( (*iter_registerdata).m_stdvec_bitrange.size() > 0 )
      {
        BitRange & br = (*iter_registerdata).m_stdvec_bitrange.at(0) ;
        if( br.m_byStartBit < 32 )
        {
          dwValue = //make bits from highmost zero
            ( m_dwEAX << ( 32 - br.m_byStartBit - br.m_byBitLength ) )
            //Bring value to least significant bit
            >> ( 32 - br.m_byBitLength ) ;
        }
        else if( br.m_byStartBit < 64 )
        {
          dwValue = //make bits from highmost zero
            ( m_dwEBX << ( 64 - br.m_byStartBit - br.m_byBitLength ) ) ;
            //Bring value to least significant bit
          dwValue >>= (
            //Number of bits for DWORD.
            32 
            - br.m_byBitLength ) ;          
        }
        else if( br.m_byStartBit < 96 )
        {
          dwValue = //make bits from highmost zero
            ( m_dwECX << ( 96 - br.m_byStartBit - br.m_byBitLength ) )
            //Bring value to least significant bit
            >> ( 
            //Number of bits for DWORD.
            32 
            - br.m_byBitLength ) ;          
        }
        else if( br.m_byStartBit < 128 )
        {
          dwValue = //make bits from highmost zero
            ( m_dwEDX << ( 128 - br.m_byStartBit - br.m_byBitLength ) )
            //Bring value to least significant bit
            >> ( 
            //Number of bits for DWORD.
            32 
            - br.m_byBitLength ) ;          
        }

        //if( iter_registerdata->m_bCalculateDiff )
        //{
        //  dwValue  = dwValue - iter_registerdata->m_ullPreviousValue ;
        //}
//        iter_registerdata->m_ullPreviousValue = ullMSR ;
        //mp_msr_data->GetTableContainingDataName(iter->m_strDataName);
        #ifdef __CYGWIN__
        m_wxstrULL = wxString::Format( wxString( wxT("%u") ), dwValue) ;
        #else
        m_wxstrULL = wxString::Format( wxString( wxT("%u") ), dwValue) ;
        #endif
        //(*iterp_wxstatictext)->SetLabel(//wxString::Format("%64u", ullValue )
        //  m_wxstrULL );
        stdvector_stdstringAttributeValue.push_back( 
          //(std::string) //wxString::Format("%64u", ullValue) 
          std::string( //wxstrULL.c_str()
            GetStdString(m_wxstrULL) )
          );
        if( m_stdvector_p_wxstatictextiter != m_stdvector_p_wxstatictext.end() 
          )
        {
  #ifdef _DEBUG
          //m_wxstrULL = wxString::Format("%x", *m_stdvector_p_wxstatictextiter );
          //(*m_stdvector_p_wxstatictextiter)->SetLabel() ;
  #endif
          (*m_stdvector_p_wxstatictextiter)->SetLabel(m_wxstrULL) ;
          ++ m_stdvector_p_wxstatictextiter ;
        }
      }
      ++ iter_registerdata ;
      ++ iterp_wxstatictext ;
    }
  }
  catch(const ReadMSRexception & ex )
  {
//    //Breakpoint possibility
//    int i = 0 ;
  }
}

void wxDynamicDialog::DisplayRegisterData(MSRdata & r_msrdata)
{
  std::vector<RegisterData>::iterator iter_registerdata = 
    r_msrdata.m_stdvec_registerdata.begin() ;
  std::vector<wxStaticText *>::iterator iterp_wxstatictext = 
    m_stdvector_p_wxstatictext.begin() ;
  try
  {
    //mp_i_cpucontroller->RdmsrEx(
    mp_cpuaccess->RdmsrEx(
      r_msrdata.m_dwIndex,
      & m_dwEAX,
      & m_dwEDX,
			1 << r_msrdata.m_byCoreID
      );
    m_ullValue = m_dwEDX  ;
    m_ullValue <<= 32 ;
    m_ullValue |= m_dwEAX ;
    //This vector data is needed to get the replacement value for a table:
    //e.g. a value depends on more than 1 other value:
    //MHz | FID | DID
    //2200|  14 | 0
    //1100|  14 | 1
    //so both FID and DID are needed in order to get the value for "MHz"
    std::vector<std::string> stdvector_stdstringAttributeValue ;
    //In this loop: get every single data at first.
    while( 
      iter_registerdata != r_msrdata.m_stdvec_registerdata.end() && 
      iterp_wxstatictext != m_stdvector_p_wxstatictext.end() )
    {
      if( (*iter_registerdata).m_stdvec_bitrange.size() > 0 )
      {
        BitRange & br = (*iter_registerdata).m_stdvec_bitrange.at(0) ;
        m_ullValue2 = //make bits from highmost zero
          (m_ullValue << ( 64 - br.m_byStartBit - br.m_byBitLength ) )
          //Bring value to least significant bit
          >> ( 64 - br.m_byBitLength ) ;
        if( iter_registerdata->m_bCalculateDiff )
        {
          m_ullValue2  = m_ullValue2 - iter_registerdata->m_ullPreviousValue ;
        }
        iter_registerdata->m_ullPreviousValue = m_ullValue ;
        //mp_msr_data->GetTableContainingDataName(iter->m_strDataName);
        #ifdef __CYGWIN__
        m_wxstrULL = wxString::Format( wxString( wxT("%llu") ), m_ullValue2) ;
        #else
        m_wxstrULL = wxString::Format( wxString( wxT("%I64u") ), m_ullValue2) ;
        #endif
        //(*iterp_wxstatictext)->SetLabel(//wxString::Format("%64u", m_ullValue2 )
        //  wxstrULL );
        stdvector_stdstringAttributeValue.push_back( 
          //(std::string) //wxString::Format("%64u", m_ullValue2)
          std::string( //wxstrULL.c_str()
            GetStdString( m_wxstrULL ) )
          );
        if( m_stdvector_p_wxstatictextiter != m_stdvector_p_wxstatictext.end() 
          )
        {
  #ifdef _DEBUG
          //wxstrULL = wxString::Format("%x", *m_stdvector_p_wxstatictextiter );
          //(*m_stdvector_p_wxstatictextiter)->SetLabel() ;
  #endif
          (*m_stdvector_p_wxstatictextiter)->SetLabel( m_wxstrULL ) ;
          ++ m_stdvector_p_wxstatictextiter ;
        }
      }
      ++ iter_registerdata ;
      ++ iterp_wxstatictext ;
    }
  }
  catch(ReadMSRexception ex)
  {
//    //Breakpoint possibility
//    int i = 0 ;
  }
}

void wxDynamicDialog::OnReloadCPUregisterToReadConfig(
  wxCommandEvent & r_wxcommandevent )
{
  ReloadCPUregisterToReadConfig() ;
}

void wxDynamicDialog::OnRuntimeCreatedControls(wxCommandEvent & wxevent)
{
  int nControlID = wxevent.GetId() ;
  if( nControlID == ID_IntervalCheckbox )
  {
    if( mp_wxcheckboxInterval->GetValue() )
    {
      DWORD dw ;
      if( mp_wxtextctrlUpdateIntervalInMs->GetValue().ToULong( &dw, 10) )
      {
        mp_wxtextctrlUpdateIntervalInMs->SetToolTip(wxT("") ) ;
        m_wxtimer.Start(dw) ;
      }
      else
      {
        mp_wxtextctrlUpdateIntervalInMs->SetToolTip(wxT("not a number") ) ;
        m_wxtimer.Start(1000) ;
      }
    }
    else
      m_wxtimer.Stop() ;
  }
  //case 
//  if( mp_wxcheckboxReloadCPUregisterToReadConfig->GetValue()
////      nControlID == //mp_wxcheckboxReloadCPUregisterToReadConfig->GetId()
////        ID_ReloadCPUregisterToReadConfig
//    )
//  {
////    ReloadCPUregisterToReadConfig() ;
//  }
//  std::string strAttrVal ;
//  //iter_registerdata = mp_msr_data->m_stdvec_registerdata.begin() ;
//  std::vector<RegisterDataTable>::iterator iter_registerdatatable = 
//    mp_msr_data->m_stdvector_registerdatatable.begin() ;
//
    //DisplayRegisterData(& mp_msr_data) ;

//  //Now we have retrieved every single register data.
//  while( 
//    //iter_registerdata != mp_msr_data->m_stdvec_registerdata.end() 
//    iter_registerdatatable != mp_msr_data->m_stdvector_registerdatatable.end()
//    )
//  {
//#ifdef _DEBUG
//    //TRACE("table for :%s\n", iter_registerdatatable->m_ararstdstr[0][0]") ;
//    std::string strTabName = iter_registerdatatable->m_ararstdstr[0] ;
//#endif
//    if( mp_msr_data->GetTableAttributeValue(
//      stdvector_stdstringAttributeValue, 
//      //iter->m_strDataName, 
//      iter_registerdatatable,
//      strAttrVal 
//        )
//      )
//    {
//      //wxBoxSizer * p_sizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
//      // p_sizerHorizontal->Add( new wxStaticText(this, wxID_ANY, 
//      //   (*iter_registerdatatable).m_str  ) 
//      //   , 0 , wxFIXED_MINSIZE, 
//      //  //Determines the border width, if the flag  parameter is set to include 
//      //  //any border flag.
//      //  2 
//      //  );
//      // wxStaticText * p_wxstatictext = new wxStaticText(this, wxID_ANY, 
//      //   strAttrVal
//      //   ) ;
//      //p_sizerHorizontal->Add( p_wxstatictext, 0 , wxFIXED_MINSIZE, 
//      //  //Determines the border width, if the flag  parameter is set to include 
//      //  //any border flag.
//      //  2 
//      //  );
//      // mp_sizerVertical->Add(p_sizerHorizontal);
//      (*iterp_wxstatictext)->SetLabel(wxString( strAttrVal ) );
//    }
//    else
//      (*iterp_wxstatictext)->SetLabel( _T("na") );
//    //++ iter_registerdata ;
//    ++ iter_registerdatatable ;
//    ++ iterp_wxstatictext ;
//  }

//  DisplayRegisterData() ;
}

void wxDynamicDialog::OnSize( wxSizeEvent & //WXUNUSED(
  sizeevent//)
  )
{
  if( mp_wxcheckboxRebuildGUIonResize->IsChecked() )
    ReBuildGUI() ;
}

void wxDynamicDialog::OnTimerEvent(wxTimerEvent &event)
{
  DisplayRegisterData() ;
}

inline void wxDynamicDialog::ReBuildGUI()
{
  //delete all contained UI controls.
  //delete mp_sizerTop ;
  //while( mp_sizerTop->m_children.GetCount() &&
  //  mp_sizerTop->Detach(//size_t index
  //  0 )
  //  )
  //{
  //  mp_sizerTop->Remove(
  //    //size_t index
  //    0 ) ;
  //}
  //undocumented method see wx/sizer.h
  //Delete (all) the windows that the outmost sizer contains.
  mp_sizerTop->DeleteWindows() ;
  mp_sizerTop->Clear( //bool delete_windows = false
    true );
//  mp_wxboxsizerOutmost->DeleteWindows() ;
//  mp_wxboxsizerOutmost->Clear( //bool delete_windows = false
//    true );
//  BuildGUI() ;
  BuildCPUregisterControls() ;
  //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizerlayout:
  //"Call this to force layout of the children anew, e.g. after having added a
  //child to or removed a child (window, other sizer or space) from the sizer
  //while keeping the current dimension."
//  mp_sizerTop->Layout() ;
  mp_wxboxsizerOutmost->Layout() ;
}

inline void wxDynamicDialog::ReloadCPUregisterToReadConfig()
{
  m_stdvector_p_wxstatictext.//empty()
    clear() ;
  //m_stdvector_p_wxstatictextiter.empty()  ;
  mp_modeldata->m_stdvector_msrdata.//empty() ;
    clear() ;
  mp_modeldata->m_stdvector_cpuiddata.clear() ;
  //::wxGetApp().m_maincontroller.Init(*mp_modeldata, & ::wxGetApp()) ;
  std::string strCPUtypeRelativeDirPath ;
  if( mp_wxx86infoandcontrolapp->m_maincontroller.GetPstatesDirPath(
      strCPUtypeRelativeDirPath)
    )
  {
//      BYTE byModel ;
//      BYTE byStepping ;
    //SAX2_CPUspecificHandler sax2handler( * p_userinterface, model );
    std::string strFamilyAndModelFilePath = strCPUtypeRelativeDirPath + ".xml" ;
    mp_wxx86infoandcontrolapp->m_maincontroller.ReadRegisterDataConfig(
      strFamilyAndModelFilePath ,
      mp_wxx86infoandcontrolapp ) ;
  }
  ReBuildGUI() ;
}
