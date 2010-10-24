/*
 * AboutDialog.cpp
 *
 *  Created on: Oct 3, 2010
 *      Author: Stefan
 */

#include <Controller/MainController.hpp>//MainController::GetSupportedCPUs(...)
#include <preprocessor_macros/BuildTimeString.h> //BUILT_TIME
#include <wxWidgets/UserInterface/AboutDialog.hpp>
//#include <wx/dialog.h> //for base class wxDialog
//#include <wx/stattext.h> //class wxStaticText
#include <wx/textctrl.h> //class wxTextCtrl
#include <wx/string.h> //class wxString

void GetAboutMessage(wxString & wxstrMessage)
{
  std::tstring stdtstr ;
  std::vector<std::tstring> stdvecstdtstring ;
//  wxString wxstrMessage ;
  MainController::GetSupportedCPUs(stdvecstdtstring) ;
  for(BYTE by = 0 ; by < stdvecstdtstring.size() ; by ++ )
  {
    stdtstr += _T("-") + stdvecstdtstring.at(by) + _T("\n") ;
  }
  if( stdtstr.empty() )
    wxstrMessage += _T("A tool for giving information about and controlling "
      "x86 CPUs");
  else
    wxstrMessage +=
      //We need a wxT()/ _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      _T("A tool for giving information about and controlling x86 CPUs ")
      wxT("(built-in):\n")
      + stdtstr +
      _T( "\n")
      _T("and for other CPUs as dynamic library") ;
  wxstrMessage += _T("\n\n")
    //"Build: " __DATE__ " " __TIME__ "GMT\n\n"
    BUILT_TIME
    //"AMD--smarter choice than Intel?\n\n"
    //"To ensure a stable operation:\n"
    _T("To give important information (that already may be contained in ")
    _T("the documentation):\n")
    _T("When undervolting there may be system instability when switching ")
    _T("from power supply operation to battery mode\n")
    _T("So test for this case if needed: test for the different p-states, ")
    _T("especially for the ones that are much undervolted.\n")
    _T("If the system is instable, heighten the voltage(s).\n")
    _T("Note that the OS may freeze if changing voltages, so you may ")
    _T("encounter data loss.\n->save all of your work before.\n\n")
    //" -when switching from power supply operation to battery,\n"
    //_T("Licence/ info: http://amd.goexchange.de / http://sw.goexchange.de")
    _T("Licence/ info: http://www.trilobyte-se.de/x86iandc")
    ;
}

AboutDialog::AboutDialog(const wxString & cr_wxstrProgramName )
  : wxDialog(
    NULL,
    wxID_ANY ,
    _T("About ") + cr_wxstrProgramName
    )
{
  wxString wxstrMessage ;
  GetAboutMessage(wxstrMessage) ;
  AddChild( new //wxStaticText(
    wxTextCtrl(
    this,
    wxID_ANY,
    wxstrMessage ,
    wxDefaultPosition,
    wxDefaultSize
#ifdef __WXGTK___
    //http://docs.wxwidgets.org/trunk/classwx_static_text.html
    // #fcba401f2915146b1ce25b90c9499ccb:
    //"This function allows to set decorated static label text, when the
    //wxST_MARKUP style is used, on those platforms which support it
    //(currently only GTK+ 2). For the other platforms or when wxST_MARKUP is
    //not used, the markup is ignored"
    ,wxST_MARKUP // to allow to select and copy text.
#endif
    , wxTE_READONLY | wxTE_MULTILINE
    )
    ) ;
//  SetSizeHints(this);
//  Fit(this);
  //Force the neighbour controls of "voltage in volt" to be resized.
  Layout() ;
}

AboutDialog::~AboutDialog()
{
  // TODO Auto-generated destructor stub
}
