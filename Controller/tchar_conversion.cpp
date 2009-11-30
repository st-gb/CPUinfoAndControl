#include "tchar_conversion.h"
#include <string> //(w)string

LPCSTR GetCharPointer( LPCTSTR lptstr )
{
#ifdef UNICODE 
  std::wstring stdwstr(lptstr) ;
  std::string stdstr( stdwstr.begin() , stdwstr.end() ) ;
  LPCSTR str = stdstr.c_str() ;
  return str ;
#else
  return lptstr ;
#endif
}

LPCSTR GetCharPointer( const WCHAR * wstr )
{
  std::wstring stdwstr(wstr) ;
  std::string stdstr( stdwstr.begin() , stdwstr.end() ) ;
  LPCSTR str = stdstr.c_str() ;
  return str ;
}
