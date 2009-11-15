#pragma once

#include <wx/event.h> //wxEvtHandler
#include <Controller/IPC/I_IPC.hpp>

class wxSocketClient ;

class wxServiceSocketClient
  : public IPC_Client
  , public wxEvtHandler
{
  BYTE m_byLastSentCommand ;
  unsigned short m_usServerPortNumber ;
  wxString m_wxstrServerHostname ;
public:
  wxSocketClient * mp_wxsocketclient ;
  //e.g. connect to server.
  //Must be virtual for executing "IPC_Client"-derived ::Init().
  BYTE Init() ;
  // socket event handler
  void OnSocketEvent(wxSocketEvent& event);
  BYTE SendMessage(BYTE ) ;
  //connects e.g. to the shared memory
  BYTE ConnectToProvider() ;
  wxServiceSocketClient() ;
  wxServiceSocketClient(
    std::string & r_strHostname 
    , WORD wPort) ;
  ~wxServiceSocketClient() ;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};
