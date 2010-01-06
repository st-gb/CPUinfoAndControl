#include "tchar_conversion.h"
#include <string> //(w)string

//LPCSTR GetCharPointer( LPCTSTR lptstr )
//{
//#ifdef UNICODE 
//  std::wstring stdwstr(lptstr) ;
//  std::string stdstr( stdwstr.begin() , stdwstr.end() ) ;
//  LPCSTR str = stdstr.c_str() ;
//  return str ;
//#else
//  return lptstr ;
//#endif
//}

//following function: wrong approach: when the string object is destructed, the pointer gets invalid.

//LPCSTR GetCharPointer( const WCHAR * wstr )
//{
//  std::wstring stdwstr(wstr) ;
//  std::string stdstr( stdwstr.begin() , stdwstr.end() ) ;
//  LPCSTR str = stdstr.c_str() ;
//  return str ;
//}
//
//LPCSTR GetCharPointer( const char * c_str )
//{
//  return c_str ;
//}
