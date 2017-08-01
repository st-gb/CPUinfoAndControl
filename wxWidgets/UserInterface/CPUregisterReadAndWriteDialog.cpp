/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   CPUregisterReadAndWriteDialog.cpp
 * Author: Stefan
 * 
 * Created on 2. März 2010, 14:45
 */
#include "CPUregisterReadAndWriteDialog.hpp"
#include <wx/button.h>
#include <wx/stattext.h> //for wxStaticText
#include <wx/sizer.h> //wxBoxSizer
#include <wx/listbox.h> //wxListBox

#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/I_CPUaccess.hpp>
//ConvertStdStringToTypename(...)
#include <Controller/character_string/stdstring_format.hpp>
//getBinaryRepresentation(...)
#include <Controller/character_string/format_as_string.hpp>
#include <Controller/character_string/stdtstr.hpp>
#include <ModelData/ModelData.hpp>
#include <ModelData/RegisterData.hpp>
//std::string GetStdString(wxString & wxstr), getwxString(...) etc.
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

//An enum guarantees a unique number for each element.
enum
{
  ID_RegisterListBox = 1
  , ID_ReloadCPUregisterToReadConfig
  , ID_WriteToMSR
  , ID_ReadFromMSR
  , ID_PreparePMC
  , ID_LastStaticWindowID
};

#include <wx/event.h> //for "BEGIN_EVENT_TABLE"

BEGIN_EVENT_TABLE(CPUregisterReadAndWriteDialog, wxDialog)
  EVT_LISTBOX(ID_RegisterListBox,CPUregisterReadAndWriteDialog::
    OnRegisterListBoxSelection)
  EVT_BUTTON(ID_ReloadCPUregisterToReadConfig,CPUregisterReadAndWriteDialog::OnReloadCPUregisterToReadConfig)
  EVT_BUTTON(ID_ReadFromMSR,CPUregisterReadAndWriteDialog::OnReadFromMSR)
  EVT_BUTTON(ID_WriteToMSR,CPUregisterReadAndWriteDialog::OnWriteToMSR)
  EVT_BUTTON(ID_PreparePMC,CPUregisterReadAndWriteDialog::OnPreparePMC)
END_EVENT_TABLE()

