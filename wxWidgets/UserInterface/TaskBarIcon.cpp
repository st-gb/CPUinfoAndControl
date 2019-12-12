/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* TaskBarIcon.cpp
 *
 *  Created on: Apr 26, 2010
 *      Author: Stefan
 *
 * from wxWidgets' "tbtest" sample: tbtest.cpp
// Created:     01/02/97
// RCS-ID:      $Id: tbtest.cpp 36336 2005-12-03 17:55:33Z vell $
 */
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/taskbar.h"
//#include <wx/string.h> //wxString::Format(...)
#include "TaskBarIcon.hpp"

#include <preprocessor_macros/logging_preprocessor_macros.h> //for LOGN(...)
//format_output_data(...)
#include <Controller/character_string/format_as_string.hpp>
#ifdef __WXMSW__
  #include <OperatingSystem/Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
  #include <OperatingSystem/Windows/PowerProfAccess/PowerProfDynLinked.hpp>
#endif //#ifdef __WXMSW__
  #include <wxWidgets/App.hpp> //wxGetApp()
  //getwxString(...)
  #include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp>

#define FIRST_CPU_CORE_THROTTLE_TEMPERATURE 40

enum {
    PU_RESTORE = //10001,
        1,
    PU_EXIT,
//    PU_CHECKMARK,
//    PU_SUB1,
//    PU_SUB2,
    SELECT_CPU_THROTTLE_TEMPERATURE,
    SELECT_MAXIMUM_CPU_CORE_MULTIPLIER,
    SELECT_POWER_SCHEME ,
    lastStaticEventID
};

WORD TaskBarIcon::s_wEventID = lastStaticEventID ;

BEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, TaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    TaskBarIcon::OnMenuExit)
//    EVT_MENU(PU_NEW_ICON,TaskBarIcon::OnMenuSetNewIcon)
//    EVT_MENU(PU_OLD_ICON,TaskBarIcon::OnMenuSetOldIcon)
//    EVT_MENU(PU_CHECKMARK,TaskBarIcon::OnMenuCheckmark)
//    EVT_UPDATE_UI(PU_CHECKMARK,TaskBarIcon::OnMenuUICheckmark)
//    EVT_TASKBAR_LEFT_DCLICK  (TaskBarIcon::OnLeftButtonDClick)
    EVT_TASKBAR_LEFT_DOWN  (TaskBarIcon::OnLeftButtonClick)
//    EVT_MENU(PU_SUB1, TaskBarIcon::OnMenuSub)
//    EVT_MENU(PU_SUB2, TaskBarIcon::OnMenuSub)
END_EVENT_TABLE()

void TaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
//  if( mp_mainframe )
//    mp_mainframe->Show(true);
  ShowMainFrame();
}

void TaskBarIcon::OnMenuExit(wxCommandEvent& )
{
  mp_mainframe->Close(true);
}
//
//static bool check = true;
//
//void TaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
//{
//       check =!check;
//}
//
//void TaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
//{
//       event.Check( check );
//}

//void TaskBarIcon::OnMenuSub(wxCommandEvent&)
//{
//    wxMessageBox(wxT("You clicked on a submenu!"));
//}

