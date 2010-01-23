#define _AFXDLL

//return values for start or stopp high load thread.
#define ENDED 0
#define STARTED 1

//#include "StdAfx.h" //for ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG

//// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"
//
//#ifdef __BORLANDC__
//    #pragma hdrstop
//#endif
//
//// for all others, include the necessary headers (this file is usually all you
//// need because it includes almost all "standard" wxWidgets headers)
//#ifndef WX_PRECOMP
//    #include "wx/wx.h"
//#endif

//#include "Controller/PumaStateCtrl.h" //for "class PumaStateCtrl"
#include <UserInterface.hpp> //for abstract class UserInterface
//#ifndef WX_PRECOMP
//    #include "wx/frame.h"
//#endif
//#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
#include <wx/timer.h> //for class wxTimer
#include "wx/power.h" //for power mgmt notification (wxPowerType et.c)
#include <vector> //for std::vector
#include <ModelData/ModelData.hpp>
#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER
  //#include "Windows/CPUcoreUsageGetterNtQuerySystemInformation.hpp"
  //#include "Windows/CPUcoreUsageGetterIWbemServices.hpp"
//#include "Windows/CPUcoreUsageNTQSI_OO.hpp"
//#include "Windows/CPUcoreUsageNTQSI_OO2.hpp"
#endif //#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER
//#include <Controller/ClocksNotHaltedCPUcoreUsageGetter.hpp>
#include <wx/frame.h>
#include <wx/dcclient.h> //for class wxPaintDC
#include <wx/dcbuffer.h> //class wxBufferedPaintDC
//#include "FreqAndVoltageSettingDlg.hpp"

class wxBufferedPaintDC ;
class CalculationThread ;
//class GriffinController ;
//class wxMenuItem ;
class FreqAndVoltageSettingDlg ;
class wxFrame ;
class wxMenuBar ;
class CPUcoreUsageGetterIWbemServices ;
class CPUcoreUsageNTQSI ;
class I_CPUcontrollerAction ;
class SpecificCPUcoreActionAttributes ;

