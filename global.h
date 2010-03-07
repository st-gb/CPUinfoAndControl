#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h> //for "FILE *"
#include <string>
//#define COMPILE_WITH_XERCES

//For E.g. cygwin
//#ifndef _WINDOWS
#ifndef _MSC_VER //MicroSoft C compiler (macro in Visual Studio )
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
  #ifdef __CYGWIN__
    //Compiler error if "windef.h" is included using MS compiler
    #include <windef.h>
  #endif
//  typedef int BOOL;
//  typedef unsigned char BYTE;
//  typedef unsigned short WORD;
//  typedef unsigned long DWORD;
//  typedef DWORD * PDWORD;
  #include <Windows_compatible_typedefs.h>
  //typedef _W64 unsigned long ULONG_PTR;
//  typedef ULONG_PTR DWORD_PTR;
#endif
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

//#define _EMULATE_TURION_X2_ULTRA_ZM82
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
#define COMPILE_WITH_DEBUG
  extern FILE * fileDebug ;
  //FILE * fileDebug;
#ifdef COMPILE_WITH_DEBUG
  //FILE * fileDebug = fopen("PumaStateCtrl_debug.txt","r");
  /*Use a block ( "{...}" to be usable in "else" statements.*/
  //#define DEBUG(...)  {fprintf(fileDebug,__VA_ARGS__);fflush(/*stdout*/fileDebug);}
  #include "./Controller/Logger.hpp" //for class Logger
  //class Logger ;
  //#ifndef _MSC_VER //else compile errors with gcc
  #include <sstream> //for class std::stringstream
  //#endif
  extern Logger g_logger ;
  #define LOG(to_ostream) { std::stringstream strstream ; \
    strstream << to_ostream; \
    /*/for g++ compiler:
    //Because I want to call Log( std::string & ) I have to create an object at
    //first*/ \
    std::string stdstr = strstream.str() ;\
    /*g_logger->Log(to_ostream) ; */ \
    g_logger.Log( stdstr ) ; \
    /*g_logger.Log("test ") ; */ }
  //#ifdef COMPILE_WITH_LOG
  #define LOGN(to_ostream) LOG (to_ostream << "\n" )
#ifdef VERBOSE_LOGGING
  #define LOGN_VERBOSE(to_ostream) LOGN(to_ostream)
#else
  #define LOGN_VERBOSE(to_ostream) /*empty->do not log*/
#endif
#if defined(__MINGW32__) //MinGW does not know wide strings
  #define LOGW(to_ostream) /*empty->do not log*/
  #define LOGWN(to_ostream) /*empty->do not log*/
#else
  #define LOGW(to_ostream) { std::wstringstream strstream ; \
    strstream << to_ostream; \
    /*/for g++ compiler:
    //Because I want to call Log( std::string & ) I have to create an object at
    //first*/ \
    std::wstring stdwstr = strstream.str() ;\
    std::string stdstr ( stdwstr.begin(), stdwstr.end() ) ;\
    /*g_logger->Log(to_ostream) ; */ \
    /*g_logger.LogW( stdwstr ) ; */ \
    g_logger.Log( stdstr ) ; \
    /*g_logger.Log("test ") ; */ }
  #define LOGWN(to_ostream) LOGW (to_ostream << L"\n" )
#endif
  //#define DEBUG(...) { g_logger->Log(__VA_ARGS__) ; }
  #define DEBUG(to_ostream) LOG(to_ostream)

  #define CPP_WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(coutArgs) { \
    std::cout coutArgs ; std::cout.flush(); \
    g_ofstream coutArgs ; g_ofstream.flush(); }
  //#define WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(...) { \
  //  fprintf(fileDebug,__VA_ARGS__); fflush(fileDebug); \
  //  printf(__VA_ARGS__); fflush(stdout); }
  //#ifndef _MSC_VER //else compile errors with gcc
  #include <iostream> //for cout
  //#endif
  #define WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(to_ostream) { \
  LOG(to_ostream << std::endl; ); \
    std::cout << to_ostream << std::endl ; std::cout.flush(); }
  //this macro may NOT be called like the other one with "..."/ "__VA_ARGS__",
  //else many errors
  #define DEBUG_COUT(coutArgs)  {std::cout coutArgs ; std::cout.flush(); }
#else
	#define DEBUG(...) //->empty
  #define DEBUG(coutArgs) //->empty
#endif//#ifdef COMPILE_WITH_DEBUG

#define SUCCESS 1
#define FAILURE 0
#define EXIT 2

#define MAX_BIT_INDEX_FOR_UL 31

#endif
