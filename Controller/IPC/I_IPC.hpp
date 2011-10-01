/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

//#include <global.h> //BYTE
#include <windef.h> //BYTE
#include <string> //class std::wstring

#include "IPCcontrolCodes.h" //enum IPCcontrolCodes
#include "IPC_data.hpp" //class IPC_data

class I_IPC_Server
{
public:
  enum
  {
    WritingIPCdataSucceeded,
    WritingIPCdataFailed,
    ReadingIPCdataSucceeded,
    ReadingIPCdataFailed
  };
  //e.g. create shared memory or create socket and listen on socket.
  //Must be virtual for executing "I_IPC_Server"-derived ::Init().
  virtual BYTE Init() = 0 ;
  ////connects e.g. to the shared memory
  //ConnectToProvider() ;
  //SendCommandAndGetResponse() ;
  void OnDisconnect() ;
  bool IsConnected() ;
};

#include <wx/thread.h> //class wxCriticalSection

class IPC_Client
{
public:
  typedef wxCriticalSection critical_section_type;
  enum IPCdataState
  {
    ReadNextIPCdata,
    IPCdataNotProcessed,
    ProcessingIPCdata,
    IPCdataProcessed
  };
  enum ResponseReturnCodes
  {
    GotResponse,
    GetsResponseNonBlocking,
    FailedToGetResponse,
    WritingIPCdataSucceeded,
    WritingIPCdataFailed,
    ReadingIPCdataSucceeded,
    ReadingIPCdataFailed
  };
  BYTE * m_arbyIPCdata ;
  DWORD m_dwIPCdataSizeInByte ;
  BYTE m_bProcessingIPCdata;
  std::wstring m_stdwstrMessage ;
  critical_section_type m_crit_secIPCdata;
  //e.g. connect to server.
  //Must be virtual for executing "IPC_Client"-derived ::ConnectToDataProvider().
  //Connects to a data provider (e.g. the CPU control service).
  virtual BYTE ConnectToDataProvider( std::string & r_stdstrMessage ) = 0 ;
  virtual void Disconnect() {} ;
  IPC_Client()
    :
    m_bProcessingIPCdata(//false
      ReadNextIPCdata),
    m_arbyIPCdata(NULL)
    {}
  //Avoid g++ warning
  //`class IPC_Client' has virtual functions but non-virtual destructor
  virtual ~IPC_Client() {} ;
  virtual bool IsConnected() //= 0 ;
    { return false ; }
  virtual BYTE SendCommandAndGetResponse(BYTE byCommand) //= 0 ;
    { return 1 ; }
  virtual BYTE SendCommandAndGetResponse(IPC_data & ipc_data) //= 0 ;
    { return 1 ; }
  BYTE SendMessage(BYTE ) ;
//  //connects e.g. to the shared memory
//  BYTE ConnectToProvider() ;
};
