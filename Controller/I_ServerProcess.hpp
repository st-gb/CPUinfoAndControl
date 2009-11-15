#pragma once

class I_ServerProcess
{
public:
  virtual void IPC_Message(BYTE byCommand) = 0 ;
};
