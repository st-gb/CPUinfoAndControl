#pragma once

#include <string>
//#include <string_typedefs.h> //LPCTSTR

//char * GetCharPointer( LPCTSTR pstr )
//{
//#ifdef UNICODE
//  std::wstring wstr ( pstr) ;
//  std::string str( wstr.begin(), wstr.end() ) ;
//  return str ;
//#else
//  return pstr ;
//#endif
//}

namespace std {

     #if defined _UNICODE || defined UNICODE

     typedef std::wstring tstring;

     #else

     typedef std::string tstring;

    #endif

}

 #if defined _UNICODE || defined UNICODE
 std::tstring operator + ( const std::tstring & r_tstring, const std::string & r_string) ;
 #endif

 std::tstring Getstdtstring(const std::string & str ) ;
 std::tstring Getstdtstring(const std::wstring & wstr ) ;
 std::wstring GetStdWstring( const std::tstring & cr_stdtstr ) ;
 std::string GetStdString(const std::string & cr_str ) ;
 std::string GetStdString(const std::wstring & cr_wstr ) ;
