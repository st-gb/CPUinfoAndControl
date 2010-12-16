#include "wxServiceSocketServer.hpp"

#include <Controller/I_ServerProcess.hpp> //class I_ServerProcess
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)

//Sourcecode adapted from wxWidgets sockets sample's "server.cpp" .

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

//VOID SocketClientThreadProc(LPVOID lpvParam)
//{
//  //wxServiceSocketServer * p_wxservicesocketserver =
//  //  (wxServiceSocketServer *) lpvParam ;
//  SocketClientThreadProcStruct * p_socketclientthreadprocstruct =
//    (SocketClientThreadProcStruct *) lpvParam ;
//  LOGN("p_namedpipeserver: " << p_socketclientthreadprocstruct)
//  if( p_socketclientthreadprocstruct )
//  {
//    wxSocketBase * p_wxsocketbaseClient =
//      p_socketclientthreadprocstruct->p_wxsocketbaseClient ;
//    p_wxsocketbaseClient
//    p_wxsocketbaseClient->ReadMsg() ;
//  }
//}

BEGIN_EVENT_TABLE(wxServiceSocketServer, wxEvtHandler)
  EVT_SOCKET(SERVER_ID,  wxServiceSocketServer::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  wxServiceSocketServer::OnSocketEvent)
END_EVENT_TABLE()

wxServiceSocketServer::wxServiceSocketServer(
  //WORD wPortNumber ,
  I_ServerProcess * p_serverprocess
  )
  : m_wNumberOfClients(0)
  , m_p_serverprocess( p_serverprocess)
  , m_p_wxsocketserver(NULL)
{
  //m_usServerPortNumber = wPortNumber ;
}

wxServiceSocketServer::~wxServiceSocketServer()
{
  if( m_p_wxsocketserver )
    delete m_p_wxsocketserver;
}

BYTE wxServiceSocketServer::Init()
{
  // Create the address - defaults to localhost:0 initially
  wxIPV4address wxipv4address;
  //http://docs.wxwidgets.org/trunk/classwx_i_p_v4address.html
  // #3833c1ce1a49de08c5a906410a8bb4e3:
  //"Set the port to that corresponding to the specified service. "
  if( //"false if something went wrong."
      ! wxipv4address.Service(m_usServerPortNumber) )
  {
    LOGN("For socket server: error setting port to " << m_usServerPortNumber )
    return 1 ;
  }
  // Create the socket
  m_p_wxsocketserver = new wxSocketServer(wxipv4address);
  if( m_p_wxsocketserver )
  {
    // We use Ok() here to see if the server is really listening
    if ( //"Returns true if the socket is initialized and ready and false in
        //other cases."
        ! m_p_wxsocketserver->Ok() )
    {
      LOGN("The socket server object is NOT successfully initialized and "
          "ready.")
      return 2 ;
    }
    else
      LOGN("The socket server object is successfully initialized and ready.")
    // Setup the event handler and subscribe to connection events
    m_p_wxsocketserver->SetEventHandler( * this, SERVER_ID);
    m_p_wxsocketserver->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_p_wxsocketserver->Notify(true);
    LOGN("End of socket server initialisation.")
//    wxAppConsole::OnRun() ;
//    OnRun() ;
//    LOGN("After OnRun().")
    LOGN("wxAppConsole::IsMainLoopRunning():"
      << wxAppConsole::IsMainLoopRunning() )
    return 0 ;
  }
  return 3 ;
}

void wxServiceSocketServer::OnServerEvent(wxSocketEvent& event)
{
  LOGN("wxServiceSocketServer::OnServerEvent begin")
  //wxString s = _("OnServerEvent: ");
  wxSocketBase * p_wxsocketbase ;

  switch( event.GetSocketEvent() )
  {
    case wxSOCKET_CONNECTION :
      LOGN( "wxSOCKET_CONNECTION\n" );
      break;
    default :
      LOGN( "Unexpected event !\n" );
      break;
  }

  //m_text->AppendText(s);

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  p_wxsocketbase = m_p_wxsocketserver->Accept(false);

  if (p_wxsocketbase)
  {
    LOGN( "New socket client connection accepted." )
    p_wxsocketbase->SetEventHandler( * this, SOCKET_ID);
    p_wxsocketbase->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    p_wxsocketbase->Notify(true);
//    DWORD dwThreadId ;
//    HANDLE hThread = ::CreateThread(
//      NULL,              // no security attribute
//      0,                 // default stack size
//      (LPTHREAD_START_ROUTINE) SocketClientThread,
//      //(LPVOID) m_handlePipe,    // thread parameter
//      this ,
//      0,                 // not suspended
//      & dwThreadId // returns thread ID
//      );
//
//     if (hThread == NULL)
//     {
//        LOGN("CreateThread failed");
//        return 0;
//     }
    m_wNumberOfClients ++ ;
  }
  else
  {
    LOGN("Error: couldn't accept a new connection\n\n")
    return;
  }
}

void wxServiceSocketServer::OnSocketEvent(wxSocketEvent & r_wxsocket_event)
{
  LOGN("wxServiceSocketServer::OnSocketEvent begin")
  //wxString s = _("OnSocketEvent: ");
  wxSocketBase * p_wxsocketbase = r_wxsocket_event.GetSocket();

  // Now we process the event
  switch( r_wxsocket_event.GetSocketEvent() )
  {
    case wxSOCKET_INPUT:
    {
      LOGN( "wxSOCKET_INPUT." )
//      // We disable input events, so that the test doesn't trigger
//      // wxSocketEvent again.
//      p_wxsocketbase->SetNotify(wxSOCKET_LOST_FLAG);

      // Which test are we going to run?
      unsigned char c;
      p_wxsocketbase->Read( & c, 1 );
      BYTE * arbyIPCdataToSend ;
      DWORD dwNumberOfBytes = m_p_serverprocess->IPC_Message(
        c, arbyIPCdataToSend) ;
      p_wxsocketbase->Write(arbyIPCdataToSend, dwNumberOfBytes) ;
      delete [] arbyIPCdataToSend ;

//      switch (c)
//      {
//        case other_DVFS_is_enabled:
//          Test1(sock);
//          break;
//        case 0xCE:
//          Test2(sock);
//          break;
//        case 0xDE: Test3(sock); break;
//        default:
//          LOGN(_("Unknown test id received from client\n\n"));
//      }

//      // Enable input events again.
//      p_wxsocketbase->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      LOGN("wxSOCKET_LOST")
      m_wNumberOfClients -- ;

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
