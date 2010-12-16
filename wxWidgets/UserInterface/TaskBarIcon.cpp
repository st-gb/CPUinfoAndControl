/*
 * TaskBarIcon.cpp
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
#include "TaskBarIcon.hpp"

#include <preprocessor_macros/logging_preprocessor_macros.h> //for LOGN(...)
#ifdef __WXMSW__
  #include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
  #include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
  #include <wxWidgets/App.hpp> //wxGetApp()
  //getwxString(...)
  #include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#endif
#include <wxWidgets/UserInterface/MainFrame.hpp>

enum {
    PU_RESTORE = //10001,
        1,
    PU_EXIT,
//    PU_CHECKMARK,
//    PU_SUB1,
//    PU_SUB2,
    SELECT_POWER_SCHEME ,
    LAST_ID
};

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
  if( mp_mainframe )
    mp_mainframe->Show(true);
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

// Overridables
//http://docs.wxwidgets.org/stable/wx_wxtaskbaricon.html#wxtaskbariconpopupmenu:
//"This method is called by the library when the user requests popup menu (on
//Windows and Unix platforms, this is when the user right-clicks the icon).
//Override this function in order to provide popup menu associated with the
//icon"
wxMenu * TaskBarIcon::CreatePopupMenu()
{
  // Try creating menus different ways
  // TODO: Probably try calling SetBitmap with some XPMs here
  //wxMenu * p_wxmenu = new wxMenu;
  p_wxmenu = new wxMenu;
  p_wxmenu->Append(PU_RESTORE, _T("&show window"));
  p_wxmenu->AppendSeparator();
//    menu->Append(PU_CHECKMARK, _T("Checkmark"),wxT(""), wxITEM_CHECK);
//    menu->AppendSeparator();
//    wxMenu * wxmenuProfiles = new wxMenu;
//    wxmenuProfiles->Append(PU_SUB1, _T("One submenu"));
//    wxmenuProfiles->AppendSeparator();
//    wxmenuProfiles->Append(PU_SUB2, _T("Another submenu"));
//    menu->Append(SELECT_POWER_SCHEME, _T("select profile"), wxmenuProfiles);
#ifdef __WXMSW__
    CreatePowerSchemesMenu() ;
#endif //#ifdef __WXMSW__
#ifndef __WXMAC_OSX__ /*Mac has built-in quit menu*/
    p_wxmenu->AppendSeparator();
    p_wxmenu->Append(PU_EXIT,    _T("E&xit"));
#endif
    return p_wxmenu;
}

void TaskBarIcon::CreatePowerSchemesMenu()
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  WORD wEventID = LAST_ID ;
  wxMenu * wxmenuPowerSchemes = new wxMenu;
  std::set<std::wstring> set_wstr ;
  std::wstring wstrActivePowerScheme ;
  PowerProfDynLinked * p_powerprofdynlinked =
   (PowerProfDynLinked * ) wxGetApp().mp_dynfreqscalingaccess ;
  p_powerprofdynlinked->GetAllPowerSchemeNames(set_wstr) ;
  p_powerprofdynlinked->GetActivePowerSchemeName(wstrActivePowerScheme) ;
  LOGWN_WSPRINTF(L"active power scheme name:%ls",
    wstrActivePowerScheme.c_str() )
  for( std::set<std::wstring>::const_iterator stdset_c_iter = set_wstr.begin() ;
      stdset_c_iter != set_wstr.end() ; ++ stdset_c_iter
    )
  {
    //TODO wxString should be unicode for e.g. Chinese language (more than 255
    // characters)
    wxString wxstr( getwxString(*stdset_c_iter) ) ;
//    wxmenuPowerSchemes->Append( wEventID , wxstr );
    wxmenuPowerSchemes->AppendRadioItem( wEventID , wxstr );
    if( wstrActivePowerScheme == *stdset_c_iter )
      wxmenuPowerSchemes->Check( wEventID, true ) ;
    m_stdmap_eventid2powerschemename.insert(std::pair<WORD,std::wstring>
      (wEventID,*stdset_c_iter) ) ;
    Connect( wEventID ++ , //wxID_ANY,
      wxEVT_COMMAND_MENU_SELECTED ,
      wxCommandEventHandler(TaskBarIcon::OnDynamicallyCreatedUIcontrol)
      );
  }
  p_wxmenu->Append(SELECT_POWER_SCHEME, _T("select Windows &power scheme"),
    wxmenuPowerSchemes);
//  wxmenuPowerSchemes->Check( LAST_ID , true ) ;
#endif //#ifdef _WIN32
}

void TaskBarIcon::OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent)
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  int nEventID = wxevent.GetId() ;
  LOGN( "taskbar icon event proc " )
  PowerProfDynLinked * p_powerprofdynlinked =
   (PowerProfDynLinked * ) wxGetApp().mp_dynfreqscalingaccess ;
  if( p_powerprofdynlinked )
  {
    LOGN( "valid pointer to power prof access class instance" )
    std::map<WORD,std::wstring>::const_iterator c_iter =
      m_stdmap_eventid2powerschemename.find(nEventID) ;
    if( c_iter != m_stdmap_eventid2powerschemename.end() )
    {
      LOGN( "setting power scheme" )
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
            + getwxString(stdstr ) ) ;
          LOGN( "setting power scheme failed" )
        }
      }
    }
  }
#endif //#ifdef _WIN32
}

void TaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
  LOGN("left mouse click on system tray icon")
  if( mp_mainframe )
  {
    mp_mainframe->Show(true);
    LOGN("after calling to show the main frame")
  }
//  mp_mainframe->Maximize(true ) ;
}

//void TaskBarIcon::SetMainFrame(MainFrame * p_mf )
//{
//  mp_mainframe = p_mf ;
//}
