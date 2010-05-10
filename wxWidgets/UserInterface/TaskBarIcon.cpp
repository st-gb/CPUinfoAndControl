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

#ifdef __WXMSW__
  #include <Windows/PowerProf/PowerProfDynLinked.hpp>
  #include <wxWidgets/App.hpp> //wxGetApp()
  #include <wxWidgets/Controller/wxStringHelper.h>
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

BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
//    EVT_MENU(PU_NEW_ICON,MyTaskBarIcon::OnMenuSetNewIcon)
//    EVT_MENU(PU_OLD_ICON,MyTaskBarIcon::OnMenuSetOldIcon)
//    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
//    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
//    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_TASKBAR_LEFT_DOWN  (MyTaskBarIcon::OnLeftButtonClick)
//    EVT_MENU(PU_SUB1, MyTaskBarIcon::OnMenuSub)
//    EVT_MENU(PU_SUB2, MyTaskBarIcon::OnMenuSub)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
  if( mp_mainframe )
    mp_mainframe->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
  mp_mainframe->Close(true);
}
//
//static bool check = true;
//
//void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
//{
//       check =!check;
//}
//
//void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
//{
//       event.Check( check );
//}

//void MyTaskBarIcon::OnMenuSub(wxCommandEvent&)
//{
//    wxMessageBox(wxT("You clicked on a submenu!"));
//}

// Overridables
wxMenu * MyTaskBarIcon::CreatePopupMenu()
{
  WORD wEventID = LAST_ID ;
    // Try creating menus different ways
    // TODO: Probably try calling SetBitmap with some XPMs here
    wxMenu * menu = new wxMenu;
    menu->Append(PU_RESTORE, _T("&show window"));
    menu->AppendSeparator();
//    menu->Append(PU_CHECKMARK, _T("Checkmark"),wxT(""), wxITEM_CHECK);
//    menu->AppendSeparator();
    wxMenu * wxmenuProfiles = new wxMenu;
    wxMenu * wxmenuPowerSchemes = new wxMenu;
//    wxmenuProfiles->Append(PU_SUB1, _T("One submenu"));
//    wxmenuProfiles->AppendSeparator();
//    wxmenuProfiles->Append(PU_SUB2, _T("Another submenu"));
//    menu->Append(SELECT_POWER_SCHEME, _T("select profile"), wxmenuProfiles);
#ifdef __WXMSW__
    std::set<std::wstring> set_wstr ;
    PowerProfDynLinked * p_powerprofdynlinked =
     (PowerProfDynLinked * ) wxGetApp().mp_dynfreqscalingaccess ;
    p_powerprofdynlinked->GetAllPowerSchemeNames(set_wstr) ;
    for( std::set<std::wstring>::const_iterator stdset_c_iter = set_wstr.begin() ;
        stdset_c_iter != set_wstr.end() ; ++ stdset_c_iter
      )
    {
      //TODO wxString should be unicode for e.g. Chinese language (more than 255
      // characters)
      wxString wxstr( getwxString(*stdset_c_iter) ) ;
      wxmenuPowerSchemes->Append( wEventID , wxstr );
      m_stdmap_eventid2powerschemename.insert(std::pair<WORD,std::wstring>
        (wEventID,*stdset_c_iter) ) ;
      Connect( wEventID ++ , //wxID_ANY,
        wxEVT_COMMAND_MENU_SELECTED ,
        wxCommandEventHandler(MyTaskBarIcon::OnDynamicallyCreatedUIcontrol)
        );
    }
    menu->Append(SELECT_POWER_SCHEME, _T("select Windows &power scheme"),
        wxmenuPowerSchemes);
#endif //#ifdef __WXMSW__
#ifndef __WXMAC_OSX__ /*Mac has built-in quit menu*/
    menu->AppendSeparator();
    menu->Append(PU_EXIT,    _T("E&xit"));
#endif
    return menu;
}

void MyTaskBarIcon::OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent)
{
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
      BYTE by = p_powerprofdynlinked->SetActivePowerScheme(c_iter->second) ;
      LOGN( "res of setting power scheme:" << (WORD) by )
      if( by == 1 )
      {
        LOGN( "setting power scheme succeeded" )
      }
      else
      {
        LOGN( "setting power scheme failed" )
      }
    }
  }
}

void MyTaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
  LOGN("left mouse click on system tray icon")
  if( mp_mainframe )
  {
    mp_mainframe->Show(true);
    LOGN("after calling to show the main frame")
  }
//  mp_mainframe->Maximize(true ) ;
}

//void MyTaskBarIcon::SetMainFrame(MainFrame * p_mf )
//{
//  mp_mainframe = p_mf ;
//}