class MainFrame:
  public wxFrame, 
  //Must be public, else MSVC++ error "C4996"
  public UserInterface
  //, public wxTimer 
{
public:
  volatile bool m_vbAnotherWindowIsActive ;
  WORD m_wFreqInMHzOfCurrentActiveCoreSettings ;
  float m_fVoltageInVoltOfCurrentActiveCoreSettings ;
  bool m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
  bool m_bRangeBeginningFromMinVoltage ;
#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER
  //CPUcoreUsageGetterNtQuerySystemInformation m_cpucoreusagegetter ;
#endif
#ifdef COMPILE_WITH_IWBEMSERVICES 
  CPUcoreUsageGetterIWbemServices m_cpucoreusagegetteriwbemservices ;
#endif
  void Notify() ; //overrides wxTimer::Notify()
  wxTimer m_timer;
  DWORD m_dwTimerIntervalInMilliseconds ;
private: 
  //PumaStateCtrl m_pumastatectrl(
  //  0, 
  //  NULL,//UserInterface()
  //  //&PumaStateCtrlDlg()
  //  this) ;
  //PumaStateCtrl * m_pumastatectrl ;
  bool b_NotFirstTime ;
  BYTE m_byNumberOfSettablePstatesPerCore ;
  BYTE m_byMenuIndexOf1stPstate ;
  CPUcoreData * mp_cpucoredata ;
  #ifdef COMPILE_WITH_CALC_THREAD
  CalculationThread * mp_calculationthread ;
  #endif
  //ClocksNotHaltedCPUcoreUsageGetter m_clocksnothaltedcpucoreusagegetter ;
  float m_fPreviousCPUusage ;
  float m_fMaxVoltage ;
  float m_fMinVoltage ;
  float m_fMaxMinusMinVoltage ;
  ULONGLONG m_ullPreviousCPUusage ;
  ULONGLONG m_ullHighestDiff ;
  ULONGLONG m_ullHighestPerformanceEventCounter2Diff ;
  ULONGLONG m_ullPreviousTimeStampCounterValue ;
  ULONGLONG m_ullPreviousPerformanceEventCounter2 ;
  ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
  //GriffinController * mp_pumastatectrl ;
  I_CPUcontroller * mp_cpucontroller ;
  //CPUcoreUsageNTQSI m_cpucoreusagentqsi ;
  int m_nLowestIDForSetVIDnFIDnDID ;
  int m_nNumberOfMenuIDsPerCPUcore ;
  Model * mp_model ;
  std::map<WORD, I_CPUcontrollerAction *> m_stdmapwmenuid2i_cpucontrolleraction ;
  std::vector<//classes derived from wxObject
    wxObject> m_stdvectorwxuicontroldata ;
  std::map<//classes derived from wxObject
    WORD, //wxObject *
    SpecificCPUcoreActionAttributes *> m_stdmapwxuicontroldata ;
  wxBitmap * mp_wxbitmap ;
  wxBitmap * mp_wxbitmapStatic ;
  wxBufferedPaintDC * mp_wxbufferedpaintdcStatic ;
  //wxBufferedPaintDC m_wxbufferedpaintdcStatic ;
  //"When a wxWindow is destroyed, it automatically deletes all its children. These children are all the objects that received the window as the parent-argument in their constructors.  
  //As a consequence, if you're creating a derived class that contains child windows, you should use a pointer to the child windows instead of the objects themself as members of the main window."
  //wxMenuBar m_wxmenubar ;
  wxMemoryDC m_wxmemorydcStatic ;
  wxMenuBar * mp_wxmenubar ;
  wxMenuItem ** m_arp_wxmenuitemPstate ;
  wxMenuItem * mp_wxmenuitemOtherDVFS ;
  wxMenuItem * mp_wxmenuitemOwnDVFS ;
  wxMenuItem ** marp_wxmenuItemHighLoadThread ;
  WORD m_wXcoordOfBeginOfYaxis ;
  WORD m_wDiagramWidth ;
  WORD m_wDiagramHeight ;

  //std::vector<//use wxString because it need's to be a subclass of wxObject 
  //  wxString> m_vecwxstring ;
  //std::vector<//The number of cores is unkown at compile time. So dyn. create 
  //  //a menu for each core at runtime. Use pointer to menus because the vector
  //  //stores copies->copy a pointer is faster than to copy a complete object.
  //  wxMenu *> m_vecp_wxmenuCore ;
  FreqAndVoltageSettingDlg * mp_freqandvoltagesettingdlg ;
  FreqAndVoltageSettingDlg ** m_arp_freqandvoltagesettingdlg ;

  //void 
  wxMenuItem * AddDynamicallyCreatedMenu(
    wxMenu * p_wxmenu,
    WORD & r_wMenuID,
    const wxString & r_wxstr
    ) ;
  wxMenuItem * AddDynamicallyCreatedMenu(
    wxMenu * p_wxmenu,
    WORD & r_wMenuID,
    const wxString & r_wxstr
    , //void (wxEvtHandler::*wxEventFunction)(wxEvent&)
    wxObjectEventFunction
    , SpecificCPUcoreActionAttributes * scaa
    );

  //void 
  BYTE AddSetPstateMenuItem(  
      wxMenu * p_wxmenuCore
      , BYTE byCoreID
      , BYTE byPstateID 
      //Must be a reference because changes to the variable should be 
      //maintained OUTside this function.
      , WORD & r_wMenuID
    ) ;
  void CreateAndInitMenuItemPointers() ;
public:

  MainFrame(
    const wxString& title, 
    const wxPoint& pos, 
    const wxSize& size
    //, GriffinController * p_pstatectrl
    , I_CPUcontroller * p_cpucontroller
    //, CPUcoreData * p_cpucoredata
    , Model * p_model
    );
  ~MainFrame() ;

  //Implement method declared in "class UserInterface" 
  //(even if with no instructions) for this class not be abstract.
  void outputAllPstates(unsigned char byCurrentP_state, int & vid){};

  wxString BuildHighLoadThreadMenuText(std::string str,
    //for displaying "start" the result of the previous action must be "ended"
    BYTE byPreviousAction = ENDED ) ;
  bool Confirm(const std::string & str) ;
  bool Confirm(std::ostrstream & r_ostrstream);

  void CreateServiceMenuItems() ;
  //void 
  BYTE CreateDynamicMenus() ;
  void DisableWindowsDynamicFreqScalingHint();
//  wxString GetSetPstateMenuItemLabel(
//    BYTE byPstateNumber
          //The "PState" class is AMD-specific.
//    , PState & pstate
//    ) ;
  void DrawAllPossibleOperatingPoints( wxDC & r_wxdcDrawOn ) ;
  void DrawCurrentVoltageSettingsCurve(
    wxDC & wxdc
    , float fMaxVoltage
    ) ;
  void DrawLowestStableVoltageCurve(
    wxDC & wxdc
    , float fMaxVoltage
    ) ;
  void DrawCurrentPstateInfo( wxDC & r_wxdc ) ;
  void DrawPerformanceStatesCrosses(
    wxDC & r_wxdc 
    , const std::set<VoltageAndFreq> & cr_stdsetmaxvoltageforfreq 
    , const wxColor * cp_wxcolor 
    ) ;
  void DrawDiagramScale(
    wxDC & wxdc ,
    //std::set<MaxVoltageForFreq>::iterator & iterstdsetmaxvoltageforfreq
    std::set<VoltageAndFreq>::iterator & iterstdsetmaxvoltageforfreq
    );
  void DrawOvervoltageProtectionCurve(
    //wxPaintDC * p_wxpaintdc
    wxDC & wxdc
//    , WORD wDiagramWidth
//    , WORD wDiagramHeight
    , float fMaxVoltage
//    , WORD wXcoordOfBeginOfYaxis
    ) ;
  void DrawVoltageFreqCross(
    wxDC & r_wxdc
    , float fVoltageInVolt
    , WORD wFreqInMHz
    , const wxColor * cp_wxcolor 
    ) ;
  void OnClose(wxCloseEvent & event) ;
  void OnDisableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEnableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEraseBackground(wxEraseEvent& event) ;
  void OnFindDifferentPstates( wxCommandEvent & WXUNUSED(event) ) ;
  void OnHighLoadThread( wxCommandEvent & WXUNUSED(event) ) ;
  void OnOwnDynFreqScaling( wxCommandEvent & WXUNUSED(wxevent) 
    //wxevent 
    ) ;
  void OnPaint(wxPaintEvent & event) ;
  void OnPstateDialog( wxCommandEvent & WXUNUSED(event) ) ;
  void OnMSR( wxCommandEvent & WXUNUSED(event) ) ;
  void OnDynamicallyCreatedUIcontrol(wxCommandEvent & event);
  void OnSize(wxSizeEvent & WXUNUSED(event)) ;
  void OnFindLowestOperatingVoltage(wxCommandEvent & WXUNUSED(event));
  void OnSaveAsDefaultPStates(wxCommandEvent & WXUNUSED(event));

  void OnContinueService(wxCommandEvent & WXUNUSED(event));
  void OnPauseService(wxCommandEvent & WXUNUSED(event));
  void OnStartService(wxCommandEvent & WXUNUSED(event));
  void OnStopService(wxCommandEvent & WXUNUSED(event));
  void OnUpdateViewInterval(wxCommandEvent & WXUNUSED(event));

#ifdef PRIVATE_RELEASE //hide the other possibilities
  void OnIncreaseVoltageForCurrentPstate(wxCommandEvent& WXUNUSED(event));
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities
  void OnQuit(wxCommandEvent & event);
//#ifdef _WINDOWS
#ifdef COMPILE_WITH_SERVICE_CAPABILITY
  void OnStartService(wxCommandEvent& event);
#endif
  //void OnSetPstate0ForBothCores(wxCommandEvent& WXUNUSED(event)) ;
  //void OnSetPstate1ForBothCores(wxCommandEvent& WXUNUSED(event)) ;//{} ;
  //void OnSetPstate2ForBothCores(wxCommandEvent& WXUNUSED(event)) ;//{} ;
  void OnAbout(wxCommandEvent & event);
  void PossiblyAskForOSdynFreqScalingDisabling();
//  void SetMenuItemLabel(
//      BYTE byCoreID
//      , BYTE byPstateNumber
//      , //const
//        PState & pstate
//      ) ;
  //void SetPumaStateController(GriffinController * p_pumastatectrl) ;
  void RedrawEverything() ;
  void SetCPUcontroller(I_CPUcontroller * );
#ifdef wxHAS_POWER_EVENTS
  //void OnSuspending(wxPowerEvent& event)
  //{
  //    wxLogMessage(_T("System suspend starting..."));
  //    if ( wxMessageBox(_T("Veto suspend?"), _T("Please answer"),
  //                      wxYES_NO, this) == wxYES )
  //    {
  //        event.Veto();
  //        wxLogMessage(_T("Vetoed suspend."));
  //    }
  //}

  //void OnSuspended(wxPowerEvent& WXUNUSED(event))
  //{
  //    wxLogMessage(_T("System is going to suspend."));
  //}

  //void OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
  //{
  //    wxLogMessage(_T("System suspend was cancelled."));
  //}

  void OnResume(wxPowerEvent& WXUNUSED(event)) ;
#endif // wxHAS_POWER_EVENTS
  void OnTimerEvent(wxTimerEvent &event);

  void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState) ;

  wxPowerType m_powerType;
  wxBatteryState m_batteryState;

  DECLARE_EVENT_TABLE()
};
