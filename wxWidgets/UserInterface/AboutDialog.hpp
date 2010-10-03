/*
 * AboutDialog.hpp
 *
 *  Created on: Oct 3, 2010
 *      Author: Stefan
 */

#ifndef ABOUTDIALOG_HPP_
#define ABOUTDIALOG_HPP_

#include <wx/dialog.h> //for base class wxDialog

//Forward declarations (to  avoid including the wxWidgets header files HERE)
//class wxDialog ;
class wxString ;

void GetAboutMessage(wxString &) ;

class AboutDialog
  : public wxDialog
{
public:
  AboutDialog(const wxString & cr_wxstrProgramName );
  virtual
  ~AboutDialog();
};

#endif /* ABOUTDIALOG_HPP_ */
