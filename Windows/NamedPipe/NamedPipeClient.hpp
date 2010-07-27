#pragma once

#ifdef _MSC_VER //MS-compiler
    #include <tchar.h> //for LPTSTR
#endif
#include <windows.h> //LPTSTR 

#include <string> //std::wstring

class NamedPipeClient
{
  bool m_bConnected ;
  HANDLE m_handleClientPipe; 
public:
  BYTE * m_arbyIPCdata ;
  DWORD m_dwSizeInByte ;
  std::wstring m_stdwstrMessage ;
  //NamedPipeClient( LPTSTR lpszPipename ) ;
  void OnDisconnect() ;
  BYTE Init() ;
  bool IsConnected() ; //{ return m_bConnected ; }
  BYTE SendCommandAndGetResponse(BYTE byMessage) ;
  NamedPipeClient()
    : m_arbyIPCdata (NULL)
  {

  }
  ~NamedPipeClient( )
  {
    ::CloseHandle(m_handleClientPipe); 
  }
};
