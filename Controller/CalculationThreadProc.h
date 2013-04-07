/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#ifdef _MSC_VER //MicroSoft C compiler
  //#include <Windef.h> // DWORD
  #include <Windows.h> //for DWORD, WINAPI etc.
  //#include <StdAfx.h>
  //#include "../StdAfx.h"
#else
//  #include <global.h> //for WORD
#endif
//#include <preprocessor_macros/Windows_compatible_typedefs.h>
#include <windef.h> //for DWORD

//#define STARTED 0
//#define ENDED 0

DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
 FindDifferentPstatesThreadProc(LPVOID lpParameter) ;
DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
 FPUcalculationThreadProc(LPVOID lpParameter) ;
DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
 HighALUloadThreadProc(LPVOID lpParameter) ;
DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
  CalculationThreadProc(LPVOID lpParameter) ;
