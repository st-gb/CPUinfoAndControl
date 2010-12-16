#pragma once

typedef unsigned long DWORD ;
typedef unsigned char BYTE ;

class I_ServerProcess
{
public:
  //return: number of bytes to send via IPC specific type.
  virtual DWORD IPC_Message(
    BYTE byCommand
//    //wide string because the string may be a chinese string for a power scheme
//    , std::wstring & stdwstrMessage
    //Reference ("&") because the array is created within the function.
    , BYTE * & r_arbyPipeDataToSend
    ) = 0 ;
};
