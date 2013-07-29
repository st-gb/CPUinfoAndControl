/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#ifndef _TASKBARICON_HPP
#define _TASKBARICON_HPP

#if defined(wxHAS_TASK_BAR_ICON)
//#if defined(COMPILE_WITH_SYSTEM_TRAY_ICON) && defined(wxHAS_TASK_BAR_ICON)
#define COMPILE_WITH_SYSTEM_TRAY_ICON
#endif

#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
#include <wx/taskbar.h>
#include <wxWidgets/icon/IconDrawer.hpp> //class wxIconDrawer

class MainFrame ;
#ifdef __WXMSW__ //for power schemes
  #include <map>
  #include <string>
#endif //#ifdef __WXMSW__

//from wxWidgets' sample tbtest
// Created:     01/02/97
// RCS-ID:      $Id: tbtest.h 36336 2005-12-03 17:55:33Z vell $
class TaskBarIcon
  : public wxTaskBarIcon
{
private:
  WORD m_1stSetMaximumCPUcoreMultiplierEventID;
  WORD m_1stThrottleCPUcoreTemperatureEventID;
  WORD m_1stSelectPowerSchemeMenuEventID;
  WORD m_wAfterLastSelectPowerSchemeMenuEventID;
  WORD m_wAfterLastThrottleCPUcoreTemperatureEventID;
  static WORD s_wEventID;
#ifdef __WXMSW__
  std::map<WORD,std::wstring> m_stdmap_eventid2powerschemename ;
#endif //#ifdef __WXMSW__
  MainFrame * mp_mainframe ;
public:
  //Should be a member of this class because its destruction should happen
  //before the destruction before an object of this class (else: program
  //crash).
  //CrashED when using an object of this class.
  wxIconDrawer m_wxicon_drawer;
//  wxIconDrawer m_colouredBarsIconDrawer;
  //Must be created on heap because else the program hangs when its
  //wxBitmap and or wxMemoryDC wasn't freed.
  wxIconDrawer * m_p_wxicon_drawer;
private:
  wxMenu * p_wxmenu ;
  wxMenu * m_p_wxmenuCPUcoreMultipliers;
  wxMenu * m_p_wxmenuThrottleTemperatures;
public:
#if defined(__WXCOCOA__)
    TaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    TaskBarIcon(MainFrame * p_mainframe)
      :
      m_1stSetMaximumCPUcoreMultiplierEventID(0),
      m_1stThrottleCPUcoreTemperatureEventID(0),
      m_1stSelectPowerSchemeMenuEventID(0),
      m_wxicon_drawer(16, 16//,8
    //    ,wxBITMAP_SCREEN_DEPTH
        )
//      , m_colouredBarsIconDrawer(16, 16//,8
//    //    ,wxBITMAP_SCREEN_DEPTH
//        )
      , m_p_wxmenuCPUcoreMultipliers(NULL)
      , m_p_wxmenuThrottleTemperatures(NULL)
#endif
    {
      LOGN("TaskBarIcon() begin")
//      m_p_wxicon_drawer = new wxIconDrawer(16, 16//,8
//        //,wxBITMAP_SCREEN_DEPTH
//        );
      m_p_wxicon_drawer = & m_wxicon_drawer;
      mp_mainframe = p_mainframe ;
      LOGN("TaskBarIcon() end")
    }
    ~TaskBarIcon() ;

    virtual wxMenu * CreatePopupMenu();
    wxMenu * CreatePowerSchemesMenu() ;
#ifdef _WIN32
    void CreatePowerSchemeMenuByNamesFromMemory(
      wxMenu * p_wxmenuPowerSchemes);
    void CreatePowerSchemeMenuByRetrieving(
      wxMenu * p_wxmenuPowerSchemes);
#endif
    wxMenu * CreateSetMaximumCPUcoreMultiplierMenu();
    wxMenu * CreateSetThrottleTemperatureMenu();
    void DisconnectEventHandlers();
    void DisconnectSelectPowerSchemeEventHandlers();
    void DisconnectOnSetThrottleTemperatureEventHandlers();
    void DrawText(
      wxIcon & r_wxicon,
      const wxString & cr_wxstrText,
      const wxColour * cp_wxcolourText
      )
    {
      if( m_p_wxicon_drawer)
        m_p_wxicon_drawer->DrawText(r_wxicon, cr_wxstrText, cp_wxcolourText);
    }
    void FreeRessources();
    float GetTemperatureViaMenuLabel(
      int nEventID//, //wxString & r_wxstrMenuLabel
    //    fTemperatureInDegC
      );
    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
//     void OnMenuCheckmark(wxCommandEvent&);
//     void OnMenuUICheckmark(wxUpdateUIEvent&);
//    void OnMenuSub(wxCommandEvent&);
//    void SetMainFrame(MainFrame * ) ;
    void OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent) ;
    void OnSetMaximumCPUcoreMultiplier(wxCommandEvent & wxevent) ;
    void OnSetThrottleTemperature(wxCommandEvent & wxevent) ;
    void ShowMainFrame();
    DECLARE_EVENT_TABLE()
};
#endif //COMPILE_WITH_SYSTEM_TRAY_ICON

#endif //_TASKBARICON_HPP