CPUregisterReadAndWriteDialog::CPUregisterReadAndWriteDialog(
  wxWindow * parent ,
    //MSRdata & r_regdata ,
    Model & r_modeldata ,
    I_CPUcontroller * p_cpucontroller
    //I_CPUaccess * i_cpuaccess
  )
  :
  wxDialog(
    parent,
    wxID_ANY //
    //ID_Dialog
    ,
    //wxString::Format("MSR register index: %x",
    //r_regdata.m_dwIndex)
    wxT("CPUregisterReadAndWriteDialog")
    , wxPoint(30, 30) //A value of (-1, -1) indicates a default size
    , wxSize(400, 200)
    , wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  , mp_cpucontroller ( p_cpucontroller )
  , mp_modeldata( & r_modeldata )
  , m_wNumIgnoreChanges( 0 )
{
  std::vector<MSRdata>::iterator itermsrdata =
    mp_modeldata->m_stdvector_msrdata.begin() ;

  if( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
    mp_msrdata = & (*itermsrdata );
  else
    mp_msrdata = NULL ;

  mp_sizerVerticalOuter = new wxBoxSizer( wxVERTICAL );
  mp_sizerAttributeNameAndValue = new wxBoxSizer(//wxVERTICAL
    wxHORIZONTAL );
  mp_sizerLeftColumn = new wxBoxSizer(wxVERTICAL);
  mp_sizerRightColumn = new wxBoxSizer(wxVERTICAL);
  //mp_wxgridsizerAttributeNameAndValue = new wxGridSizer( //number of columns 
  //  2 ) ;

  mp_wxgridsizerAttributeNameAndValue = new wxFlexGridSizer( 
    // int cols: number of columns
    3); //3 columns: "register ID list box", "
//  wxButton * p_wxbuttonMSR = new wxButton(
//      this
//      , m_wControlID
//      , _T("write to MSR register")
//    ) ;
  wxListBox * p_wxlistbox = CreateRegisterIDlistBox();

  itermsrdata =
    mp_modeldata->m_stdvector_msrdata.begin() ;
  if( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
    ShowRegisterAttributes( *itermsrdata ) ;

  mp_sizerAttributeNameAndValue->Add( p_wxlistbox , 0 , wxEXPAND | wxBOTTOM ) ;
  //mp_sizerAttributeNameAndValue->Add( mp_sizerLeftColumn );
  //mp_sizerAttributeNameAndValue->Add( mp_sizerRightColumn ) ;
  mp_sizerAttributeNameAndValue->Add( mp_wxgridsizerAttributeNameAndValue ) ;

  AddActionButtons();

  //SetSizer(mp_sizerAttributeNameAndValue );
  //mp_sizerVerticalOuter->Add( mp_sizerAttributeNameAndValue ) ;
  //SetSizer(mp_sizerVerticalOuter) ;
  SetSizer( mp_sizerAttributeNameAndValue);
  //mp_sizerTop->SetSizeHints(this);
  //mp_sizerTop->Fit(this);
}

CPUregisterReadAndWriteDialog::CPUregisterReadAndWriteDialog(
  const CPUregisterReadAndWriteDialog& orig)
{
}

wxListBox * CPUregisterReadAndWriteDialog::CreateRegisterIDlistBox()
{
  std::vector<MSRdata>::iterator itermsrdata =
    mp_modeldata->m_stdvector_msrdata.begin() ;
  p_wxlistbox = new wxListBox( this, //wxID_ANY
    ID_RegisterListBox ) ;
  //mp_sizerVerticalOuter->Add( p_wxlistbox ) ;
  while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() )
  {
    //ullMSR = dwEDX  ;
    //ullMSR <<= 32 ;
    //ullMSR |= dwEAX ;
    //BuildGUI( *itermsrdata ) ,
    //itermsrdata->m_dwIndex ;
    if( itermsrdata->m_stdstrRegisterName.empty() )
    {
      //Create object here and not as parameter (else:
      // g++ warning: "taking addesss of temporary".
      wxString wxstr =
        //& wxString::Format("%u",itermsrdata->m_dwIndex )
        wxString::Format(
          //Use wxT() to enable to compile with both unicode and ANSI.
          wxT("%lu") , itermsrdata->m_dwIndex ) ;
      p_wxlistbox->InsertItems( 1, & wxstr , p_wxlistbox->GetCount() ) ;
    }
    else
    {
      //Create object here and not as parameter (else:
      // g++ warning: "taking addesss of temporary".
      wxString wxstr =
        //& wxString::Format("%u",itermsrdata->m_dwIndex )
        wxWidgets::getwxString( itermsrdata->m_stdstrRegisterName ) ;
      p_wxlistbox->InsertItems( 1, & wxstr , p_wxlistbox->GetCount() ) ;
    }
    ++ itermsrdata ;
  }
  return p_wxlistbox;
}

void CPUregisterReadAndWriteDialog::AddActionButtons()
{
  wxBoxSizer * p_wxsizerButtons = new wxBoxSizer( wxVERTICAL ) ;
  mp_wxbuttonWriteMSR = new wxButton( this, ID_WriteToMSR ,
    wxT("write to MSR") ) ;
  mp_wxbuttonWriteMSR->SetToolTip( wxT("writes to the CPU's Model Specific "
    "Register") ) ;
  //mp_sizerAttributeNameAndValue->Add( mp_wxbuttonWriteMSR) ;
  p_wxsizerButtons->Add( mp_wxbuttonWriteMSR) ;
  wxButton * p_wxbuttonReadFromMSR = new wxButton( this, ID_ReadFromMSR , 
    wxT("read from MSR") ) ;
  //mp_sizerAttributeNameAndValue->Add( p_wxbuttonReadFromMSR) ;
  p_wxsizerButtons->Add( p_wxbuttonReadFromMSR) ;
  wxButton * p_wxbuttonPreparePMC = new wxButton( this, ID_PreparePMC , 
    wxT("prepare for PMC") ) ;
  p_wxsizerButtons->Add( p_wxbuttonPreparePMC) ;

  mp_sizerAttributeNameAndValue->Add( p_wxsizerButtons ) ;
}

void CPUregisterReadAndWriteDialog::ShowRegisterAttributes( //const
  MSRdata & r_msrdata )
{
  std::vector<RegisterData>::iterator iter_registerdata =
    r_msrdata.m_stdvec_registerdata.begin() ;
  wxTextCtrl * p_wxtextctrl ;
  wxBoxSizer * p_wxsizerAttributeNameAndValue ;
  m_stdvec_p_wxtextctrl.clear() ;
  mp_wxgridsizerAttributeNameAndValue->Clear(
    //bool delete_windows = false 
    true ) ;

  WORD m_wWindowID = ID_LastStaticWindowID ;
  //std::vector<RegisterData>  ;
  while( iter_registerdata != r_msrdata.m_stdvec_registerdata.end() )
  {
    p_wxsizerAttributeNameAndValue = new wxBoxSizer( wxHORIZONTAL ) ;
    //mp_sizerLeftColumn->Add( 
    //p_wxsizerAttributeNameAndValue->Add(
    wxStaticText * p_wxstatictext = new wxStaticText(this, wxID_ANY ,
      wxWidgets::getwxString( iter_registerdata->m_strDataName )
      );
    p_wxstatictext->SetForegroundColour(*wxBLUE);
    mp_wxgridsizerAttributeNameAndValue->Add(
      p_wxstatictext
      , 0 //proportion
      , wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL
      ) ;
    if( iter_registerdata->m_stdvec_bitrange.empty() )
      mp_wxgridsizerAttributeNameAndValue->Add(
        new wxStaticText(this, wxID_ANY ,
          wxT("-") ) 
        , 0 //proportion
        , wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL
        ) ;
    else
    {
      BitRange & r_br = iter_registerdata->m_stdvec_bitrange.at(0) ;
      mp_wxgridsizerAttributeNameAndValue->Add(
        new wxStaticText( this, wxID_ANY ,
          wxString::Format(
            //Use wxT() to enable to compile with both unicode and ANSI.
            wxT("%u:%u"), r_br.m_byStartBit ,
          //End bit= start bit + bitlenght - 1: e.g. startbit 0, bitlenght: 8
          //-> endbit=7
          r_br.m_byStartBit + 
          r_br.m_byBitLength - 1 )
          )
        , 0 //proportion
        , wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL
        , 2 //int border
        ) ;
    }
    p_wxtextctrl = new wxTextCtrl(this, m_wWindowID ) ;
    //EVT_TEXT(id, func)
    Connect( m_wWindowID ++ , //wxID_ANY,
      wxEVT_COMMAND_TEXT_UPDATED ,
      wxCommandEventHandler(CPUregisterReadAndWriteDialog::OnChangedText)
      );
   
    //mp_sizerRightColumn->Add(
    //p_wxsizerAttributeNameAndValue->Add(
    mp_wxgridsizerAttributeNameAndValue->Add(
      p_wxtextctrl ) ;
    mp_sizerLeftColumn->Add( p_wxsizerAttributeNameAndValue ) ;
    m_stdvec_p_wxtextctrl.push_back( p_wxtextctrl ) ;
    ++ iter_registerdata ;
  }
}

CPUregisterReadAndWriteDialog::~CPUregisterReadAndWriteDialog() {
}

//TODO if text control changes, other text controls get false values?!
void CPUregisterReadAndWriteDialog::OnChangedText(wxCommandEvent & wxevent )
{
//  if( m_wNumIgnoreChanges )
//    -- m_wNumIgnoreChanges ;
//  else
  {
    std::vector<RegisterData>::iterator iter_registerdata =
      mp_msrdata->m_stdvec_registerdata.begin() ;
    int nMenuEventID = wxevent.GetId() ;
    WORD wRegisterDataIndexChangedContent = nMenuEventID -
        ID_LastStaticWindowID ;
    RegisterData & r_registerdata = mp_msrdata->m_stdvec_registerdata.at( 
      wRegisterDataIndexChangedContent ) ;
    if( ! r_registerdata.m_stdvec_bitrange.empty() )
    {
      BitRange & r_bitrangeChangedAttrValue = r_registerdata.m_stdvec_bitrange.
          front() ;
      WORD wCurrentRegisterDataIndex = 0 ;

      wxTextCtrl * p_wxtextctrl = m_stdvec_p_wxtextctrl.at(
        wRegisterDataIndexChangedContent ) ;
      wxString wxstrChanged = p_wxtextctrl->GetValue() ;
//      char * p_ch = (char *) wxstrChanged.fn_str() ;
      ULONGLONG ullFromChangedTextCtrl ;
//      if( wxstrChanged.
//        //http://docs.wxwidgets.org/2.8.4/wx_wxstring.html#wxstringtoulong:
//        // "Returns true on success [...]"
      //ToULongLong() always returned false with MinGW (+ unicode)
//        ToULongLong( & ullFromChangedTextCtrl )
//        )
      std::string stdstrFromTextControl = wxWidgets::GetStdString(wxstrChanged ) ;
      ConvertStdTStringToTypename<ULONGLONG>( ullFromChangedTextCtrl,
          stdstrFromTextControl) ;
      {
        BYTE byAffectedAttrValueEndBit ;
        ULONGLONG ullFromTextCtrlToChange ;
        ULONGLONG ullWriteToTextCtrl ;
        ULONGLONG ullClearingBitMask ;
        std::string stdstrValueToModify ;
        wxString wxstrULL ;
        wxString wxstrValueToModify ;
        //mp_msrdata->
        std::vector<wxTextCtrl * >::iterator iter_p_wxtextctrl =
            m_stdvec_p_wxtextctrl.begin() ;
        while( iter_registerdata != mp_msrdata->m_stdvec_registerdata.end() 
          && iter_p_wxtextctrl != m_stdvec_p_wxtextctrl.end() 
          )
        {
          if( wCurrentRegisterDataIndex != wRegisterDataIndexChangedContent &&
            ! iter_registerdata->m_stdvec_bitrange.empty() )
          {
            BitRange & r_bitrangeAffectedAttrValue = iter_registerdata->
                m_stdvec_bitrange.front() ;
            //Wenn Ueberschneidung: 
            //Start des zu vergleichenden VOR Ende des anderen;
            if( r_bitrangeAffectedAttrValue.m_byStartBit < 
              r_bitrangeChangedAttrValue.m_byStartBit + 
                r_bitrangeChangedAttrValue.m_byBitLength
              //Ende des zu vergleichenden hinter Start des anderen.
              && 
              r_bitrangeAffectedAttrValue.m_byStartBit + 
                r_bitrangeAffectedAttrValue.m_byBitLength > 
              r_bitrangeChangedAttrValue.m_byStartBit 
              )
            {
              wxstrValueToModify = (*iter_p_wxtextctrl)->GetValue() ;
              //e.g. if only 1 bit for an MSR value is changed: change this 1 bit
              //  in any other attributes that cover it:
              // e.g. for bit "0" the 1 bit is changed. So the MSR attribute value 
              //  for the attribute that covers the whole MSR must also be changed at
              //  bit pos. "0".
//              if( wxstrValueToModify.
//                //http://docs.wxwidgets.org/2.8.4/wx_wxstring.html#wxstringtoulong:
//                // "Returns true on success [...]"
//                  //ToULongLong() always returned false with MinGW (+ unicode)
//                ToULongLong( & ullFromTextCtrlToChange )
//                )
              std::string stdstrValueToModify = wxWidgets::GetStdString(wxstrValueToModify) ;
              ConvertStdTStringToTypename<ULONGLONG>( ullFromTextCtrlToChange,
                  stdstrValueToModify) ;
              {
                //adopt the start bit:
                //-if the digit for a single bit that started at bit pos 8 was changed and
                //   the affected other attribute starts at bitpos 0:
                //   shift 8 bits left
                //-if the digits for bits that start at bit pos 0 were changed and
                //   the affected other attribute starts at bitpos 8:
                //   shift 8 bits right
                if( r_bitrangeChangedAttrValue.m_byStartBit > 
                  r_bitrangeAffectedAttrValue.m_byStartBit )
                {
                  // e.g. bit 22 was changed, affected attribute begins at bit 0: 
                  //  in order to overwrite bit 22 of the affected value: shift 22 bits left
                  ullWriteToTextCtrl = ullFromChangedTextCtrl << 
                    ( r_bitrangeChangedAttrValue.m_byStartBit - 
                     r_bitrangeAffectedAttrValue.m_byStartBit ) ;
                  //   
                }
                else
                {
                  byAffectedAttrValueEndBit = 
                    r_bitrangeAffectedAttrValue.m_byStartBit + 
                    r_bitrangeAffectedAttrValue.m_byBitLength - 1 ;
                  //if( byAffectedAttrValueEndBit 
                  ullWriteToTextCtrl = ullFromChangedTextCtrl 
                    //Make right bits zero: e.g. if bitmask "0:1" : shift 63
                    //bits to left
                    << ( 63 - byAffectedAttrValueEndBit ) ;
                }
                ullWriteToTextCtrl = ullWriteToTextCtrl 
                  //Make left bits zero: e.g. if bitmask "0:1" : shift 63 bits
                  //to right
                  >> ( 64 - r_bitrangeAffectedAttrValue.m_byBitLength ) ;
                if( r_bitrangeChangedAttrValue.m_byBitLength < 64 )
                {
                  ullClearingBitMask = 1 ;
                  //E.g. for m_byBitLength = 8: 256
                  ullClearingBitMask <<= r_bitrangeChangedAttrValue.
                    m_byBitLength ;
                  //E.g. for m_byBitLength = 8: 256 -1 = 255 = 1111 1111bin
                  ullClearingBitMask -= 1 ;
                  //shift the msk to the appropriate pos.
                  ullClearingBitMask <<= r_bitrangeChangedAttrValue.
                    m_byStartBit ;
                  //Invert bitwise : 
  // 00000000 00000000 00000000 00000000 00000000 00000000 11111111 00000000 ->
  // 11111111 11111111 11111111 11111111 11111111 11111111 00000000 11111111
                  ullClearingBitMask = ~ullClearingBitMask ;
                }
                else
                  ullClearingBitMask = 0ULL ;
                //Make bits within the bitrange to zeros.
                ullFromTextCtrlToChange &= ullClearingBitMask ;
                ullFromTextCtrlToChange |= ullWriteToTextCtrl ;
                //(*iter_p_wxtextctrl)->GetValue()
                #ifdef __CYGWIN__
                wxstrULL = wxString::Format( wxString(
                  //Use wxT() to enable to compile with both unicode and ANSI.
                  wxT("%llu") ), //ullWriteToTextCtrl
                  ullFromTextCtrlToChange ) ;
                #else
                wxstrULL = wxString::Format( wxString(
                  //Use wxT() to enable to compile with both unicode and ANSI.
                  wxT("%I64u") ), //ullWriteToTextCtrl
                  ullFromTextCtrlToChange ) ;
                #endif
//                //calling "SetValue()" causes an immediate jump into this
//                //function (OnChangedText() ).
//                //But because this change is made program internal it should
//                //not be processed.
//                ++ m_wNumIgnoreChanges ;
                (*iter_p_wxtextctrl)->/*SetValue( wxstrULL );*/
                  //http://docs.wxwidgets.org/stable/wx_wxtextctrl.html:
                    //does not generate a wxEVT_COMMAND_TEXT_UPDATED
                  ChangeValue( wxstrULL );
              }
            }
          } //if
          ++ wCurrentRegisterDataIndex ;
          ++ iter_registerdata ;
          ++ iter_p_wxtextctrl ;
        }
      }
    }
  }
}

void CPUregisterReadAndWriteDialog::OnReloadCPUregisterToReadConfig(
  wxCommandEvent & evt )
{

}

void CPUregisterReadAndWriteDialog::OnRegisterListBoxSelection(
  wxCommandEvent & evt )
{
  wxArrayInt wxarrintSelections ;
//  int nNumberOfSelections =
    p_wxlistbox->GetSelections(wxarrintSelections) ;
  if( wxarrintSelections.GetCount () > 0 )
  {
    int nIndex = wxarrintSelections.Last () ;

    std::vector<MSRdata>::iterator itermsrdata =
      mp_modeldata->m_stdvector_msrdata.begin() ;
    int nCount = 0 ;
    while( itermsrdata != mp_modeldata->m_stdvector_msrdata.end() 
      && nIndex != nCount )
    {
      ++ nCount ;
      ++ itermsrdata ;
    }
    mp_msrdata = & (*itermsrdata) ;
    mp_sizerLeftColumn->Clear( //bool delete_windows = false 
      true ) ;
    mp_sizerRightColumn->Clear( //bool delete_windows = false 
      true ) ;
    ShowRegisterAttributes(*itermsrdata) ;
    //mp_sizerVerticalOuter->Fit(this);
    //Force the controls to be resized.
    //mp_sizerVerticalOuter->Layout() ;
    //Layout() ;
    //mp_sizerLeftColumn->Layout() ;
    //mp_sizerRightColumn->Layout() ;

//    mp_wxgridsizerAttributeNameAndValue->Layout() ;
    mp_sizerAttributeNameAndValue->Layout() ;

    //mp_sizerLeftColumn->RecalcSizes() ;
    //mp_sizerRightColumn->RecalcSizes() ;
  }
}

void CPUregisterReadAndWriteDialog::OnPreparePMC(
  wxCommandEvent & evt )
{
  mp_cpucontroller->PrepareForNextPerformanceCounting(
    1 , 0 ) ;
}

void CPUregisterReadAndWriteDialog::OnReadFromMSR(
  wxCommandEvent & evt )
{
  if( mp_msrdata )
  {
    /** Use data type "uint32_t" because this is the exact width of a CPU 
     *    register used by the "rdmsr" instruction */
    uint32_t lowmostBits, highmostBits ;
    std::vector<RegisterData>::iterator iter_registerdata =
      mp_msrdata->m_stdvec_registerdata.begin() ;
    std::vector<wxTextCtrl * >::const_iterator c_iter_p_wxtextctrl = 
      m_stdvec_p_wxtextctrl.begin() ;
    ULONGLONG ullValue , ullFromMSR;
    wxString wxstrULL ;
    mp_cpucontroller->RdmsrEx( 
      mp_msrdata->m_dwIndex
      , lowmostBits
      , highmostBits
      , 1 << mp_msrdata->m_byCoreID
      ) ;
    ullFromMSR = highmostBits ;
    ullFromMSR <<= 32 ;
    ullFromMSR |= lowmostBits ;
    while( iter_registerdata != mp_msrdata->m_stdvec_registerdata.end() 
      && c_iter_p_wxtextctrl != m_stdvec_p_wxtextctrl.end() 
      )
    {
      if( (*iter_registerdata).m_stdvec_bitrange.size() > 0 )
      {
        BitRange & br = (*iter_registerdata).m_stdvec_bitrange.at(0) ;
        ullValue = //make bits from highmost zero
          ( ullFromMSR << ( 64 - br.m_byStartBit - br.m_byBitLength ) ) 
          //Bring value to least significant bit
          >> ( 64 - br.m_byBitLength ) ;
        //mp_msr_data->GetTableContainingDataName(iter->m_strDataName);
        //#ifdef __CYGWIN__
        #ifndef _WIN32 //Linux, MinGW, Cygwin (, ...)
        wxstrULL = wxString::Format( wxString( wxT("%llu") ), ullValue) ;
        #else
        wxstrULL = wxString::Format( wxString( wxT("%I64u") ), ullValue) ;
        #endif
//        //calling "SetValue()" causes an immediate jump into this function
//        //(OnChangedText() ).
//        //But because this change is made program internal it should not be
//        //processed.
//        ++ m_wNumIgnoreChanges ;
        (*c_iter_p_wxtextctrl)->//SetValue( wxstrULL ) ;
          //http://docs.wxwidgets.org/stable/wx_wxtextctrl.html:
            //does not generate a wxEVT_COMMAND_TEXT_UPDATED
          ChangeValue( wxstrULL );
      }
      ++ iter_registerdata ;
      ++ c_iter_p_wxtextctrl ;
    }
  }  
}

void CPUregisterReadAndWriteDialog::OnWriteToMSR(
  wxCommandEvent & evt )
{
  LOGN( /*FULL_FUNC_NAME <<*/ "begin")
  if( mp_msrdata )
  {
    ULONGLONG ullWriteToMSR = 0ULL ;
//    ULONGLONG ullFromTextCtrl = 0ULL ;
    wxULongLong_t ullFromTextCtrl = 0ULL ;
    ULONGLONG ullMaxValueForBitLength ;
    std::vector<RegisterData>::iterator iter_registerdata =
      mp_msrdata->m_stdvec_registerdata.begin() ;
    std::vector<wxTextCtrl * >::const_iterator c_iter_p_wxtextctrl =
        m_stdvec_p_wxtextctrl.begin() ;
    wxString wxstr ;
    std::string stdstrFromTextControl ;
    std::vector<wxString> vec_wxstr ;
    wxString wxstrAllValues ;
    //std::vector<RegisterData>  ;
    while( iter_registerdata != mp_msrdata->m_stdvec_registerdata.end() 
      && c_iter_p_wxtextctrl != m_stdvec_p_wxtextctrl.end() 
      )
    {
      if( ! iter_registerdata->m_stdvec_bitrange.empty() )
      {
        BitRange & br = iter_registerdata->m_stdvec_bitrange.front() ;
        wxstr = (*c_iter_p_wxtextctrl)->GetValue() ;
//        char * p_ch = (char *) wxstr.fn_str() ;
//        vec_wxstr.push_back(wxstr) ;
        wxstrAllValues += wxstr + wxT(" ") ;
        stdstrFromTextControl = wxWidgets::GetStdString(wxstr) ;
        //ullMSR = 
//        if( wxstr.
//          //http://docs.wxwidgets.org/2.8.4/wx_wxstring.html#wxstringtoulong:
//          // "Returns true on success [...]"
            //ToULongLong() always returned false with MinGW (+ unicode)
//          ToULongLong(&ullFromTextCtrl)
//          )
        ConvertStdTStringToTypename<ULONGLONG>(ullFromTextCtrl,
          stdstrFromTextControl) ;
        {
          //Prevent bit shift of "1" 64 bits: this would lead to a zero ("0") .
          if( br.m_byBitLength < 64 )
          {
            ullMaxValueForBitLength = 
              //e.g. bitlength 3: 1bin << 3 = 1000bin = 8;  8-1 = 7
              ( 1ULL << br.m_byBitLength ) - 1 ;
            if( ullFromTextCtrl > ullMaxValueForBitLength )
              wxMessageBox(
                //Use wxT() to enable to compile with both unicode and ANSI.
                wxT("Wert zu gross")
                ) ;
            else
              ullWriteToMSR |= //( ullFromTextCtrl >> br.m_byBitLength ) <<
                //br.m_byStartBit ;
                ullFromTextCtrl << br.m_byStartBit ;
          }
          else
            ullWriteToMSR = ullFromTextCtrl ;
        }
//        else
//          wxMessageBox( wxT("converting string \"") + wxstr +
//            wxString::Format( wxT("\" to ulonglong: %u failed"),
//              ullFromTextCtrl )
//            ) ;
      }
      ++ iter_registerdata ;
      ++ c_iter_p_wxtextctrl ;
    }
  #ifdef _DEBUG
    ullWriteToMSR = ullWriteToMSR ;
  #endif
    DWORD dwHigh = ullWriteToMSR >> 32 ;
    DWORD dwLow = ullWriteToMSR ;
    std::string std_strLowBytes = getBinaryRepresentation(dwLow);
    std::string std_strHighBytes = getBinaryRepresentation(dwHigh);
    //Because wxString::Format "%s" needs wchar_t if Unicode und "char" if ANSI?!
    std::tstring std_tstrLowBytes = Getstdtstring(std_strLowBytes);
    std::tstring std_tstrHighBytes = Getstdtstring(std_strHighBytes);
    if( wxMessageBox( //wxstrAllValues +
      wxString::Format(
        wxT(//"writing "
            "Should write "
            "low bytes "
            //"%lu"
            "\n\"%s\" (binary)\n"
            " and high bytes "
            //"%lu"
            "\n\"%s\" (binary)\n"
            " to MSR index %lu to "
          "core %u ?") ,
//        , dwLow
        std_tstrLowBytes.c_str(),
        //dwHigh ,
        std_tstrHighBytes.c_str(),
        mp_msrdata->m_dwIndex ,
        mp_msrdata->m_byCoreID
        )
      , wxT("write to MSR now?")
      , wxYES_NO
        )
        == wxYES
      )
      //The CPU controller should be asked to write to the MSR because this is
      //the CPU-specific part that knows which MSRs are dangerous to write to.
      mp_cpucontroller->WrmsrEx(
        mp_msrdata->m_dwIndex ,
        dwLow ,
        dwHigh ,
        1 << mp_msrdata->m_byCoreID
        ) ;
  }
}
