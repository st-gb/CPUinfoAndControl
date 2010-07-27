/*
 * WTSSendMessageRunTimeDynLinked.cpp
 *
 *  Created on: Jun 25, 2010
 *      Author: Stefan
 */
#include <global.h>
#include "WTSSendMessageRunTimeDynLinked.hpp"
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <windows.h> //::GetProcAddress()
#include <tchar.h> //for _T()

BOOL WTSSendMessage(
  __in   HANDLE hServer,
  __in   DWORD SessionId,
  __in   LPTSTR pTitle,
  __in   DWORD TitleLength,
  __in   LPTSTR pMessage,
  __in   DWORD MessageLength,
  __in   DWORD Style,
  __in   DWORD Timeout,
  __out  DWORD *pResponse,
  __in   BOOL bWait
)
{
  HMODULE handleWtsapi32DLL = ::LoadLibrary( _T("Wtsapi32.dll") );
  if ( handleWtsapi32DLL )
  {
    LOGN( "got handle to Wtsapi32.dll" )
#ifdef UNICODE
    char arch [] = "WTSSendMessageW" ;
#else
    char arch [] = "WTSSendMessageA" ;
#endif
    pfnWTSSendMessage pfn_wtssendmessage = (pfnWTSSendMessage)
    ::GetProcAddress( handleWtsapi32DLL, arch );
    if( pfn_wtssendmessage )
    {
      LOGN( "got pointer to " << arch)
      //TODO WTSSendMessage fails with error code 5 (access denied)
      BOOL bool_ = (*pfn_wtssendmessage) (
        hServer,
        SessionId,
        pTitle,
        TitleLength,
        pMessage,
        MessageLength,
        Style,
        Timeout,
        pResponse,
        bWait
        ) ;
      if( ! bool_ )
        LOGN("calling " << arch << " failed: " <<
          LocalLanguageMessageAndErrorCodeA(::GetLastError() ) )
      LOGN( "ret of " << arch << ":" << bool_ << " " << *pResponse )
    }
    ::FreeLibrary( handleWtsapi32DLL );
  }
  return FALSE ;
}
