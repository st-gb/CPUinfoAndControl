#include "wxServiceSocketServer.hpp"

#ifdef __CYGWIN__
  //For not declaring
  //  -int select(int, _types_fd_set*, _types_fd_set*, _types_fd_set*, const timeval*)'
  //  -"struct timeval"
  //   in cygwin/.../w32api/winsock.h -> no errors
  #define __INSIDE_CYGWIN__
#endif
#include "wx/socket.h"
//#include <Controller/IPC/I_IPC.hpp>

// IDs for the controls and the menu commands
enum
{
  // id for sockets
  SERVER_ID = 100,
  SOCKET_ID
};

struct SocketClientThreadProcStruct
{
  wxServiceSocketServer * p_wxservicesocketserver ;
  wxSocketBase * p_wxsocketbaseClient ;
};

VOID SocketClientThreadProc(LPVOID lpvParam)
{
  //wxServiceSocketServer * p_wxservicesocketserver = 
  //  (wxServiceSocketServer *) lpvParam ;
  SocketClientThreadProcStruct * p_socketclientthreadprocstruct =
    (SocketClientThreadProcStruct *) lpvParam ;
  LOGN("p_namedpipeserver: " << p_socketclientthreadprocstruct)
  if( p_socketclientthreadprocstruct )
  {
    wxSocketBase * p_wxsocketbaseClient = 
      p_socketclientthreadprocstruct->p_wxsocketbaseClient ;
    p_wxsocketbaseClient
    p_wxsocketbaseClient->ReadMsg() ;
  }
}

BEGIN_EVENT_TABLE(wxServiceSocketServer, wxEvtHandler)
  EVT_SOCKET(SERVER_ID,  wxServiceSocketServer::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  wxServiceSocketServer::OnSocketEvent)
END_EVENT_TABLE()

wxServiceSocketServer::wxServiceSocketServer(
  WORD wPortNumber)
{
  m_usServerPortNumber = wPortNumber ;
}

wxServiceSocketServer::wxServiceSocketServer()
{
  delete m_wxsocketserver;
}

BYTE wxServiceSocketServer::Init()
{
  // Create the address - defaults to localhost:0 initially
  wxIPV4address wxipv4address;
  wxipv4address.Service(m_usServerPortNumber);
  
  // Create the socket
  m_wxsocketserver = new wxSocketServer(wxipv4address);
  // We use Ok() here to see if the server is really listening
  //if (! m_wxsocketserver->Ok() )
  // Setup the event handler and subscribe to connection events
  m_wxsocketserver->SetEventHandler(*this, SERVER_ID);
  m_wxsocketserver->SetNotify(wxSOCKET_CONNECTION_FLAG);
  m_wxsocketserver->Notify(true);
}

void wxServiceSocketServer::OnServerEvent(wxSocketEvent& event)
{
  //wxString s = _("OnServerEvent: ");
  wxSocketBase * p_wxsocketbase ;

  //switch(event.GetSocketEvent())
  //{
  //  case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
  //  default                  : s.Append(_("Unexpected event !\n")); break;
  //}

  //m_text->AppendText(s);

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  p_wxsocketbase = m_wxserversocket->Accept(false);

  if (p_wxsocketbase)
  {
    LOGN(_("New client connection accepted\n\n"))
    p_wxsocketbase->SetEventHandler(*this, SOCKET_ID);
    p_wxsocketbase->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    p_wxsocketbase->Notify(true);
    DWORD dwThreadId ;
    HANDLE hThread = ::CreateThread( 
      NULL,              // no security attribute 
      0,                 // default stack size 
      (LPTHREAD_START_ROUTINE) SocketClientThread, 
      //(LPVOID) m_handlePipe,    // thread parameter 
      this ,
      0,                 // not suspended 
      & dwThreadId // returns thread ID 
      );

     if (hThread == NULL) 
     {
        LOGN("CreateThread failed"); 
        return 0;
     }
    //m_numClients++;
  }
  else
  {
    LOGN("Error: couldn't accept a new connection\n\n")
    return;
  }
}

void wxServiceSocketServer::OnSocketEvent(wxSocketEvent& event)
{
  //wxString s = _("OnSocketEvent: ");
  wxSocketBase * p_wxsocketbase = event.GetSocket();

  // First, print a message
  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT : LOGN(_("wxSOCKET_INPUT\n")); break;
    case wxSOCKET_LOST  : LOGN(_("wxSOCKET_LOST\n")); break;
    default             : LOGN(_("Unexpected event !\n")); break;
  }

  //m_text->AppendText(s);

  // Now we process the event
  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT:
    {
      // We disable input events, so that the test doesn't trigger
      // wxSocketEvent again.
      p_wxsocketbase->SetNotify(wxSOCKET_LOST_FLAG);

      // Which test are we going to run?
      unsigned char c;
      p_wxsocketbase->Read(&c, 1);

      switch (c)
      {
        case other_DVFS_is_enabled: 
          Test1(sock);
          break;
        case 0xCE: Test2(sock); break;
        case 0xDE: Test3(sock); break;
        default:
          LOGN(_("Unknown test id received from client\n\n"));
      }

      // Enable input events again.
      p_wxsocketbase->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      //m_numClients--;

      // Destroy() should be used instead of delete wherever possible,
      // due to the fact that wxSocket uses 'delayed events' (see the
      // documentation for wxPostEvent) and we don't want an event to
      // arrive to the event handler (the frame, here) after the socket
      // has been deleted. Also, we might be doing some other thing with
      // the socket at the same time; for example, we might be in the
      // middle of a test or something. Destroy() takes care of all
      // this for us.

      LOGN(_("Socket lost->Deleting socket.\n\n"));
      p_wxsocketbase->Destroy();
      break;
    }
    default: ;
  }
}
