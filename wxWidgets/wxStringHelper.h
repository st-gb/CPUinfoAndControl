/* 
 * File:   wxStringHelper.h
 * Author: sgebauer
 *
 * Created on 15. November 2009, 18:53
 */

#ifndef _WXSTRINGHELPER_H
#define	_WXSTRINGHELPER_H

#include <string>
#include <wx/string.h> //wxString

wxString getwxString(std::string & str ) ;

std::string getstdstring(wxString & wxstr) ;

#endif	/* _WXSTRINGHELPER_H */

