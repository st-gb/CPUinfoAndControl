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
//#include <Controller/stdtstr.hpp>

wxString getwxString(const std::string & str ) ;
//wxString getwxString(std::tstring & tstr ) ;
wxString getwxString(const std::wstring & stdwstr ) ;

std::string GetStdString(wxString & wxstr) ;

#endif	/* _WXSTRINGHELPER_H */

