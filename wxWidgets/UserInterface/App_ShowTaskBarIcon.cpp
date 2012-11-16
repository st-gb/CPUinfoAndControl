/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
//#include <wx/bitmap.h> //wxBITMAP_DEFAULT_TYPE

//http://stackoverflow.com/questions/59670/how-to-get-rid-of-deprecated-conversion-from-string-constant-to-char-warning
// : "I believe passing -Wno-write-strings to gcc will suppress this warning."
#pragma GCC diagnostic ignored "-Wwrite-strings"
  #include <images/x86IandC.xpm>
//ENable g++ "deprecated conversion from string constant to 'char*'" warning
#pragma GCC diagnostic warning "-Wwrite-strings"

#include <wxWidgets/App.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <wxWidgets/UserInterface/MainFrame.hpp>//class MainFrame
#include <wxWidgets/UserInterface/TaskBarIcon.hpp>//class TaskBarIcon

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

TaskBarIcon * wxX86InfoAndControlApp::CreateTaskBarIcon(
  TaskBarIcon * & r_p_taskbaricon,
  const char * p_chTaskBarIconName
  )
{
  if( ! r_p_taskbaricon )
  {
    r_p_taskbaricon = new TaskBarIcon(mp_frame);
    if( r_p_taskbaricon )
      LOGN(FULL_FUNC_NAME << "--successfully created \"" << p_chTaskBarIconName
        << "\" task bar icon.")
  }
  return r_p_taskbaricon;
}

void wxX86InfoAndControlApp::DeleteTaskBarIcon(
  TaskBarIcon * & r_p_taskbaricon,
  const char * p_chTaskBarIconName
  )
{
  if( r_p_taskbaricon )
  {
    r_p_taskbaricon->RemoveIcon();
    LOGN("after removing the \"" << p_chTaskBarIconName << "\" system tray icon")
    delete r_p_taskbaricon;
    LOGN("after deleting the \"" << p_chTaskBarIconName << "\" task bar icon")
    //Mark the pointer that the memory for it was deleted.
    r_p_taskbaricon = NULL;
  }
}

//inline
void wxX86InfoAndControlApp::DeleteTaskBarIcons()
{
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  LOGN(//"wxX86InfoAndControlApp::DeleteTaskBarIcons()"
    FULL_FUNC_NAME <<
    "--begin" )
  if( m_p_HighestCPUcoreTemperatureTaskBarIcon )
  {
    //Removing the icon is neccessary to exit the app/
    //else the icon may not be hidden after exit.
    LOGN("before removing the system tray icon")
    m_p_HighestCPUcoreTemperatureTaskBarIcon->RemoveIcon() ;
    LOGN("after removing the highest CPU core temp. system tray icon")

    //This program does not terminate correctly (does not reach
    //MainController's destructor when a dynamically created.
    //taskbar icon is not deleted.
    m_p_HighestCPUcoreTemperatureTaskBarIcon->DisconnectEventHandlers();

    //Also deleted in the wxWidgets "tbtest" sample (not automatically
    //deleted?!).
    delete //mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon;
      m_p_HighestCPUcoreTemperatureTaskBarIcon;
    LOGN("after deleting the highest CPU core temperature system tray icon")
//    mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon = NULL ;

//    mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->RemoveIcon() ;
    //Must free the dependant bitmaps BEFORE calling "RemoveIcon()"?
//    mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->FreeRessources();

    //Mark the pointer that the memory for it was deleted.
    m_p_HighestCPUcoreTemperatureTaskBarIcon = NULL;
  }

  DeleteTaskBarIcon(m_p_CPUcoreUsagesTaskbarIcon, "CPU core usages");
  DeleteTaskBarIcon(m_p_CPUcoresMultipliersTaskbarIcon, "CPU cores multipliers");

  LOGN(//"wxX86InfoAndControlApp::DeleteTaskBarIcons()"
    FULL_FUNC_NAME <<
    "--end")
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
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
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  //from wxWidgets sample tbtest.cpp
  // Created:     01/02/97
  // RCS-ID:      $Id: tbtest.cpp 36336 2005-12-03 17:55:33Z vell $
  LOGN( FULL_FUNC_NAME << "--begin" )

  if( mp_modelData->m_userinterfaceattributes.m_bShowCPUcoreUsagesIconInTaskBar)
    CreateTaskBarIcon(m_p_CPUcoreUsagesTaskbarIcon, "CPU core usages");
  if( mp_modelData->m_userinterfaceattributes.
      m_bShowCPUcoresMultipliersIconInTaskBar)
    CreateTaskBarIcon(m_p_CPUcoresMultipliersTaskbarIcon,
      "CPU cores multipliers in % of min and max");

  if( CreateTaskBarIcon(m_p_HighestCPUcoreTemperatureTaskBarIcon,
      "highest CPU core temperature") )
  {
    if( m_p_HighestCPUcoreTemperatureTaskBarIcon->m_p_wxicon_drawer )
    {
      wxDC & r_wxdc = m_p_HighestCPUcoreTemperatureTaskBarIcon->
        m_p_wxicon_drawer->GetDC();
      wxFont & wxfont = (wxFont &) r_wxdc.GetFont();
      wxfont.SetPointSize(m_model.m_userinterfaceattributes.
        m_nCPUcoreTempTaskBarIconFontSizeInPoint);
      r_wxdc.SetFont(wxfont);
    }
  //    #if defined(__WXCOCOA__)
  //      m_dockIcon = new TaskBarIcon(wxTaskBarIcon::DOCK);
  //    #endif
  //      wxIcon wxicon(
  //        //Use wxT() macro to enable to compile with both unicode and ANSI.
  //        wxT("x86IandC.ico") ,
  //        wxBITMAP_TYPE_ICO
  //        ) ;
  //      if( wxicon.IsOk() )
      bool bSetIcon = true;
#ifdef __linux__
      wxIcon wxicon( x86IandC_xpm ) ;
#else
      wxIcon wxicon ;
      if( ! GetX86IandCiconFromFile(wxicon) )
      {
        wxicon = wxIcon ( x86IandC_xpm );
//        bSetIcon = false;
      }
#endif
      if( bSetIcon )
      {
        if( m_p_HighestCPUcoreTemperatureTaskBarIcon->SetIcon( //wxICON(sample),
            //m_taskbaricon.SetIcon(
            wxicon ,
#ifdef __linux__
            wxT("bla")
#else
            mp_modelData->m_stdtstrProgramName
#endif
            )
          )
        {
          LOGN(FULL_FUNC_NAME << "--successfully set system tray icon.")
          return true ;
        }
        else
  //          ::wxMessageBox(wxT("Could not set task bar icon."),
  //            getwxString(m_stdtstrProgramName) );
//          MessageWithTimeStamp(
//            //Avoid g++ warning "deprecated conversion from string constant to 'WCHAR*'
//            (WCHAR *)
//            L"Could not set task bar icon." );
          mp_frame->mp_wxmenuFile->Enable(MainFrame::ID_MinimizeToSystemTray,
              false);
      }
  }
  else
    LOGN( FULL_FUNC_NAME << "--failed to create task bar icon object")
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  return false;
}
