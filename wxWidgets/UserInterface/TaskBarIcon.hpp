#ifndef _TASKBARICON_HPP
#define _TASKBARICON_HPP

#include <wx/taskbar.h>

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
  wxMenu * p_wxmenu ;
public:
#if defined(__WXCOCOA__)
    TaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    TaskBarIcon(MainFrame * p_mainframe)
#endif
    {
      mp_mainframe = p_mainframe ;
    }
//    TaskBarIcon() ;

    void OnLeftButtonClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
//     void OnMenuCheckmark(wxCommandEvent&);
//     void OnMenuUICheckmark(wxUpdateUIEvent&);
//    void OnMenuSub(wxCommandEvent&);
    virtual wxMenu *CreatePopupMenu();
    void CreatePowerSchemesMenu() ;
//    void SetMainFrame(MainFrame * ) ;
    void OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent) ;
    DECLARE_EVENT_TABLE()
};

#endif //_TASKBARICON_HPP
