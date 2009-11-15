#pragma once

#ifdef _MSC_VER //MS-compiler
    #include <tchar.h> //for LPTSTR
#endif
#include <windows.h> //LPTSTR 

class NamedPipeClient
{
  bool m_bConnected ;
  HANDLE m_handleClientPipe; 
public:
  //NamedPipeClient( LPTSTR lpszPipename ) ;
  void OnDisconnect() ;
  BYTE Init() ;
  bool IsConnected() { return m_bConnected ; }
  BYTE SendMessage(BYTE byMessage) ;
  ~NamedPipeClient( )
  {
    ::CloseHandle(m_handleClientPipe); 
  }
};
