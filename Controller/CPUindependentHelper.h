/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
