/* 
 * File:   tchar.h
 * Author: Stefan
 *
 * Created on 6. August 2009, 21:29
 */
//This file is a replacement for MSVC's tchar.h
#ifndef _TCHAR_H
#define	_TCHAR_H

#if defined( _UNICODE) || defined(UNICODE)
  #ifndef _T //e.g. wxWidgets also defines _T() macro
    #define _T(x) L ## x
  #endif
  typedef wchar_t TCHAR ;
#ifdef _MSC_VER //MS compiler
  #pragma  message( "using wchar_t for TCHAR" )
#endif
#else
  //#include <string.h> //for strrchr
  #ifndef _T
    #define _T(x) x
  #endif
  typedef char TCHAR ;
#ifdef _MSC_VER //MicroSoft compiler
  #pragma  message( "using char for TCHAR" )
#endif
#endif
typedef TCHAR * LPTSTR ;
typedef TCHAR _TCHAR ;

#ifdef	__cplusplus
extern "C" {
#endif

    #if defined(_UNICODE)
        #if !defined(_MBCS)
        #endif
    #else
        //#ifndef _MBCS
            #define _tcsncpy strncpy
            #define csrchr strrchr
            #define _tcsrchr strrchr
            #define _tcstoul    strtoul
        //#endif
    #endif


#ifdef	__cplusplus
}
#endif

#endif	/* _TCHAR_H */

