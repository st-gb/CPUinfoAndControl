#if !defined(ERRORCODEFROMGETLASTERRORTOSTRING_H__INCLUDED_)
#define ERRORCODEFROMGETLASTERRORTOSTRING_H__INCLUDED_

#ifdef _MSC_VER //if MicroSoft-Compiler
    //damit nicht der Compiler-Fehler C2146 auftritt
    //#include <Afx.h>
#endif
//#include <windef.h> //for DWORD
#include <string>

typedef unsigned long DWORD ;
//diese Funktion liefert die zum DWORD-(unsigned long-) Wert 
//der Windows-API-Funktion "DWORD GetLastError(void)" 
//korrepsondierende Fehlermeldung als Text in der Standard-
//Sprache der laufenden Windows-Sitzung
//CString 
DWORD GetLastErrorMessageString(
  DWORD dwErrorCodeConformToGetLastError ,
  std::string & strErrorMessage
  );
std::string GetLastErrorMessageString(
  DWORD dwErrorCodeConformToGetLastError
  //std::string & strErrorMessage
  );

#endif // !defined(ERRORCODEFROMGETLASTERRORTOSTRING_H__INCLUDED_)
