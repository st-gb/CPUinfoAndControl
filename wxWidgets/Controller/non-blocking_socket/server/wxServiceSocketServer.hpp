/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

//#include <Controller/I_ServerProcess.hpp> //class I_ServerProcess
#include <Controller/IPC/I_IPC.hpp>//base class I_IPC_Server
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
#include <wx/app.h> //wxEvtHandler
#include <wx/event.h> //wxEvtHandler
#include <Controller/IPC/I_IPC.hpp> //class I_IPC_Server

//class I_IPC_Server ;
class I_ServerProcess ;
class wxSocketEvent ;
class wxSocketServer ;

//wxIMPLEMENT_APP(wxServiceSocketServer)

class wxServiceSocketServer
  :
  public I_IPC_Server
  //public I_ServerProcess
  //For receiving events in "OnSocketEvent(wxSocketEvent & ) ;" .
  , public wxEvtHandler
  //For receiving events in "OnSocketEvent(wxSocketEvent & ), event loop.
//  , public wxAppConsole
{
  unsigned short m_wNumberOfClients ;
  unsigned short m_usServerPortNumber ;
  I_ServerProcess * m_p_serverprocess ;
  wxSocketServer * m_p_wxsocketserver ;
public:
  //Overwrites BYTE I_IPC_Server::Init()
  BYTE Init() ;
  ////connects e.g. to the shared memory
  //ConnectToProvider() ;
  //SendMessage() ;
  void OnDisconnect() ;
//  //Implements the pure virtual function "int wxAppConsole::OnRun()"
//  int OnRun()
//  {
//    //wxAppConsole::MainLoop  (      )
//    MainLoop() ;
//    return 0 ;
//  }
//  int MainLoop()
//  {
//    //from http://max.deppert.de/?p=43 :
//    //"Achtung: Dies ist erst ab wxWidgets 2.9.0 möglich!"
//    // create event loop
//      wxEventLoop loop;
//      // order wxAppConsole to process events
//      ProcessPendingEvents();
//      // start created event loop
//      return loop.Run();
//
//  }
  //from / see wxWidgets "sockets" sample
  void OnServerEvent(wxSocketEvent & event) ;
  void OnSocketEvent(wxSocketEvent & event);
  bool IsConnected() ;
  void SetServerPortNumber( unsigned short usServerPortNumber)
  {
    m_usServerPortNumber = usServerPortNumber ;
  }
  BYTE SetServerPortNumberAndInit(unsigned short usServerPortNumber)
  {
    SetServerPortNumber(usServerPortNumber);
    return Init();
  }
  wxServiceSocketServer( //WORD wPortNumber ,
    I_ServerProcess * p_serverprocess
    ) ;
  ~wxServiceSocketServer() ;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};
