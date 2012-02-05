/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
class ModelData;
//class wxDialog ;
class wxString ;

void GetAboutMessage(wxString &) ;

class AboutDialog
  : public wxDialog
{
  const Model & m_c_r_model_data;
public:
  AboutDialog(const Model & c_r_model_data,
    const wxString & cr_wxstrProgramName );
  virtual
  ~AboutDialog();
};

#endif /* ABOUTDIALOG_HPP_ */
