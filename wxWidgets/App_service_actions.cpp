/*
 * App_service_actions.cpp
 *
 *  Created on: 01.08.2011
 *      Author: Stefan
 */
#include "App.hpp"
//for getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

bool wxX86InfoAndControlApp::ContinueServiceViaIPC(wxString & r_wxstrMessage)
{
  std::string stdstrMessage ;
  //Lock concurrent access to p_i_ipcclient from another thread.
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    //mp_wxx86infoandcontrolapp->
      m_wxcriticalsectionIPCobject ) ;
  if( //! ::wxGetApp().m_ipcclient.IsConnected()
      ! //mp_wxx86infoandcontrolapp->
      IPC_ClientIsConnected()
    )
    //::wxGetApp().m_ipcclient.Init() ;
    //mp_wxx86infoandcontrolapp->
    m_p_i_ipcclient->ConnectToDataProvider(
      stdstrMessage ) ;
  if( //::wxGetApp().m_ipcclient.IsConnected()
      //mp_wxx86infoandcontrolapp->
      IPC_ClientIsConnected()
    )
  {
  //    ::wxGetApp().m_ipcclient.SendCommandAndGetResponse(continue_service) ;
  //    wxString wxstr = getwxString( ::wxGetApp().m_ipcclient.m_stdwstrMessage ) ;
  //    ::wxMessageBox( wxT("message from the service:\n") + wxstr ) ;
    if( //mp_wxx86infoandcontrolapp->
        m_p_i_ipcclient )
    {
      //mp_wxx86infoandcontrolapp->
        m_p_i_ipcclient->SendCommandAndGetResponse(
        continue_service) ;
      r_wxstrMessage = getwxString( //mp_wxx86infoandcontrolapp->
        m_p_i_ipcclient->m_stdwstrMessage ) ;
      return true;
    }
  }
  return false;
}
