#pragma once //include guard

#ifdef _MSC_VER //MicroSoft C compiler
  //#include <Windef.h> // DWORD
  #include <Windows.h> //for DWORD, WINAPI etc.
  //#include <StdAfx.h>
  //#include "../StdAfx.h"
#else
  #include <global.h> //for WORD
#endif
#include <Windows_compatible_typedefs.h>

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
