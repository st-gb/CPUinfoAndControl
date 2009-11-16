/* 
 * File:   tchar.h
 * Author: Stefan
 *
 * Created on 6. August 2009, 21:29
 */
//This file is a replacement for MSVC's tchar.h
#ifndef _TCHAR_H
#define	_TCHAR_H

#ifdef _UNICODE
  #define _T(x) L ## x
  typedef wchar_t TCHAR ;
#else
  //#include <string.h> //for strrchr
  #define _T(x) x
  typedef char TCHAR ;
#endif

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

