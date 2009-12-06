
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#ifdef __CYGWIN__
  //For not declaring
  //  -int select(int, _types_fd_set*, _types_fd_set*, _types_fd_set*, const timeval*)'
  //  -"struct timeval"
  //   in cygwin/.../w32api/winsock.h -> no errors
  #define __INSIDE_CYGWIN__
#endif
#include "wx/socket.h"
#include "wx/url.h"
#include "wx/wfstream.h"

#include "wxServiceSocketClient.hpp"

//#pragma lib "wxbase28d_net.lib"
#ifdef _DEBUG
//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "wxbase28d_net.lib" )
#else
//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "wxbase28_net.lib" )
#endif
//#pragma lib "wsock32.lib"

BEGIN_EVENT_TABLE(wxServiceSocketClient, wxEvtHandler)
  //EVT_SOCKET( SOCKET_ID, wxServiceSocketClient::OnSocketEvent)
END_EVENT_TABLE()


BYTE wxServiceSocketClient::ConnectToProvider()
{
  wxIPV4address wxipv4addressServer ;
  wxipv4addressServer.Hostname(m_wxstrServerHostname);
  wxipv4addressServer.Service(m_usServerPortNumber);

  mp_wxsocketclient->Connect( 
    wxipv4addressServer , 
    false //blocking mode == true
    );
  return 1 ;
}

BYTE wxServiceSocketClient::Init()
{
// Create the socket
  mp_wxsocketclient = new wxSocketClient();

  // Setup the event handler and subscribe to most events
  //mp_wxsocketclient->SetEventHandler(*this, SOCKET_ID);
  mp_wxsocketclient->SetNotify(
    wxSOCKET_CONNECTION_FLAG |
    wxSOCKET_INPUT_FLAG |
    wxSOCKET_LOST_FLAG
    );
  //According to the notify value, this function enables or disables socket 
  //events. If notify is true, the events configured with SetNotify will be 
  //sent to the application. If notify is false; no events will be sent.
  mp_wxsocketclient->Notify(true);

  //mp_wxsocketclient->Connect(addr, 
  //  false //blocking mode == true
  //  );
  //mp_wxsocketclient->WaitOnConnect(10);

  //if (mp_wxsocketclient->IsConnected())
  return 1 ;
}

void wxServiceSocketClient::OnSocketEvent(wxSocketEvent & r_wxsocketevent )
{
  switch( r_wxsocketevent.GetSocketEvent() )
  {
    case wxSOCKET_INPUT      : 
      //s.Append(_("wxSOCKET_INPUT\n"));
      break;
    case wxSOCKET_LOST       : 
      //s.Append(_("wxSOCKET_LOST\n")); 
      break;
    case wxSOCKET_CONNECTION : 
      //s.Append(_("wxSOCKET_CONNECTION\n")); 
      break;
    default                  : 
      //s.Append(_("Unexpected event !\n")); 
      break;
  }
}

BYTE wxServiceSocketClient::SendMessage(BYTE by)
{
  m_byLastSentCommand = by ;
  if( mp_wxsocketclient->IsConnected() )
    //mp_wxsocketclient->
    //[...]it writes a short header before so that ReadMsg knows how much 
    //data should it actually read. So, a buffer sent with WriteMsg must 
    //be read with ReadMsg. 
    //This function always waits for the entire buffer to be sent, unless an error occurs.
    //WriteMsg(const void * buffer, wxUint32 nbytes)
    mp_wxsocketclient->Write(&by,1) ;
  //mp_wxsocketclient->ReadMsg(const void * buffer, wxUint32 nbytes)
  return 1 ;
}

wxServiceSocketClient::wxServiceSocketClient()
  : mp_wxsocketclient (NULL)
{
  
}

wxServiceSocketClient::wxServiceSocketClient(
  std::string & r_strHostname 
  , WORD wPort)
  : mp_wxsocketclient (NULL)
  //Constructor that converts to wide string if appropriate
  , m_wxstrServerHostname( r_strHostname.c_str() ,wxConvLocal )
{
  m_usServerPortNumber = wPort ;
}

wxServiceSocketClient::~wxServiceSocketClient()
{
  if( mp_wxsocketclient )
    mp_wxsocketclient->Close();
}