wxMenu * TaskBarIcon::CreateSetThrottleTemperatureMenu()
{
  LOGN(/*"CreateSetThrottleTemperatureMenu "*/ "begin")
  bool bSetCheckMarkForTemperature = false;
//  wxMenu * m_p_wxmenuThrottleTemperatures = new wxMenu();
//  if( ! m_p_wxmenuThrottleTemperatures )
    m_p_wxmenuThrottleTemperatures = new wxMenu();
  wxString wx_str;

  if( ! m_1stThrottleCPUcoreTemperatureEventID )
    m_1stThrottleCPUcoreTemperatureEventID = s_wEventID;
  else
    s_wEventID = m_1stThrottleCPUcoreTemperatureEventID;
  LOGN("1st CPU core throttle temperature event ID: " <<
      m_1stThrottleCPUcoreTemperatureEventID )
  for(BYTE byTemperature = //40
      FIRST_CPU_CORE_THROTTLE_TEMPERATURE; byTemperature < 100; byTemperature += 2)
  {
    wx_str = wxString::Format( wxT("%u"), byTemperature);
    m_p_wxmenuThrottleTemperatures->AppendRadioItem( s_wEventID , wx_str );

    //Set mark if >= because (and not simply "==") the temperature is as
    //integer here and might be configured as floating point value.
    if( ! bSetCheckMarkForTemperature && (float) byTemperature >=
        wxGetApp().m_model.m_cpucoredata.m_fThrottleTempInDegCelsius
       )
    {
      m_p_wxmenuThrottleTemperatures->Check( s_wEventID, true );
      bSetCheckMarkForTemperature = true;
    }
//    m_stdmap_eventid2powerschemename.insert(std::pair<WORD,std::wstring>
//      (wEventID, * std_set_std_wstr_c_iterPowerSchemeName) ) ;

    Connect( //s_wEventID ++
        s_wEventID , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnSetThrottleTemperature)
      );
    LOGN("Connected event ID " << s_wEventID << " to "
      "\"OnSetThrottleTemperature\"")
    ++ s_wEventID;
  }
  m_wAfterLastThrottleCPUcoreTemperatureEventID = s_wEventID;
  LOGN("Event ID after last CPU core throttle temperature event ID: " <<
      m_wAfterLastThrottleCPUcoreTemperatureEventID )
  return m_p_wxmenuThrottleTemperatures;
}

wxMenu * TaskBarIcon::CreateSetMaximumCPUcoreMultiplierMenu()
{
  LOGN(/*"CreateSetMaximumCPUcoreMultiplierMenu "*/ "begin")
//  bool bSetCheckMarkForCPUcoreMultiplier = false;
//  wxMenu * m_p_wxmenuThrottleTemperatures = new wxMenu();
//  if( ! m_p_wxmenuThrottleTemperatures )
    m_p_wxmenuCPUcoreMultipliers = new wxMenu();
  wxString wx_strContextMenuLabel;

  if( ! m_1stSetMaximumCPUcoreMultiplierEventID )
    m_1stSetMaximumCPUcoreMultiplierEventID = s_wEventID;
  else
    s_wEventID = m_1stSetMaximumCPUcoreMultiplierEventID;
  LOGN("1st CPU core throttle temperature event ID: " <<
      m_1stSetMaximumCPUcoreMultiplierEventID )
  std::set<float> & r_std_set_floatAvailableMultipliers = wxGetApp().m_model.
    m_cpucoredata.m_stdset_floatAvailableMultipliers;
  for(std::set<float>::const_iterator c_iter =
      r_std_set_floatAvailableMultipliers.begin(); c_iter !=
          r_std_set_floatAvailableMultipliers.end(); ++ c_iter
     )
  {
    wx_strContextMenuLabel = wxString::Format( wxT("%f"), * c_iter);
    m_p_wxmenuCPUcoreMultipliers->AppendRadioItem( s_wEventID ,
      wx_strContextMenuLabel );

    //Set mark if >= because (and not simply "==") the temperature is as
    //integer here and might be configured as floating point value.
    if( * c_iter == wxGetApp().m_model.m_cpucoredata.m_fMaximumCPUcoreMultiplier
       )
    {
      m_p_wxmenuCPUcoreMultipliers->Check( s_wEventID, true );
    }
    Connect( //s_wEventID ++
        s_wEventID , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnSetMaximumCPUcoreMultiplier)
      );
    LOGN("Connected event ID " << s_wEventID << " to "
      "\"OnSetMaximumCPUcoreMultiplier\"")
    ++ s_wEventID;
  }
//  m_wAfterLastThrottleCPUcoreTemperatureEventID = s_wEventID;
//  LOGN("Event ID after last CPU core throttle temperature event ID: " <<
//      m_wAfterLastThrottleCPUcoreTemperatureEventID )
  return m_p_wxmenuCPUcoreMultipliers;
}

// Overridables
//http://docs.wxwidgets.org/stable/wx_wxtaskbaricon.html#wxtaskbariconpopupmenu:
//"This method is called by the library when the user requests popup menu (on
//Windows and Unix platforms, this is when the user right-clicks the icon).
//Override this function in order to provide popup menu associated with the
//icon"

