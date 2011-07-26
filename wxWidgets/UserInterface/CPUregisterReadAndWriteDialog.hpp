/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   CPUregisterReadAndWriteDialog.hpp
 * Author: Stefan
 *
 * Created on 2. März 2010, 14:45
 */

#ifndef _CPUREGISTERWRITEDIALOG_HPP
#define	_CPUREGISTERWRITEDIALOG_HPP

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

#include <wx/dialog.h>
#include <vector>

//Forward declaration instead of include's(=faster)
class Model ;
class I_CPUcontroller ;
class MSRdata ;

class CPUregisterReadAndWriteDialog
  : public wxDialog
{
private:
  I_CPUcontroller * mp_cpucontroller ;
  Model * mp_modeldata ;
  MSRdata * mp_msrdata ;
  wxBoxSizer * mp_sizerAttributeNameAndValue ;
  wxBoxSizer * mp_sizerLeftColumn ;
  wxBoxSizer * mp_sizerRightColumn ;
  wxBoxSizer * mp_sizerVerticalOuter ;
  wxButton * mp_wxbuttonWriteMSR ;
  wxGridSizer * mp_wxgridsizerAttributeNameAndValue ;
  wxListBox * p_wxlistbox ;
  std::vector<wxTextCtrl * > m_stdvec_p_wxtextctrl ;
  unsigned long m_wNumIgnoreChanges ;
public:
  CPUregisterReadAndWriteDialog(
    wxWindow * parent ,
    //MSRdata & r_regdata ,
    Model & r_modeldata ,
    I_CPUcontroller * p_cpucontroller
    //I_CPUaccess * i_cpuaccess 
    );
  CPUregisterReadAndWriteDialog(const CPUregisterReadAndWriteDialog& orig);
  virtual ~CPUregisterReadAndWriteDialog();
  void ShowRegisterAttributes( //const
    MSRdata & r_msrdata ) ;
private:
  void OnChangedText(wxCommandEvent & evt ) ;
  void OnPreparePMC( wxCommandEvent & evt ) ;
  void OnReadFromMSR( wxCommandEvent & evt ) ;
  void OnRegisterListBoxSelection( wxCommandEvent & evt ) ;
  void OnWriteToMSR( wxCommandEvent & evt ) ;
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif	/* _CPUREGISTERWRITEDIALOG_HPP */

