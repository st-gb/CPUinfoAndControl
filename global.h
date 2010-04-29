#pragma once
#ifndef GLOBAL_H
  #define GLOBAL_H

  //#include <stdio.h> //for "FILE *"
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

  //Keep away the dependency on Logger class for dyn libs.
  #if defined(COMPILE_FOR_CPUCONTROLLER_DYNLIB) //&& !defined(_DEBUG)
  //  #define LOG(...) /* ->empty/ no instructions*/
  //  #else
    #ifdef _DEBUG //only debug DLLs should log
      #define COMPILE_WITH_LOG
      #define COMPILE_WITH_DEBUG
    #endif
  #else //#if defined(COMPILE_FOR_CPUCONTROLLER_DYNLIB) && !defined(_DEBUG)
    //Even release version of the service / GUI should output to the log file.
    #define COMPILE_WITH_LOG
    #if defined(_DEBUG)
      #define COMPILE_WITH_DEBUG
    #endif
  //  extern FILE * fileDebug ;
    //FILE * fileDebug;
  #endif//#if defined(COMPILE_FOR_CPUCONTROLLER_DYNLIB) && !defined(_DEBUG)

  #ifdef _DEBUG
    //this macro may NOT be called like the other one with "..."/ "__VA_ARGS__",
    //else many errors
    #define DEBUG_COUT(coutArgs)  {std::cout << coutArgs ; std::cout.flush(); }
    #define DEBUG_COUTN(coutArgs) { std::cout << coutArgs << "\n" ; \
      std::cout.flush(); }
    #define DEBUG_WCOUTN(coutArgs) { std::wcout << coutArgs << L"\n" ; \
      std::cout.flush(); }
    //MinGW is able to use wprintf() but not std::wcout()
    #define DEBUG_WPRINTFN(...) { wprintf(__VA_ARGS__) ; wprintf(L"\n"); \
      fflush(stdout) ; }
  #else
    #define DEBUG(...) //->empty
    //#define DEBUG(coutArgs) //->empty
    //#define DEBUG(to_ostream) /*->empty / no instructions*/
    #define DEBUGN(to_ostream) /*->empty / no instructions*/
    #define DEBUGWN(to_ostream) /*->empty / no instructions*/
    #define DEBUG_COUT(coutArgs) //->empty
    #define DEBUG_COUTN(coutArgs) //->empty
    #define DEBUG_WCOUTN(coutArgs) //->empty
    #define DEBUG_WPRINTFN(...) //->empty
  #endif //#ifdef _DEBUG

#ifdef COMPILE_WITH_LOG
    //FILE * fileDebug = fopen("x86IandC_debug.txt","r");
    /*Use a block ( "{...}" to be usable in "else" statements.*/
    //#define DEBUG(...)  {fprintf(fileDebug,__VA_ARGS__);fflush(/*stdout*/fileDebug);}
    #include "./Controller/Logger.hpp" //for class Logger
    //class Logger ;
    //#ifndef _MSC_VER //else compile errors with gcc
    #include <sstream> //for class std::stringstream
    //#endif
    extern Logger g_logger ;
    //LOGxx macros: should log no matter whether release or debug
    #define LOG(to_ostream) { std::stringstream strstream ; \
      strstream << to_ostream; \
      /*/for g++ compiler:
      //Because I want to call Log( std::string & ) I have to create an object at
      //first*/ \
      std::string stdstr = strstream.str() ;\
      /*g_logger->Log(to_ostream) ; */ \
      g_logger.Log( stdstr ) ; \
      /*g_logger.Log("test ") ; */ }
//    #endif
    //#ifdef COMPILE_WITH_LOG
    #define LOGN(to_ostream) LOG (to_ostream << "\n" )
    #if defined(__MINGW32__) //MinGW does not know std::wstringstream
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
    #endif //#if defined(__MINGW32__)

    //for outputting wide strings also with MinGW:
//    #define LOG_VIA_WSPRINTFN(...) { wchar_t ar_wch[1024] ; \
//      wwsprintfW(ar_wch,__VA_ARGS__) ;

    #include <iostream> //for std::cout
    //#endif
    #define WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(to_ostream) { \
    LOG(to_ostream << std::endl; ); \
      std::cout << to_ostream << std::endl ; std::cout.flush(); }

  //The DEBUGxx() macros that use / depend on LOGxx(), should be inside the
  //   #ifdef COMPILE_WITH_LOG
  #ifdef COMPILE_WITH_DEBUG

    #ifdef _DEBUG
      //#define DEBUG(...) { g_logger->Log(__VA_ARGS__) ; }
      //This macro should only be expanded to log outputs on debug versions.
      #define DEBUG(to_ostream) LOG(to_ostream)
      #define DEBUGN(to_ostream) LOGN(to_ostream)
      #define DEBUGWN(to_ostream) LOGWN(to_ostream)
    #else
    #endif //#ifdef _DEBUG
    #define CPP_WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(coutArgs) { \
      std::cout coutArgs ; std::cout.flush(); \
      g_ofstream coutArgs ; g_ofstream.flush(); }
    //Use C comment, else compiler warning: multi-line comment because of "\" at
    // line end.
    /*#define WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(...) { \
    //  fprintf(fileDebug,__VA_ARGS__); fflush(fileDebug); \
    //  printf(__VA_ARGS__); fflush(stdout); } */
    //#ifndef _MSC_VER //else compile errors with gcc

  #else
  #endif//#ifdef COMPILE_WITH_DEBUG
#else //#ifdef COMPILE_WITH_LOG
  #define LOG(to_ostream) /*empty->do not log*/
  #define LOGN(to_ostream) /*empty->do not log*/
  #define LOGW(to_ostream) /*empty->do not log*/
  #define LOGWN(to_ostream) /*empty->do not log*/
#endif //#ifdef COMPILE_WITH_LOG
#ifdef VERBOSE_LOGGING
  #define LOGN_VERBOSE(to_ostream) LOGN(to_ostream)
#else
  #define LOGN_VERBOSE(to_ostream) /*empty->do not log*/
#endif

  #define SUCCESS 1
  #define FAILURE 0
  #define EXIT 2

  #define MAX_BIT_INDEX_FOR_UL 31

#endif //#ifndef GLOBAL_H
