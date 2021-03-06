/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#define _AFXDLL

//return values for start or stop high load thread.
#define ENDED 0
#define STARTED 1

//#include "StdAfx.h" //for ULONGLONG
//#include <preprocessor_macros/Windows_compatible_typedefs.h> //for ULONGLONG
#include <winnt.h> //for ULONGLONG

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
//#include <Xerces/PStateConfig.hpp> //class VoltageForFrequencyConfiguration
#include <wxWidgets/icon/IconDrawer.hpp>
//For typedef "criticalsection_type" :
#include <Controller/multithread/criticalsection_type.hpp>
//#include "FreqAndVoltageSettingDlg.hpp"
#include <map> //class std::map
#include <set> //class std::set
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
class ICPUcoreUsageGetter ;
class SpecificCPUcoreActionAttributes ;
class VoltageAndFreq;
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

//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define COMPILE_WITH_SERVICE_PROCESS_CONTROL
#endif

class MainFrame
  : public wxFrame //,
//  //Must be public, else MSVC++ error "C4996"
//  public UserInterface
  //, public wxTimer 
{
public:
    enum
    {
      ID_Quit = 1
      , ID_About
      , ID_AttachCPUcontrollerDynLib
      , ID_DetachCPUcontrollerDynamicLibrary
      , ID_SetCPUcontrollerDynLibForThisCPU

      , ID_AttachCPUusageGetterDynLib
      , ID_DetachCPUusageGetterDynLib
      , ID_SetCPUusageGetterDynLibForThisCPU

      , ID_MinimizeToSystemTray
    //#ifdef COMPILE_WITH_MSR_EXAMINATION
      , ID_ShowExamineCPUregistersDialog
      , ID_WriteToMSRdialog
      //, ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq //,
      //This means to disable e.g.:
      //-Windows' scaling (by ACPI objects?)
      //-WRITE to performance state MSR registers by programs like RMclock
      , ID_DisableOtherVoltageOrFrequencyAccess
      , ID_EnableOtherVoltageOrFrequencyAccess
      , ID_EnableOrDisableOwnDVFS //,

    #ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
      , ID_ContinueService
      , ID_PauseService
      , ID_StartService
      , ID_StopService
      , ID_ConnectToService
      , ID_ConnectToOrDisconnectFromService
      , ID_DisconnectFromService
    #endif

      , ID_UpdateViewInterval
      , ID_SaveAsDefaultPstates
      , ID_Collect_As_Default_Voltage_PerfStates
      , ID_FindDifferentPstates
      , ID_ShowVoltageAndFrequencySettingsDialog
    //#endif
      , TIMER_ID
    //#ifdef _WINDOWS
    #ifdef COMPILE_WITH_SERVICE_CAPABILITY
      , ID_Service
    #endif
    //  ID_SetPstate3ForCore0,
    //  ID_FindLowestOperatingVoltage
    #ifdef PRIVATE_RELEASE //hide the other possibilities
      ,ID_IncreaseVoltageForCurrentPstate
    #endif //#ifdef PRIVATE_RELEASE //hide the other possibilities
      , ID_LoadDetectInstableCPUcoreVoltageDynLib
      , ID_UnloadDetectInstableCPUcoreVoltageDynLib
      ,
      ID_LastStaticEventID
    }; //event IDs enum
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
  static float * s_arfTemperatureInDegreesCelsius ;//= NULL ;
  //ClocksNotHaltedCPUcoreUsageGetter m_clocksnothaltedcpucoreusagegetter ;
  float m_fMaxMinusMinVoltage ;
  float m_fMaxVoltage ;
  float m_fMinVoltage ;
//  float m_fPreviousCPUusage ;
public:
  float m_fVoltageInVoltOfCurrentActiveCoreSettings ;
private:
//  FreqAndVoltageSettingDlg * mp_freqandvoltagesettingdlg ;
  //Array of pointers to dialogs
  FreqAndVoltageSettingDlg ** m_arp_freqandvoltagesettingdlg ;
public:
  FreqAndVoltageSettingDlg * m_p_freqandvoltagesettingsdialog;
private:
  I_CPUcontroller * mp_i_cpucontroller ;
  int m_nLowestIDForSetVIDnFIDnDID ;
  int m_nNumberOfMenuIDsPerCPUcore ;
public:
  Model * mp_model ;
private:
  std::map<WORD, I_CPUcontrollerAction *> m_stdmapwmenuid2i_cpucontrolleraction ;
  std::map<//classes derived from wxObject
    WORD, //wxObject *
    SpecificCPUcoreActionAttributes *> m_stdmapwxuicontroldata ;
  std::vector<//classes derived from wxObject
    wxObject> m_stdvectorwxuicontroldata ;
  std::vector<FreqAndVoltageSettingDlg * > m_stdvec_p_freqandvoltagesettingdlg ;
  std::vector<wxMenu *> m_vecp_wxmenuCore ;
public:
  volatile bool m_vbAnotherWindowIsActive ;
  unsigned m_uiRightmostEndOfVoltageString;
  WORD m_wFreqInMHzOfCurrentActiveCoreSettings ;
  WORD m_wMaxFreqWidth ;
  WORD m_wMaximumCPUcoreFrequency ;
  WORD m_wMaxVoltageWidth ;
  WORD m_wMaxFreqInMHzTextWidth ;
  WORD m_wMaxVoltageInVoltTextWidth ;
  WORD m_wMaxTemperatureTextWidth ;
  WORD m_wTextHeight ;
  WORD m_wYcoordinate ;
private:
  wxBitmap * mp_wxbitmap ;
  wxBitmap * mp_wxbitmapStatic ;
  wxBufferedPaintDC * mp_wxbufferedpaintdcStatic ;
  wxCriticalSection m_wxcriticalsectionCPUctlAccess ;
#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER
  criticalsection_type m_crit_secVoltAndFreqDlgs;
#endif //#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER

  //wxDynLibCPUcontroller * mp_wxdynlibcpucontroller ;
  //wxDynLibCPUcoreUsageGetter * mp_wxdynlibcpucoreusagegetter ;
  //wxBufferedPaintDC m_wxbufferedpaintdcStatic ;
  static wxIcon s_wxiconTemperature ;
  static wxIcon s_wxiconCPUcoreUsages;
  static wxIcon s_wxiconCPUcoresMultipliers;
//  wxIconDrawer m_wxicon_drawer;
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
  wxMenu * m_p_wxmenuGUI ;
public:
  wxMenu * mp_wxmenuFile ;
private:
  wxMenu * p_wxmenuService ;
  wxMenu * m_p_wxmenuCore;
//  wxMenuItem ** m_arp_wxmenuitemPstate ;
  wxMenuItem * mp_wxmenuitemOtherDVFS ;
  wxMenuItem * mp_wxmenuitemOwnDVFS ;
public:
  wxMenuItem * m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib;
private:
//  wxMenuItem ** marp_wxmenuItemHighLoadThread ;
  wxMenuItem * mp_wxmenuitemCollectAsDefaultVoltagePerfStates ;
  wxString m_wxstrTitle ;
  static wxString s_wxstrHighestCPUcoreTemperative ;
  static wxString s_wxstrTaskBarIconToolTip;
public:
  wxTimer m_wxtimer;
//  wxTimer * m_p_wxtimer;
private:
  wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp ;
  WORD m_wXcoordOfBeginOfYaxis ;
  WORD m_wMinYcoordInDiagram ;
  WORD m_wDiagramWidth ;
  WORD m_wDiagramHeight ;
//  Xerces::VoltageForFrequencyConfiguration
//    m_xerces_voltage_for_frequency_configuration ;

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
    const wxString & cr_wxstrTitle,
    const wxPoint & cr_wxpointPos,
    const wxSize & cr_wxsize
    , I_CPUcontroller * p_cpucontroller
    //, CPUcoreData * p_cpucoredata
    , Model * p_model
    , wxX86InfoAndControlApp * mp_wxx86infoandcontrolapp
    );
  ~MainFrame() ;

  void AllowCPUcontrollerAccess() ;

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
  inline void ConnectToDataProvider_Inline() ;
  inline void CreateGUImenuItems();