//If CreatePopupMenu returns NULL (this happens by default), no menu is shown,
//otherwise the menu is displayed and then deleted by the library as soon as
//the user dismisses it. The events can be handled by a class derived from
//wxTaskBarIcon.
wxMenu * TaskBarIcon::CreatePopupMenu()
{
  LOGN(/*"TaskBarIcon::CreatePopupMenu() "*/ "begin")
  // Try creating menus different ways
  // TODO: Probably try calling SetBitmap with some XPMs here

  //The menus need to be created each time this function is called because
  //http://docs.wxwidgets.org/2.8/wx_wxtaskbaricon.html#wxtaskbariconcreatepopupmenu:
  //"the menu is displayed and then deleted by the library as soon as
    //the user dismisses it."
  //wxMenu * p_wxmenu = new wxMenu;
  p_wxmenu = new wxMenu;
  p_wxmenu->Append(PU_RESTORE, _T("&show main window"));
  p_wxmenu->AppendSeparator();
//  if( ! m_p_wxmenuThrottleTemperatures )
    CreateSetThrottleTemperatureMenu();
  LOGN(//"TaskBarIcon::CreatePopupMenu()--"
    "before appending the throttle menu "
    << m_p_wxmenuThrottleTemperatures << " to the root menu")
  p_wxmenu->Append(SELECT_CPU_THROTTLE_TEMPERATURE,
    wxT("select CPU core throttle temperature"),
    m_p_wxmenuThrottleTemperatures);

  CreateSetMaximumCPUcoreMultiplierMenu();
  p_wxmenu->Append(SELECT_MAXIMUM_CPU_CORE_MULTIPLIER,
    wxT("select maximum CPU core multiplier"),
    m_p_wxmenuCPUcoreMultipliers);

//    menu->Append(PU_CHECKMARK, _T("Checkmark"),wxT(""), wxITEM_CHECK);
//    menu->AppendSeparator();
//    wxMenu * wxmenuProfiles = new wxMenu;
//    wxmenuProfiles->Append(PU_SUB1, _T("One submenu"));
//    wxmenuProfiles->AppendSeparator();
//    wxmenuProfiles->Append(PU_SUB2, _T("Another submenu"));
//    menu->Append(SELECT_POWER_SCHEME, _T("select profile"), wxmenuProfiles);
#ifdef __WXMSW__
  wxMenu * p_wxmenuPowerSchemes = CreatePowerSchemesMenu() ;
  p_wxmenu->Append(SELECT_POWER_SCHEME, _T("select Windows &power scheme"),
    p_wxmenuPowerSchemes);
#endif //#ifdef __WXMSW__
#ifndef __WXMAC_OSX__ /*Mac has built-in quit menu*/
    p_wxmenu->AppendSeparator();
    p_wxmenu->Append(PU_EXIT,    _T("E&xit"));
#endif
  LOGN(/*"TaskBarIcon::CreatePopupMenu() "*/ "end")
  return p_wxmenu;
}

wxMenu * TaskBarIcon::CreatePowerSchemesMenu()
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  LOGN(/*"TaskBarIcon::CreatePowerSchemesMenu() "*/ "begin")

  wxMenu * p_wxmenuPowerSchemes = new wxMenu;

  if( ! m_1stSelectPowerSchemeMenuEventID )
    m_1stSelectPowerSchemeMenuEventID = s_wEventID;
  else
    s_wEventID = m_1stSelectPowerSchemeMenuEventID;
  LOGN("1st select power scheme event ID: " <<
      m_1stThrottleCPUcoreTemperatureEventID )

  wxX86InfoAndControlApp & r_wxx86infoandcontrolapp = wxGetApp();
  int nNumPowerSchemeNamesFromMemory = r_wxx86infoandcontrolapp.
    m_std_vec_std_wstrPowerSchemeName.size();
  LOGN( "# power scheme names from memory: " <<
    nNumPowerSchemeNamesFromMemory)
  if( nNumPowerSchemeNamesFromMemory > 0)
  {
    CreatePowerSchemeMenuByNamesFromMemory(p_wxmenuPowerSchemes);
  }
  else
    CreatePowerSchemeMenuByRetrieving(p_wxmenuPowerSchemes);

  m_wAfterLastSelectPowerSchemeMenuEventID = s_wEventID;
  LOGN(//"TaskBarIcon::CreatePowerSchemesMenu() --"
    "return " << p_wxmenuPowerSchemes )
