#pragma once

//#include <global.h> //BYTE
#include <windef.h> //BYTE
#include <string> //class std::wstring

enum IPCcontrolCodes
{
  pause_DVFS = 0 
  , start_DVFS
  , stop_DVFS
  , continue_DVFS
  , setVoltageAndFrequency
  , stop_service
  , pause_service
  , continue_service
  , copy_attribute_data
  , get_CPU_load
  //The DVFS state can't be determined as normal user (not admin) on WinXP.
  //So make it available via IPC from the DVFS service.
  , other_DVFS_is_enabled
  , disable_other_DVFS
  , enable_other_DVFS
  , sync_GUI_show_data_with_service
  //This should get the current data like temperature, CPU core load, voltage,
  // frequency.
  , get_current_CPU_data
  //Gets min/ max/ wanted voltages (etc.) from the service.
  , get_configuration_data
};

class I_IPC_Server
{
public:
  //e.g. create shared memory or create socket and listen on socket.
  //Must be virtual for executing "I_IPC_Server"-derived ::Init().
  virtual BYTE Init() = 0 ;
  ////connects e.g. to the shared memory
  //ConnectToProvider() ;
  //SendCommandAndGetResponse() ;
  void OnDisconnect() ;
  bool IsConnected() ;
};

class IPC_Client
{
public:
  BYTE * m_arbyIPCdata ;
  DWORD m_dwIPCdataSizeInByte ;
  std::wstring m_stdwstrMessage ;
  //e.g. connect to server.
  //Must be virtual for executing "IPC_Client"-derived ::ConnectToDataProvider().
  //Connects to a data provider (e.g. the CPU control service).
  virtual BYTE ConnectToDataProvider( std::string & r_stdstrMessage ) = 0 ;
  virtual void Disconnect() {} ;
  IPC_Client()
    :
    m_arbyIPCdata(NULL)
    {}
  //Avoid g++ warning
  //`class IPC_Client' has virtual functions but non-virtual destructor
  virtual ~IPC_Client() {} ;
  virtual bool IsConnected() //= 0 ;
    { return false ; }
  virtual BYTE SendCommandAndGetResponse(BYTE byCommand) //= 0 ;
    { return 1 ; }
  BYTE SendMessage(BYTE ) ;
//  //connects e.g. to the shared memory
//  BYTE ConnectToProvider() ;
};
