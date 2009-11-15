#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include <wx/wx.h>
#endif

//#include <wx/socket.h>
#include <wx/event.h> //wxEvtHandler
#include <Controller/IPC/I_IPC.hpp> //class I_IPC_Server

//class I_IPC_Server ;
class wxSocketServer ;

class wxServiceSocketServer
  : public I_IPC_Server
  , public wxEvtHandler
{
  wxSocketServer * m_wxsocketserver ;
public:
  BYTE Init() ;
  ////connects e.g. to the shared memory
  //ConnectToProvider() ;
  //SendMessage() ;
  void OnDisconnect() ;
  bool IsConnected() ;
};
