//#include <wxWidgets/wxStringHelper.h>
#include "wxStringHelper.h"

wxString getwxString(const std::string & str )
{
//#ifdef wxUSE_WCHAR_T
//see wx/chartype.h:wxUSE_UNICODE_WCHAR->"typedef wchar_t wxStringCharType;"
#ifdef wxUSE_UNICODE_WCHAR 
  std::wstring wstr(str.begin(), str.end() ) ;
  wxString wxstr( wstr.c_str() ) ;
#else
  wxString wxstr(( const unsigned char * ) str.c_str() ) ;
#endif
 return  wxstr ;
}

//wxString getwxString(std::tstring & tstr )
//{
//#ifdef wxUSE_UNICODE_WCHAR
//  std::wstring wstr(str.begin(), str.end() ) ;
//  wxString wxstr( wstr.c_str() ) ;
//#else
//  wxString wxstr(( const unsigned char * ) tstr.c_str() ) ;
//#endif
//}

wxString getwxString(const std::wstring & stdwstr )
{
//#ifdef wxUSE_WCHAR_T
//see wx/chartype.h:wxUSE_UNICODE_WCHAR->"typedef wchar_t wxStringCharType;"
#ifdef wxUSE_UNICODE_WCHAR 
  //std::wstring wstr(str.begin(), str.end() ) ;
  wxString wxstr( stdwstr.c_str() ) ;
#else
  std::string stdstr(stdwstr.begin(), stdwstr.end() ) ;
  wxString wxstr(( const unsigned char * ) stdstr.c_str() ) ;
#endif
 return  wxstr ;
}

std::string getstdstring(wxString & wxstr)
{
//#ifdef wxUSE_WCHAR_T
//see wx/chartype.h:wxUSE_UNICODE_WCHAR->"typedef wchar_t wxStringCharType;"
#ifdef wxUSE_UNICODE_WCHAR 
  std::wstring wstr( //(wchar_t *) //wxstr.c_str().AsWChar()
    //wxstr.c_str()
    //wc_str() returns wchar_t no matter if wxString uses "char" or "wchar_t".
    wxstr.wc_str() 
    ) ;
  std::string str(wstr.begin(), wstr.end() ) ;
#else
  std::string str(( const char * ) wxstr.c_str() ) ;
#endif
 return str ;
}
