#pragma once

//faster than to include the whole "Windows.h". 
//Additionally this is more portable to Linux.
#include <string_typedefs.h>

//LPCSTR GetCharPointer( LPCTSTR lptstr ) ;
//following function: wrong approach: when the string object is destructed, the pointer gets invalid.
//LPCSTR GetCharPointer( const WCHAR * wstr ) ;
//LPCSTR GetCharPointer( const char * str ) ;