//  wxmenuPowerSchemes->Check( lastStaticEventID , true ) ;
  return p_wxmenuPowerSchemes;
#else //#ifdef _WIN32
  return NULL;
#endif //#ifdef _WIN32
}

#ifdef _WIN32
void TaskBarIcon::CreatePowerSchemeMenuByNamesFromMemory(
  wxMenu * p_wxmenuPowerSchemes)
{
  LOGN( "begin")
  wxX86InfoAndControlApp & r_wxx86infoandcontrolapp = wxGetApp();

  std::vector<std::wstring> & r_std_vec_std_wstrPowerSchemeName =
    r_wxx86infoandcontrolapp.m_std_vec_std_wstrPowerSchemeName;

  r_wxx86infoandcontrolapp.GetAvailablePowerSchemesViaIPC(
      r_std_vec_std_wstrPowerSchemeName,
      r_wxx86infoandcontrolapp.m_ui16ActivePowerSchemeIndex);

  std::vector<std::wstring>::const_iterator
    c_iter_std_wstr_c_iterPowerSchemeName =
    r_std_vec_std_wstrPowerSchemeName.begin();
  uint16_t ui16Index = 0;
  LOGN( "active power scheme index:" <<
      r_wxx86infoandcontrolapp.m_ui16ActivePowerSchemeIndex)
//  std::wstring * p_wstrActivePowerScheme = NULL;
  while( c_iter_std_wstr_c_iterPowerSchemeName !=
      r_std_vec_std_wstrPowerSchemeName.end() )
  {
    wxString wxstrPowerSchemeName( wxWidgets::getwxString(
      * c_iter_std_wstr_c_iterPowerSchemeName) ) ;
//    wxmenuPowerSchemes->Append( wEventID , wxstr );
    p_wxmenuPowerSchemes->AppendRadioItem( s_wEventID , wxstrPowerSchemeName );

    if( ui16Index == r_wxx86infoandcontrolapp.m_ui16ActivePowerSchemeIndex)
//        p_wstrActivePowerScheme = & *c_iter_std_wstr_c_iterPowerSchemeName;
      p_wxmenuPowerSchemes->Check( s_wEventID, true ) ;

    m_stdmap_eventid2powerschemename.insert(std::pair<WORD,std::wstring>
      (s_wEventID, * c_iter_std_wstr_c_iterPowerSchemeName) ) ;
    LOGN(//"TaskBarIcon::CreatePowerSchemesMenu()--"
      "adding event with event ID " << s_wEventID )
    Connect( s_wEventID ++ , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnDynamicallyCreatedUIcontrol)
      );

    ++ ui16Index;
    ++ c_iter_std_wstr_c_iterPowerSchemeName;
  }
  LOGN( "end")
}

void TaskBarIcon::CreatePowerSchemeMenuByRetrieving(
    wxMenu * p_wxmenuPowerSchemes)
{
  LOGN( "begin")
  std::set<std::wstring> stdset_std_wstrPowerSchemeName ;
  std::wstring wstrActivePowerScheme ;
  PowerProfDynLinked * p_powerprofdynlinked =
   (PowerProfDynLinked * ) wxGetApp().mp_dynfreqscalingaccess ;
  p_powerprofdynlinked->GetAllPowerSchemeNames(stdset_std_wstrPowerSchemeName) ;
  p_powerprofdynlinked->GetActivePowerSchemeName(wstrActivePowerScheme) ;
  LOGWN_WSPRINTF(L"active power scheme name:%ls",
    wstrActivePowerScheme.c_str() )

  for( std::set<std::wstring>::const_iterator
      c_iter_std_set_std_wstr_c_iterPowerSchemeName =
      stdset_std_wstrPowerSchemeName.begin() ;
      c_iter_std_set_std_wstr_c_iterPowerSchemeName !=
      stdset_std_wstrPowerSchemeName.end() ;
      ++ c_iter_std_set_std_wstr_c_iterPowerSchemeName
    )
  {
    //TODO wxString should be unicode for e.g. Chinese language (more than 255
    // characters)
    wxString wxstrPowerSchemeName( wxWidgets::getwxString(
      * c_iter_std_set_std_wstr_c_iterPowerSchemeName) ) ;
//    wxmenuPowerSchemes->Append( wEventID , wxstr );
    p_wxmenuPowerSchemes->AppendRadioItem( s_wEventID , wxstrPowerSchemeName );
    if( wstrActivePowerScheme == * c_iter_std_set_std_wstr_c_iterPowerSchemeName )
      p_wxmenuPowerSchemes->Check( s_wEventID, true ) ;
    m_stdmap_eventid2powerschemename.insert(std::pair<WORD,std::wstring>
      (s_wEventID, * c_iter_std_set_std_wstr_c_iterPowerSchemeName) ) ;
    LOGN(//"TaskBarIcon::CreatePowerSchemesMenu()--"
      "adding event with event ID " << s_wEventID )
    Connect( s_wEventID ++ , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnDynamicallyCreatedUIcontrol)
      );
  }
  LOGN( "end")
}
#endif //#ifdef _WIN32

