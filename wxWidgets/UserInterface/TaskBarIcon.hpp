#ifndef _TASKBARICON_HPP
#define _TASKBARICON_HPP

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
  //Must be created on heap because else the program hangs when its
  //wxBitmap and or wxMemoryDC wasn't freed.
  wxIconDrawer * m_p_wxicon_drawer;
private:
  wxMenu * p_wxmenu ;
public:
#if defined(__WXCOCOA__)
    TaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    TaskBarIcon(MainFrame * p_mainframe)
      :
      m_wxicon_drawer(16, 16//,8
    //    ,wxBITMAP_SCREEN_DEPTH
        )
#endif
    {
//      m_p_wxicon_drawer = new wxIconDrawer(16, 16//,8
//        //,wxBITMAP_SCREEN_DEPTH
//        );
      m_p_wxicon_drawer = & m_wxicon_drawer;
      mp_mainframe = p_mainframe ;
    }
    ~TaskBarIcon() ;

    virtual wxMenu * CreatePopupMenu();
    void CreatePowerSchemesMenu() ;
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
    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
//     void OnMenuCheckmark(wxCommandEvent&);
//     void OnMenuUICheckmark(wxUpdateUIEvent&);
//    void OnMenuSub(wxCommandEvent&);
//    void SetMainFrame(MainFrame * ) ;
    void OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent) ;
    DECLARE_EVENT_TABLE()
};

#endif //_TASKBARICON_HPP
