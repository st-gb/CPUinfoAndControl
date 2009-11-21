#pragma once

//faster than to include the whole "Windows.h". 
//Additionally this is more portable to Linux.
#include <string_typedefs.h>

LPCSTR GetCharPointer( LPCTSTR lptstr ) ;
