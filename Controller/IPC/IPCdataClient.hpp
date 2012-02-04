#pragma once

//Base class for clients that connect to an IPC  data provider (e.g.
// wxWidgets GUI app)
class IPCdataClient
{
//  wxCriticalSection m_wxcriticalsectionIPCobject ;
//#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//  //NamedPipeClient m_ipcclient ;
//  //Must be a pointer because it is/ should be determined/ changeable at
//  //_runtime_ .
//  IPC_Client * m_p_i_ipcclient ;
//#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//  IPC_data s_ipc_data;
//public:
  void FetchCPUcoreDataFromIPC();
  volatile bool m_vbGotCPUcoreData ;
};
