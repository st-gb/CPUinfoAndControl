/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//from wxWidgets socket sample
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
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
#include "wx/string.h" //wxStringBase::npos
#include "wx/url.h"
#include "wx/wfstream.h"

#include "wxServiceSocketClient.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//GetStdString(const wxString & ), getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

#ifdef _MSC_VER //if MicroSoft compiler
//#pragma lib "wxbase28d_net.lib"
#ifdef _DEBUG
//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "wxbase28d_net.lib" )
#else
//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "wxbase28_net.lib" )
#endif
//#pragma lib "wsock32.lib"
#endif //#ifdef _MSC_VER //if MicroSoft compiler

#define SOCKET_ID 1

using namespace NonBlocking ;

BEGIN_EVENT_TABLE(wxServiceSocketClient, wxEvtHandler)
  EVT_SOCKET( SOCKET_ID, wxServiceSocketClient::OnSocketEvent)
END_EVENT_TABLE()

bool wxServiceSocketClient::AllBytesRead( void * p_vData, DWORD dwSize)
{
  //May be NULL.
  if( mp_wxsocketclient )
  {
    mp_wxsocketclient->Read(
      p_vData //void * buffer
      , dwSize //wxUint32 nbytes
      ) ;
    wxUint32 wxui32LastCount = mp_wxsocketclient->LastCount();
    //http://docs.wxwidgets.org/stable/wx_wxsocketbase.html#wxsocketbaseread:
    // "Use LastCount to verify the number of bytes actually read."
    if( wxui32LastCount == dwSize )
      return true ;
  }
  return false ;
}

BYTE wxServiceSocketClient::ConnectToDataProvider(std::string & r_stdstrMessage)
{
  LOGN( FULL_FUNC_NAME << "--begin")
  wxIPV4address wxipv4addressServer ;

  wxipv4addressServer.Hostname(m_wxstrServerHostname);
  wxipv4addressServer.Service(m_usServerPortNumber);

//  if( ! mp_wxsocketclient )
//    mp_wxsocketclient = new wxSocketClient() ;
  Init() ;
  //May be NULL.
  if( mp_wxsocketclient )
  {
    bool bWait = false;
    if( mp_wxsocketclient->
      //http://docs.wxwidgets.org/trunk/classwx_socket_client.html
      // #581cdb757cce6020c8caac1ddd74a599:
      //"true if the connection is established and no error occurs."

      //"If wait was false, and Connect returns false, you should still be
      //prepared to handle the completion of this connection request, either
      //with WaitOnConnect or by watching wxSOCKET_CONNECTION and
      //wxSOCKET_LOST events.
      Connect(
        wxipv4addressServer ,
        //http://docs.wxwidgets.org/trunk/classwx_socket_client.html
        // #581cdb757cce6020c8caac1ddd74a599:
        //"If wait is true, Connect() will wait until the connection completes.
        //Warning: This will block the GUI."

        //bool wait = true
        bWait
        //blocking mode == true
//        true
        )
      )
    {
      LOGN("The socket connection is established.")
      return true ;
    }
    else
    {
      if( bWait == true )
        LOGN("The socket connection failed.")
      else
      {
        bool bWaitOnConnectResult =
          mp_wxsocketclient->WaitOnConnect(
            //http://docs.wxwidgets.org/stable/wx_wxsocketclient.html#wxsocketclientwaitonconnect:
            //"Number of seconds to wait. If -1, it will wait for the default
            //timeout, as set with SetTimeout."
  //          -1
            10
            );
        LOGN( FULL_FUNC_NAME << "--WaitOnConnect(...) returned " <<
          bWaitOnConnectResult)
//        bWaitOnConnectResult = false;
      }
    }
    bool bIsConnected = mp_wxsocketclient->IsConnected();
    if(bIsConnected)
      mp_wxsocketclient->SetTimeout(
        //seconds
        10);
    return bIsConnected;
  }
  LOGN( FULL_FUNC_NAME << "--return 0")
  return //0 ;
    false;
}

void wxServiceSocketClient::Disconnect()
{
  if(mp_wxsocketclient)
    mp_wxsocketclient->Close();
}

