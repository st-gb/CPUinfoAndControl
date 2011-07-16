/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#ifndef __X86I_AND_C_CRITICAL_SECTION_TYPE_HPP
  #define __X86I_AND_C_CRITICAL_SECTION_TYPE_HPP

//#ifdef __WXMSW__
//because of c:\wxwidgets-2.9.0\include\wx\thread.h(453): error C2208: 
//   'void': Keine Elemente definiert, die diesen Typ verwenden
//http://trac.wxwidgets.org/ticket/11482:
//if you include <windows.h> you must include <wx/msw/winundef.h> after it.

//#ifdef _MSC_VER //Microsoft compiler
  //wx/msw/winundef.h:267:54: error: missing binary operator before token "("
// (line: "   #if defined( __GNUG__ ) && !wxCHECK_W32API_VERSION( 0, 5 )" )
//  #include <wx/msw/gccpriv.h> //for wxCHECK_W32API_VERSION
//  #include <wx/msw/winundef.h>
//  #include <wx/version.h> //wxCHECK_W32API_VERSION(major, minor)
//  #include "wx/msw/winundef.h"
//#endif

//C:\Libraries\wxWidgets-2.9.1\include/wx/thread.h:463:12: error: declaration
//does not declare anything
// Windows headers define it
#ifdef Yield
    #undef Yield
#endif
#include <wx/thread.h> //for class wxCriticalSection

////Fwd. decl.
//class wxCriticalSection;

typedef wxCriticalSection criticalsection_type ;

//#else
//
//class I_CriticalSection
//{
//  public:
//  void Enter() {}
//  void Leave() {}
//} ;
//
//typedef I_CriticalSection criticalsection_type ;

//#endif //ifdef __WXMSW__
#endif //__X86I_AND_C_CRITICAL_SECTION_TYPE_HPP
