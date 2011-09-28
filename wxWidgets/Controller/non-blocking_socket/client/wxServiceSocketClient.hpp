/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <wx/event.h> //wxEvtHandler
#include <Controller/IPC/I_IPC.hpp>
#include <windef.h> //BYTE, DWORD
#include <wx/msw/winundef.h>

#undef SendMessageA

class wxSocketClient ;
class wxSocketEvent ;

namespace NonBlocking
{
  class wxServiceSocketClient
    :
    //Must be inherited from a base class in order the change the type of a
    // pointer of an IPC_Client subclass at _runtime_ .
    public IPC_Client
    //For receiving events in "OnSocketEvent(wxSocketEvent & ) ;" .
    , public wxEvtHandler
  {
    BYTE m_byLastSentCommand ;
//    BYTE * m_arbyIPCdata ;
//    DWORD m_dwIPCdataSizeInByte ;
//    WORD m_wPortNumber ;
    bool m_bReadIPCdata;
    unsigned short m_usServerPortNumber ;
  public:
    wxSocketClient * mp_wxsocketclient ;
  private:
    wxString m_wxstrServerHostname ;
  public:
    inline bool AllBytesRead( void * p_vData, DWORD dwSize) ;
    void Disconnect();
    bool GetResponse() ;
    void GetServerPathAndPort( const wxString & cr_wxstrURL ) ;
    //e.g. connect to server.
    //Must be virtual for executing "IPC_Client"-derived ::ConnectToDataProvider().
    BYTE Init() ;
    bool IsConnected() ;
    // socket event handler
    void OnSocketEvent(wxSocketEvent & event);
    BYTE SendMessage(BYTE ) ;
    //connects e.g. to the shared memory
    BYTE ConnectToDataProvider(std::string & r_stdstrMessage) ;
    void InitializeMemberVariables()
    {
      m_bReadIPCdata = false;
      m_arbyIPCdata = NULL;
      m_dwIPCdataSizeInByte = 0;
    }
    BYTE SendCommandAndGetResponse(BYTE byMessage) ;
    BYTE SendCommandAndGetResponse(IPC_data & r_ipc_data);
    void SetServerHostname(const char *);
    void SetServerPortNumber( uint16_t ui16PortNumber)
    {
      m_usServerPortNumber = ui16PortNumber;
    }
    wxServiceSocketClient() { InitializeMemberVariables(); }
    wxServiceSocketClient( const wxString & cr_wxstrURL ) ;
    wxServiceSocketClient(
      std::string & r_strHostname
      , WORD wPort) ;
    ~wxServiceSocketClient() ;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
  };
}