void TaskBarIcon::FreeRessources()
{
  LOGN(/*"TaskBarIcon::FreeRessources() "*/ "begin--m_p_wxicon_drawer:"
    << m_p_wxicon_drawer)
  if( m_p_wxicon_drawer )
  {
//    delete m_p_wxicon_drawer;
//    m_p_wxicon_drawer = NULL;
  }
  LOGN(/*"TaskBarIcon::FreeRessources() "*/ "end")
}

float TaskBarIcon::GetTemperatureViaMenuLabel(
    int nEventID//, //wxString & r_wxstrMenuLabel
//    fTemperatureInDegC
    )
{
  wxString wxstrMenuLabel;
  // in "src/common/menucmn.cpp", line 612:
  //GetLabel() failed at once in wxString wxMenuBase::GetLabel( int id ) const:,
  // because the context menu is destroyed immediately after its creation?
  //Afterwards the taskbar icon context menu popped up no more.
//  r_wxstrMenuLabel
  wxstrMenuLabel = m_p_wxmenuThrottleTemperatures->
    //see http://docs.wxwidgets.org/trunk/classwx_menu.html#e912f9dec96a0bd585fe562938447d7d
    GetLabel(nEventID);
  LOGN(/*"taskbar icon--"*/ "requested to set CPU core throttle temperature of "
    << wxWidgets::GetStdString( wxstrMenuLabel) << " degrees Celsius" )
  double dTemperatureInDegC;
  wxstrMenuLabel.ToDouble( & dTemperatureInDegC);
  float fTemperatureInDegC = (float) dTemperatureInDegC;
  return fTemperatureInDegC;
}

void TaskBarIcon::OnSetMaximumCPUcoreMultiplier(wxCommandEvent & wxevent)
{
  int nEventID = wxevent.GetId() ;
  LOGN( "event ID:" << nEventID)
//  wxString wxstrMenuLabel;
//  GetTemperatureViaMenuLabel(nEventID, wxstrMenuLabel);

//  m_1stThrottleCPUcoreTemperatureEventID
  float fMaximumCPUcoreMultiplier = wxGetApp().m_model.m_cpucoredata.
    m_arfAvailableMultipliers[nEventID -
                              m_1stSetMaximumCPUcoreMultiplierEventID];
  LOGN( "requested to set maximum CPU core multiplier to "
    << fMaximumCPUcoreMultiplier )
  std::string std_str = format_output_data(
    (BYTE *) & fMaximumCPUcoreMultiplier, sizeof(fMaximumCPUcoreMultiplier),
    80);
  LOGN( "data to send to the service:" << std_str)
  wxGetApp().IPC_ClientSendCommandAndGetResponse(
    setMaximumCPUcoreMultplier, //4
    //size of float(4 byte) + command byte
    5
    //sizeof(fMaximumCPUcoreMultiplier)
    , (BYTE *) & fMaximumCPUcoreMultiplier
    );
  wxGetApp().m_model.m_cpucoredata.m_fMaximumCPUcoreMultiplier =
    fMaximumCPUcoreMultiplier;
}

