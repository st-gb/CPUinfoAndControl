/* 
 * File:   Windows_compatible_typedefs.h
 * Author: sgebauer
 *
 * Created on 15. November 2009, 15:20
 */

#ifndef _WINDOWS_COMPATIBLE_TYPEDEFS_H
#define	_WINDOWS_COMPATIBLE_TYPEDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef __MSC_VER
  #if defined(X64) || defined(_M_X64 ) || defined(_WIN64 )
    #define __W64 _w64
  #else
    #define __W64 /*empty string*/    
  #endif
#endif

typedef int BOOL ;
#define FALSE 0
#ifndef TRUE //Avoid MSVC warning " warning C4005: 'TRUE': Macro-redefinition"
  #define TRUE !FALSE
#endif

////see winnt.h
//typedef unsigned long long ULONGLONG ;

//Especially for Linux gcc/g++; see MSVC's "basetsd.h"
//if compiled with cygwin? compiler:
//  "error: duplicate â€˜unsignedâ€™"
#if !defined( __CYGWIN__ ) && !defined(_MSC_VER) && (__MINGW32__==0)
  //with MinGW: "../../Windows_compatible_typedefs.h:32: error: 
  //redeclaration of C++ built-in type `long'"
  typedef __W64 unsigned long DWORD_PTR ;
#endif
typedef unsigned char BYTE ;
typedef unsigned short WORD ;
typedef unsigned long DWORD ;
typedef DWORD * PDWORD ;
typedef wchar_t WCHAR ;
#define WINAPI __stdcall
//MS compiler already has __int64 defined: 
//"error C2632: '__int64' followed by '__int64' is illegal"
#ifndef _MSC_VER 
  //typedef unsigned long long __int64 ;
#endif //#ifndef _MSC_VER
typedef void * LPVOID;
//typedef ULONG_MAX ;

////from winnt.h:
//#define _HAVE_INT64
//#if defined(_HAVE_INT64) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64)
//typedef __int64 LONGLONG;
//typedef unsigned __int64 DWORDLONG;
//#else
//typedef double LONGLONG,DWORDLONG;
//#endif


#ifdef	__cplusplus
}
#endif

#endif	/* _WINDOWS_COMPATIBLE_TYPEDEFS_H */

