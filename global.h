#pragma once
#ifndef GLOBAL_H
  #define GLOBAL_H

  //#include <stdio.h> //for "FILE *"
//  #include <string>
  //#define COMPILE_WITH_XERCES

  //For E.g. cygwin
  //#ifndef _WINDOWS

  //if NOT using MicroSoft C compiler (macro in Visual Studio )
  // (_tcscmp is already defined then-> redefined warning)
  #if !defined(_MSC_VER) && !defined(_TCHAR_H_)
    #define _W64
    //If NO MultiByteCharacterSet
    #ifndef _MBCS
      #define _tcscmp strcmp
      #define _tcslen strlen
      #define _tstoi atoi
    #endif
  #endif

  #ifndef _W64
    #define _W64
  #endif

  #ifndef _WINDEF_ //windef.h already defines type "WORD"
    #ifndef _MSC_VER //Compiler error if "windef.h" is included using MS compiler
      #include <windef.h>
    #endif
  //  typedef int BOOL;
  //  typedef unsigned char BYTE;
  //  typedef unsigned short WORD;
  //  typedef unsigned long DWORD;
  //  typedef DWORD * PDWORD;
    #ifndef _WINDOWS
      #include <Windows_compatible_typedefs.h>
    #endif
    //typedef _W64 unsigned long ULONG_PTR;
  //  typedef ULONG_PTR DWORD_PTR;
  #endif //_WINDEF
  //#ifndef WIN32
  //#ifndef _WINDOWS
  //#ifndef MS_COMPILER
  //#ifndef _MSC_VER //MicroSoft C compiler (macro in Visual Studio )
  //  #define _tcscmp strcmp
  //	typedef char _TCHAR ;
  //	typedef int BOOL;
  //	#define FALSE 0
  //	typedef DWORD * PDWORD;
  //  #ifndef _T
  ////  #define _T(x) (_TCHAR *)x
  //  //Compatible with preprocessor MACRO from >>wxDir<</include/wx/wxchar.h
  //    #define _T(x) x
  //  #endif
  //#endif

  #ifdef _DEBUG
    //#define _EMULATE_TURION_X2_ULTRA_ZM82
  #endif
  //#ifndef WORD
  //  #define WORD unsigned long
  //#endif
  //#include <windef.h> //for WORD

  //#define COMPILE_WITH_CPU_SCALING
  //#define PUBLIC_RELEASE
  //Uncomment the following line for performance reasons etc.

  #include <preprocessor_macros/logging_preprocessor_macros.h>

  #define SUCCESS 1
  #define FAILURE 0
  #define EXIT 2

  #define MAX_BIT_INDEX_FOR_UL 31

#endif //#ifndef GLOBAL_H
