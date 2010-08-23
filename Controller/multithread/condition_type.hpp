#pragma once

#define CONDITION_TYPE_WXWIDGETS
#ifdef _WINDOWS
//  #define CONDITION_TYPE_WIN32
#endif
//http://en.wikipedia.org/wiki/C_preprocessor#Conditional_compilation:
//"Note that comparison operations only work with integers"
#define CONDITION_TYPE WXWIDGETS

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
