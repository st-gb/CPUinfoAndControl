#pragma once

#include <string>

namespace std {

     #if defined _UNICODE || defined UNICODE

     typedef std::wstring tstring;

     #else

     typedef std::string tstring;

    #endif

}
