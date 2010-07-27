#pragma once

#ifdef __WXMSW__
//because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208: 
//   'void': Keine Elemente definiert, die diesen Typ verwenden
//http://trac.wxwidgets.org/ticket/11482:
//if you include <windows.h> you must include <wx/msw/winundef.h> after it.
#ifdef _MSC_VER //Microsoft compiler
#include <wx/msw/winundef.h>
#endif
#include <wx/thread.h> //for class wxCriticalSection

typedef wxMutex mutex_type ;

#else

class I_Mutex
{
  public:
  void Lock() {}
  void Unlock() {}
} ;

typedef I_Mutex mutex_type ;

#endif //ifdef __WXMSW__
