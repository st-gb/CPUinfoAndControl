/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * AboutDialog.cpp
 *
 *  Created on: Oct 3, 2010
 *      Author: Stefan
 */

#include <Controller/MainController.hpp>//MainController::GetSupportedCPUs(...)
#include <preprocessor_macros/BuildTimeString.h> //BUILT_TIME
#include <wxWidgets/UserInterface/AboutDialog.hpp>
#include <wx/bitmap.h> //class wxBitmap
//#include <wx/dialog.h> //for base class wxDialog
//#include <wx/stattext.h> //class wxStaticText
#include <wx/textctrl.h> //class wxTextCtrl
#include <wx/statbmp.h> //class wxStaticBitmap
#include <wx/string.h> //class wxString
#include <wx/sizer.h> //class wxBoxSizer
//DISable warning, from
//http://stackoverflow.com/questions/59670/how-to-get-rid-of-deprecated-conversion-from-string-constant-to-char-warning
// : "I believe passing -Wno-write-strings to gcc will suppress this warning."
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include <images/street_lamp_80x321_256_indexed_colors.xpm>
//ENable warning
#pragma GCC diagnostic warning "-Wwrite-strings"

//Lebensweisheiten:
wxString g_ar_wxstrWorldlyWisdom [] = {
  wxT("Happiness is only real when shared--Alexander Supertramp") ,
  wxT("People (even criminal) usually are the result of the interaction "
    "between them and their environment")
};
WORD g_wNumberOfWorldlyWisdomStrings = 2;

wxString GetRandomWorldlyWisdom()
{
  //from http://www.cplusplus.com/reference/clibrary/cstdlib/rand/:
  /* initialize random seed: */
  srand ( time(NULL) );

  /* generate secret number: */
  int nRandomNumber = rand() % g_wNumberOfWorldlyWisdomStrings;
  return g_ar_wxstrWorldlyWisdom[nRandomNumber];
}

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
    wxT("\n\n")
    + GetRandomWorldlyWisdom()
    ;
}

AboutDialog::AboutDialog(const wxString & cr_wxstrProgramName )
  : wxDialog(
    NULL,
    wxID_ANY ,
    _T("About ") + cr_wxstrProgramName,
    wxDefaultPosition,
//    wxDefaultSize,
    wxSize(600, 400),
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
{
  wxString wxstrMessage ;
  GetAboutMessage(wxstrMessage) ;

  wxBoxSizer * p_wxboxsizer = new wxBoxSizer(wxHORIZONTAL) ;

  wxTextCtrl * p_wxtextctrl = new //wxStaticText(
      wxTextCtrl(
      this,
//      NULL,
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
      ,
      //prevent editing the text
      wxTE_READONLY
      | wxTE_MULTILINE
    );
//  AddChild( p_wxtextctrl) ;
  p_wxboxsizer->Add(p_wxtextctrl,
    1 //1=the control should take more space if the sizer is enlarged
    , wxEXPAND
    );
  wxBitmap wxbitmapStreetLamp(street_lamp_80x321_256_indexed_colors_xpm);
//  wxPanel * p_wxpanel = new wxPanel();
//  p_wxpanel->SetB
  //from http://ubuntuforums.org/archive/index.php/t-1486783.html
  //("[SOLVED] [C++/wx] wxStaticBitmap->SetBitmap(wxBitmap) = "Segmentation fault")
  wxStaticBitmap * p_wxstaticbitmap = new wxStaticBitmap//();
      (this, wxID_ANY, wxbitmapStreetLamp);
  p_wxstaticbitmap->SetToolTip( wxT("a street lamp from former GDR "
    "symbolizing East Berlin"));
//  p_wxstaticbitmap->SetBitmap(wxbitmapStreetLamp);
  p_wxboxsizer->Add( p_wxstaticbitmap);
  SetSizer(p_wxboxsizer);
//  AddChild(p_wxstaticbitmap);
//  SetSizeHints(this);
//  Fit(this);
  //Force the neighbour controls of "voltage in volt" to be resized.
  Layout() ;
}

AboutDialog::~AboutDialog()
{
  // TODO Auto-generated destructor stub
}