void TaskBarIcon::OnSetThrottleTemperature(wxCommandEvent & wxevent)
{
  int nEventID = wxevent.GetId() ;
  LOGN( "event ID:" << nEventID)
//  wxString wxstrMenuLabel;
//  GetTemperatureViaMenuLabel(nEventID, wxstrMenuLabel);

//  m_1stThrottleCPUcoreTemperatureEventID
  float fTemperatureInDegC = //(float) dTemperatureInDegC;
    FIRST_CPU_CORE_THROTTLE_TEMPERATURE + ( nEventID -
      m_1stThrottleCPUcoreTemperatureEventID ) * 2;
  LOGN(//"taskbar icon--"
      "requested to set CPU core throttle temperature of "
    << fTemperatureInDegC << " degrees Celsius"
    )
  std::string std_str = format_output_data( (BYTE *) & fTemperatureInDegC, 4,
    80);
  LOGN(//"TaskBarIcon::OnSetThrottleTemperature--"
      "data to send to the service:"
    << std_str)
  wxGetApp().IPC_ClientSendCommandAndGetResponse(
    setCPUcoreThrottleTemperature, //4,
    //size of float(4 byte) + command byte
    5,
    (BYTE *) & fTemperatureInDegC
    );
  wxGetApp().m_model.m_cpucoredata.m_fThrottleTempInDegCelsius =
    fTemperatureInDegC;
}

void TaskBarIcon::OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent)
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  int nEventID = wxevent.GetId() ;
  LOGN( /*"taskbar icon event proc "*/ "begin" )

  std::map<WORD,std::wstring>::const_iterator c_iter =
    m_stdmap_eventid2powerschemename.find(nEventID) ;
  if( c_iter != m_stdmap_eventid2powerschemename.end() )
  {
//    LOGN( "setting power scheme" << c_iter->second)
//      LOGWN( L"before setting power scheme with name \"" << c_iter->second
//        L"\" as active scheme" )
    //TODO if own DVFS in GUI or service running: ask for pausing/ ending it
//      wxGetApp().PossiblyAskForOSdynFreqScalingDisabling() ;
//      PerCPUcoreAttributes * p_percpucoreattributes = & mp_cpucoredata->
//        m_arp_percpucoreattributes[ //p_atts->m_byCoreID
//        0 ] ;
//      //DynFreqScalingThread * p_dynfreqscalingthread
//      if ( p_percpucoreattributes->mp_dynfreqscalingthread )
//        mp_mainframe->EndDynVoltAndFreqScalingThread(p_percpucoreattributes) ;

    if( wxGetApp().m_std_vec_std_wstrPowerSchemeName.size() > 0 )
    {
      wxGetApp().SetPowerSchemeViaIPC(c_iter->second);
    }
    else
    {
//      SetPowerSchemeDirectly();
      PowerProfDynLinked * p_powerprofdynlinked =
       (PowerProfDynLinked * ) wxGetApp().mp_dynfreqscalingaccess ;
      if( p_powerprofdynlinked )
      {
        LOGN( "valid pointer to power prof access class instance" )

        //BYTE by
        DWORD dw = p_powerprofdynlinked->SetActivePowerScheme(c_iter->second) ;
        LOGN( "result of setting power scheme:" << //(WORD) by
          dw )
        if( //by == 1
          dw == 0 )
        {
          LOGN( "setting power scheme succeeded" )
        }
        else
        {
          // error message for error number 65535 causes an exception?!
          if( dw != 65535 )
          {
            std::string stdstr = ::LocalLanguageMessageFromErrorCodeA( dw ) ;
            ::wxMessageBox( wxT("setting power scheme failed: ")
              + wxWidgets::getwxString(stdstr ) ) ;
            LOGN( "setting power scheme failed" )
          }
        }
      }
    }
  }
#endif //#ifdef _WIN32
}

void TaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
  LOGN("left mouse click on system tray icon")
  if( mp_mainframe->IsVisible() )
    mp_mainframe->Hide();
  else
    ShowMainFrame();
}

// wxWidgets' src/common/wincmn.cpp, Zeile 334:
// "Any additional event handlers should be popped before the window is
// deleted as otherwise the last handler will be left with a dangling
// pointer to this window result in a difficult to diagnose crash later on."
void TaskBarIcon::DisconnectEventHandlers()
{
  LOGN(/*"TaskBarIcon::DisconnectEventHandlers() "*/ "begin")
  DisconnectSelectPowerSchemeEventHandlers();
  DisconnectOnSetThrottleTemperatureEventHandlers();
  LOGN(/*"TaskBarIcon::DisconnectEventHandlers() "*/ "end")
}

