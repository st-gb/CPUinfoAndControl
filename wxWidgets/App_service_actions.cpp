/*
 * App_service_actions.cpp
 *
 *  Created on: 01.08.2011
 *      Author: Stefan
 */
#include "App.hpp"
//for getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
//::format_output_data(...)
#include <Controller/character_string/format_as_string.hpp>

#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
bool wxX86InfoAndControlApp::ContinueServiceViaIPC(wxString & r_wxstrMessage)
{
  std::string stdstrMessage ;
  LOGN( "begin")
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
    LOGN( "IPC is connected")
  //    ::wxGetApp().m_ipcclient.SendCommandAndGetResponse(continue_service) ;
  //    wxString wxstr = getwxString( ::wxGetApp().m_ipcclient.m_stdwstrMessage ) ;
  //    ::wxMessageBox( wxT("message from the service:\n") + wxstr ) ;
//    if( //mp_wxx86infoandcontrolapp->
//        m_p_i_ipcclient )
//    {
      LOGN( "before sending the continue command")
      //mp_wxx86infoandcontrolapp->
//        m_p_i_ipcclient->SendCommandAndGetResponse(
//      IPC_ClientSendCommandAndGetResponse_Inline(
      IPC_ClientSendCommandAndGetResponse(
        continue_service) ;
      LOGN( "after sending the continue command")
      r_wxstrMessage = wxWidgets::getwxString( //mp_wxx86infoandcontrolapp->
        m_p_i_ipcclient->m_stdwstrMessage ) ;
      return true;
//    }
  }
  else
    LOGN( "IPC is NOT connected")
  return false;
}

bool wxX86InfoAndControlApp::GetAvailablePowerSchemesViaIPC(
  std::vector<std::wstring> & r_std_vec_std_wstrPowerSchemeName,
  uint16_t & r_ui16ActivePowerSchemeIndex
  )
{
  //Important if appended to multiple times.
  r_std_vec_std_wstrPowerSchemeName.clear();
  std::string stdstrMessage ;
  LOGN( "begin")
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
//    m_p_i_ipcclient->ConnectToDataProvider(
//      stdstrMessage ) ;
    IPCclientConnectToDataProvider(
      stdstrMessage);
  if( //::wxGetApp().m_ipcclient.IsConnected()
      //mp_wxx86infoandcontrolapp->
      IPC_ClientIsConnected()
    )
  {
    LOGN( "IPC is connected")
  //    ::wxGetApp().m_ipcclient.SendCommandAndGetResponse(continue_service) ;
  //    wxString wxstr = getwxString( ::wxGetApp().m_ipcclient.m_stdwstrMessage ) ;
  //    ::wxMessageBox( wxT("message from the service:\n") + wxstr ) ;
//    if( //mp_wxx86infoandcontrolapp->
//        m_p_i_ipcclient )
//    {
      LOGN( "before sending the getAvailablePowerSchemeNames command")
      //mp_wxx86infoandcontrolapp->
//        m_p_i_ipcclient->SendCommandAndGetResponse(
//      IPC_ClientSendCommandAndGetResponse_Inline(
      IPC_ClientSendCommandAndGetResponse(
        getAvailablePowerSchemeNames) ;
      LOGN( "after sending the getAvailablePowerSchemeNames command")

      wchar_t wch;
      std::wstring std_wstrPowerSchemeName;
      LOGN( "# of data bytes:" <<
          m_p_i_ipcclient->m_dwIPCdataSizeInByte)
      LOGN( "data from IPC: " <<
        ::format_output_data(m_p_i_ipcclient->m_arbyIPCdata,
          m_p_i_ipcclient->m_dwIPCdataSizeInByte, 80)
        )
      wchar_t * p_wch = (wchar_t *) m_p_i_ipcclient->m_arbyIPCdata;
      for( unsigned uiIndex = 0; uiIndex <
        m_p_i_ipcclient->m_dwIPCdataSizeInByte
        //active power scheme index number
        - 2;
        uiIndex += 2)
      {
//        wch = * ( (wchar_t *) m_p_i_ipcclient->m_arbyIPCdata + byIndex );
        wch = * p_wch;
        if( wch == L'\n')
        {
          LOGN( "adding power scheme name \"" <<
            ::GetStdString_Inline( std_wstrPowerSchemeName) << "\" to memory." )
          r_std_vec_std_wstrPowerSchemeName.push_back(std_wstrPowerSchemeName);
          std_wstrPowerSchemeName.clear();
        }
        else
        {
//          LOGN("adding character "
//            "\"" << wch << "\""
//            << "(" << (char)wch << ")"
//            << " to power scheme name")
          std_wstrPowerSchemeName += wch;
        }
        ++ p_wch;
      }
      r_ui16ActivePowerSchemeIndex = ( * (uint16_t *)
        (m_p_i_ipcclient->m_arbyIPCdata + m_p_i_ipcclient->
        m_dwIPCdataSizeInByte - 2)
        );
      LOGN( "active power scheme index:" <<
          r_ui16ActivePowerSchemeIndex)
    return true;
//    }
  }
  else
    LOGN( "IPC is NOT connected")
  return false;
}

bool wxX86InfoAndControlApp::SetPowerSchemeViaIPC(
    const std::wstring & r_std_wstrPowerSchemeName)
{
  LOGN( "begin")
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
    LOGN( "IPC is connected")

    IPC_ClientSendCommandAndGetResponse(
        setActivePowerScheme,
        //1 wchar_t has 2 bytes
        r_std_wstrPowerSchemeName.length() * 2
        //command byte
        + 1,
        (BYTE *) r_std_wstrPowerSchemeName.data()
        );
    LOGN( "after sending the continue command")
    return true;
  }
  return false;
}
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
