// stdafx.h : Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//
#pragma once

//The "#ifdef _WINDOWS" to be compilable even under Linux has to be 
//here because if an "#ifdef ..." sourrounding "#include "stdafx.h"": 
//"fatal error C1020: Unerwartetes #endif" under MSVC++
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
 #ifndef __CYGWIN__

  #define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden.
  #include <stdio.h>
  //#ifdef WIN32
    #include <tchar.h>
  //#endif //#ifdef WIN32
 #endif
#endif //#ifdef _WINDOWS

// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
