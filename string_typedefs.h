#pragma once

//from winnt.h
typedef char CHAR;
#define CONST               const
typedef CONST CHAR *LPCSTR, *PCSTR;

#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

#ifdef  UNICODE
typedef CONST WCHAR *LPCWSTR, *PCWSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
#else
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;
#endif
//Keep 1 newline after this line to avoid g++ warning"no newline at end of file"
