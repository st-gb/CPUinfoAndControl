/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

//#include <winbase.h> //for Sleep(DWORD) ;
#ifdef __CYGWIN__
  #include <unistd.h>
  #define SLEEP_1_MILLI_SECOND usleep(1000);
#else
  #include <windows.h> //for Sleep(DWORD) ;
  #define SLEEP_1_MILLI_SECOND Sleep(1);
#endif

#ifdef LINK_TO_WINRING0_STATICALLY
  //#include <windef.h> //for  WINAPI etc. in OlsApi.h
  //#include <WINDOWS.H> //for  WINAPI in OlsApi.h
  ////#define WINAPI      __stdcall
  //#include <OlsApi.h>
#endif

//helper function
//  void * GetHandleToDLLFunction(const std::string & strFuncName)
//  {
//    void * pfn ;
//    pfn = (void *) GetProcAddress(m_hinstanceWinRingDLL,strFuncName.c_str() );
//    return pfn ;
//  }
