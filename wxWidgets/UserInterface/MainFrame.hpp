#define _AFXDLL

//return values for start or stop high load thread.
#define ENDED 0
#define STARTED 1

//#include "StdAfx.h" //for ULONGLONG
#include <preprocessor_macros/Windows_compatible_typedefs.h> //for ULONGLONG

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

//#ifndef WX_PRECOMP
//    #include "wx/frame.h"
//#endif
//#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
//#include <wx/dcbuffer.h> //class wxBufferedPaintDC
#include <wx/dcclient.h> //for class wxPaintDC
#include <wx/frame.h> //for base class wxFrame
#include "wx/power.h" //for power mgmt notification (wxPowerType etc.)
#include <wx/timer.h> //for class wxTimer
#include <wx/thread.h> //for class wxCriticalSection

//#include <Controller/ICalculationThread.hpp> //for "started", "ended"
//#include <ModelData/ModelData.hpp>
//#include <UserInterface/UserInterface.hpp> //for abstract class UserInterface
#include <Xerces/PStateConfig.hpp> //class XercesConfigurationHandler
//#include "FreqAndVoltageSettingDlg.hpp"
#include <vector> //for std::vector

//forward declarations (faster than an #include)
class wxBufferedPaintDC ;
class CalculationThread ;
class CPUcoreData ;
//class wxMenuItem ;
class FreqAndVoltageSettingDlg ;
class Model ;
class PerCPUcoreAttributes ;
class I_CPUcontrollerAction ;
class I_CPUcontroller ;
class SpecificCPUcoreActionAttributes ;
class wxX86InfoAndControlApp ;
class wxDynLibCPUcontroller ;
class wxDynLibCPUcoreUsageGetter ;
class wxFrame ;
class wxMenuBar ;

class VoltageAndMulti
{
public:
  float m_fMultiplier ;
  float m_fVoltageInVolt ;
};

class MainFrame:
  public wxFrame //,
