#pragma once

typedef unsigned long DWORD ;
typedef unsigned char BYTE ;

#include "IPC/IPC_data.hpp"

class I_ServerProcess
{
public:
  //return: number of bytes to send via IPC specific type.
  virtual DWORD IPC_Message(
//    BYTE byCommand
////    //wide string because the string may be a chinese string for a power scheme
////    , std::wstring & stdwstrMessage
//    //Reference ("&") because the array is created within the function.
//    , BYTE * & r_arbyPipeDataToSend
    IPC_data & r_ipc_data
    ) = 0 ;
};
