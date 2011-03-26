/*
 * App_ShowTaskBarIcon.cpp
 *
 *  Created on: 05.03.2011
 *      Author: Stefan
 */
#ifndef WX_PRECOMP
  #include "wx/app.h"
#endif
#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
//#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include <wx/filename.h> //wxFileName::GetPathSeparator(...)

#include <wxWidgets/App.hpp>

bool wxX86InfoAndControlApp::ShowTaskBarIcon(MainFrame * p_mf )
{
  LOGN("App::ShowTaskBarIcon() begin")
//      m_systemtray_icon_notification_window = mp_frame->GetHandle() ;
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  #ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON
  ShowTaskBarIconViaWindowsAPI();
  #else
  mp_frame = p_mf ;
  return ShowTaskBarIconUsingwxWidgets();
  #endif //#ifdef USE_WINDOWA_API_SYSTEM_TRAY_ICON
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON

  LOGN("App::ShowTaskBarIcon() end")
  return false ;
}

//inline
void wxX86InfoAndControlApp::DeleteTaskBarIcon()
{
  LOGN("wxX86InfoAndControlApp::DeleteTaskBarIcon() begin")
  if( mp_taskbaricon )
  {
    //Also deleted in the wxWidgets "tbtest" sample (not automatically
    //deleted?!).
    delete //mp_wxx86infoandcontrolapp->mp_taskbaricon;
      mp_taskbaricon;
    LOGN("after deleting the system tray icon")
//    mp_wxx86infoandcontrolapp->mp_taskbaricon = NULL ;
    //Mark the pointer that the memory for it was deleted.
    mp_taskbaricon = NULL;
  }
  LOGN("wxX86InfoAndControlApp::DeleteTaskBarIcon() end")
}

void wxX86InfoAndControlApp::ShowTaskBarIconViaWindowsAPI()
{
  //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON
  HICON hicon = (HICON) ::LoadImage(
       //http://msdn.microsoft.com/en-us/library/ms648045%28v=VS.85%29.aspx:
       //"To load a stand-alone resource (icon, cursor, or bitmap file)�for
  //     example, c:\myimage.bmp�set this parameter to NULL."
       NULL ,//__in  HINSTANCE hinst,
  //     __in  LPCTSTR lpszName,
       "x86IandC.ico" ,
       IMAGE_ICON ,//__in  UINT uType,
  //     0 ,//__in  int cxDesired,
  //     0, //__in  int cyDesired,
       16,
       16,
       LR_LOADFROMFILE //__in  UINT fuLoad
     );
//      std::cout << "wndproc hicon:" << hicon << "\n" ;
  m_systemtrayaccess.m_hwndReceiveIconNotifications =
      (HWND) m_systemtray_icon_notification_window ;
  m_systemtrayaccess.ShowIconInSystemTray(hicon,"x86Info&Control") ;
  #endif //#ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON
}

//return : true: setting the task bar icon succeeded.
bool wxX86InfoAndControlApp::ShowTaskBarIconUsingwxWidgets()
{
  //from wxWidgets sample tbtest.cpp
  // Created:     01/02/97
  // RCS-ID:      $Id: tbtest.cpp 36336 2005-12-03 17:55:33Z vell $
  if( ! mp_taskbaricon )
  {
    mp_taskbaricon = new TaskBarIcon(mp_frame);
  //      m_taskbaricon.SetMainFrame(mp_frame) ;
    if( mp_taskbaricon )
    {
  //    #if defined(__WXCOCOA__)
  //      m_dockIcon = new TaskBarIcon(wxTaskBarIcon::DOCK);
  //    #endif
  //      wxIcon wxicon(
  //        //Use wxT() macro to enable to compile with both unicode and ANSI.
  //        wxT("x86IandC.ico") ,
  //        wxBITMAP_TYPE_ICO
  //        ) ;
  //      if( wxicon.IsOk() )
      wxIcon wxicon ;
      if( //http://docs.wxwidgets.org/stable/wx_wxicon.html:
          //"true if the operation succeeded, false otherwise."
        wxicon.LoadFile(
        //Use wxT() macro to enable to compile with both unicode and ANSI.
        wxT("x86IandC.ico") ,
        wxBITMAP_TYPE_ICO
        )
        )
      {
        if( mp_taskbaricon->SetIcon( //wxICON(sample),
            //m_taskbaricon.SetIcon(
            wxicon ,
            mp_modelData->m_stdtstrProgramName
            )
          )
        {
          LOGN("set system tray icon")
          return true ;
        }
        else
  //          ::wxMessageBox(wxT("Could not set task bar icon."),
  //            getwxString(m_stdtstrProgramName) );
          MessageWithTimeStamp(
            //Avoid g++ warning "deprecated conversion from string constant to 'WCHAR*'
            (WCHAR *)
            L"Could not set task bar icon." );
      }
    }
  }
  return false;
}
