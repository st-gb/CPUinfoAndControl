#pragma once

#include <global.h> //BYTE

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
  //e.g. connect to server.
  //Must be virtual for executing "IPC_Client"-derived ::Init().
  virtual BYTE Init() = 0 ;
  BYTE SendMessage(BYTE ) ;
  //connects e.g. to the shared memory
  BYTE ConnectToProvider() ;
};
