/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once /* Include guard. */

/** Currently the wxWidgets type of condition class even for the service
 *  is used because the only dynamic library handler code which has more than 
 *  1200 lines of code also uses wxWidgets.
 *  So it can be exchanged if there is a native dynamic handler class. */
#define CONDITION_TYPE_WXWIDGETS

#ifdef COMPILE_AS_GUI
 /** http://en.wikipedia.org/wiki/C_preprocessor#Conditional_compilation:
  * "Note that comparison operations only work with integers" */
 #define CONDITION_TYPE WXWIDGETS
#else //#ifdef COMPILE_AS_GUI
 #ifdef _WIN32 //built-in define for MinGW/ eclipse CDT on Windows
  #define CONDITION_TYPE_WIN32
 #endif
 /** see https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
  * built-in define for gcc under Linux */
 #ifdef __linux__
//  #define CONDITION_TYPE_PTHREAD
 #endif
#endif //#ifdef COMPILE_AS_GUI
#ifdef _WINDOWS
//  #define CONDITION_TYPE_WIN32
#endif

#if CONDITION_TYPE == WX

#endif

#ifdef CONDITION_TYPE_WXWIDGETS
  //#if CONDITION_TYPE == WXWIDGETS
  #ifdef __WXMSW__
    //because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208:
    //   'void': Keine Elemente definiert, die diesen Typ verwenden
    //http://trac.wxwidgets.org/ticket/11482:
    //if you include <windows.h> you must include <wx/msw/winundef.h> after it.
    #ifdef _MSC_VER //Microsoft compiler
      #include <wx/msw/winundef.h>
    #endif //_MSC_VER

//    #include <wx/thread.h> //for class wxCondition
    //typedef wxCondition condition_type ;

  //#else
  #endif //ifdef __WXMSW__

  #include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>
  typedef wxConditionBasedI_Condition condition_type ;

#endif //#if CONDITION_TYPE == WXWIDGETS

#ifdef CONDITION_TYPE_WIN32
//#if CONDITION_TYPE == WINDOWS
  #include <Windows/multithread/Win32EventBasedCondition.hpp>
//#endif
  typedef Win32EventBasedCondition condition_type ;

//typedef I_Condition condition_type ;

//#endif //ifdef __WXMSW__
#endif //#ifdef CONDITION_TYPE_WIN32

#ifdef CONDITION_TYPE_PTHREAD
  #include <OperatingSystem/POSIX/multithread/Event.hpp>
  typedef pthread::Condition condition_type;
#endif
