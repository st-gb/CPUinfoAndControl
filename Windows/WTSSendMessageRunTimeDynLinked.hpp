/*
 * WTSSendMessageRunTimeDynLinked.hpp
 *
 *  Created on: Jun 25, 2010
 *      Author: Stefan
 */

#ifndef WTSSENDMESSAGERUNTIMEDYNLINKED_HPP_
#define WTSSENDMESSAGERUNTIMEDYNLINKED_HPP_

#include <windef.h> //for BOOL
#include <specstrings.h> //for __in, __out

typedef BOOL ( * pfnWTSSendMessage) (
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
  );

//from http://msdn.microsoft.com/en-us/library/aa383842%28VS.85%29.aspx:
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
);

#endif /* WTSSENDMESSAGERUNTIMEDYNLINKED_HPP_ */
