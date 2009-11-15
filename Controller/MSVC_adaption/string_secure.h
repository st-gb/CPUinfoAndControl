/* 
 * File:   crtdbg.h
 * Author: Stefan
 *
 * Created on 4. April 2009, 11:38
 */

#ifndef _STRING_SECURE_H
#define	_STRING_SECURE_H


#include <string.h>
#include <stdarg.h> //for va_list, for vsprintf_s under Visual Studio 2005
#include <stdio.h> //for sprintf(), for vsprintf_s under Visual Studio 2005

#ifdef __CYGWIN__
#ifdef	__cplusplus
extern "C" {
#endif

    #define _tcslen strlen
  //The following functions are wrapper for the "..._s" (">>s<<ecure" funcktions)
  //that do not exist on cygwin (and including MSVC's header files when also
  //including cygwin header files messes all up).

  //Emulates "strcpy_s" of Visual Studio
  //errno_t
  void strcpy_s(
    char * p_chDest
    , int   nStringLenInclTerminating0Char
    , const char * cp_chString ) ;
//  {
//    strcpy(p_chDest, cp_chString ) ;
//  }

  int sprintf_s(
    char * p_chBuffer
    , int nSize
    , const char * cp_chFormat //e.g. "%s->%s"
    , ... //arg_list //const char * cp_
    ) ;
//  {
//    va_list argzeiger;
//    va_start(argzeiger,cp_chFormat);
//    return sprintf(p_chBuffer, cp_chFormat,
//      //va_list
//      argzeiger ) ;
//  }

  int sscanf_s(
    char   *      p_chBuffer,
    const char * cp_chFormat //e.g. "%d.%d.%d %d:%d:%d",
    , ... ) ;
//  {
//    va_list argzeiger;
//    va_start(argzeiger,cp_chFormat);
//    return sscanf(p_chBuffer, cp_chFormat,
//      argzeiger ) ;
//  }

  int strncpy_s(
    char * p_chBuffer
    , size_t sizeDestStringInChars//cntCmp+1
    , const char * cp_chSourceStr//, n
    , size_t sizeNumCharsToCopy//, cntCmp
    ) ;
//  {
//    strncpy(p_chBuffer, cp_chSourceStr ,sizeNumCharsToCopy ) ;
//    return 0 ;
//  }

  int vsprintf_s(
    char   *      p_chBuffer
    , size_t sizeDestStringInChars
    , const char * cp_chFormat //e.g. "%d.%d.%d %d:%d:%d",
    //, ...
    , va_list argptr
    ) ;
//  {
//    //returns the number of chars written withou term. 0 char or negative value
//    //on error.
//    int nvsprintfRet = vsprintf(p_chBuffer, cp_chFormat, argptr) ;
//    //if( nvsprintfRet <= sizeDestStringInChars && nvsprintfRet >= 0 )
//    return nvsprintfRet ;
//  }

#ifdef	__cplusplus
}
#endif
#endif //#ifdef __CYGWIN__

#endif	/* _STRING_SECURE_H */

