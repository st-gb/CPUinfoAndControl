#pragma once

#include <Controller/IPC/I_IPC.hpp>
#include <Controller/I_ServerProcess.hpp>
#include <windows.h>  //for HANDLE, ..

#define PIPE_BUFFER_SIZE 4096

//namespace Windows
//{
  class NamedPipeServer
    : public I_IPC_Server
  {
    PSECURITY_DESCRIPTOR mp_security_descriptor ;
  public:
    HANDLE m_handlePipe ;
    NamedPipeServer( I_ServerProcess * p_serverprocess) ;
    ~NamedPipeServer() ;
    I_ServerProcess * mp_serverprocess ;
    //e.g. create shared memory or create socket and listen on socket.
    BYTE Init( ) ;//LPTSTR lpszPipename = "\\\\.\\pipe\\CPUcontrollerService" ) ;
    ////connects e.g. to the shared memory
    //BYTE ConnectToProvider() ;
    BYTE CreateSecAttributes(SECURITY_ATTRIBUTES & sa ) ;
  };
//};
