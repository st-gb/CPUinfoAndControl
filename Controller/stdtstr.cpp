#include "stdtstr.hpp"

   #if defined _UNICODE || defined UNICODE

   std::tstring operator + (
    const std::tstring & r_tstring,
     const std::string & r_string
    )
   {
     std::tstring tstr = r_tstring + std::wstring( r_string.begin(),
       r_string.end() ) ;
     return tstr ;
   }
   #endif