bool wxServiceSocketClient::GetResponse()
{
  LOGN("wxServiceSocketClient::GetResponse begin")
  if( m_arbyIPCdata )
    delete [] m_arbyIPCdata ;
  //s.Append(_("wxSOCKET_INPUT\n"));
  if( AllBytesRead( & m_dwIPCdataSizeInByte, 4 ) )
  {
    m_arbyIPCdata = new BYTE[ m_dwIPCdataSizeInByte ] ;
    if( m_arbyIPCdata && AllBytesRead( & m_arbyIPCdata, m_dwIPCdataSizeInByte )
      )
    {
      return true ;
    }
  }
  return false ;
}

BYTE wxServiceSocketClient::Init()
{
  LOGN( FULL_FUNC_NAME << "--begin")
// Create the socket
  mp_wxsocketclient = new wxSocketClient();

  if( mp_wxsocketclient )
  {
    LOGN("Successfully created a socket client object.")
    mp_wxsocketclient->SetTimeout(
      //int seconds
      10);
    // Setup the event handler and subscribe to most events
    mp_wxsocketclient->SetEventHandler( * this, SOCKET_ID);

    mp_wxsocketclient->SetNotify(
      wxSOCKET_CONNECTION_FLAG |
      wxSOCKET_INPUT_FLAG |
      wxSOCKET_LOST_FLAG
      );
    //According to the notify value, this function enables or disables socket
    //events. If notify is true, the events configured with SetNotify will be
    //sent to the application. If notify is false; no events will be sent.
    mp_wxsocketclient->Notify(true);
    LOGN("End of socket client init.")
  }

  //mp_wxsocketclient->Connect(addr, 
  //  false //blocking mode == true
  //  );
  //mp_wxsocketclient->WaitOnConnect(10);

  //if (mp_wxsocketclient->IsConnected())
  LOGN( FULL_FUNC_NAME << "--return 1")
  return 1 ;
}

bool wxServiceSocketClient::IsConnected()
{
  LOGN("wxServiceSocketClient::IsConnected begin")
  bool bIsConnected = false;
  //May be NULL.
  if( mp_wxsocketclient )
  {
    bIsConnected = mp_wxsocketclient->IsConnected();
//    return bIsConnected ;
  }
//  return false ;
  LOGN("wxServiceSocketClient::IsConnected return" << bIsConnected)
  return bIsConnected;
}

void wxServiceSocketClient::OnSocketEvent(wxSocketEvent & r_wxsocketevent )
{
  LOGN("wxServiceSocketClient::OnSocketEvent begin")
  enum wxSocketNotify e_wxsocketnotify = r_wxsocketevent.GetSocketEvent();
  switch( e_wxsocketnotify )
  {
    case wxSOCKET_INPUT:
      {
      LOGN("wxSOCKET_INPUT")
//      GetResponse() ;
      bool bAllBytesRead = false;
      BYTE bProcessingIPCdata = false;

      m_crit_secIPCdata.Enter();
      bProcessingIPCdata = m_bProcessingIPCdata;
//      m_bReadingIPCdata = true;
      m_crit_secIPCdata.Leave();

      if( bProcessingIPCdata != IPC_Client::ReadNextIPCdata )
      {
        DWORD dwIPCdataSizeInByte;
        BYTE by;
        bAllBytesRead = AllBytesRead( & dwIPCdataSizeInByte, 4 );
        while( ! AllBytesRead(& by, 1) );
      }
      else
      {
        //Prevent accessing the array in another thread at the same time.
        m_crit_secIPCdata.Enter();
        if( m_dwIPCdataSizeInByte == 0 || m_bReadIPCdata )
        {
          if( m_arbyIPCdata )
          {
            delete [] m_arbyIPCdata;
            m_arbyIPCdata = NULL;
          }
          bAllBytesRead = AllBytesRead( & m_dwIPCdataSizeInByte, 4 );
          if( bAllBytesRead )
          {
            if( m_dwIPCdataSizeInByte < 10000 )
            {
              m_arbyIPCdata = new BYTE[ m_dwIPCdataSizeInByte ] ;
              m_bReadIPCdata = false;
            }
            else
            {
              m_dwIPCdataSizeInByte += 0;
//              BYTE by;
//              while( ! AllBytesRead(& by, 1) );
            }
          }
        }
        else
        {
          if( m_arbyIPCdata)
            bAllBytesRead = AllBytesRead( m_arbyIPCdata,
              m_dwIPCdataSizeInByte );

          //For the next input.
  //        m_dwIPCdataSizeInByte = 0;
          m_bReadIPCdata = true;
          m_bProcessingIPCdata = IPC_Client::IPCdataNotProcessed;
        }
        m_crit_secIPCdata.Leave();
      }
//      mp_wxsocketclient->LastCount() ;
      }
      break;
    case wxSOCKET_LOST       : 
      LOGN("wxSOCKET_LOST")
      //s.Append(_("wxSOCKET_LOST\n")); 
      break;
    case wxSOCKET_CONNECTION : 
      LOGN("wxSOCKET_CONNECTION")
      //s.Append(_("wxSOCKET_CONNECTION\n")); 
      break;
    default                  : 
      //s.Append(_("Unexpected event !\n")); 
      break;
  }
}

