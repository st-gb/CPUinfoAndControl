#include "LocalLanguageMessageFromErrorCode.h"
//Else: "fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]"
#define _AFXDLL 
//#include <AfxWin.h> //for AfxMessageBox(...)->muss MultiThreaded DLL ("/MD") sein
#include <windows.h> //for SERVICE_TABLE_ENTRY, ... CreateService

std::string LocalLanguageMessageFromErrorCode(DWORD dwErrorCode)
{
    std::string stdstrMessage ;
    LPVOID lpMsgBuf;

    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
 
    stdstrMessage = std::string((char*)lpMsgBuf) ;
    //Release memory allocated by "::FormatMessage(...)"?
    ::LocalFree(lpMsgBuf);

    return stdstrMessage ;
}
