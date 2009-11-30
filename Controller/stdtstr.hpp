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
