/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DynFreqScalingDlg.hpp"
#include <ModelData/CPUcoreData.hpp>

BEGIN_EVENT_TABLE(DynFreqScalingDlg, wxDialog)
//  EVT_BUTTON( //wxID_OK
//    wxID_APPLY , DynFreqScalingDlg::OnOK_Button)
//  EVT_BUTTON(wxID_CANCEL, DynFreqScalingDlg::OnCancelButton)
END_EVENT_TABLE()

DynFreqScalingDlg::DynFreqScalingDlg( 
  wxWindow * p_wxwindowParent
  , CPUcoreData & r_cpucoredata
  )
  : wxDialog( 
    p_wxwindowParent 
    , wxID_ANY
    , _T("settings for Dynamic Voltage and Frequency Scaling")
    , wxPoint(30, 30) //A value of (-1, -1) indicates a default size
    , wxSize(400, 400)
    , wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  , m_floatvalidator( & //m_fThrottleTempInDegCelsius 
    r_cpucoredata.m_fThrottleTempInDegCelsius )
  , mp_cpucoredata( & r_cpucoredata )
{
  wxBoxSizer * sizerVert = new wxBoxSizer(wxVERTICAL);
  mp_wxbuttonOK = new wxButton(this, wxID_OK ) ;
  mp_wxbuttonCancel = new wxButton(this, wxID_CANCEL ) ;
  p_wxtextctrlThrottleTemp = new wxTextCtrl( this, wxID_ANY, //wxT("90")
    wxString::Format(
      //Use wxT() to enable to compile with both unicode and ANSI.
      wxT("%f") ,mp_cpucoredata->m_fThrottleTempInDegCelsius)
    ) ;
  p_wxtextctrlMilliSecondsWait = new wxTextCtrl(this, wxID_ANY , //wxT("200")
      wxString::Format(
        //Use wxT() to enable to compile with both unicode and ANSI.
        wxT("%u") ,mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS )
    ) ;
  p_wxtextctrlCPUusageForThrottleToPstate1 = new wxTextCtrl(
    this
    , wxID_ANY 
    , wxT("0.7")
    , wxDefaultPosition 
    , wxDefaultSize
    , 0 //default style is 0
    , m_floatvalidator 
    ) ;
  sizerVert->Add( new wxStaticText( this, wxID_ANY,
    _T("threshold temperature in degrees Celsius for CPU core frequency throttling:") )
    ) ;
  sizerVert->Add( p_wxtextctrlThrottleTemp ) ;
  sizerVert->Add( new wxStaticText( this, wxID_ANY, 
    _T("milliseconds to wait till next temperature/ CPU core usage poll:") )
    ) ;
  sizerVert->Add( p_wxtextctrlMilliSecondsWait ) ;
  sizerVert->Add( mp_wxbuttonOK ) ;
  sizerVert->Add( mp_wxbuttonCancel) ;
  SetSizer(sizerVert);
}

void DynFreqScalingDlg::OnOK_Button(wxCommandEvent & r_wxcommandevent)
{
//  double dThrottleTemp ;
//  unsigned long ulMilliSeconds ;
  DEBUGN("DynFreqScalingDlg::OnOK_Button begin")
  //TODO uncomment
//  if(
//    p_wxtextctrlMilliSecondsWait->GetValue().ToULong( & ulMilliSeconds )
//    )
//  {
//    //m_floatvalidator.Validate(p_wxtextctrlMilliSecondsWait) ;
//    mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS = ulMilliSeconds ;
//  }
//  if(
//      p_wxtextctrlThrottleTemp->GetValue().ToDouble(& dThrottleTemp )
//    )
//  {
//    mp_cpucoredata->m_fThrottleTempInDegCelsius = dThrottleTemp ;
//  }
  DEBUGN("DynFreqScalingDlg::OnOK_Button end")
}
