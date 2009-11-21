#include <wxWidgets/wxStringHelper.h>

wxString getwxString(std::string & str )
{
#ifdef wxUSE_WCHAR_T
  std::wstring wstr(str.begin(), str.end() ) ;
  wxString wxstr( wstr.c_str() ) ;
#else
  wxString wxstr(( const unsigned char * ) str.c_str() ) :
#endif
 return  wxstr ;
}

std::string getstdstring(wxString & wxstr)
{
#ifdef wxUSE_WCHAR_T
  std::wstring wstr( (wchar_t *) wxstr.c_str() ) ;
  std::string str(wstr.begin(), wstr.end() ) ;
#else
  std::string str(( const unsigned char * ) wxstr.c_str() ) ;
#endif
 return str ;
}