#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  void CreateServiceMenuItems() ;
#endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  //void 
  BYTE CreateDynamicMenus() ;
  void Create1DialogAndMenuForAllCores() ;
  void CreateDialogAndMenuForEveryCore() ;
  void DenyCPUcontrollerAccess() ;
  void DetermineMaxVoltageAndMaxFreqDrawWidth(wxDC & r_wxdc) ;
  void DetermineTextHeight(wxDC & r_wxdc) ;
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
  inline void DrawCPUcoreIDs(
    wxDC & r_wxdc,
    wxCoord & wxcoordX,
    wxCoord wxcoordTextHeight);
  inline void DrawCurrentCPUcoreFrequency(
    wxDC & r_wxdc,
    const wxString ar_wxstrCPUcoreFreqInMHz [],
    wxCoord & wxcoordX,
    wxCoord wxcoordTextHeight
    );
  inline void DrawCurrentCPUcoreTemperature(
    wxDC & r_wxdc,
    const wxString ar_wxstrCPUcoreTemperature [],
    wxCoord & wxcoordX,
    wxCoord wxcoordTextHeight
    );
  inline void DrawCPUcoreVoltage(
    wxDC & r_wxdc,
    const wxString ar_wxstrCPUcoreVoltageInVolt [],
    wxCoord & wxcoordX,
    wxCoord wxcoordTextHeight
    );
  inline void DrawCPUcoreUsages(wxDC & r_wxdc,
    const ICPUcoreUsageGetter * p_cpucoreusagegetter,
    wxCoord wxcoordX,
    wxCoord wxcoordTextHeight
    );
  void DrawCurrentCPUcoreInfo( wxDC & r_wxdc ) ;
  inline void DrawFrequency(
    wxDC & r_wxdc ,
    WORD wFrequencyInMHz ,
  //  WORD wMaxFreqInMHz ,
    wxCoord wxcoordWidth ,
    wxCoord wxcoordHeight ,
    WORD wLeftEndOfCurrFreqText ,
    wxString wxstrFreq ,
    WORD & wXcoordinate ,
    WORD & wYcoordinate ,
    std::map<WORD,WORD>::iterator & r_iterstdmapYcoord2RightEndOfFreqString ,
    std::map<WORD,WORD> & stdmapYcoord2RightEndOfFreqString ,
    std::map<WORD,WORD>::iterator & r_iterstdmap_ycoord2rightendoffreqstringToUse
    , bool bCalculateMaxTextHeight
    ) ;
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
  void DrawVoltageGraph(
    //wxPaintDC * p_wxpaintdc
    wxDC & wxdc
//    , WORD wDiagramWidth
//    , WORD wDiagramHeight
    , float fMaxVoltage
//    , WORD wXcoordOfBeginOfYaxis
    , const std::set<VoltageAndFreq> & c_r_stdsetvoltageandfreq
    ) ;
  void DrawFrequencyMarksAndLines(wxDC & r_wxdc, WORD & wMaximumYcoordinateForFrequency);
  void DrawVoltage(wxDC & r_wxdc, float fVoltage) ;
  wxCoord DrawVoltage(wxDC & r_wxdc, float fVoltage, WORD wYcoordinate,
    unsigned uiXcoordinateInPixels) ;
  void DrawVoltageFreqCross(
    wxDC & r_wxdc
    , float fVoltageInVolt
    , WORD wFreqInMHz
    , const wxColor * cp_wxcolor 
    ) ;
  void DrawVoltageScale(wxDC & r_wxdc );
  void DynVoltnFreqScalingEnabled() ;
  void EndDynVoltAndFreqScalingThread( PerCPUcoreAttributes *
    p_percpucoreattributes ) ;
  inline float GetClosestMultiplier(int nXcoordionate, float &
    fReferenceClockInMHz);
  inline float GetClosestVoltageForYcoordinate(int nYcoordinate);
  inline void GetCPUcoreInfoFromDataProvider(
    ICPUcoreUsageGetter * & p_cpucoreusagegetter ,
    I_CPUcontroller * & p_cpucontroller //,
    );
  inline bool GetCPUcoreInfoDirectlyOrFromService(
    ICPUcoreUsageGetter * & p_cpucoreusagegetter ,
    I_CPUcontroller * & p_cpucontroller //,
//    wxString & wxstrCPUcoreUsage
    , bool bGetCPUcoreUsage
    ) ;
  inline WORD GetXcoordinateForFrequency( WORD wFreqInMHz) ;
  inline WORD GetYcoordinateForFrequency( WORD wFreqInMHz) ;
  unsigned GetYcoordinateForVoltage(float fVoltageInVolt ) ;
  void HandleResumeForAllVoltAndFreqDlgs();
  inline bool IsCPUcontrollerAccessAllowedThreadSafe()
  {
    //Control access to m_bAllowCPUcontrollerAccess between threads.
    wxCriticalSectionLocker wxcriticalsectionlocker(
      m_wxcriticalsectionCPUctlAccess) ;
    return m_bAllowCPUcontrollerAccess ;
  }
  void OnCollectAsDefaultVoltagePerfStates( wxCommandEvent & WXUNUSED(event) ) ;
  void OnClose(wxCloseEvent & event) ;
  void OnDisconnectFromService( wxCommandEvent & WXUNUSED(event) ) ;
  void OnDisableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEnableOtherVoltageOrFrequencyAccess( wxCommandEvent & WXUNUSED(event) ) ;
  void OnEraseBackground(wxEraseEvent& event) ;
  void OnFindDifferentPstates( wxCommandEvent & WXUNUSED(event) ) ;
  void OnHighLoadThread( wxCommandEvent & WXUNUSED(event) ) ;
  void OnMinimizeToSystemTray( wxCommandEvent & WXUNUSED(event) ) ;
  void OnLeftMouseButtonDown(wxMouseEvent & r_wxmouseevent);
  void OnMoveMouse(wxMouseEvent & WXUNUSED(r_wxmouseevent) );
  void OnSysTrayIconClick( wxCommandEvent & WXUNUSED(event) ) ;
  void OnOwnDynFreqScaling( wxCommandEvent & WXUNUSED(wxevent) 
    //wxevent 
    ) ;
  void OnPaint(wxPaintEvent & event) ;
  void OnVoltageAndFrequencySettingsDialog( wxCommandEvent & WXUNUSED(event) ) ;
  void OnShowExamineCPUregistersDialog( wxCommandEvent & WXUNUSED(event) ) ;
  void OnShowCPUregistersReadAndWriteDialog( wxCommandEvent & WXUNUSED(event) ) ;
  void OnDynamicallyCreatedUIcontrol(wxCommandEvent & event);
  void OnSaveAsCPUcontrollerDynLibForThisCPU(
    wxCommandEvent & WXUNUSED(event) ) ;
  void OnSize(wxSizeEvent & WXUNUSED(event)) ;
  void OnSizing(wxSizeEvent & WXUNUSED(event)) ;
  void OnFindLowestOperatingVoltage(wxCommandEvent & WXUNUSED(event));
  void OnSaveVoltageForFrequencySettings(wxCommandEvent & WXUNUSED(event));

  void OnConnectToService( wxCommandEvent & WXUNUSED(event) ) ;
  void OnConnectToOrDisconnectFromService(wxCommandEvent & WXUNUSED(event) ) ;
  void OnContinueService(wxCommandEvent & WXUNUSED(event));
  void OnPauseService(wxCommandEvent & WXUNUSED(event));