//  //Must be public, else MSVC++ error "C4996"
//  public UserInterface
  //, public wxTimer 
{
public:
  void Notify() ; //overrides wxTimer::Notify()
private: 
  bool m_bAllowCPUcontrollerAccess ;
  bool m_bCPUcoreUsageConsumed ;
  bool m_bDiagramNotDrawn ;
public:
  bool m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
private:
  VoltageAndMulti * mp_ar_voltage_and_multi ;
  bool m_bNotFirstTime ;
  bool m_bRangeBeginningFromMinVoltage ;
  BYTE m_byIndexOf1stCPUcontrollerRelatedMenu ;
  BYTE m_byNumberOfSettablePstatesPerCore ;
  BYTE m_byMenuIndexOf1stPstate ;
  #ifdef COMPILE_WITH_CALC_THREAD
  CalculationThread * mp_calculationthread ;
  #endif
  CPUcoreData * mp_cpucoredata ;
public:
  DWORD m_dwTimerIntervalInMilliseconds ;
private:
  //ClocksNotHaltedCPUcoreUsageGetter m_clocksnothaltedcpucoreusagegetter ;
  float m_fMaxMinusMinVoltage ;
  float m_fMaxVoltage ;
  float m_fMinVoltage ;
  float m_fPreviousCPUusage ;
public:
  float m_fVoltageInVoltOfCurrentActiveCoreSettings ;
private:
  FreqAndVoltageSettingDlg * mp_freqandvoltagesettingdlg ;
  FreqAndVoltageSettingDlg ** m_arp_freqandvoltagesettingdlg ;
  I_CPUcontroller * mp_i_cpucontroller ;
  int m_nLowestIDForSetVIDnFIDnDID ;
  int m_nNumberOfMenuIDsPerCPUcore ;
  Model * mp_model ;
  std::map<WORD, I_CPUcontrollerAction *> m_stdmapwmenuid2i_cpucontrolleraction ;
  std::map<//classes derived from wxObject
    WORD, //wxObject *
    SpecificCPUcoreActionAttributes *> m_stdmapwxuicontroldata ;
  std::vector<//classes derived from wxObject
    wxObject> m_stdvectorwxuicontroldata ;
  std::vector<wxMenu *> m_vecp_wxmenuCore ;
  ULONGLONG m_ullHighestDiff ;
  ULONGLONG m_ullHighestPerformanceEventCounter2Diff ;
  ULONGLONG m_ullPreviousCPUusage ;
  ULONGLONG m_ullPreviousPerformanceEventCounter2 ;
  ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
  ULONGLONG m_ullPreviousTimeStampCounterValue ;
public:
  volatile bool m_vbAnotherWindowIsActive ;
  WORD m_wFreqInMHzOfCurrentActiveCoreSettings ;
  WORD m_wMaxFreqWidth ;
  WORD m_wMaximumCPUcoreFrequency ;
  WORD m_wMaxVoltageWidth ;
  WORD m_wMaxFreqInMHzTextWidth ;
  WORD m_wMaxVoltageInVoltTextWidth ;
  WORD m_wMaxTemperatureTextWidth ;
private:
  wxBitmap * mp_wxbitmap ;
  wxBitmap * mp_wxbitmapStatic ;
  wxBufferedPaintDC * mp_wxbufferedpaintdcStatic ;
  wxCriticalSection m_wxcriticalsectionCPUctlAccess ;
  //wxDynLibCPUcontroller * mp_wxdynlibcpucontroller ;
  //wxDynLibCPUcoreUsageGetter * mp_wxdynlibcpucoreusagegetter ;
  //wxBufferedPaintDC m_wxbufferedpaintdcStatic ;
  //http://docs.wxwidgets.org/2.6/wx_windowdeletionoverview.html#windowdeletionoverview:
  //"When a wxWindow is destroyed, it automatically deletes all its children. 
  //These children are all the objects that received the window as the 
  //parent-argument in their constructors.  
  //As a consequence, if you're creating a derived class that contains child 
  //windows, you should use a pointer to the child windows instead of the 
  //objects themself as members of the main window."
  //wxMenuBar m_wxmenubar ;
  wxMemoryDC m_wxmemorydcStatic ;
  wxMenuBar * mp_wxmenubar ;
  wxMenu * p_wxmenuExtras ;
  wxMenu * mp_wxmenuFile ;
  wxMenu * p_wxmenuService ;
//  wxMenuItem ** m_arp_wxmenuitemPstate ;
  wxMenuItem * mp_wxmenuitemOtherDVFS ;
  wxMenuItem * mp_wxmenuitemOwnDVFS ;
//  wxMenuItem ** marp_wxmenuItemHighLoadThread ;
  wxMenuItem * mp_wxmenuitemCollectAsDefaultVoltagePerfStates ;
  wxString m_wxstrTitle ;
public:
  wxTimer m_wxtimer;
//  wxTimer * m_p_wxtimer;
private:
  wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp ;
  WORD m_wXcoordOfBeginOfYaxis ;
  WORD m_wMinYcoordInDiagram ;
  WORD m_wDiagramWidth ;
  WORD m_wDiagramHeight ;
  XercesConfigurationHandler m_xercesconfigurationhandler ;

  //std::vector<//use wxString because it need's to be a subclass of wxObject 
  //  wxString> m_vecwxstring ;
  //std::vector<//The number of cores is unkown at compile time. So dyn. create 
  //  //a menu for each core at runtime. Use pointer to menus because the vector
  //  //stores copies->copy a pointer is faster than to copy a complete object.
  //  wxMenu *> m_vecp_wxmenuCore ;

//  //void
//  wxMenuItem * AddDynamicallyCreatedMenu(
//    wxMenu * p_wxmenu,
//    WORD & r_wMenuID,
//    const wxString & r_wxstr
//    ) ;
//  wxMenuItem * AddDynamicallyCreatedMenu(
//    wxMenu * p_wxmenu,
//    WORD & r_wMenuID,
//    const wxString & r_wxstr
//    , //void (wxEvtHandler::*wxEventFunction)(wxEvent&)
//    wxObjectEventFunction
//    , SpecificCPUcoreActionAttributes * scaa
//    );
//  //void
//  BYTE AddSetPstateMenuItem(
//      wxMenu * p_wxmenuCore
//      , BYTE byCoreID
//      , BYTE byPstateID
//      //Must be a reference because changes to the variable should be
//      //maintained OUTside this function.
//      , WORD & r_wMenuID
//    ) ;
//  void CreateAndInitMenuItemPointers() ;
public:

  MainFrame(
    const wxString& title, 
    const wxPoint& pos, 
    const wxSize& size
    , I_CPUcontroller * p_cpucontroller
    //, CPUcoreData * p_cpucoredata
    , Model * p_model
    , wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp
    );
  ~MainFrame() ;

  void AllowCPUcontrollerAccess() ;
  void DenyCPUcontrollerAccess() ;
  void EndDynVoltAndFreqScalingThread( PerCPUcoreAttributes * 
    p_percpucoreattributes ) ;

  //Implement method declared in "class UserInterface" 
  //(even if with no instructions) for this class not be abstract.
//  void outputAllPstates(unsigned char byCurrentP_state, int & vid){};

//  wxString BuildHighLoadThreadMenuText(std::string str,
//    //for displaying "start" the result of the previous action must be "ended"
//    BYTE byPreviousAction = //ENDED
//      ICalculationThread::ended ) ;
  bool Confirm(const std::string & str) ;
//  bool Confirm(std::ostrstream & r_ostrstream);
  void CPUcontrollerDynLibAttached(const wxString & wxstrFilePath ) ;
  void CPUcontrollerDeleted() ;
  void CPUcoreUsageGetterAttached(const wxString & wxstrFilePath ) ;
  void CPUcoreUsageGetterDeleted() ;

  inline void CreateFileMenu() ;
  void CreateServiceMenuItems() ;
  //void 
  BYTE CreateDynamicMenus() ;
  void DetermineMaxVoltageAndMaxFreqDrawWidth(wxDC & r_wxdc) ;
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
  inline bool IsCPUcontrollerAccessAllowedThreadSafe()
  {
    {
    //Control access to m_bAllowCPUcontrollerAccess between threads.
    wxCriticalSectionLocker wxcriticalsectionlocker(
      m_wxcriticalsectionCPUctlAccess) ;
    return m_bAllowCPUcontrollerAccess ;
    }
  }
  void OnCollectAsDefaultVoltagePerfStates( wxCommandEvent & WXUNUSED(event) ) ;
  void OnClose(wxCloseEvent & event) ;
  void OnDisableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEnableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEraseBackground(wxEraseEvent& event) ;
  void OnFindDifferentPstates( wxCommandEvent & WXUNUSED(event) ) ;
  void OnHighLoadThread( wxCommandEvent & WXUNUSED(event) ) ;
  void OnMinimizeToSystemTray( wxCommandEvent & WXUNUSED(event) ) ;
  void OnSysTrayIconClick( wxCommandEvent & WXUNUSED(event) ) ;
  void OnOwnDynFreqScaling( wxCommandEvent & WXUNUSED(wxevent) 
    //wxevent 
    ) ;
  void OnPaint(wxPaintEvent & event) ;
  void OnPstateDialog( wxCommandEvent & WXUNUSED(event) ) ;
  void OnMSR( wxCommandEvent & WXUNUSED(event) ) ;
  void OnWriteToCPUregister( wxCommandEvent & WXUNUSED(event) ) ;
  void OnDynamicallyCreatedUIcontrol(wxCommandEvent & event);
  void OnSize(wxSizeEvent & WXUNUSED(event)) ;
  void OnFindLowestOperatingVoltage(wxCommandEvent & WXUNUSED(event));
  void OnSaveAsDefaultPStates(wxCommandEvent & WXUNUSED(event));

  void OnConnectToOrDisconnectFromService(wxCommandEvent & WXUNUSED(event) ) ;
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
  void OnAttachCPUcontrollerDLL(wxCommandEvent & event);
  void OnAttachCPUcoreUsageGetterDLL(wxCommandEvent & event);
  void OnDetachCPUcontrollerDLL(wxCommandEvent & event);
  void OnDetachCPUcoreUsageGetterDLL(wxCommandEvent & event);
//  void OnInitDialog( wxInitDialogEvent & event ) ;
  void PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
  void PossiblyAskForOSdynFreqScalingDisabling();
//  void SetMenuItemLabel(
//      BYTE byCoreID
//      , BYTE byPstateNumber
//      , //const
//        PState & pstate
//      ) ;
  //void SetPumaStateController(GriffinController * p_pumastatectrl) ;
  void RecreateDisplayBuffers() ;
  void RedrawEverything() ;
  void SetCPUcontroller(I_CPUcontroller * );
  void StoreCurrentVoltageAndFreqInArray(
    wxDC & r_wxdc
  //  VoltageAndFreq * & r_ar_voltageandfreq
    , wxString  r_ar_wxstrFreqInMHz []
    , wxString r_ar_wxstrVoltageInVolt []
  //  , float r_ar_fTempInCelsius []
    , wxString r_ar_wxstrTempInCelsius []
    , I_CPUcontroller * p_cpucontroller
    ) ;
#ifdef wxHAS_POWER_EVENTS
  void OnSuspending(wxPowerEvent& event) ;
  //{
  //    wxLogMessage(_T("System suspend starting..."));
  //    if ( wxMessageBox(_T("Veto suspend?"), _T("Please answer"),
  //                      wxYES_NO, this) == wxYES )
  //    {
  //        event.Veto();
  //        wxLogMessage(_T("Vetoed suspend."));
  //    }
  //}
  void OnSuspended(wxPowerEvent& WXUNUSED(event)) ;
  //{
  //    wxLogMessage(_T("System is going to suspend."));
  //}
  void OnSuspendCancel(wxPowerEvent& WXUNUSED(event)) ;
  //{
  //    wxLogMessage(_T("System suspend was cancelled."));
  //}
  void OnResume(wxPowerEvent& WXUNUSED(event)) ;
#endif // wxHAS_POWER_EVENTS
  void OnTimerEvent(wxTimerEvent &event);
//  void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState) ;
//  wxPowerType m_powerType;
//  wxBatteryState m_batteryState;
  DECLARE_EVENT_TABLE()
};