BYTE wxServiceSocketClient::SendCommandAndGetResponse(BYTE byMessage)
{
  LOGN("wxServiceSocketClient::SendCommandAndGetResponse("
    << (WORD) byMessage << ") begin")
  //May be NULL.
  if( mp_wxsocketclient )
  {
    mp_wxsocketclient->Write( & byMessage , 1 ) ;
    return GetResponse() ;
//      if( m_arbyIPCdata )
//        return IPC
  }
  return 1 ;
}

BYTE wxServiceSocketClient::SendCommandAndGetResponse(IPC_data & r_ipc_data)
{
  if( mp_wxsocketclient && IsConnected() )
  {
    BYTE bProcessingIPCdata;
    m_crit_secIPCdata.Enter();
    bProcessingIPCdata = m_bProcessingIPCdata;
    m_crit_secIPCdata.Leave();
    if( bProcessingIPCdata == IPC_Client:://IPCdataProcessed
        ReadNextIPCdata)
      mp_wxsocketclient->Write( & r_ipc_data.m_byCommand, 1 ) ;
//    return GetResponse() ;
    if( m_arbyIPCdata && m_bReadIPCdata)
      return IPC_Client:://GetsResponseNonBlocking;
        GotResponse;
  }
  return IPC_Client::FailedToGetResponse;
}

void wxServiceSocketClient::SetServerHostname(const char * c_p_ch )
{
//  std::string std_strServerHostname(c_p_ch);
  m_wxstrServerHostname = //getwxString(std_strServerHostname);
    GetwxString_Inline(c_p_ch);
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
    mp_wxsocketclient->Write( & by, 1 ) ;
  //mp_wxsocketclient->ReadMsg(const void * buffer, wxUint32 nbytes)
  return 1 ;
}

void wxServiceSocketClient::GetServerPathAndPort(
  const wxString & cr_wxstrURL )
{
  size_t size_tIndexOfColon = cr_wxstrURL.rfind( wxT(':') ) ;
  if( size_tIndexOfColon != //wxStringBase::npos
      wxString::npos
    )
  {
    m_wxstrServerHostname = cr_wxstrURL.substr( 0, size_tIndexOfColon ) ;
    LOGN("GetServerPathAndPort()--server host name:"
      << GetStdString( m_wxstrServerHostname) )
    wxString wxstrPortNumber = cr_wxstrURL.substr( size_tIndexOfColon + 1 ) ;
    LOGN("GetServerPathAndPort()--server port number:"
      << GetStdString( wxstrPortNumber) )
    //see http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString
    // #wxString_to_integer_number:
    if( ! wxstrPortNumber.ToULong( ( unsigned long *) & m_usServerPortNumber )
      )
    { /* error! */
      LOGN("Error converting the port number string to an unsigned long "
          "number")
    }
    else
      LOGN("Successfully converted the port number string to unsigned long "
          "number \"" << m_usServerPortNumber << "\"" )
  }
}

wxServiceSocketClient::wxServiceSocketClient( const wxString & cr_wxstrURL )
  : mp_wxsocketclient (NULL)
{
  InitializeMemberVariables();
  GetServerPathAndPort(cr_wxstrURL) ;
}

wxServiceSocketClient::wxServiceSocketClient(
  std::string & r_strHostname 
  , WORD wPort)
  : mp_wxsocketclient (NULL)
  //Constructor that converts to wide string if appropriate
  , m_wxstrServerHostname( r_strHostname.c_str() ,wxConvLocal )
{
  m_usServerPortNumber = wPort ;
  InitializeMemberVariables();
}

wxServiceSocketClient::~wxServiceSocketClient()
{
  if( mp_wxsocketclient )
    mp_wxsocketclient->Close();
}
