//Wenn der Compiler-Fehler "C1010" auftritt, dann liegt das
//bestimmt an der Befehlszeilenoption /Yu (Verwenden einer 
//vorkompilierten Header-Datei) siehe Men� "Projekt" 
//Men�punkt "Einstellungen..." Karteireiter "C/C++" Textbox
//"Projekt Optionen" Befehlszeilenoption "Yu" (in der 
//Textbox unten, also eventuell herunterscrollen)
//dann entweder die direkt hinter "Yu" angegebene Datei 
//einbinden ("#include ") oder die Befehlszeilenoption "/Yu"
//entfernen
//#include "stdafx.h"
#include "ErrorCodeFromGetLastErrorToString.h"
#include <string> //std::string
//#ifndef _MSC_VER
  #include <windows.h> //for ::FormatMessage(...)
  #include <winnt.h> //for MAKELANGID
//#include <Controller/tchar_conversion.h> //GetCharPointer(...)
#include <Controller/stdtstr.hpp> //GetStdString(...)
//#endif

//diese Funktion liefert die zum DWORD-(unsigned long-) Wert 
//der Windows-API-Funktion "DWORD GetLastError(void)" 
//korrepsondierende Fehlermeldung als Text in der Standard-
//Sprache der laufenden Windows-Sitzung
//CString 
//std::string 
DWORD GetLastErrorMessageString(
  DWORD dwErrorCodeConformToGetLastError ,
  std::string & strErrorMessage
  )
{
  DWORD dwRet ;
	//CString strErrorMessage;
  //std::string strErrorMessage;
	LPVOID lpMsgBuf = NULL;
  //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/debug/base/formatmessage.htm:
  //"If the function fails, the return value is zero. 
  //To get extended error information, call GetLastError."
  if( ! ::FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
    NULL, 
		dwErrorCodeConformToGetLastError, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf, 
    0, 
    NULL 
      )
    )
    dwRet = ::GetLastError() ;
  else
	  //strErrorMessage = (LPCTSTR)lpMsgBuf;
    strErrorMessage = //std::string( GetCharPointer( ((LPCTSTR) lpMsgBuf) ) );
      GetStdString( (LPCTSTR) lpMsgBuf ) ; 

	if( lpMsgBuf != NULL )
    ::LocalFree(lpMsgBuf);

	return //strErrorMessage;
    dwRet ;
}

std::string GetLastErrorMessageString(
  DWORD dwErrorCodeConformToGetLastError
  //std::string & strErrorMessage
  )
{
  std::string strErrorMessage ;
  GetLastErrorMessageString(
    dwErrorCodeConformToGetLastError ,
    strErrorMessage 
    ) ;
  return strErrorMessage ;
}
