#pragma once

#include <string> //std::wstring

#ifdef _MSC_VER //MS-compiler
    #include <tchar.h> //for LPTSTR
#endif
#include <windows.h> //LPTSTR 
//#include <specstrings.h> //for __out
#include <Controller/multithread/criticalsection_type.hpp>
#include <Controller/IPC/I_IPC.hpp> //for base class I_IPC_Client

//Forward declarations.
class Model ;

namespace Windows
{
class NamedPipeClient
  :
  //Must be inherited from a base class in order the change the type of a
  // pointer of an IPC_Client subclass at _runtime_ .
  public IPC_Client
{
  BOOL m_bool ;
  bool m_bConnected ;
  volatile bool m_vbIsReadingOrWriting ;
  HANDLE m_handleClientPipe; 
  Model & m_r_model ;
  criticalsection_type m_criticalsection_typeIOandIsconnected ;
public:
  volatile bool m_vbIsGettingCPUcoreData ;
//  BYTE * m_arbyIPCdata ;
//  DWORD m_dwIPCdataSizeInByte ;
  DWORD m_dwState , m_dwCurInstances ;
  DWORD m_dwMaxUserNameSize ;
//  std::wstring m_stdwstrMessage ;
  //NamedPipeClient( LPTSTR lpszPipename ) ;
  void Disconnect() ;
  //_Must_ be defined here, else ~"undefined reference to Disconnect_Inline
  // in MainFrame.cpp".
  inline void Disconnect_Inline()
  {
    ::CloseHandle(m_handleClientPipe);
  }
  inline bool GetConnectionStateViaGetNamedPipeHandleState() ;
  void OnDisconnect() ;
  BYTE Init()
  {
    std::string stdstrMessage ;
    return ConnectToDataProvider(stdstrMessage) ;
  }
  //Connects to a data provider (e.g. the CPU control service).
  BYTE ConnectToDataProvider(std::string & r_stdstrMessage) ;
  bool IsConnected() ; //{ return m_bConnected ; }
  inline BOOL Read(
    //__out
    LPVOID lpBuffer,
    //__in
    DWORD nNumberOfBytesToRead,
    //__out_opt
    LPDWORD lpNumberOfBytesRead,
    //__inout_opt
    LPOVERLAPPED lpOverlapped
    ) ;
//  BYTE SendCommandAndGetResponse(BYTE byMessage) ;
  BYTE SendCommandAndGetResponse(IPC_data & ipc_data);
  inline DWORD SendDataSizeInByte(
    //  BYTE * ar_byData,
    DWORD dwByteSize
    );
  inline BOOL Write(
    LPCVOID lpBuffer,
    //__in
    DWORD nNumberOfBytesToWrite,
    //__out_opt
    LPDWORD lpNumberOfBytesWritten,
    //__inout_opt
    LPOVERLAPPED lpOverlapped
    ) ;
  inline BYTE WriteCommand(BYTE byCommand);
  inline BYTE WriteDataSizeInByte(IPC_data & r_ipc_data);
  inline BYTE WriteDataBelongingToCommand(IPC_data & r_ipc_data);
  NamedPipeClient(Model & r_model ) ;
  ~NamedPipeClient() ;
};
}
