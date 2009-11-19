#include "wxDynamicDialog.hpp"
#include <wx/button.h>
#include <wx/checkbox.h>
//#include <wx/dialog.h>
#include <wx/stattext.h> //for wxStaticText
#include <wx/sizer.h> //wxBoxSizer
#include <wx/timer.h> //wxTimer 
#include <vector>
//#include "ModelData/RegisterData.hpp"
#include "ModelData/ModelData.hpp"
//#include "Controller/PumaStateCtrl.h"
//#include "Controller/GriffinController.hpp"
#include <Controller/I_CPUcontroller.hpp>
#include <wxWidgets/App.hpp> //for wxGetApp() / DECLARE_APP
#include <wxWidgets/wxStringHelper.h>

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
END_EVENT_TABLE()

wxDynamicDialog::wxDynamicDialog(//RegisterData 
  wxWindow * parent ,
  //MSRdata & r_regdata ,
  Model & r_modeldata ,
  //GriffinController * p_pumastatecontrol
  I_CPUcontroller * p_cpucontroller
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
  //, mp_pumastatecontrol ( p_pumastatecontrol )
  , mp_cpucontroller ( p_cpucontroller )
  , mp_modeldata ( & r_modeldata)
{
  m_wControlID = 1 ;
  m_wxtimer.SetOwner(this, ID_Timer) ;
  //mp_regdata = & r_regdata ;
  //mp_msr_data = & r_regdata ;
  BuildGUI() ;

  Connect( 
    //m_wControlID //++ 
    ID_ReloadCPUregisterToReadConfig ,
    //, 
    wxID_ANY, 
    wxEVT_COMMAND_BUTTON_CLICKED, 
    wxCommandEventHandler(wxDynamicDialog::OnRuntimeCreatedControls) 
    );
  Connect( 
    ID_IntervalCheckbox ,
    wxEVT_COMMAND_CHECKBOX_CLICKED, 
    wxCommandEventHandler(wxDynamicDialog::OnRuntimeCreatedControls) 
    );
}

void wxDynamicDialog::BuildGUI()
{
  std::vector<MSRdata>::iterator itermsrdata = 
    mp_modeldata->m_stdvector_msrdata.begin() ;
  //DWORD dwEAX,dwEDX, dwAffMask = 1 ;
  //ULONGLONG ullMSR ;
  mp_sizerTop = new wxBoxSizer(//wxVERTICAL
    wxHORIZONTAL );
  mp_wxcheckboxReloadCPUregisterToReadConfig = new wxCheckBox(
    this 
    , ID_ReloadCPUregisterToReadConfig
    , _T("reload CPU register to read configuration") 
    ) ;
  mp_wxcheckboxInterval = new wxCheckBox(
    this 
    , ID_IntervalCheckbox
    , _T("interval") 
    ) ;
  mp_wxtextctrlUpdateIntervalInMs = new wxTextCtrl(
    this 
    , wxID_ANY
    , _T("interval in ms") 
    ) ;
  mp_sizerLeftColumn = new wxBoxSizer(wxVERTICAL);
  mp_sizerRightColumn = new wxBoxSizer(wxVERTICAL);
  wxButton * p_wxbuttonMSR = new wxButton(
      this
      , m_wControlID 
      , _T("read from MSR register") 
    ) ;
  mp_sizerLeftColumn->Add( 
    mp_wxcheckboxReloadCPUregisterToReadConfig
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );
  mp_sizerLeftColumn->Add( 
    mp_wxcheckboxInterval
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );
  mp_sizerRightColumn->Add( 
    mp_wxtextctrlUpdateIntervalInMs
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );
  mp_sizerLeftColumn->Add( 
    p_wxbuttonMSR
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );
  mp_sizerRightColumn->Add( 
    //p_wxbuttonMSR
    //new wxStaticText(
    //  this, 
    //  wxID_ANY, 
    //  "" 
    //  )
    new wxButton(
      this
      , wxID_ANY 
      , _T("dummy button") 
    )
    , 0 
    , wxFIXED_MINSIZE, 
    //Determines the border width, if the flag  parameter is set to include 
    //any border flag.
    2 
    );

  mp_sizerTop->Add( mp_sizerLeftColumn ); 
  mp_sizerTop->Add( mp_sizerRightColumn ); 
  while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
  {
    //ullMSR = dwEDX  ;
    //ullMSR <<= 32 ;
    //ullMSR |= dwEAX ;
    BuildGUI( *itermsrdata ) ,
    ++ itermsrdata ;
  }
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
  SetSizer(mp_sizerTop );
  mp_sizerTop->SetSizeHints(this);
  mp_sizerTop->Fit(this);
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
     wxStaticText * p_wxstatictext = new wxStaticText(
       this
       , wxID_ANY
       //(*iter).m_str  
       , wxString(wxT("") )
       ) ;
    //p_sizerHorizontal->Add( new wxStaticText(
    mp_sizerLeftColumn->Add( new wxStaticText(
      this, 
      wxID_ANY, 
      getwxString( (*iter_registerdata).m_strDataName )
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
      //Determines the border width, if the flag parameter is set to include 
      //any border flag.
      , 2 
      );       
     //m_stdmap_p_wxstatictext
     m_stdvector_p_wxstatictext.push_back(p_wxstatictext) ;
    //p_sizerHorizontal->Add( 
    mp_sizerRightColumn->Add( 
      p_wxstatictext
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
     //mp_sizerTop->Add(
     //  p_sizerHorizontal
     // //"[...] used in wxBoxSizer to indicate if a child of a sizer can 
     // //change its size in the main orientation of the wxBoxSizer - where 
     // //0 stands for not changeable and 
     // //a value of more than zero is interpreted relative to the value of 
     // //other children of the same wxBoxSizer. "
     // //1 
     // , 0,
     // //wxEXPAND | //wxALL
     //   wxBOTTOM
     // ,
     // //http://docs.wxwidgets.org/2.6/wx_wxsizer.html#wxsizeradd:
     // //"Determines the border width, if the flag  parameter is set to
     // //include any border flag."
     // //10 
     // 0
     // );
     ++ iter_registerdata ;
  }
  //std::vector<RegisterDataTable>::iterator iter_registerdatatable =
  //  //mp_msr_data->
  //  r_msrdata.m_stdvector_registerdatatable.begin() ;

  ////Now add controls for values dependand on register values.
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

void wxDynamicDialog::DisplayRegisterData()
{
  m_stdvector_p_wxstatictextiter = m_stdvector_p_wxstatictext.begin() ;
  std::vector<MSRdata>::iterator itermsrdata = 
    mp_modeldata->m_stdvector_msrdata.begin() ;
  while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
  {
    DisplayRegisterData(*itermsrdata) ;

    ++ itermsrdata ;
  }
}

void wxDynamicDialog::DisplayRegisterData(MSRdata & r_msrdata)
{
  std::vector<RegisterData>::iterator iter_registerdata = 
    r_msrdata.m_stdvec_registerdata.begin() ;
  std::vector<wxStaticText *>::iterator iterp_wxstatictext = 
    m_stdvector_p_wxstatictext.begin() ;

  DWORD dwEAX,dwEDX, dwAffMask = //TODO change to be compatible with more 
    //than 1 CPU core
    1 ;
  ULONGLONG ullMSR , ullValue, ullDiff ;
  try
  {
    //mp_pumastatecontrol->RdmsrEx(//itermsrdata->
    mp_cpucontroller->RdmsrEx(
      r_msrdata.m_dwIndex,
      dwEAX,
      dwEDX, 
      dwAffMask
      );
    ullMSR = dwEDX  ;
    ullMSR <<= 32 ;
    ullMSR |= dwEAX ;
    //This vector data is needed to get the replacement value for a table:
    //e.g. a value depends on more than 1 other value:
    //MHz | FID | DID
    //2200|  14 | 0
    //1100|  14 | 1
    //so both FID and DID are needed in order to get the value for "MHz"
    std::vector<std::string> stdvector_stdstringAttributeValue ;
    wxString wxstrULL ;
    //In this loop: get every single data at first.
    while( 
      iter_registerdata != r_msrdata.m_stdvec_registerdata.end() && 
      iterp_wxstatictext != m_stdvector_p_wxstatictext.end() )
    {
      if( (*iter_registerdata).m_stdvec_bitrange.size() > 0 )
      {
        BitRange & br = (*iter_registerdata).m_stdvec_bitrange.at(0) ;
        ullValue = //make bits from highmost zero
          (ullMSR << 64 - br.m_byStartBit - br.m_byBitLength ) 
          //Bring value to least significant bit
          >> ( 64 - br.m_byBitLength ) ;
        if( iter_registerdata->m_bCalculateDiff )
        {
          ullValue  = ullValue - iter_registerdata->m_ullPreviousValue ;
        }
        iter_registerdata->m_ullPreviousValue = ullMSR ;
        //mp_msr_data->GetTableContainingDataName(iter->m_strDataName);
        wxstrULL = wxString::Format( wxString( wxT("%I64u") ), ullValue) ;
        //(*iterp_wxstatictext)->SetLabel(//wxString::Format("%64u", ullValue )
        //  wxstrULL );
        stdvector_stdstringAttributeValue.push_back( 
          //(std::string) //wxString::Format("%64u", ullValue) 
          std::string( //wxstrULL.c_str()
            getstdstring(wxstrULL) )
          );
        if( m_stdvector_p_wxstatictextiter != m_stdvector_p_wxstatictext.end() 
          )
        {
  #ifdef _DEBUG
          //wxstrULL = wxString::Format("%x", *m_stdvector_p_wxstatictextiter );
          //(*m_stdvector_p_wxstatictextiter)->SetLabel() ;
  #endif
          (*m_stdvector_p_wxstatictextiter)->SetLabel(wxstrULL) ;
          ++ m_stdvector_p_wxstatictextiter ;
        }
      }
      ++ iter_registerdata ;
      ++ iterp_wxstatictext ;
    }
  }
  catch(ReadMSRexception ex)
  {
    int i = 0 ;
  }
}

void wxDynamicDialog::OnRuntimeCreatedControls(wxCommandEvent & wxevent)
{
  int nControlID = wxevent.GetId() ;
  if( nControlID == ID_IntervalCheckbox )
  {
    if( mp_wxcheckboxInterval->GetValue() )
      m_wxtimer.Start(1000) ;
    else
      m_wxtimer.Stop() ;
  }
  //case 
  if( mp_wxcheckboxReloadCPUregisterToReadConfig->GetValue() )
  {
    m_stdvector_p_wxstatictext.//empty() 
      clear() ;
    //m_stdvector_p_wxstatictextiter.empty()  ;
    mp_modeldata->m_stdvector_msrdata.//empty() ;
      clear() ;
    ::wxGetApp().m_maincontroller.Init(*mp_modeldata, & ::wxGetApp()) ;
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
    mp_sizerTop->DeleteWindows() ;
    mp_sizerTop->Clear( //bool delete_windows = false 
      true );
    BuildGUI() ;
    mp_sizerTop->Layout() ;
  }
 // if
//  DWORD dwEAX,dwEDX, dwAffMask = 1 ;
//	  //DWORD & dwEAX,			// bit  0-31
//	  //DWORD & dwEDX,			// bit 32-63
//  mp_pumastatecontrol->RdmsrEx(mp_msr_data->m_dwIndex,dwEAX,dwEDX, dwAffMask);
//  ULONGLONG ullMSR = dwEDX  ;
//  ullMSR <<= 32 ;
//  ullMSR |= dwEAX ;
//  ULONGLONG ullValue ;
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
  DisplayRegisterData() ;
}

void wxDynamicDialog::OnTimerEvent(wxTimerEvent &event)
{
  DisplayRegisterData() ;
}