void TaskBarIcon::DisconnectSelectPowerSchemeEventHandlers()
{
  if( Disconnect( //s_wEventID ++ , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnDynamicallyCreatedUIcontrol)
      )
    )
    LOGN("OnDynamicallyCreatedUIcontrol() has been removed as event handler.")
  else
    LOGN_ERROR("Failed to remove \"OnDynamicallyCreatedUIcontrol()\" as event "
      "handler.")
  //If the 1st event ID was set.
  if( m_1stThrottleCPUcoreTemperatureEventID )
  {
    for(WORD wEventID = m_1stSelectPowerSchemeMenuEventID;
        wEventID < m_wAfterLastSelectPowerSchemeMenuEventID; ++ wEventID)
    {
      if( Disconnect(wEventID) )
        LOGN("\"OnDynamicallyCreatedUIcontrol\" has been removed as event "
          "handler for event ID \"" << wEventID << "\"" )
      else
        LOGN_ERROR("Failed to remove \"OnDynamicallyCreatedUIcontrol\" as event "
          "handler for event ID \"" << wEventID << "\"" )
    }
  }
}

void TaskBarIcon::DisconnectOnSetThrottleTemperatureEventHandlers()
{
  LOGN(/*"DisconnectOnSetThrottleTemperatureEventHandlers "*/ "begin")
  if( Disconnect( //s_wEventID ++ , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnSetThrottleTemperature)
      )
    )
    LOGN("\"OnSetThrottleTemperature\" has been removed as event handler.")
  else
    LOGN_ERROR("Failed to remove \"OnSetThrottleTemperature\" as event handler.")
  //If the 1st event ID was set.
  if( m_1stThrottleCPUcoreTemperatureEventID )
  {
    for(WORD wEventID = m_1stThrottleCPUcoreTemperatureEventID;
        wEventID < m_wAfterLastThrottleCPUcoreTemperatureEventID; ++ wEventID)
    {
      if( Disconnect(wEventID) )
        LOGN("\"OnSetThrottleTemperature\" has been removed as event handler "
          "for event ID \"" << wEventID << "\"" )
      else
        LOGN_ERROR("Failed to remove \"OnSetThrottleTemperature\" as event handler "
          "for event ID \"" << wEventID << "\"" )

    }
  }
  LOGN(/*"DisconnectOnSetThrottleTemperatureEventHandlers "*/ "end")
}

void TaskBarIcon::ShowMainFrame()
{
  if( mp_mainframe )
  {
    mp_mainframe->Show(true);
    //from http://stackoverflow.com/questions/2550660/how-can-i-ensure-that-a-wxframe-is-brought-to-the-foreground:
    mp_mainframe->
      //http://docs.wxwidgets.org/trunk/classwx_window.html#54808c933f22a891c5db646f6209fa4d:
      //"Notice that this function only requests the window manager to raise
      //this window to the top of Z-order. Depending on its configuration,
      //the window manager may raise the window, not do it at all or indicate
      //that a window requested to be raised in some other way, e.g. by
      //flashing its icon if it is minimized."
      Raise();
    mp_mainframe->Maximize(
      //bool maximize=true
      //"If true, maximizes the window, otherwise it restores it."
      false //->"restore the window (window size is < maximized size(?) )
      );
  //    mp_mainframe->RequestUserAttention (//int flags=wxUSER_ATTENTION_INFO
  //      );
    LOGN("after calling to show the main frame")
  }
  //  mp_mainframe->Maximize(true ) ;
}

TaskBarIcon::~TaskBarIcon()
{
  LOGN(/*"~TaskBarIcon() "*/ "(" << this << ") begin")
  //Disconnect event handlers to avoid a program crash:
  // when exiting in wxWidgets' /src/common/wincmn.cpp, Zeile 334:
     //(Event-) handler ist nicht mehr da:
  // ->die GUI belegt 700 MB Speicher nach wenigen Sekunden/ Minuten

  DisconnectEventHandlers();
  //  m_wxicon_drawer.ReleaseRessources();
  FreeRessources();
  LOGN(/*"~TaskBarIcon() "*/ "end")
}
//void TaskBarIcon::SetMainFrame(MainFrame * p_mf )
//{
//  mp_mainframe = p_mf ;
//}
