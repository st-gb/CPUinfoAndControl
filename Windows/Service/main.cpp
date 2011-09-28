/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//Else: "fatal error C1189: #error :  Building MFC application with /MD[d]
// (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or
// do not use /MD[d]"
//#define _AFXDLL
//#include <AfxWin.h> //for AfxMessageBox(...)->muss MultiThreaded DLL ("/MD")
// sein
//for int ::CallFromMainFunction(int argc, char * argv[])
#include <Windows/Service/CallFromMainFunction.h>

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

//GetTCHARarray_Inline(...)
#include <Controller/character_string/stdtstr.hpp>

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main
  //_tmain
  ( int argc, char * ar_p_chProgramArguments[]
  //TCHAR * ar_p_chProgramArguments[]
  )
{
  TCHAR ** ar_tchProgramArguments = ::GetTCHARarray_Inline(
    (const char **) ar_p_chProgramArguments, argc);
  ::OpenLogFile(//ar_p_chProgramArguments
    ar_tchProgramArguments);
  int n = ::CallFromMainFunction(argc, //ar_p_chProgramArguments
      ar_tchProgramArguments, NULL);
  if( ar_tchProgramArguments != ar_p_chProgramArguments)
    ::DeleteTCHARarray(argc, ar_tchProgramArguments);
  return n;
}
