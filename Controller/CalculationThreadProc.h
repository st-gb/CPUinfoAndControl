#pragma once //include guard

#ifdef _MSC_VER //MicroSoft C compiler
  //#include <Windef.h> // DWORD
  #include <Windows.h> //for DWORD, WINAPI etc.
  //#include <StdAfx.h>
  //#include "../StdAfx.h"
#else
  #include <global.h> //for WORD
#endif

DWORD WINAPI FPUcalculationThreadProc(LPVOID lpParameter) ;
DWORD WINAPI HighALUloadThreadProc(LPVOID lpParameter) ;
DWORD WINAPI CalculationThreadProc(LPVOID lpParameter) ;
