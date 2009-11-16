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

wxString getwxString(std::string & str )
{
#ifdef wxUSE_WCHAR_T
  std::wstring wstr(str.begin(), str.end() ) ;
  wxString wxstr( wstr) ;
#else
  wxString wxstr(( const unsigned char * ) str.c_str() ) :
#endif
 return  wxstr ;
}

std::string getstdstring(wxString & wxstr)
{
#ifdef wxUSE_WCHAR_T
  std::wstring wstr(wxstr.c_str() ) ;
  std::string str(wstr.begin(), wstr.end() ) ;
#else
  std::string str(( const unsigned char * ) wxstr.c_str() ) ;
#endif
 return str ;
}

#endif	/* _WXSTRINGHELPER_H */

