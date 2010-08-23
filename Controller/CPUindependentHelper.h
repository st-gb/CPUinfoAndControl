#pragma once 

////#include <yvals.h>
//#include <stdlib.h>
//#include "stdafx.h"
//#include "global.h"
#ifdef _MSC_VER //MicroSoft C compiler
  #include <Windef.h> // DWORD
  //#include <Windows.h> //for DWORD, WINAPI etc.
  //#include <StdAfx.h>
  //#include "../StdAfx.h"
#endif
#include <string>

BYTE GetCoreIDFromAffinityMask(DWORD dwAffinityMask ) ;