//  void OnShowVoltageAndFrequencySettingsDialog(wxCommandEvent & );
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
  void OnLoadDetectInstableCPUcoreVoltageDynLib(wxCommandEvent & event);
  void OnUnloadDetectInstableCPUcoreVoltageDynLib(wxCommandEvent & event);
//  void OnInitDialog( wxInitDialogEvent & event ) ;
  void PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
  void PossiblyAskForOSdynFreqScalingDisabling();
//  void SetMenuItemLabel(
//      BYTE byCoreID
//      , BYTE byPstateNumber
//      , //const
//        PState & pstate
//      ) ;
  void RecreateDisplayBuffers() ;
  void RedrawEverything() ;
  void SetCPUcontroller(I_CPUcontroller * );
  inline void Show1VoltnFreqSettingsDialogForEveryCPUcore(wxCommandEvent &);
  inline void ShowCPUcoreUsagesInTaskBar(I_CPUcontroller * p_i_cpucontroller);
  inline void ShowCPUcoresMultipliersInTaskBar(
    I_CPUcontroller * p_i_cpucontroller);
  inline void ShowHighestCPUcoreTemperatureInTaskBar(
    I_CPUcontroller * p_i_cpucontroller) ;
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
  void OnRedrawEverything(wxCommandEvent & evt );
  void OnTimerEvent(wxTimerEvent &event);
//  void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState) ;
//  wxPowerType m_powerType;
//  wxBatteryState m_batteryState;
  DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_COMMAND_REDRAW_EVERYTHING, wxNewEventType() )
//"EVT_BUTTON" also expands to "DECLARE_EVENT_TYPE"
//  DECLARE_EVENT_TYPE(wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE, 7777)
END_DECLARE_EVENT_TYPES()
