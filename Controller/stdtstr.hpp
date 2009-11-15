#pragma once

#include <string>

namespace std {

     #if defined _UNICODE || defined UNICODE

     typedef wstring tstring;

     #else

     typedef string tstring;

    #endif

}
