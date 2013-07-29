/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <Controller/IPC/I_IPC.hpp>
#include <Controller/I_ServerProcess.hpp>
#include <Controller/IPC/I_IPC_DataHandler.hpp>
#include <windows.h>  //for HANDLE, ..

#define PIPE_BUFFER_SIZE 4096

//namespace Windows
//{
  class NamedPipeServer
    : public I_IPC_Server
  {
    //    //The data handler should be a separate class because the received data or
    //    //the data to send should be able to be in different data formats.
    //    //So one class could send (and receive) XML data while another class might
    //    //send/ receive binary encoded data.
    I_IPC_DataHandler * mp_ipc_datahandler ;
    //http://msdn.microsoft.com/en-us/library/aa379561%28VS.85%29.aspx:
    //"Several functions that use the SECURITY_DESCRIPTOR structure require that
    //this structure be aligned on a valid pointer boundary in memory.
    //These boundaries vary depending on the type of processor used.
    //Memory allocation functions such as malloc  and LocalAlloc return properly
    //aligned pointers."
    PSECURITY_DESCRIPTOR mp_security_descriptor ;
    SECURITY_ATTRIBUTES m_security_attributes ;
  public:
    HANDLE m_handlePipe ;
    LPTSTR m_lpszPipename ;

    void CreateDownPrivilegedPipe() ;
    NamedPipeServer(
      I_ServerProcess * p_serverprocess
//    , I_IPC_DataHandler & r_ipc_datahandler
      ) ;
    NamedPipeServer(
//      I_ServerProcess * p_serverprocess
      I_IPC_DataHandler & r_ipc_datahandler
      ) ;
    //"virtual" too avoid g++ warning: "warning: `class NamedPipeServer' has
    //  virtual functions but non-virtual destructor"
    virtual ~NamedPipeServer() ;
    I_ServerProcess * mp_serverprocess ;
    //e.g. create shared memory or create socket and listen on socket.
    BYTE Init( ) ;//LPTSTR lpszPipename = "\\\\.\\pipe\\CPUcontrollerService" ) ;
    ////connects e.g. to the shared memory
    //BYTE ConnectToProvider() ;
  };
//};
