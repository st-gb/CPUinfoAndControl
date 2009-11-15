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
  EVT_BUTTON(wxID_APPLY, DynFreqScalingDlg::OnApplyButton)
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
  , m_floatvalidator( & //m_fThrottleTemp 
    r_cpucoredata.m_fThrottleTemp )
  , mp_cpucoredata( & r_cpucoredata )
{
  wxBoxSizer * sizerVert = new wxBoxSizer(wxVERTICAL);
  mp_wxbuttonApply = new wxButton(this, wxID_APPLY ) ;
  p_wxtextctrlThrottleTemp = new wxTextCtrl(this, wxID_ANY ) ;
  p_wxtextctrlMilliSecondsWait = new wxTextCtrl(this, wxID_ANY , "200" ) ;
  p_wxtextctrlCPUusageForThrottleToPstate1 = new wxTextCtrl(
    this
    , wxID_ANY 
    , "0.7" 
    , wxDefaultPosition 
    , wxDefaultSize
    , 0 //default style is 0
    , m_floatvalidator 
    ) ;
  sizerVert->Add( p_wxtextctrlThrottleTemp ) ;
  sizerVert->Add( new wxStaticText( this, wxID_ANY, 
    _T("milliseconds to wait till next DVFS") ) 
    ) ;
  sizerVert->Add( p_wxtextctrlMilliSecondsWait ) ;
  sizerVert->Add( mp_wxbuttonApply ) ;
  SetSizer(sizerVert);
}

void DynFreqScalingDlg::OnApplyButton(wxCommandEvent & r_wxcommandevent)
{
  long lMilliSeconds ;
  if( 
    p_wxtextctrlMilliSecondsWait->GetValue().ToLong( & lMilliSeconds ) 
    )
  {
    //m_floatvalidator.Validate(p_wxtextctrlMilliSecondsWait) ;
    mp_cpucoredata->m_wMilliSecondsWaitBetweenDFVS = lMilliSeconds ;
  }
}
