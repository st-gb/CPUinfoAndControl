/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#ifdef __WXMSW__
//because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208: 
//   'void': Keine Elemente definiert, die diesen Typ verwenden
//http://trac.wxwidgets.org/ticket/11482:
//if you include <windows.h> you must include <wx/msw/winundef.h> after it.
#ifdef _MSC_VER //Microsoft compiler
#include <wx/msw/winundef.h>
#endif

//gc++ error: "C:\Libraries\wxWidgets-2.9.1\include/wx/thread.h:463:12:
// error: declaration does not declare anything"
//( at line "    static void Yield();" )
//#include <wx/thread.h> //for class wxCriticalSection

class wxMutex;

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
