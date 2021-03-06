/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */

//"For compilers that support precompilation, includes "wx/wx.h"."
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "MainFrame.hpp"

//wxWidgets include files
#include <wx/defs.h> //for wxBG_STYLE_CUSTOM
#include <wx/dcbuffer.h> //for class wxBufferedPaintDC
#include <wx/dynlib.h> //wxDynamicLibrary::GetDllExt()
#include <wx/filename.h> //wxFileName::GetPathSeparator(...)
#include "wx/frame.h" //for base class wxFrame
//#include <wx/icon.h> //for class wxIcon
#include <wx/menu.h> //for class wxMenu, class wxMenuBar
#include <wx/menuitem.h> //class wxMenuItem
#include <wx/numdlg.h> //for ::wxGetNumberFromUser(...)
#include <wx/stattext.h> //for wxStaticText
#include <wx/string.h> //for wxString::Format(...)
#include <wx/timer.h> //for EVT_TIMER (,...?)
//#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include <wx/wx.h> //for wxMessageBox(...) (,etc.)
//#include "wx/window.h"
//#include <wx/menubar.h>

//#include "../Controller/RunAsService.h" //for MyServiceStart etc.
#include "FreqAndVoltageSettingDlg.hpp"
//SUPPRESS_UNUSED_VARIABLE_WARNING(...)
#include <compiler/GCC/suppress_unused_variable.h>
#include <Controller/CalculationThreadProc.h>
//for GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt
//  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  );
#include <Controller/CPU-related/GetInterpolatedVoltageFromFreq.hpp>
//::wxGetApp().mp_cpucoreusagegetter
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
////GetFilenameWithoutExtension(const std::string &)
//#include <Controller/FileSystem/GetFilenameWithoutExtension/\_
//GetFilenameWithoutExtension.hpp>
////for ::GetErrorMessageFromLastErrorCodeA(...)
//#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/IDynFreqScalingAccess.hpp>
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
//WriteFileContent(...)
#include <InputOutput/WriteFileContent/WriteFileContent.hpp>
//for member m_stdmapwmenuid2i_cpucontrolleraction
#include <Controller/I_CPUcontrollerAction.hpp>//class I_CPUcontrollerAction
#include <Controller/IPC/I_IPC.hpp> //enum IPCcontrolCodes
#include <Controller/character_string/stdtstr.hpp> //Getstdtstring(...)
//DISable g++ "deprecated conversion from string constant to 'char*'" warning,
//from
//http://stackoverflow.com/questions/59670/how-to-get-rid-of-deprecated-conversion-from-string-constant-to-char-warning
// : "I believe passing -Wno-write-strings to gcc will suppress this warning."
#pragma GCC diagnostic ignored "-Wwrite-strings"
  #include <x86IandC.xpm>
//ENable g++ "deprecated conversion from string constant to 'char*'" warning
#pragma GCC diagnostic warning "-Wwrite-strings"
#include <ModelData/ModelData.hpp> //class CPUcoreData
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <ModelData/RegisterData.hpp>
//#include <ModelData/HighLoadThreadAttributes.hpp>
#include <ModelData/SpecificCPUcoreActionAttributes.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //#include <Windows/CalculationThread.hpp>
  #include <OperatingSystem/Windows/DLLloadError.hpp>
  #include <OperatingSystem/Windows/ErrorCode/ErrorCodeFromGetLastErrorToString.h>
  #include <OperatingSystem/Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
  #include <OperatingSystem/Windows/Service/ServiceBase.hpp>
#endif
#include <wxWidgets/App.hpp> //for wxGetApp() / DECLARE_APP
#include <wxWidgets/Controller/wxDynLibCPUcontroller.hpp>
#include <wxWidgets/Controller/wxDynLibCPUcoreUsageGetter.hpp>
//#include <wxWidgets/wxStringHelper.h>
//getwxString(const std::string &) / getwxString(const std::wstring &)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
//class NonBlocking::wxServiceSocketClient
#include <wxWidgets/Controller/non-blocking_socket/client/\
wxServiceSocketClient.hpp>
#include <wxWidgets/DynFreqScalingThread.hpp>
#include <wxWidgets/icon/CreateTextIcon.hpp> //CreateTextIcon(...)
#include <wxWidgets/ModelData/wxCPUcoreID.hpp>
//#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
#include <wxWidgets/UserInterface/TaskBarIcon.hpp>//class TaskBarIcon
//#endif
#include <wxWidgets/UserInterface/AboutDialog.hpp> //class AboutDialog
#include <wxWidgets/UserInterface/DynFreqScalingDlg.hpp>
#include <Xerces/XMLAccess.hpp>
#include "wxExamineCPUregistersDialog.hpp"
#ifdef COMPILE_WITH_MSR_EXAMINATION
  #include "CPUregisterReadAndWriteDialog.hpp"
#endif //COMPILE_WITH_MSR_EXAMINATION
//#include <limits.h>
#ifndef MAXWORD
  #define MAXWORD 65535
#endif //#ifndef MAXWORD
#include <map> //std::map
#include <set> //std::set
#include <valarray> //class std::valarray
//#include <xercesc/framework/MemBufInputSource.hpp>

#include <hardware/CPU/fastest_data_type.h> //typedef 

#ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON
  #include "SystemTrayAccess.hpp"
#endif //#ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON

#ifdef _MSC_VER
  #define __FLT_MIN__ FLT_MIN
#endif

class wxObject ;

extern CPUcontrolBase * gp_cpucontrolbase ;

//Static class variables must (also) be declared/ defined in the source file.
float * MainFrame::s_arfTemperatureInDegreesCelsius = NULL ;
wxIcon MainFrame::s_wxiconTemperature ;
wxIcon MainFrame::s_wxiconCPUcoreUsages;
wxIcon MainFrame::s_wxiconCPUcoresMultipliers;
wxString MainFrame::s_wxstrHighestCPUcoreTemperative ;
wxString MainFrame::s_wxstrTaskBarIconToolTip =
  wxT("x86IandC--highest CPU core temperature [°C]") ;

DEFINE_LOCAL_EVENT_TYPE( wxEVT_COMMAND_REDRAW_EVERYTHING )

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_REDRAW_EVERYTHING,
    MainFrame::OnRedrawEverything)
  //"Cross-Platform GUI Programming with wxWidgets"
  // (Copyright © 2006 Pearson Education, Inc.)
  // ISBN 0-13-147381-6  "First printing, July 2005"
  //"CHAPTER 5 Drawing and Printing" ->
  //"UNDERSTANDING DEVICE CONTEXTS" -> "Drawing on Windows with wxPaintDC" :
//  "[...] another thing you can do to make
//  drawing smoother (particularly when resizing) is to paint the background in
//  your paint handler, and not in an erase background handler. All the painting
//  will then be done in your buffered paint handler, so you don’t see the back-
//  ground being erased before the paint handler is called. Add an empty erase
//  background handler, and call  SetBackgroundStyle with  wxBG_STYLE_CUSTOM to
//  hint to some systems not to clear the background automatically."
  EVT_ERASE_BACKGROUND(MainFrame::OnEraseBackground)
  EVT_MENU(ID_Quit, MainFrame::OnQuit)
  EVT_MENU(ID_About, MainFrame::OnAbout)
  EVT_MENU(ID_AttachCPUcontrollerDynLib, MainFrame::OnAttachCPUcontrollerDLL)
  EVT_MENU(ID_DetachCPUcontrollerDynamicLibrary, MainFrame::OnDetachCPUcontrollerDLL)
  EVT_MENU(ID_AttachCPUusageGetterDynLib, MainFrame::OnAttachCPUcoreUsageGetterDLL)
  EVT_MENU(ID_DetachCPUusageGetterDynLib, MainFrame::OnDetachCPUcoreUsageGetterDLL)
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  EVT_MENU(ID_MinimizeToSystemTray, MainFrame::OnMinimizeToSystemTray)
#endif
#ifdef COMPILE_WITH_MSR_EXAMINATION
  EVT_MENU(ID_ShowExamineCPUregistersDialog, MainFrame::OnShowExamineCPUregistersDialog)
  EVT_MENU(ID_WriteToMSRdialog, MainFrame::OnShowCPUregistersReadAndWriteDialog)
#endif
  //EVT_MENU( ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq ,
  //  MainFrame::OnDynamicallyCreatedUIcontrol )
  EVT_MENU( ID_DisableOtherVoltageOrFrequencyAccess ,
    MainFrame::OnDisableOtherVoltageOrFrequencyAccess ) 
  EVT_MENU( ID_EnableOtherVoltageOrFrequencyAccess ,
    MainFrame::OnEnableOtherVoltageOrFrequencyAccess ) 
  EVT_MENU( ID_EnableOrDisableOwnDVFS ,
    MainFrame::OnOwnDynFreqScaling )
  EVT_MENU( ID_LoadDetectInstableCPUcoreVoltageDynLib,
    MainFrame::OnLoadDetectInstableCPUcoreVoltageDynLib)
  EVT_MENU( ID_UnloadDetectInstableCPUcoreVoltageDynLib,
    MainFrame::OnUnloadDetectInstableCPUcoreVoltageDynLib)
  EVT_MENU( ID_UpdateViewInterval ,
    MainFrame::OnUpdateViewInterval )
    EVT_MENU( ID_SetCPUcontrollerDynLibForThisCPU ,
    MainFrame::OnSaveAsCPUcontrollerDynLibForThisCPU )
  EVT_MENU( ID_SaveAsDefaultPstates ,
    MainFrame::OnSaveVoltageForFrequencySettings )
  EVT_MENU( ID_FindDifferentPstates ,
    MainFrame::OnFindDifferentPstates )
  EVT_MENU( ID_Collect_As_Default_Voltage_PerfStates ,
    MainFrame::OnCollectAsDefaultVoltagePerfStates )
  EVT_MENU( ID_ShowVoltageAndFrequencySettingsDialog ,
    MainFrame:://OnShowVoltageAndFrequencySettingsDialog
    OnVoltageAndFrequencySettingsDialog)

#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  EVT_MENU( ID_ContinueService ,
    MainFrame::OnContinueService )
  EVT_MENU( ID_PauseService ,
    MainFrame::OnPauseService )
  EVT_MENU( ID_StartService ,
    MainFrame::OnStartService )
  EVT_MENU( ID_StopService ,
    MainFrame::OnStopService )
  EVT_MENU( ID_ConnectToService ,
    MainFrame::OnConnectToService )
  EVT_MENU( ID_ConnectToOrDisconnectFromService ,
    MainFrame::OnConnectToOrDisconnectFromService )
  EVT_MENU( ID_DisconnectFromService ,
    MainFrame::OnDisconnectFromService )
#endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
//#endif
  //#ifdef _WINDOWS
#ifdef COMPILE_WITH_SERVICE_CAPABILITY
  EVT_MENU(ID_Service, MainFrame::OnStartService)
#endif
  //EVT_MENU(ID_SetPstate0ForBothCores, MyFrame::OnSetPstate0ForBothCores)
  //EVT_MENU(ID_SetPstate1ForBothCores, MyFrame::OnSetPstate1ForBothCores)
  //EVT_MENU(ID_SetPstate2ForBothCores, MyFrame::OnSetPstate2ForBothCores)
  //EVT_MENU(ID_FindLowestOperatingVoltage, MainFrame::OnFindLowestOperatingVoltage)
#ifdef PRIVATE_RELEASE //hide the other possibilities
  EVT_MENU(ID_IncreaseVoltageForCurrentPstate, MainFrame::OnIncreaseVoltageForCurrentPstate)
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities
#ifdef wxHAS_POWER_EVENTS
  EVT_POWER_RESUME(MainFrame::OnResume)
  EVT_POWER_SUSPENDING(MainFrame::OnSuspending)
  EVT_POWER_SUSPENDED(MainFrame::OnSuspended)
  EVT_POWER_SUSPEND_CANCEL(MainFrame::OnSuspendCancel)
#endif // wxHAS_POWER_EVENTS

  //For stopping the DynVoltAndFreqScal thread that accesses the wxApp.
  //So stop the DVFS thread before destroying the wxApp object to avoid
  //crashes.
  EVT_CLOSE( MainFrame::OnClose ) 
//  EVT_INIT_DIALOG(MainFrame::OnInitDialog)
  //If no EVT_PAINT macro and Connect(wxEVT_PAINT, 
  //  wxPaintEventHandler(MyFrame::OnPaint));
  // : 100% CPU load.
  EVT_PAINT  (MainFrame::OnPaint)
  EVT_SIZE(MainFrame::OnSize)
  EVT_SIZING(MainFrame::OnSizing)

  EVT_MOTION(MainFrame::OnMoveMouse)
  EVT_LEFT_DOWN(MainFrame::OnLeftMouseButtonDown)
  //EVT_TIMER(-1,MainFrame::OnTimerEvent)
  EVT_TIMER(TIMER_ID, MainFrame::OnTimerEvent)
  //EVT_COMMAND_RIGHT_CLICK(FIRST_TRAYNOTIFY_ID, MainFrame::OnSysTrayIconClick)
//  EVT_MOUSE_EVENTS(FIRST_TRAYNOTIFY_ID, MainFrame::OnSysTrayIconClick)
END_EVENT_TABLE()

#define ATTACH_CPU_CONTROLLER_DYNAMIC_LIBRARY_T_STRING \
_T("Attach CPU &controller dynamic library...")
#define ATTACH_CPU_CORE_USAGE_GETTER_DYNAMIC_LIBRARY_T_STRING \
_T("Attach CPU &usage getter dynamic library...")
#define DETACH_CPU_CONTROLLER_DYNAMIC_LIBRARY_T_STRING \
_T("Detach CPU controller dynamic library")
#define DETACH_CPU_CORE_USAGE_GETTER_DYNAMIC_LIBRARY_T_STRING \
_T("Detach CPU usage getter dynamic library")

//for CPU_TEMP_IS_BELOW_CRITICAL, CPU_TEMP_IS_CRITICAL
#include <Controller/CPU-related/CPU_core_temperature_defs.h>

inline void MainFrame::CreateFileMenu()
{
  LOGN(/*"CreateFileMenu()"*/ "begin")
  wxMenuItem * p_wxmenuitem ;
  mp_wxmenuFile = new wxMenu;
  //wxMenu * p_wxmenuCore1 = new wxMenu;
//  wxMenu * p_wxmenuNorthBridge = new wxMenu;
  mp_wxmenuFile->Append( ID_About, _T("&About...") );
  mp_wxmenuFile->AppendSeparator();
  p_wxmenuitem = mp_wxmenuFile->Append( ID_AttachCPUcontrollerDynLib,
    ATTACH_CPU_CONTROLLER_DYNAMIC_LIBRARY_T_STRING );
  if( ! mp_wxx86infoandcontrolapp->GetCPUaccess() )
    p_wxmenuitem->SetHelp (
      //TODO more imformative/ user-friendly message (but may not be too long)
      wxT("No CPU access. See log file/ start this program as admin.") ) ;
  mp_wxmenuFile->Append( ID_DetachCPUcontrollerDynamicLibrary,
    DETACH_CPU_CONTROLLER_DYNAMIC_LIBRARY_T_STRING );
  mp_wxmenuFile->Append( ID_SetCPUcontrollerDynLibForThisCPU,
    wxString::Format(
      wxT("save as CPU controller dynamic library for this CPU "
        "(vendor:%s family:%u model:%u stepping:%u)")
      , wxWidgets::getwxString( mp_model->m_cpucoredata.m_strVendorID.c_str() ).c_str()
      , mp_model->m_cpucoredata.m_wFamily
      , (WORD) mp_model->m_cpucoredata.m_byModel
      , (WORD) mp_model->m_cpucoredata.m_byStepping
      )
    );
  if( ! //gp_cpucontrolbase->mp_wxdynlibcpucontroller
    mp_wxx86infoandcontrolapp->m_p_cpucontrollerDynLib
    )
  {
    mp_wxmenuFile->Enable( ID_SetCPUcontrollerDynLibForThisCPU, false ) ;
    mp_wxmenuFile->Enable( ID_DetachCPUcontrollerDynamicLibrary, false ) ;
  }
  mp_wxmenuFile->AppendSeparator();
  p_wxmenuitem = mp_wxmenuFile->Append( ID_AttachCPUusageGetterDynLib,
    ATTACH_CPU_CORE_USAGE_GETTER_DYNAMIC_LIBRARY_T_STRING );
  if( ! mp_wxx86infoandcontrolapp->GetCPUaccess() )
    p_wxmenuitem->SetHelp (
      //TODO more imformative/ user-friendly message (but may not be too long)
      wxT("No CPU access. See log file/ start this program as admin.") ) ;
  mp_wxmenuFile->Append( ID_DetachCPUusageGetterDynLib,
    DETACH_CPU_CORE_USAGE_GETTER_DYNAMIC_LIBRARY_T_STRING );
  if( ! //gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter
    mp_wxx86infoandcontrolapp->m_p_cpucoreusagegetterDynLib
    )
    mp_wxmenuFile->Enable( ID_DetachCPUusageGetterDynLib, false ) ;
  mp_wxmenuFile->AppendSeparator();
  mp_wxmenuFile->Append( ID_SaveAsDefaultPstates,
    //wxT("Save &performance states settings...")
    wxT("Save \"&voltage at/for frequency\" settings...")
    );
  //Only add menu item if creating the system tray icon succeeded: else one
  // can hide the window but can not restore it: if this process started
  //elevated one can't even close it!
  if( mp_wxx86infoandcontrolapp->ShowTaskBarIcon(this) )
  {
    mp_wxmenuFile->AppendSeparator();
//    mp_wxx86infoandcontrolapp->ShowTaskBarIcon(this) ;
    mp_wxmenuFile->Append( ID_MinimizeToSystemTray,
      _T("minimize this window to the "
          //"system tray"
          "task bar") );
  }
//#endif //COMPILE_WITH_TASKBAR
  mp_wxmenuFile->AppendSeparator();
  //mp_wxmenuFile->Append( ID_Service, _T("Run As Service") );
  mp_wxmenuFile->Append( ID_Quit, _T("E&xit") );

  //p_wxmenuBar->Append( mp_wxmenuFile, _T("&File") );
  //m_wxmenubar.Append( mp_wxmenuFile, _T("&File") );
  mp_wxmenubar->Append( mp_wxmenuFile, _T("&File") );
  LOG("after file menu creation"//\n"
    )
}

/** @brief creates menu items for the "GUI" menu. */
inline void MainFrame::CreateGUImenuItems()
{
  m_p_wxmenuGUI = NULL ;
  //#ifdef COMPILE_WITH_SERVICE_CONTROL
  #ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
    if( ! m_p_wxmenuGUI )
      m_p_wxmenuGUI = new wxMenu;
    mp_wxmenuitemOtherDVFS = m_p_wxmenuGUI->Append(
      //ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq
      ID_DisableOtherVoltageOrFrequencyAccess
      //_T("&CPU % min and max.")
      //_T("enable or disable OS's dynamic frequency scaling")
      , wxT("DISable OS's dynamic frequency scaling")
      );
    mp_wxmenuitemOtherDVFS = m_p_wxmenuGUI->Append(
      ID_EnableOtherVoltageOrFrequencyAccess
      , wxT("ENable OS's dynamic frequency scaling")
      );
    LOGN("after appending menu item \"disable OS's dynamic frequency scaling\"")
    //If one can not change the power scheme (Windows) etc.
    if( //mp_i_cpucontroller->mp_dynfreqscalingaccess->
      mp_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->
        ChangeOtherDVFSaccessPossible()
      )
      LOGN("Changing other DVFS is possible." )
    else
    {
      LOGN("Changing other DVFS is not possible." )
      mp_wxmenuitemOtherDVFS->Enable(false);
      mp_wxmenuitemOtherDVFS->SetHelp (
        wxT("Start e.g. as administrator to gain access") ) ;
      //mp_wxmenuitemOtherDVFS->SetItemLabel (wxT("dd") ) ;
      LOGN("changing other DVFS not possible")
    }
    if( //mp_i_cpucontroller->mp_dynfreqscalingaccess->EnablingIsPossible()
      mp_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->EnablingIsPossible()
      )
    {
      LOGN("enabling other DVFS is possible")
  //    std::tstring stdtstr = p_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->
  //        GetEnableDescription() ;
      std::wstring stdwstr = mp_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->
          GetEnableDescription() ;
      mp_wxmenuitemOtherDVFS = m_p_wxmenuGUI->Append(
        ID_EnableOtherVoltageOrFrequencyAccess
        //_T("enable OS's dynamic frequency scaling")
        //GetDisableDescrpition() under Windows may return "activate 'performance' power scheme ".
        //Use GetwxString(...) because GetEnableDescription() may return
        // -std::wstring although wxString uses char strings.
        // -std::string although wxString uses wchar_t strings.
        , wxWidgets::getwxString(
          //mp_i_cpucontroller->mp_dynfreqscalingaccess->GetEnableDescription()
  //        stdtstr
          stdwstr
           )
        );
      LOGN("after appending menu item \"" << GetStdString(stdwstr) << "\"")
    }
  #endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
    LOG("after extras menu creation"//\n"
      )
    if( ! m_p_wxmenuGUI )
      m_p_wxmenuGUI = new wxMenu;

    std::string stdstr = "set update view interval" ;
    LOGN("before appending menu item " << stdstr )
    m_p_wxmenuGUI->Append(
      ID_UpdateViewInterval,
      //_T("&CPU % min and max.")
      wxWidgets::getwxString( stdstr )
      );
    stdstr = "collect p-states as default voltage p-states" ;
    LOGN("before appending menu item " << stdstr )
    mp_wxmenuitemCollectAsDefaultVoltagePerfStates = m_p_wxmenuGUI->
      AppendCheckItem(
      ID_Collect_As_Default_Voltage_PerfStates,
      //_T("&CPU % min and max.")
      wxWidgets::getwxString( stdstr )
      );
  //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
    //wxMenu * m_p_wxmenuGUI = new wxMenu;
  //#endif
    //if( ! p_cpucontroller->mp_model->m_cpucoredata.
    //  m_stdsetvoltageandfreqDefault.empty()
    //  )
    {
      if( ! m_p_wxmenuGUI )
        m_p_wxmenuGUI = new wxMenu;//(wxT("Graphical User Interface"));
      stdstr = "enable own Dynamic Voltage and Frequency Scaling" ;
      LOGN("before appending menu item " << stdstr )
      mp_wxmenuitemOwnDVFS = m_p_wxmenuGUI->Append(
        ID_EnableOrDisableOwnDVFS
        , wxWidgets::getwxString( stdstr )
        );
      if( //p_cpucontroller->mp_model->m_cpucoredata.
        mp_model->m_cpucoredata.
        m_stdsetvoltageandfreqWanted.empty()
        )
      {
        //Does not work.
  //      mp_wxmenuitemOwnDVFS->Enable(false) ;
  //      m_p_wxmenuGUI->Enable(ID_EnableOrDisableOwnDVFS , false ) ;
        //      mp_wxmenuitemOwnDVFS->Enable(false) ;
        mp_wxmenuitemOwnDVFS->SetHelp( wxT("no desired voltages for frequencies"
          " available->no DVFS possible") ) ;
      }
    }
  //#endif //#ifdef _WIN32
  #ifdef COMPILE_WITH_MSR_EXAMINATION
    if( ! m_p_wxmenuGUI )
      m_p_wxmenuGUI = new wxMenu;
    m_p_wxmenuGUI->Append(ID_ShowExamineCPUregistersDialog,
      wxT("e&xamine CPUID and MSR CPU registers...")
        );
    m_p_wxmenuGUI->Append(ID_WriteToMSRdialog,
        wxT("read from and write to MSR dialog...") );
  #endif

    m_p_wxmenuGUI->Append( ID_LoadDetectInstableCPUcoreVoltageDynLib,
      wxT("Load dynamic library for detecting an instable CPU core voltage...")
      );
    m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib = m_p_wxmenuGUI->
      Append( ID_UnloadDetectInstableCPUcoreVoltageDynLib,
      wxT("Unload dynamic library for detecting an instable CPU core voltage")
      );
    if( m_p_wxmenuGUI )
    {
      LOGN("before adding menu \"GUI\"")
      mp_wxmenubar->Append(m_p_wxmenuGUI, //_T("E&xtras")
         wxT("&GUI") );
    }
}

MainFrame::MainFrame(
  const wxString & cr_wxstrTitle,
  const wxPoint & cr_wxpointTopLeftCornerPosition,
  const wxSize & cr_wxsize
  , I_CPUcontroller * p_cpucontroller
  //, CPUcoreData * p_cpucoredata
  , Model * p_model
  , wxX86InfoAndControlApp * p_wxx86infoandcontrolapp
  )
  : wxFrame( (wxFrame *) NULL, -1, cr_wxstrTitle,
    cr_wxpointTopLeftCornerPosition, cr_wxsize
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    //"Use this style to force a complete redraw of the window whenever it is
    //resized instead of redrawing just the part of the window affected by
    //resizing. Note that this was the behaviour by default before 2.5.1
    //release and that if you experience redraw problems with code which
    //previously used to work you may want to try this. Currently this style
    //applies on GTK+ 2 and Windows only, and full repainting is always done
    //on other platforms."
    //kurz: Stil ist notwendig, um das ganze Diagramm neu zu zeichnen
    , wxFULL_REPAINT_ON_RESIZE //| wxCLIP_CHILDREN
        //Necessary for showing a title bar
        | wxDEFAULT_FRAME_STYLE
    )
  //, mp_cpucoredata(p_cpucoredata)
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  , m_bAllowCPUcontrollerAccess ( true )
  , m_bCPUcoreUsageConsumed( true )
  , m_bDiagramNotDrawn(true)
  , mp_ar_voltage_and_multi( NULL )
  , m_bNotFirstTime(false)
  , m_bRangeBeginningFromMinVoltage ( true )
//  , s_arfTemperatureInDegreesCelsius( NULL )
#ifdef COMPILE_WITH_CALC_THREAD
  , mp_calculationthread( NULL )
#endif
  , mp_cpucoredata( & p_model->m_cpucoredata )
  , m_dwTimerIntervalInMilliseconds (1000)
//  , m_fPreviousCPUusage(0.0f)
//  , mp_freqandvoltagesettingdlg(NULL)
  , m_arp_freqandvoltagesettingdlg ( NULL )
  , m_p_freqandvoltagesettingsdialog(NULL)
  , mp_i_cpucontroller ( p_cpucontroller)
  , mp_model ( p_model )
//, m_bConfirmedYet(true)
  , m_vbAnotherWindowIsActive(false)
  , m_wMaximumCPUcoreFrequency ( 0 )
  , m_wMaxFreqInMHzTextWidth ( 0 )
  , m_wMaxVoltageInVoltTextWidth ( 0 )
  , m_wMaxTemperatureTextWidth ( 0 )
  //, m_wxbufferedpaintdcStatic( this )
  //Necessary for the timer to run:
  , mp_wxbitmap(NULL)
  , mp_wxbitmapStatic (NULL)
  , mp_wxbufferedpaintdcStatic( NULL)
//  , m_wxicon_drawer(16,16//,8
////    ,wxBITMAP_SCREEN_DEPTH
//    )
  //, mp_wxdynlibcpucontroller ( NULL )
  //, mp_wxdynlibcpucoreusagegetter ( NULL )
  , m_wxstrTitle(cr_wxstrTitle)
  , m_wxtimer(this)
  , mp_wxx86infoandcontrolapp ( p_wxx86infoandcontrolapp )
//  , m_xerces_voltage_for_frequency_configuration( p_model )
{
  LOGN("begin of main frame creation"//\n"
    )

  wxIcon wxicon;
  if( p_wxx86infoandcontrolapp->GetX86IandCiconFromFile(wxicon) )
  {
    SetIcon( wxicon);
  }
  else
  {
//    p_wxx86infoandcontrolapp->MessageWithTimeStamp(
//      GetStdWstring( wxT("setting icon for the main frame failed") )
//        );
    LOGN("setting icon from file for the main frame failed")
    wxIcon wxiconThisDialog( x86IandC_xpm ) ;
    SetIcon(x86IandC_xpm);
  }
  LOGN("# CPU cores: " <<
    (WORD) mp_model->m_cpucoredata.m_byNumberOfCPUCores)
//  m_p_wxtimer = new wxTimer( this ) ;
  mp_ar_voltage_and_multi = new VoltageAndMulti[
    mp_model->m_cpucoredata.m_byNumberOfCPUCores ] ;

  //"Cross-Platform GUI Programming with wxWidgets"
  // (Copyright © 2006 Pearson Education, Inc.)
  // ISBN 0-13-147381-6  "First printing, July 2005"
  //"CHAPTER 5 Drawing and Printing" ->
  //"UNDERSTANDING DEVICE CONTEXTS" -> "Drawing on Windows with wxPaintDC" :
  //"[...] call  SetBackgroundStyle with  wxBG_STYLE_CUSTOM to
  //  hint to some systems not to clear the background automatically."
  SetBackgroundStyle(wxBG_STYLE_CUSTOM );
//  m_bConfirmedYet = true ;
  mp_wxmenubar = new wxMenuBar;

  CreateFileMenu() ;

//#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  CreateServiceMenuItems() ;
#endif
//  CreateAndInitMenuItemPointers() ;

  //p_wxmenuCore0->Append( ID_FindLowestOperatingVoltage,
  //  _T("find lowest operating voltage") );

  //p_wxmenuAllCores->Append( ID_FindLowestOperatingVoltage,
  //_T("find lowest operating voltage") );

//#ifdef PRIVATE_RELEASE //hide the other possibilities
//  p_wxmenuCore0->Append( ID_IncreaseVoltageForCurrentPstate,
//    _T("increase voltage for current p-state (stabilize machine)") );
//#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities

  //UpdatePowerSettings(wxPOWER_UNKNOWN, wxBATTERY_UNKNOWN_STATE);

  CreateGUImenuItems();

  if( mp_i_cpucontroller != NULL )
  {
    CreateDynamicMenus();
  }
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
//    ////Connect the action, that is a class derived from class xx directly
//    ////with the menu item so that it is ensured to be the correct action
//    ////(calling the action in an event function is more error-prone)
//    ////TODO release memory
//    ////, new CalculationThread(byCoreID, HighALUloadThreadProc)
//    //, wxCommandEventHandler( MainFrame::OnOwnDynFreqScaling )
//    //) ;
//  mp_wxmenubar->Append(m_p_wxmenuGUI, _T("E&xtras") );
//#endif
  //mp_wxmenubar->Append( p_wxmenuNorthBridge, _T("&NorthBridge") );
  //TODO program crash here for unicode versions (for working versions
  // an OnSize() event was intermediate)
  LOGN("before setting menu bar " << mp_wxmenubar)
  //Just for testing.
//  wxSleep(5) ;
  //SetMenuBar( p_wxmenuBar );
  //SetMenuBar( & m_wxmenubar );
  SetMenuBar( mp_wxmenubar );

  LOGN("before creating status bar")
  CreateStatusBar();
//  const char bits [] = {
//      0,0,0,0,
//      1,1,1,1,
//      0,0,0,0,
//      1,1,1,1
//    } ;
//  wxIcon wxicon(bits,4,4) ;
//  SetIcon(wxicon);
//  Connect(wxEVT_PAINT, wxPaintEventHandler(MainFrame::OnPaint));
  //SetStatusText( _T("Welcome to wxWidgets!") );
  LOGN("before starting the update view timer")
  //m_wxtimer.Start(1000);
  m_wxtimer.Start(m_dwTimerIntervalInMilliseconds);
//  m_p_wxtimer->Start(m_dwTimerIntervalInMilliseconds);
  //http://docs.wxwidgets.org/stable/wx_wxtimer.html#wxtimersetowner:
  //"Associates the timer with the given owner object. When the timer is
  //running, the owner will receive timer events with id equal to id
  //specified here."
  m_wxtimer.SetOwner(this, TIMER_ID) ;
//  Connect(wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnSize));

  if( mp_wxx86infoandcontrolapp->//mp_wxdynlibcpucontroller
      m_p_cpucontrollerDynLib
    )
  {
    wxString wxstrCPUcontrollerDynLibPath(
//      //Use getwxString() to enable to compile with both unicode and ANSI.
//      getwxString(
//      mp_model->m_stdstrCPUcontrollerDynLibPath )
//      ) ;
    //http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString:
    mp_model->m_stdstrCPUcontrollerDynLibPath.c_str(), wxConvUTF8 );
#if wxUSE_UNICODE == 1
    DEBUGWN_WSPRINTF(L"CPU controller path as wide string:%ls",
      //wxstrCPUcontrollerDynLibPath.c_str()
      wxstrCPUcontrollerDynLibPath.wc_str() ) ;
#endif
    CPUcontrollerDynLibAttached(wxstrCPUcontrollerDynLibPath) ;
  }
  if( mp_wxx86infoandcontrolapp->//mp_wxdynlibcpucoreusagegetter
      m_p_cpucoreusagegetterDynLib
    )
  {
    wxString wxstrCPUcoreUsageGetterDynLibPath(
      //Use getwxString() to enable to compile with both unicode and ANSI.
      wxWidgets::getwxString(
      mp_model->m_stdstrCPUcoreUsageGetterDynLibPath)
      ) ;
    CPUcoreUsageGetterAttached(wxstrCPUcoreUsageGetterDynLibPath) ;
  }
//  mp_wxx86infoandcontrolapp->ShowTaskBarIcon(this) ;
  LOGN("end of main frame creation"//\n"
    )
//  RedrawEverything() ;
//  Refresh() ;
//  InitDialog() ;
}

MainFrame::~MainFrame()
{
  LOGN("begin" /*" of main frame's destructor"*/ )
  //TODO Error here.the problem was with destructor of wxMenuBar.->create 
  //wxMenuBar via "new"?
  //for(BYTE byCoreID = 0 ; byCoreID < //m_byCoreNumber
  //  mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCoreID )
  //{
  //  //Release memory from the heap.
  //  delete m_vecp_wxmenuCore.at(byCoreID) ;
  //}

  //Release memory for array of pointers.
//  delete [] m_arp_wxmenuitemPstate ;
//  delete [] marp_wxmenuItemHighLoadThread ;
	#ifdef COMPILE_WITH_CALC_THREAD
  if( mp_calculationthread )
    delete mp_calculationthread ;
	#endif
  //Only terminates correctly when deleted from here (is also deleted in
  //the wxTopLevelWindow's (wxDialog's) destructor in wxWidgets' "tbtest.cpp"
  //from the "taskbar" sample.
  mp_wxx86infoandcontrolapp->DeleteTaskBarIcons();
  LOGN("end" /*" of main frame's destructor"*/ )
}

//void MyFrame::AddMenu()
//{
//  //wxEventTableEntry(type, winid, idLast, fn, obj)
//  EVT_MENU(ID_LastStaticEventID+1,MyFrame::OnDynamicMenu);
//}
//
//void MyFrame::OnDynamicMenu(wxCommandEvent &event)
//{
//	
//}

//wxString MainFrame::BuildHighLoadThreadMenuText(
//  std::string str,
//  BYTE byPreviousAction)
//{
//  wxString wxstr = byPreviousAction == //ENDED ?
//    ICalculationThread::ended ?
//    //We need a _T() macro (wide char-> L"", char->"") for EACH
//    //line to make it compatible between char and wide char.
//    _T("Start") :
//    //We need a _T() macro (wide char-> L"", char->"") for EACH
//    //line to make it compatible between char and wide char.
//    _T("End") ;
//  ////Invert the menu item's checked state.
//  //marp_wxmenuItemHighLoadThread[byCoreID]->Check(
//  //    ! marp_wxmenuItemHighLoadThread[byCoreID]->IsChecked () ) ;
////  marp_wxmenuItemHighLoadThread[byCoreID]->SetText(
////      wxstr +
////      str
////      );
//    return wxstr + wxString(
//      //TODO this conversion may not work
//      (const wxChar*) str.c_str() ) ;
//}

//bool MainFrame::Confirm(const std::string & str)
//{
//  //::AfxMessageBox(str.c_str());
//
//  //To not show too many dialogs that the timer would bring up.
////  if( m_bConfirmedYet )
////  {
////    m_bConfirmedYet = false ;
////    ::wxMessageBox(wxString(
////      //TODO this conversion may not work
////      (const wxChar * ) str.c_str() )
////      );
////    m_bConfirmedYet = true ;
////  }
//  //m_bConfirmedYet = true ;
//  return true;
//}

//bool MainFrame::Confirm(std::ostrstream & r_ostrstream
//  //std::ostream & r_ostream
//  )
//{
//  bool bReturn = true ;
//  DEBUGN("begin");
//  //Must set this, else text may follow after the string we want.
//  //I had program crashes with the following method:
//  //pch[r_ostrstream.pcount()] = '\0' ;
//  //r_ostrstream.ends();
//  r_ostrstream.put('\0'); //the same as "ends()" does.
////  char *pch = r_ostrstream.str() ;
//  //r_ostrstream.flush();
//  //To not show too many dialogs that the timer would bring up.
////  if( m_bConfirmedYet )
////  {
////    m_bConfirmedYet = false ;
////    int nReturn = ::wxMessageBox( wxString(
////      //TODO this conversion may not work
////      (const wxChar * ) pch ) ,
////      //We need a _T() macro (wide char-> L"", char->"") for EACH
////      //line to make it compatible between char and wide char.
////      wxString( _T("Message") ) , wxCANCEL | wxOK ) ;
////    if( nReturn == wxCANCEL )
////      bReturn = false ;
////    m_bConfirmedYet = true ;
////  }
//  //return true;
//  DEBUGN("end");
//  return bReturn ;
//}

inline void MainFrame::ConnectToDataProvider_Inline()
{
  mp_wxx86infoandcontrolapp->ConnectToDataProviderAndShowResult() ;
}

#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
void MainFrame::CreateServiceMenuItems()
{
  wxString wxstrConnectOrDisconnect ;
  p_wxmenuService = new wxMenu ;
  //TODO: set bitmap for item
  //http://docs.wxwidgets.org/trunk/classwx_menu_item.html#a2b5d6bcb820b992b1e4709facbf6d4fb:
  //"SetBitmap() must be called before the item is appended to the menu"
//  wxMenuItem * p_wxmenuitem = new wxMenuItem();
//  p_wxmenuitem->SetBitmap();
  if( ServiceBase::CanStartService() )
  {
    p_wxmenuService->Append( ID_StartService , wxT("&start"),
      wxT("start the x86I&C service via the Service Control Manager") ) ;
  }
  else
    p_wxmenuService->Append( ID_StartService , wxT("&start"),
      wxT("insufficient rights to start the x86I&C service via the Service "
      "Control Manager") ) ;

  //Stopping a service can be done via Inter Process Communication. (Else if
  //via Service Control manager then needs the same? rights/ privileges as
  //starting a service).
  p_wxmenuService->Append( ID_StopService , wxT("s&top"),
    wxT("stop the x86I&C service via InterProcess Communication or via "
      "Service Control Manager") );

#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  p_wxmenuService->Append( ID_ConnectToService,
    wxT("connect..."), wxT("connect to an arbitrary address to the (x86IandC) "
      "CPU core data provider/ server/ service") );
//  if( mp_wxx86infoandcontrolapp->m_ipcclient.IsConnected() )
//    wxstrConnectOrDisconnect = wxT("disconnect") ;
//  else
    wxstrConnectOrDisconnect = wxT("c&onnect to ") + wxWidgets::getwxString(
      mp_model->m_userinterfaceattributes.m_std_wstrServiceAddress);
  p_wxmenuService->Append( ID_ConnectToOrDisconnectFromService,
    wxstrConnectOrDisconnect, wxT("connect to the (x86IandC) "
      "CPU core data provider/ server/ service"
//      " specified in the x86IandC "
//      "config file"
      ) );
  p_wxmenuService->Append( ID_DisconnectFromService, wxT("&DISCOnnect"),
    wxT("DISconnect from the (x86IandC) "
      "CPU core data provider/ server/ service if currently connected") );
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  //pause and continue is possible via service ctrl mgr
  p_wxmenuService->Append( ID_ContinueService, wxT("&Continue"), wxT("continue "
    "the Dynamic Voltage and Frequency Scaling done by the x86IandC service/ "
    "server") );
  p_wxmenuService->Append( ID_PauseService , wxT("&Pause"), wxT("pause "
    "the Dynamic Voltage and Frequency Scaling done by the x86IandC service/ "
    "server") );
//  p_wxmenuService->Append( ID_StartService , _T("&Start") );
//  p_wxmenuService->Append( ID_StopService , _T("Sto&p") );
    
  mp_wxmenubar->Append( p_wxmenuService, wxT("&Service") ) ;
  LOGN("end" /*" of CreateServiceMenuItems"*/ )
}
#endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL

////void
//wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
//  wxMenu * p_wxmenu,
//  WORD & r_wMenuID,
//  const wxString & r_wxstr
//  )
//{
//  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
//  Connect( r_wMenuID ++ , //wxID_ANY,
//    wxEVT_COMMAND_MENU_SELECTED ,
//    wxCommandEventHandler(MainFrame::OnDynamicallyCreatedUIcontrol)
//    );
//  return p_wxmenuitemAppended ;
//}

//wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
//  wxMenu * p_wxmenu,
//  WORD & r_wMenuID,
//  const wxString & r_wxstr
//  , //void (wxEvtHandler::*wxEventFunction)(wxEvent&)
//  wxObjectEventFunction wxeee
//  , SpecificCPUcoreActionAttributes * p_scaa
//  )
//{
//  m_stdmapwxuicontroldata.insert(
//    std::make_pair(
//      r_wMenuID ,
//      //TODO release memory
//      p_scaa
//      )
//    ) ;
//  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
//  Connect( r_wMenuID ++ , //wxID_ANY,
//    wxEVT_COMMAND_MENU_SELECTED ,
//    //wxCommandEventHandler(
//    //wxEventFunction //)
//    wxeee
//    );
//  return p_wxmenuitemAppended ;
//}

//wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
//  wxMenu * p_wxmenu,
//  WORD & r_wMenuID,
//  const wxString & r_wxstr,
//  //Use a concrete class as parameter because this is more flexible than
//  //using a function with (a) fixed parameter type(s).
//  //Another option: use C functions: then parameter here would be:
//  //"void func(PVOID)"
//  //function A (PVOID pv)
//  //{ ActionAParamStruct * paramstr = (ActionAParamStruct)
//  // ActionAParamStruct: necessary parameters for execution (like CPU core ID)
//  //   pv ; }
//  //
//  I_CPUcontrollerAction * icpuca
//  )
//{
//  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
//  m_stdmapwmenuid2i_cpucontrolleraction.insert(
//    std::make_pair (r_wMenuID,icpuca)
//    ) ;
//  Connect( r_wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
//    wxCommandEventHandler(MainFrame::OnDynamicallyCreatedUIcontrol)
//    );
//  return p_wxmenuitemAppended ;
//}

//void MainFrame::CreateAndInitMenuItemPointers()
//{
//  marp_wxmenuItemHighLoadThread = new wxMenuItem * [
//      mp_cpucoredata->m_byNumberOfCPUCores ] ;
//  for( BYTE byIndex = 0 ; byIndex < mp_cpucoredata->m_byNumberOfCPUCores ;
//      ++ byIndex )
//      marp_wxmenuItemHighLoadThread[ byIndex ] = NULL ;
//}
//
////void
////Return: TRUE: success.
//BYTE MainFrame::AddSetPstateMenuItem(
//  wxMenu * p_wxmenuCore
//  , BYTE byCoreID
//  , BYTE byPstateID
//  //Must be a reference because changes to the variable should be
//  //maintained OUTside this function.
//  , WORD & r_wMenuID
//  )
//{
//    BYTE byReturnValue = FALSE ;
//    return byReturnValue ;
//}

void MainFrame::Create1DialogAndMenuForAllCores()
{
  wxMenu * p_wxmenuCore ;
  p_wxmenuCore = new wxMenu;
  if( p_wxmenuCore )
  {
    m_p_wxmenuCore = p_wxmenuCore;
    p_wxmenuCore->Append(//ID_LastStaticEventID
      ID_ShowVoltageAndFrequencySettingsDialog,
      _T("set frequency and voltage ") );
    mp_wxmenubar->Append(
      p_wxmenuCore,
      //We need a wxT() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      wxT("core(s)")
      );
//    Connect( ID_LastStaticEventID, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
//      wxCommandEventHandler(
//        MainFrame::OnVoltageAndFrequencySettingsDialog )
//      );
//    LOGN("Connected event ID" << ID_LastStaticEventID << " to" <<
//      "MainFrame::OnPstateDialog.")
  }
}

void MainFrame::CreateDialogAndMenuForEveryCore()
{
//  BYTE byReturnValue ;
  WORD wMenuID = ID_LastStaticEventID;
  wxMenu * p_wxmenuCore ;
  m_arp_freqandvoltagesettingdlg = new FreqAndVoltageSettingDlg * [
    mp_cpucoredata->m_byNumberOfCPUCores];
  if(m_arp_freqandvoltagesettingdlg)
  {
    BYTE byPointerSize = sizeof( m_arp_freqandvoltagesettingdlg[0] ) ;
    //important: init pointers with NULL
    memset(
      m_arp_freqandvoltagesettingdlg
      , //NULL
        0
      , byPointerSize * mp_cpucoredata->m_byNumberOfCPUCores );
  //TRACE("sizeof: %u\n", sizeof(m_arp_freqandvoltagesettingdlg));
  #ifdef _DEBUG
  //  int i = sizeof(m_arp_freqandvoltagesettingdlg) ;
  #endif
    for( BYTE byCoreID = 0 ; byCoreID < //m_byCoreNumber
      mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCoreID )
    {
      p_wxmenuCore = new wxMenu;
      //Memorize dynamically created menus in order to delete them of a
      // CPU controller DLL is attached more than once (else the wxWindows
      // are deleted automatically when the superordinate window is closed).
      m_vecp_wxmenuCore.push_back(p_wxmenuCore);
      p_wxmenuCore->Append(wMenuID, _T("set frequency and voltage ") );
      //for(BYTE byDivisorID = 0 ; byDivisorID < FIRST_RESERVED_DIVISOR_ID ;
      //  ++ byDivisorID )
      //{
      //  Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
      //    wxCommandEventHandler(MyFrame::OnRuntimeCreatedMenu)//, & m_vecwxstring.back()
      //    );
      //}
      if( byCoreID == 0 )
        m_nLowestIDForSetVIDnFIDnDID = wMenuID ;
  //    wxCPUcoreID wxcpucoreid(byCoreID)  ;
  //    m_stdvectorwxuicontroldata.push_back( //wxCPUcoreID(byCoreID)
  //      wxcpucoreid
  //      //wxObject()
  //      ) ;
      m_stdmapwxuicontroldata.insert(
        std::make_pair(
          wMenuID ,
          //TODO release memory
          //new wxCPUcoreID(byCoreID)
          new SpecificCPUcoreActionAttributes(byCoreID)
          //wxcpucoreid
          //wxObject()
          )
        ) ;
  #ifdef _DEBUG
  //    wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
        //&
        m_stdmapwxuicontroldata.find( wMenuID )->second ;
      //wxCPUcoreID & r_wxcpucoreid = (wxCPUcoreID &) //wxevent.m_callbackUserData ;
      //  m_stdmapwxuicontroldata.find( wMenuID )->second ;
  #endif
//      Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
//        wxCommandEventHandler(
//          //MainFrame::OnDynamicallyCreatedUIcontrol
//          MainFrame::OnVoltageAndFrequencySettingsDialog )//, & m_vecwxstring.back()
//        //new wx
//        //, & m_stdvectorwxuicontroldata.back()
//        );
//      LOGN("connected event ID" << wMenuID - 1 << " to" <<
//        "MainFrame::OnPstateDialog")
      p_wxmenuCore->Append(wMenuID, _T("find different p-states") );
      Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(
        MainFrame::OnFindDifferentPstates )
        );
      LOGN("connected event ID" << wMenuID - 1 << " to" <<
        "MainFrame::OnFindDifferentPstates")
      #ifdef COMPILE_WITH_CALC_THREAD
      marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
        p_wxmenuCore,
        wMenuID, //_T("high load thread (for stability check)")
        BuildHighLoadThreadMenuText(
         "high FPU load thread (for stability check)"
          )
        //Connect the action, that is a class derived from class xx directly
        //with the menu item so that it is ensured to be the correct action
        //(calling the action in an event function is more error-prone)
        //TODO release memory
        //, new CalculationThread(byCoreID, FPUcalculationThreadProc)
        , wxCommandEventHandler( MainFrame::OnHighLoadThread )
        , new CalculationThread(
          byCoreID
          , FPUcalculationThreadProc
          , & ::wxGetApp()
          , ::wxGetApp().GetCPUcontroller()
          )
        ) ;
      //#endif //#ifdef COMPILE_WITH_CALC_THREAD
      marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
        p_wxmenuCore,wMenuID, //_T("high load thread (for stability check)")
        BuildHighLoadThreadMenuText(
        std::string( "high ALU load thread (for stability check)" )
          )
        //Connect the action, that is a class derived from class xx directly
        //with the menu item so that it is ensured to be the correct action
        //(calling the action in an event function is more error-prone)
        //TODO release memory
        //, new CalculationThread(byCoreID, HighALUloadThreadProc)
        , wxCommandEventHandler( MainFrame::OnHighLoadThread )
        , new CalculationThread(
          byCoreID
          , HighALUloadThreadProc
          , & ::wxGetApp()
          , ::wxGetApp().GetCPUcontroller()
          )
        ) ;
      #endif //    #ifdef COMPILE_WITH_CALC_THREAD

      //marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
      //  p_wxmenuCore,
      //  wMenuID, //_T("high load thread (for stability check)")
      //   _T("own DVFS")
      //  //Connect the action, that is a class derived from class xx directly
      //  //with the menu item so that it is ensured to be the correct action
      //  //(calling the action in an event function is more error-prone)
      //  //TODO release memory
      //  //, new CalculationThread(byCoreID, HighALUloadThreadProc)
      //  , wxCommandEventHandler( MainFrame::OnOwnDynFreqScaling )
      //  , new SpecificCPUcoreActionAttributes(byCoreID)
      //  ) ;
      //OnOwnDynFreqScaling

      //m_arp_wxmenuitemPstate[byCoreID * NUMBER_OF_PSTATES + 1] =
      //    AddDynamicallyCreatedMenu(p_wxmenuCore,wMenuID, _T("Set p-state &1")) ;
      m_byNumberOfSettablePstatesPerCore = //NUMBER_OF_PSTATES ;
        0 ;
      if( byCoreID == 0 )
        m_byMenuIndexOf1stPstate = wMenuID - m_nLowestIDForSetVIDnFIDnDID ;
      //if( typeid (mp_i_cpucontroller ) == typeid(GriffinController) )
      //{
      //  for( byPstateID = 0 ; byPstateID < //3
      //    m_byNumberOfSettablePstatesPerCore ; ++ byPstateID &&
      //      //if == TRUE
      //      byReturnValue
      //     )
      //    byReturnValue = AddSetPstateMenuItem(
      //      p_wxmenuCore, byCoreID, byPstateID, wMenuID ) ;
      //}
//      if( byReturnValue )
      {
        if( byCoreID == 0 )
        {
          m_nNumberOfMenuIDsPerCPUcore = wMenuID - ID_LastStaticEventID ;
          //For removing per-core menus after unloading a CPU controller.
          m_byIndexOf1stCPUcontrollerRelatedMenu = mp_wxmenubar->GetMenuCount()
            ;
        }
          //marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
          //    p_wxmenuCore,wMenuID, _T("high load thread (for stability check)")) ;
          //m_wxmenubar.Append(p_wxmenuCore, _T("for core &")+ byCoreID);
          mp_wxmenubar->Append(
            p_wxmenuCore,
            wxString::Format(
            //We need a _T() macro (wide char-> L"", char->"") for EACH
            //line to make it compatible between char and wide char.
            _T("%s%u")
            //, _T("for core &")
            , _T("core &")
            , //_T('0'+byCoreID)
              byCoreID )
            );
          //marp_wxmenuItemHighLoadThread[byCoreID]->Check(true) ;
          //marp_wxmenuItemHighLoadThread[byCoreID]->Enable(false);
      }
    } //for-loop
  }
}

//void
//Return value:
BYTE MainFrame::CreateDynamicMenus()
{
//  BYTE byPstateID = 0 ;
  BYTE byReturnValue =
      //Needs to be TRUE for the 1st loop condition evaluation.
      TRUE ;
//  WORD wMenuID = ID_LastStaticEventID;
  //m_vecwxstring.push_back(wxString)
//  wxMenu * p_wxmenuCore ;
  LOGN("CPU core menu creation--number of CPU cores: " <<
    (WORD) mp_cpucoredata->m_byNumberOfCPUCores )
//  byReturnValue =
//  CreateDialogAndMenuForEveryCore() ;
  Create1DialogAndMenuForAllCores() ;

  //SetMenuBar(&m_wxmenubar);
  return byReturnValue ;
}

void MainFrame::AllowCPUcontrollerAccess()
{
  wxCriticalSectionLocker wxcriticalsectionlocker( 
    m_wxcriticalsectionCPUctlAccess ) ;
  m_bAllowCPUcontrollerAccess = true ;
}

void MainFrame::DenyCPUcontrollerAccess()
{
  DEBUGN("MainFrame::DenyCPUcontrollerAccess() begin" )

  wxCriticalSectionLocker wxcriticalsectionlocker( 
    m_wxcriticalsectionCPUctlAccess ) ;
  m_bAllowCPUcontrollerAccess = false ;
  DEBUGN("MainFrame::DenyCPUcontrollerAccess() end" )
}

void MainFrame::DisableWindowsDynamicFreqScalingHint()
{
   wxMessageBox(
     //We need a _T() macro (wide char-> L"", char->"") for EACH 
     //line to make it compatible between char and wide char.
     _T("Remember: the OS' dynamic frequency scaling should be ")
     _T("deactivated the change to take effect\n")
     _T("see help/ disable by: ")
     _T("Start->control panel->energy options")
     _T("->change the current power plan")
     _T("->change extended energy options")
     _T("->set minimal and maximal CPU load to the same percentage") 
     );
}

void MainFrame::OnClose(wxCloseEvent & event )
{
  LOGN("begin" /*"Requested to close the main frame"*/)
  //Stop the timer (indirectly calls OnPaint()-> so another IPC thread could
  //be spawned).
  m_wxtimer.Stop() ;
  LOGN("Stopped the \"update view\" timer")
//  m_p_wxtimer->Stop() ;
  mp_wxx86infoandcontrolapp->EndGetCPUcoreDataViaIPCthread() ;
  //May be NULL at startup.
  if( mp_cpucoredata->m_arp_percpucoreattributes
    // CPU cores > 0
    && mp_cpucoredata->m_byNumberOfCPUCores )
  {
    PerCPUcoreAttributes * p_percpucoreattributes = & mp_cpucoredata->
        m_arp_percpucoreattributes[ //p_atts->m_byCoreID 
        0 ] ;
    if ( p_percpucoreattributes->mp_dynfreqscalingthread )
    {
      p_percpucoreattributes->mp_dynfreqscalingthread->Stop() ;
      LOGN("stopped the Dynamic Voltage and Frequency Scaling thread")
     //p_percpucoreattributes->mp_dynfreqscalingthread->Delete() ;
      p_percpucoreattributes->mp_dynfreqscalingthread = NULL ;
    }
  }
  mp_wxx86infoandcontrolapp->CheckForChangedVoltageForFrequencyConfiguration();
  mp_wxx86infoandcontrolapp->DeleteTaskBarIcons();
  LOGN("before destroying the mainframe")
  //see http://docs.wxwidgets.org/2.8/wx_windowdeletionoverview.html:
  this->Destroy() ;
  LOGN("after destroying the mainframe")
}

void MainFrame::OnCollectAsDefaultVoltagePerfStates(
  wxCommandEvent & WXUNUSED(event) )
{
  mp_model->m_bCollectPstatesAsDefault = 
    mp_wxmenuitemCollectAsDefaultVoltagePerfStates->IsChecked () ;
}

void MainFrame::OnConnectToService( wxCommandEvent & WXUNUSED(event) )
{
  wxString wxstrTextFromUser = ::wxGetTextFromUser(
    wxT("enter server address") //const wxString & message,
    //const wxString & caption = "Input text",
    //const wxString & default_value = "",
    //wxWindow * parent = NULL,
    //int x = wxDefaultCoord,
    //int y = wxDefaultCoord,
    //bool centre = true
    ) ;
//  std::string strstrMessage ;
  mp_wxx86infoandcontrolapp->ConnectIPCclient(
    wxstrTextFromUser //,
    //strstrMessage
    ) ;
}

void MainFrame::OnConnectToOrDisconnectFromService(
  wxCommandEvent & WXUNUSED(event) )
{
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  if( //::wxGetApp().m_ipcclient.IsConnected()
      mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
      )
    ::wxMessageBox(
      //Use wxT() to enable to compile with both unicode and ANSI.
      wxT("already connected to the service")
      ) ;
  else
  {
//    if( ::wxGetApp().m_ipcclient.ConnectToDataProvider() )
//      p_wxmenuService->SetLabel( ID_ConnectToOrDisconnectFromService ,
//        wxT( "disconnect" )
//        ) ;
    ConnectToDataProvider_Inline() ;
  }
#endif
}

void MainFrame::OnContinueService(wxCommandEvent & WXUNUSED(event))
{
  LOGN(//"OnContinueService--"
      "begin")
  //ServiceBase::ContinueService( //mp_model->m_strServiceName.c_str() 
  //  "CPUcontrolService" );
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  wxString wxstrMessageFromDataProvider;
  if( ! mp_wxx86infoandcontrolapp->
      ContinueServiceViaIPC(wxstrMessageFromDataProvider)
    )
//    ::wxMessageBox( wxT("message from the service:\n") +
//        wxstrMessageFromDataProvider ) ;
//  else
    ::wxMessageBox(
      //Use wxT() to enable to compile with both unicode and ANSI.
      wxT("could not continue because not connected to the service")
      ) ;
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
}

void MainFrame::OnDisableOtherVoltageOrFrequencyAccess(
  wxCommandEvent & WXUNUSED(event) )
{
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  ::wxGetApp().mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
}

void MainFrame::OnDisconnectFromService(
  wxCommandEvent & WXUNUSED(event) )
{
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  if( //mp_wxx86infoandcontrolapp->m_ipcclient.IsConnected()
      mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
    )
    //mp_wxx86infoandcontrolapp->m_ipcclient.Disconnect_Inline() ;
    mp_wxx86infoandcontrolapp->IPCclientDisconnect() ;
  else
  {
    ::wxMessageBox(
      //Use wxT() to enable to compile with both unicode and ANSI.
      wxT("NOT connected to the service")
      ) ;
  }
#endif
}

void MainFrame::OnEnableOtherVoltageOrFrequencyAccess(
  wxCommandEvent & WXUNUSED(event) )
{
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  ::wxGetApp().mp_dynfreqscalingaccess->EnableFrequencyScalingByOS() ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
}

void MainFrame::OnFindDifferentPstates( wxCommandEvent & WXUNUSED(event) )
{
  //May be NULL at startup (before a controller DLL becomes attached).
  if( mp_i_cpucontroller )
  {
    //wxString wxstrInput = ::wxGetTextFromUser( wxstrMessage , wxT("input"), 
    //  wxT("1000") ) ;
    ////::wxGetNumberFromUser
    ////If the user has input text (and has NOT pressed "cancel")
    //if( ! wxstrInput.empty() )
    //{
    //  unsigned long ulMs ;
    //  bool bSuccess = wxstrInput.ToULong(& ulMs) ;
    //  if ( bSuccess )
    //  {
    //    //ReadMsrEx() returned false results if used with a time and a 10 ms interval.
    //    if( ulMs < 100 )
    //      wxMessageBox( wxT("the number is too low. "
    //      "Getting the current CPU frequency returned wrong values with < 100 ms") ) ;
    //    else
    //    {
    //      m_dwTimerIntervalInMilliseconds = ulMs ;
    //      m_wxtimer.Stop() ;
    //      m_wxtimer.Start(ulMs) ;
    //    }
    //  }
    //  else
    //  {
    //    wxMessageBox( wxT("You did not enter a valid integer number") ) ;
    //  }
    //}
    //Must create dynamically, else the CalculationThread is destroyed after leaving
    //this block.
    //if( ! mp_calculationthread )
    //{
    //  mp_calculationthread = new CalculationThread( 
    //  0 
    //  , FindDifferentPstatesThreadProc
    //  , & ::wxGetApp() 
    //  , ::wxGetApp().GetCPUcontroller() 
    //  ) ;
    //}
    //if( mp_calculationthread )
    //  mp_calculationthread->Execute() ;
    long lMin = ::wxGetNumberFromUser(
      wxT("input mininum frequency:"), 
      wxT("prompt:"), 
      wxT("caption:"), 
      mp_i_cpucontroller->GetMinimumFrequencyInMHz() ,
      0 ,
      mp_i_cpucontroller->GetMaximumFrequencyInMHz()    
      ) ;
    //If the user enters an invalid value or cancels the dialog, the function will return -1.
    if( lMin == -1 )
      ::wxMessageBox(wxT("either invalid value or cancel") ) ;
    else
    {
      long lMax = ::wxGetNumberFromUser(
        wxT("input maxinum frequency:"), 
        wxT("prompt:"), 
        wxT("caption:"), 
        mp_i_cpucontroller->GetMaximumFrequencyInMHz() ,
        lMin ,
        mp_i_cpucontroller->GetMaximumFrequencyInMHz()
        ) ;
      if( lMax == -1 )
        ::wxMessageBox(wxT("either invalid value or cancel") ) ;
      else
      {
        //TODO use available _mulipliers_ and call GetVoltageAndFrequency(
        //  voltage, multiplier, refclock, )
        //instead of frequencies
//        std::set<VoltageAndFreq> stdsetvoltageandfreq ;
//        std::set<VoltageAndFreq>::iterator iter ;
//        mp_i_cpucontroller->GetAllPossibleFrequencies( stdsetvoltageandfreq ) ;
//        //iter = stdsetvoltageandfreq.find(lMin) ;
//        iter = stdsetvoltageandfreq.begin() ;
//        while( iter != stdsetvoltageandfreq.end() )
//        {
//          if( lMin >= iter->m_wFreqInMHz )
//            break ;
//          iter ++ ;
//        }
//
//
//        //if( iter != stdsetvoltageandfreq.end() )
//        {
//          float fVolt ;
//          WORD wFreq ;
//          //for( WORD wFreq = lMin ; wFreq < lMax ; wFreq += 50 )
//          while( iter != stdsetvoltageandfreq.end() )
//          {
//            //mp_i_cpucontroller->GetNearestHigherPossibleFreqInMHz( lMin ) ;
//            mp_i_cpucontroller->SetFreqAndVoltageFromFreq( //wFreq,
//              iter->m_wFreqInMHz ,
//              mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
//              //mp_model->m_cpucoredata.m_stdsetvoltageandfreqPossibleByCPU
//              //stdsetvoltageandfreq
//              , 0 ) ;
//            ::wxMilliSleep(100) ;
//            mp_i_cpucontroller->GetCurrentPstate( //wFreq,
//              wFreq ,
//              fVolt
//              , 0 ) ;
//            if( wFreq == iter->m_wFreqInMHz )
//              if( mp_model->m_cpucoredata.AddDefaultVoltageForFreq( fVolt, wFreq ) )
//                RedrawEverything() ;
//            ++ iter ;
//          }
//        }
      }
    }
    //::wxMilliSleep()
  } //if( mp_i_cpucontroller )
}

void MainFrame::OnLoadDetectInstableCPUcoreVoltageDynLib(wxCommandEvent & event)
{
#ifdef _WIN32
  mp_wxx86infoandcontrolapp->LoadDetectInstableCPUcoreVoltageDynLib();
#endif
}

void MainFrame::OnUnloadDetectInstableCPUcoreVoltageDynLib(wxCommandEvent & event)
{
#ifdef _WIN32
  mp_wxx86infoandcontrolapp->UnloadDetectInstableCPUcoreVoltageDynLib();
#endif
}

void MainFrame::OnMinimizeToSystemTray(wxCommandEvent & WXUNUSED(event))
{
  Hide() ;
}

void MainFrame::OnMoveMouse(wxMouseEvent & r_wxmouseevent)
{
  LOGN( "begin")
  wxPoint wxpoint = r_wxmouseevent.GetPosition();

  float fReferenceClockInMHz;
  const float fMultiplier = GetClosestMultiplier(wxpoint.x,
    fReferenceClockInMHz);
  const float fClosestFrequency = fMultiplier * fReferenceClockInMHz;

  const float fClosestVoltageToYcoordinate = GetClosestVoltageForYcoordinate(
    wxpoint.y);

  SetStatusText( wxString::Format( wxT("(%f Volt,%f MHz)"),
    fClosestVoltageToYcoordinate, fClosestFrequency ) );
  LOGN( "end")
}

void MainFrame::OnLeftMouseButtonDown(wxMouseEvent & r_wxmouseevent)
{
  const wxPoint & c_r_wxpointPos = r_wxmouseevent.GetPosition();

  if( m_p_freqandvoltagesettingsdialog )
  {
    float fVoltageInVolt = GetClosestVoltageForYcoordinate(c_r_wxpointPos.y);
    float fReferenceClockInMHz;
    float fMultiplier = GetClosestMultiplier(c_r_wxpointPos.x,
      fReferenceClockInMHz);

    m_p_freqandvoltagesettingsdialog->SetMultiplierSliderToClosestValue(
      fMultiplier);
    m_p_freqandvoltagesettingsdialog->HandleMultiplierValueChanged() ;

    BYTE byIndexForClosestVoltage = m_p_freqandvoltagesettingsdialog->
      SetVoltageSliderToClosestValue(fVoltageInVolt) ;
    m_p_freqandvoltagesettingsdialog->ChangeVoltageSliderValue(
      byIndexForClosestVoltage) ;

    m_p_freqandvoltagesettingsdialog = NULL;
    SetCursor(wxNullCursor);
  }
}

void MainFrame::OnPauseService(wxCommandEvent & WXUNUSED(event))
{
  LOGN( //"OnPauseService"
    "begin")
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  wxString wxstrMessageFromService;
  mp_wxx86infoandcontrolapp->PauseService(wxstrMessageFromService);
//  ::wxMessageBox( wxT("message from the service:\n") + wxstrMessageFromService
//    ) ;
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
}

//void MainFrame::OnShowVoltageAndFrequencySettingsDialog(
//    wxCommandEvent & WXUNUSED(event) )
//{
//
//}

void MainFrame::OnStartService(wxCommandEvent & WXUNUSED(event))
{
  #ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  mp_wxx86infoandcontrolapp->StartService() ;
  #endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
}

void MainFrame::OnStopService(wxCommandEvent & WXUNUSED(event))
{
#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  //ServiceBase::StopService( //mp_model->m_strServiceName.c_str() 
  //  "CPUcontrolService" );
  mp_wxx86infoandcontrolapp->StopService() ;
#endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
}

void MainFrame::OnSysTrayIconClick(wxCommandEvent & WXUNUSED(event))
{
  ::wxMessageBox( wxT("OnSysTrayIconClick") ) ;
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
//  wxString wxstrMessage ;
//  GetAboutMessage(wxstrMessage) ;
//  ::wxMessageBox(
//    wxstrMessage ,
//    _T("About ") //_T(PROGRAM_NAME)
//    //+ mp_i_cpucontroller->mp_model->m_stdtstrProgramName
//    + mp_model->m_stdtstrProgramName
//    ,
//    wxOK | wxICON_INFORMATION //,
////    this
//    );
  AboutDialog * p_aboutdialog = new AboutDialog(
    * mp_model, wxWidgets::getwxString( mp_model->m_stdtstrProgramName) ) ;
  p_aboutdialog->//Show() ;
    ShowModal();
  p_aboutdialog->Destroy() ;
}

void MainFrame::OnAttachCPUcontrollerDLL (wxCommandEvent & event)
{
  wxString wxstrExtension = wxT("") ;
  //Under Windows it returns ".dll"
  //wxstrExtension = wxDynamicLibrary::CanonicalizeName(wxstrExtension) ;
  wxstrExtension = wxDynamicLibrary::GetDllExt() ;
  //Get rid of the leading ".".
  wxstrExtension = wxstrExtension.Right( wxstrExtension.length() - 1 ) ;
  wxString wxstrCPUcontrollerDynLibFilePath = ::wxFileSelector(
    wxT("Select dynamic library for CPU info and/or control")
    , wxEmptyString //default_path
    , wxEmptyString //default_filename
    , wxstrExtension //default_extension
    , wxT("Dynamic Library files|*.") + wxstrExtension + wxT("|all files|*.*")//wildcard
    , wxFD_OPEN
    ) ;
  if ( ! wxstrCPUcontrollerDynLibFilePath.empty() )
  {
    try
    {
      //wxDynLibCPUcontroller * p_wxdynlibcpucontroller = new
      //wxDynLibCPUcontroller(
      LOGN("before creating dyn lib controller object")
      DEBUGN("address of model: " << mp_model )
      DEBUGN("address of cpucoredata: " << & mp_model->m_cpucoredata )

      //Before creating a new controller the old one should be deleted.
      gp_cpucontrolbase->PossiblyDeleteCPUcontrollerDynLib() ;

//      gp_cpucontrolbase->mp_wxdynlibcpucontroller = new wxDynLibCPUcontroller(
//        wxstrFilePath
//        , mp_wxx86infoandcontrolapp->GetCPUaccess()
//        , mp_wxx86infoandcontrolapp
//        ) ;
//      mp_wxx86infoandcontrolapp->CreateDynLibCPUcontroller(
      //TODO ANSI string may fail if Chinese localization (->use wchar_t)
      std::string stdstrCPUcontrollerDynLibFilePath = wxWidgets::GetStdString(
        wxstrCPUcontrollerDynLibFilePath ) ;
      wxGetApp().m_wxstrCPUcontrollerDynLibFilePath =
        wxstrCPUcontrollerDynLibFilePath ;

      if( mp_wxx86infoandcontrolapp->m_dynlibhandler.CreateDynLibCPUcontroller(
          stdstrCPUcontrollerDynLibFilePath )
        )
      {
        LOGN("before setting dyn lib controller as CPU controller")

        mp_wxx86infoandcontrolapp->SetCPUcontroller( //p_wxdynlibcpucontroller
  //        gp_cpucontrolbase->mp_wxdynlibcpucontroller
          mp_wxx86infoandcontrolapp->m_p_cpucontrollerDynLib
          ) ;

        LOGN("after setting dyn lib controller as CPU controller")

        CreateDynamicMenus() ;

        LOGN("after creating per CPU core menus " )

        CPUcontrollerDynLibAttached(wxstrCPUcontrollerDynLibFilePath) ;
        //(Re-)start the "update view" timer.
        m_wxtimer.Start(m_dwTimerIntervalInMilliseconds);
      }
    }
    catch( const CPUaccessException & ex )
    {
      ::wxMessageBox( wxT("Error message: ") +
        //wxString(
          //Use getwxString() to enable to compile with both unicode and ANSI.
          wxWidgets::getwxString(
            ex.m_stdstrErrorMessage)
//          )
        , wxT("DLL error")
        ) ;
    }
    LOGN(//"OnAttachCPUcontrollerDLL "
        "end" )
  }
}

void MainFrame::OnAttachCPUcoreUsageGetterDLL (wxCommandEvent & event)
{
  wxString wxstrExtension = wxT("") ;
  //Under Windows it returns ".dll"
  //wxstrExtension = wxDynamicLibrary::CanonicalizeName(wxstrExtension) ;
  wxstrExtension = wxDynamicLibrary::GetDllExt() ;
  //Get rid of the leading ".".
  wxstrExtension = wxstrExtension.Right( wxstrExtension.length() - 1 ) ;
    wxString wxstrFilePath = ::wxFileSelector( 
    wxT("Select CPU core usage getter dynamic library") 
    , wxEmptyString
    , wxEmptyString
    , wxstrExtension
    , wxT("Dynamic Library files|*.") + wxstrExtension + wxT("|all files|*.*")//wildcard
    , wxFD_OPEN
    ) ;
  if ( ! wxstrFilePath.empty() )
  {
    try
    {
      gp_cpucontrolbase->PossiblyDeleteCPUcoreUsageGetter() ;
      //wxDynLibCPUcontroller * p_wxdynlibcpucontroller = new wxDynLibCPUcontroller(
//      gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter = new
//        wxDynLibCPUcoreUsageGetter(
//        //std::string(
//        wxstrFilePath
//          //.//fn_str()
//          //c_str() )
//        , mp_wxx86infoandcontrolapp->GetCPUaccess()
//        , * mp_cpucoredata
//        ) ;
//      mp_wxx86infoandcontrolapp->CreateDynLibCPUcoreUsageGetter(
      std::string stdstr = wxWidgets::GetStdString( wxstrFilePath) ;
      if( mp_wxx86infoandcontrolapp->m_dynlibhandler.CreateDynLibCPUcoreUsageGetter(
          stdstr )
        )
      {
        mp_wxx86infoandcontrolapp->//SetCPUcoreUsageGetter( //p_wxdynlibcpucontroller
          //mp_wxdynlibcpucoreusagegetter ) ;
          mp_cpucoreusagegetter =
          //gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter ;
          mp_wxx86infoandcontrolapp->m_p_cpucoreusagegetterDynLib ;
        //CreateDynamicMenus() ;

        CPUcoreUsageGetterAttached(wxstrFilePath) ;

        //TODO necessary? (because the CPU core number got from the usage
        //getter may change )
        RedrawEverything() ;
      }
    }
    catch( CPUaccessException & ex )
    {
      wxMessageBox( wxT("Error message: ") +
        //wxString(
        //Use getwxString() to enable to compile with both unicode and ANSI.
        wxWidgets::getwxString(
        ex.m_stdstrErrorMessage)
        , wxT("DLL error")
        ) ;
    }
  }
}

void MainFrame::CPUcontrollerDynLibAttached(const wxString & wxstrFilePath )
{
//  if( ! mp_ar_voltage_and_multi )
//  mp_ar_voltage_and_multi = new VoltageAndMulti[
//    mp_model->m_cpucoredata.m_byNumberOfCPUCores ] ;
  mp_wxmenuFile->Enable( ID_DetachCPUcontrollerDynamicLibrary
    , //const bool enable
    true ) ;
  mp_wxmenuFile->SetLabel( ID_DetachCPUcontrollerDynamicLibrary ,
    wxT( //"detach"
      //"unload" is a better word because it expresses that the Dynamic
      //library is removed from the memory(?)
      "unload"
      " CPU controller ") + wxstrFilePath ) ;

  mp_wxmenuFile->Enable( ID_SetCPUcontrollerDynLibForThisCPU, true ) ;

  //If both CPU controller and the CPU usage getter exist, DVFS is possible.
  if( mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter )
    m_p_wxmenuGUI->Enable( ID_EnableOrDisableOwnDVFS
      , //const bool enable
      true ) ;
}

void MainFrame::CPUcontrollerDeleted()
{
  mp_i_cpucontroller = NULL;

  PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
  //mp_model->m_cpucoredata.ClearCPUcontrollerSpecificAtts() ;
  mp_wxmenuFile->Enable( ID_DetachCPUcontrollerDynamicLibrary
    , //const bool enable
    false ) ;
  m_p_wxmenuGUI->Enable( ID_EnableOrDisableOwnDVFS
    , //const bool enable
    false ) ;
}

void MainFrame::CPUcoreUsageGetterAttached(const wxString & wxstrFilePath)
{
  mp_wxmenuFile->Enable( ID_DetachCPUusageGetterDynLib
    , //const bool enable
    true ) ;
  mp_wxmenuFile->SetLabel( ID_DetachCPUusageGetterDynLib ,
    wxT( //"detach"
      //"unload" is a better word because it expresses that the Dynamic
      //library is removed from the memory(?)
      "unload "
      " core usage getter ") + wxstrFilePath ) ;
  if( mp_wxx86infoandcontrolapp->GetCPUcontroller() )
    m_p_wxmenuGUI->Enable( ID_EnableOrDisableOwnDVFS
      , //const bool enable
      true ) ;

  //TODO do the folowing in base class (CPUcontrolBase)
  WORD wNumberOfLogicalCPUcores = mp_wxx86infoandcontrolapp->
    mp_cpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
  if( wNumberOfLogicalCPUcores == 0 && ! mp_cpucoredata->m_byNumberOfCPUCores )
  {
    wNumberOfLogicalCPUcores = 1 ;
    LOGN("setting # CPU cores to " << wNumberOfLogicalCPUcores)
    //TODO correct CPU core number
    //Set -> allocate array for OnPaint()
    mp_cpucoredata->SetCPUcoreNumber(wNumberOfLogicalCPUcores) ;
  }
}
void MainFrame::CPUcoreUsageGetterDeleted()
{
  mp_wxmenuFile->Enable( ID_DetachCPUusageGetterDynLib
    , //const bool enable
    false ) ;
  m_p_wxmenuGUI->Enable( ID_EnableOrDisableOwnDVFS
    , //const bool enable
    false ) ;
}

void MainFrame::OnDetachCPUcontrollerDLL (wxCommandEvent & event)
{
  gp_cpucontrolbase->PossiblyDeleteCPUcontrollerDynLib() ;
}

void MainFrame::OnDetachCPUcoreUsageGetterDLL (wxCommandEvent & event)
{
  gp_cpucontrolbase->PossiblyDeleteCPUcoreUsageGetter() ;
}

void MainFrame::OnHighLoadThread( wxCommandEvent & //WXUNUSED(wxevent) 
  wxevent )
{
//  WORD wEventID = wxevent.GetId() ;
  #ifdef COMPILE_WITH_CALC_THREAD
//  HighLoadThreadAttributes p_hlta =  (HighLoadThreadAttributes * )
//    m_stdmapwxuicontroldata.find( wEventID )->second ;
  CalculationThread * p_ct = (CalculationThread *)
    m_stdmapwxuicontroldata.find( wEventID )->second ;
  if( p_ct )
    p_ct->Execute() ;
  #endif //#ifdef COMPILE_WITH_CALC_THREAD
}

void MainFrame::OnOwnDynFreqScaling( wxCommandEvent & //WXUNUSED(wxevent) 
  wxevent )
{
  mp_wxx86infoandcontrolapp->StartDynamicVoltageAndFrequencyScaling() ;
}

void MainFrame::Show1VoltnFreqSettingsDialogForEveryCPUcore(wxCommandEvent &
  wxevent )
{
  WORD wEventID = wxevent.GetId() ;
  //wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
  //  & m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //wxCPUcoreID & r_wxcpucoreid = (wxCPUcoreID &) //wxevent.m_callbackUserData ;
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //const wxObject & wxobj = //wxevent.m_callbackUserData ;
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //wxCPUcoreID & r_wxcpucoreid2 = (wxCPUcoreID &) wxobj ;
  //const wxCPUcoreID * p_wxcpucoreid2 = (const wxCPUcoreID *) & wxobj ;
  //std::map <WORD, wxObject> :: iterator iter =
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() ) ;
  //wxCPUcoreID wxcpucoreid = (wxCPUcoreID ) iter->second ;
  //  wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
  //    //&
  //    m_stdmapwxuicontroldata.find( wEventID )->second ;
  SpecificCPUcoreActionAttributes * p = (SpecificCPUcoreActionAttributes *)
    m_stdmapwxuicontroldata.find( wEventID )->second ;

  BYTE byCoreID = //p_wxcpucoreid->m_byID
    p->m_byCoreID ;
  //BYTE byCoreID = 0 ;
  if( m_arp_freqandvoltagesettingdlg[byCoreID] )
    m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
  else
  {
    //If created as local vaiable on stack the dialog would disappear
    //immediately.
    m_arp_freqandvoltagesettingdlg[byCoreID] = new
      FreqAndVoltageSettingDlg(
      this
      ,  mp_i_cpucontroller
      , byCoreID
      );
    //Allocating succeeded.
    if( m_arp_freqandvoltagesettingdlg[byCoreID] )
      m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
  }
}

void MainFrame::OnVoltageAndFrequencySettingsDialog( wxCommandEvent &
  //WXUNUSED(event)
  wxevent )
{
  LOGN(/*"OnVoltageAndFrequencySettingsDialog"*/ "begin" )
  //May be NULL at startup.
  if( mp_i_cpucontroller )
  {
#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
    Show1VoltnFreqSettingsDialogForEveryCPUcore();
#else
    FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg = new
      FreqAndVoltageSettingDlg(
      this
      ,  mp_i_cpucontroller
      , 0
      );
    //Allocating succeeded.
    if( p_freqandvoltagesettingdlg  )
    {
#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER
      m_crit_secVoltAndFreqDlgs.Enter();
      m_stdvec_p_freqandvoltagesettingdlg.push_back(
        p_freqandvoltagesettingdlg) ;
      m_crit_secVoltAndFreqDlgs.Leave();
#endif //#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER

      p_freqandvoltagesettingdlg->Show(true);
      LOGN( "after showing the volt n freq dialog")
    }
#endif
  }
}

void MainFrame::OnQuit(wxCommandEvent& //WXUNUSED(event)
                     event)
{
  Close(TRUE);
}

void MainFrame::DrawAllPossibleOperatingPoints(
  wxDC & r_wxdcDrawOn
  )
{
  //May be NULL at startup.
  if( mp_i_cpucontroller )
  {
    //float fCurrentVoltageInVolt ;
    //WORD wXcoordinate ;
    //WORD wYcoordinate ;
//    WORD wMaxFreqInMHz =
//      mp_i_cpucontroller->mp_model->m_cpucoredata.
//      m_wMaxFreqInMHz ;

//    WORD wCurrentFreqInMHz =
//      mp_i_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
//    BYTE byMinVoltageID = mp_cpucoredata->m_byMinVoltageID ;
//    BYTE byMaxVoltageID = mp_cpucoredata->m_byMaxVoltageID ;
    //BYTE byCurrentVoltageID ;
    //do 
    //{
    //  for( byCurrentVoltageID = byMinVoltageID ; 
    //    byCurrentVoltageID <= byMaxVoltageID ; ++ byCurrentVoltageID )
    //  {
    //    fCurrentVoltageInVolt = PState::GetVoltageInVolt( byCurrentVoltageID ) ;
    //    wXcoordinate =
    //      m_wXcoordOfBeginOfYaxis +
    //        (float) wCurrentFreqInMHz /
    //        (float) wMaxFreqInMHz * m_wDiagramWidth ;
    //    wYcoordinate =
    //      m_wDiagramHeight - fCurrentVoltageInVolt
    //      / m_fMaxVoltage * m_wDiagramHeight ;
    //    r_wxdcDrawOn.DrawLine( 
    //      //wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
    //      wXcoordinate
    //      , wYcoordinate
    //      , //wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
    //      wXcoordinate
    //      //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
    //      //"Note that the point (x2, y2) is not part of the line and is 
    //      //not drawn by this function (this is consistent with the 
    //      //behaviour of many other toolkits)."
    //      + 1
    //      , //(200-fVoltage*100) 
    //      wYcoordinate
    //      //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
    //      //"Note that the point (x2, y2) is not part of the line and is 
    //      //not drawn by this function (this is consistent with the 
    //      //behaviour of many other toolkits)."
    //      + 1 
    //      ) ;
    //  }
    //    mp_cpucontroller->GetNearestHigherPossibleFreqInMHz(
//    wCurrentFreqInMHz + 1 ) ;
    //} while( wCurrentFreqInMHz != mp_cpucoredata->m_wMaxFreqInMHz ) ;
  }
}

void MainFrame::DrawPerformanceStatesCrosses(
  wxDC & r_wxdc 
  , const std::set<VoltageAndFreq> & cr_stdsetmaxvoltageforfreq 
  , const wxColor * cp_wxcolor 
  )
{
  LOGN(//"DrawPerformanceStatesCrosses "
    "begin")
  const wxPen wxpenCurrent = r_wxdc.GetPen() ;
#ifdef _DEBUG
  const wxColor wxcolor = wxpenCurrent.GetColour() ;
  int nPenWidth = wxpenCurrent.GetWidth() ;
#endif
  std::set<VoltageAndFreq>::const_iterator iter = 
    cr_stdsetmaxvoltageforfreq.begin() ;
  while( iter != cr_stdsetmaxvoltageforfreq.end() )
  {
    DrawVoltageFreqCross(
      r_wxdc
      , iter->m_fVoltageInVolt
      , iter->m_wFreqInMHz
      , cp_wxcolor
      ) ;
    ++ iter ;
  }
#ifdef _DEBUG
  if( nPenWidth == 3 )
    nPenWidth = 3 ;
#endif
  //Restore the pen.
  r_wxdc.SetPen( wxpenCurrent ) ;
}

void MainFrame::DrawDiagramScale(
  wxDC & r_wxdc ,
  //std::set<MaxVoltageForFreq>::iterator & iterstdsetmaxvoltageforfreq
  std::set<VoltageAndFreq>::iterator & r_iterstdsetvoltageandfreq
  )
{
  LOGN_DEBUG(//"DrawDiagramScale "
    "mp_i_cpucontroller:" << mp_i_cpucontroller)
  LOGN(//"DrawDiagramScale "
    "m_wMaximumCPUcoreFrequency:" <<
    m_wMaximumCPUcoreFrequency)
  //May be NULL at startup.
  if( //mp_i_cpucontroller
    m_wMaximumCPUcoreFrequency )
  {
//    WORD wXcoordinate = 0, wYcoordinate = 0;
    //float fMinVoltage ;
    //float fMaxMinusMinVoltage ;
    std::set<VoltageAndFreq> & r_stdsetvoltageandfreq = 
      //mp_i_cpucontroller->
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
    //Prevents a concurrent modification e.g. by
    // "mp_model->m_cpucoredata.AddDefaultVoltageForFreq(...)"
    // in wxDynLibCPUcontroller::GetCurrentVoltageAndFrequency(...)
    wxCriticalSectionLocker wxcriticalsectionlockerVoltageAndFreq(
      mp_model->m_cpucoredata.m_wxcriticalsection) ;
//    std::set<WORD> setRightEndOfFreqString ;
    r_iterstdsetvoltageandfreq = r_stdsetvoltageandfreq.begin() ;
  #ifdef _DEBUG
    const wxPen & wxpenCurrent = r_wxdc.GetPen() ;
    const wxColor wxcolor = wxpenCurrent.GetColour() ;
//    int nPenWidth = wxpenCurrent.GetWidth() ;
  #endif

    LOGN( //"DrawDiagramScale "
      "m_wDiagramWidth:" << m_wDiagramWidth )
    //fMinVoltage = mp_i_cpucontroller->GetMinimumVoltageInVolt() ;
    //fMaxMinusMinVoltage = m_fMaxVoltage - fMinVoltage ;
//    for( ; r_iterstdsetvoltageandfreq !=
//        //mp_i_cpucontroller->mp_model->
//        //m_setmaxvoltageforfreq.end() ; //++ iterstdvecmaxvoltageforfreq
//        r_stdsetvoltageandfreq.end() ;
//        //++ iterstdsetmaxvoltageforfreq
//        ++ r_iterstdsetvoltageandfreq
//        )
//    {
////      DrawFrequency(
////        r_wxdc,
////        //wFrequencyInMHz
////        r_iterstdsetvoltageandfreq->m_wFreqInMHz ,
////        wxcoordWidth ,
////        wxcoordHeight ,
////        wLeftEndOfCurrFreqText ,
////        wxstrFreq ,
////        wXcoordinate ,
////        wYcoordinate ,
////        r_iterstdmapYcoord2RightEndOfFreqString ,
////        stdmapYcoord2RightEndOfFreqString ,
////        r_iterstdmap_ycoord2rightendoffreqstringToUse
////        ) ;
//      wXcoordinate = GetXcoordinateForFrequency( r_iterstdsetvoltageandfreq->
//        m_wFreqInMHz) ;
////      wYcoordinate = r_iterstdmap_ycoord2rightendoffreqstringToUse->first ;
//      LOGN( "DrawDiagramScale
          //x coord:" << wXcoordinate
//        << "y coord:" << wYcoordinate )
//      //mapRightEndOfFreqString2yCoord.insert(
//      //  std::pair<WORD,WORD> ( wLeftEndOfCurrFreqText + wxcoordWidth, wYcoordinate ) ) ;
//      //Draw vertical line for current frequency mark.
//      //p_wxpaintdc->DrawLine(wXcoordinate, 0, wXcoordinate, wDiagramHeight) ;
//      //wxmemorydc.DrawLine(wXcoordinate, 0, wXcoordinate, wDiagramHeight) ;
//      r_wxdc.DrawLine(wXcoordinate, 0, wXcoordinate, m_wDiagramHeight
//        //wYcoordinate
//        ) ;
////      DrawVoltage(
////        r_wxdc ,
////        ( * r_iterstdsetvoltageandfreq).m_fVoltageInVolt
////        ) ;
//      //Draw horizontal line for current voltage mark.
//      //p_wxpaintdc->DrawLine(wXcoordOfBeginOfYaxis, wYcoordinate,
//      //wxmemorydc.DrawLine(wXcoordOfBeginOfYaxis, wYcoordinate,
//      r_wxdc.DrawLine(m_wXcoordOfBeginOfYaxis, wYcoordinate,
//        m_wDiagramWidth + m_wXcoordOfBeginOfYaxis, wYcoordinate ) ;
//    }
  } //if( m_wMaximumCPUcoreFrequency )
//  else //m_wMaximumCPUcoreFrequency = 0
//  {
//  }

  WORD wMaximumYcoordinateForFrequency = 0;
  WORD wMaximumHeightForFrequencyMarks = 0;
  wxCoord wxcoordTextWidth, wxcoordTextHeight;
  r_wxdc.GetTextExtent(
    wxT("I")
    , & wxcoordTextWidth
    , & wxcoordTextHeight
    //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
    ) ;
//  int nHalfTextHeightInPixels = wxcoordTextHeight / 2;

  if( mp_i_cpucontroller->m_fReferenceClockInMHz )
  {
    //Do not draw: only calculate the max. (text) height of the frequency
    //scale.
    DrawFrequencyMarksAndLines(r_wxdc, wMaximumYcoordinateForFrequency);
    wMaximumHeightForFrequencyMarks = wMaximumYcoordinateForFrequency -
      m_wDiagramHeight
      //For the 1st line.
      + wxcoordTextHeight;
  }

  //->line for min./ lowest voltage/ 0V directly above frequency marks.
  m_wDiagramHeight += 50 - wMaximumHeightForFrequencyMarks //-
    //nHalfTextHeightInPixels
    ;

  DrawVoltageScale(r_wxdc);

//  //The frequency should start under min. voltage.
//  m_wDiagramHeight += nHalfTextHeightInPixels;

  if( mp_i_cpucontroller->m_fReferenceClockInMHz )
  {
    //Now do really draw.
    DrawFrequencyMarksAndLines(r_wxdc, wMaximumYcoordinateForFrequency);
  }

 LOGN(//"DrawDiagramScale "
   "end")
}

void MainFrame::DrawLowestStableVoltageCurve(
  wxDC & wxdc
  , float fMaxVoltage
  )
{
  //May be NULL at startup.
  if( mp_i_cpucontroller )
  {
    std::set<VoltageAndFreq>::const_iterator ciLower =
      mp_cpucoredata->m_stdsetvoltageandfreqLowestStable.begin() ;
      ////because of the "MaxVoltageForFreq::<" operator the set is sorted in
      ////DESCENDING order.
      ////Points BEYOND the last element now.
      //mp_cpucoredata->m_stdsetvoltageandfreqLowestStable.end() ;
    ////Now it should point to the last element.
    //-- ciLower ;
    if( ciLower != mp_cpucoredata->m_stdsetvoltageandfreqLowestStable.end()
      )
    {
      std::set<VoltageAndFreq>::const_iterator ciHigher =
        ciLower ;
      ciHigher ++ ;
      //ciHigher -- ;
      if( ciHigher != mp_cpucoredata->m_stdsetvoltageandfreqLowestStable.end()
        )
      {
        float fVoltage ;
        WORD wYcoordinate ;
        WORD wMaxFreqInMHz =
          //mp_i_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
          m_wMaximumCPUcoreFrequency ;
        WORD wCurrentFreqInMHz ;

        for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis
          //0
          //Begin with 1 to avoid div by zero.
          1 ;
          wCurrentXcoordinateInDiagram < //wxcoordWidth
          m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram
          )
        {
          wCurrentFreqInMHz =
            //Explicit cast to avoid (g++) warning.
            (WORD)
            (
              (float) wMaxFreqInMHz /
              ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
            ) ;
          if( ciHigher->m_wFreqInMHz < wCurrentFreqInMHz )
          {
            std::set<VoltageAndFreq>::const_iterator ciBeyondHigher = ciHigher ;
            ++ ciBeyondHigher ;
            if( ciBeyondHigher != mp_cpucoredata->
              m_stdsetvoltageandfreqLowestStable.end()
              )
            {
              ++ ciLower ;
              ++ ciHigher ;
              //-- ciLower ;
              //-- ciHigher ;
            }
          }
          //If current freq is in between.
          if( ciHigher != mp_cpucoredata->m_stdsetvoltageandfreqLowestStable.end()
            && ciLower->m_wFreqInMHz <= wCurrentFreqInMHz &&
            ciHigher->m_wFreqInMHz >= wCurrentFreqInMHz
            )
          {
            //mp_i_cpucontroller->GetInterpolatedVoltageFromFreq(
            //  wCurrentFreqInMHz
            //  , *ciHigher
            //  , *ciLower
            //  , fVoltage
            //  ) ;
            //mp_i_cpucontroller->
            GetInterpolatedVoltageFromFreq(
              wCurrentFreqInMHz
              , fVoltage
              , mp_cpucoredata->m_stdsetvoltageandfreqLowestStable
              ) ;
            wYcoordinate = GetYcoordinateForVoltage(fVoltage);

            //p_wxpaintdc->DrawLine(
            wxdc.DrawLine(
              wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
              //wDiagramHeight -
              //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
              //* wDiagramHeight ,
              wYcoordinate ,
              wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
              //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
              //"Note that the point (x2, y2) is not part of the line and is
              //not drawn by this function (this is consistent with the
              //behaviour of many other toolkits)."
              + 1
              , wYcoordinate
              //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
              //"Note that the point (x2, y2) is not part of the line and is
              //not drawn by this function (this is consistent with the
              //behaviour of many other toolkits)."
              + 1
              ) ;
          }
        }// for
      }
    }
  }
}

void MainFrame::DrawVoltageGraph(
  //wxPaintDC * p_wxpaintdc
  wxDC & wxdc
//    , WORD wDiagramWidth
//    , WORD wDiagramHeight
  , float fMaxVoltage
//    , WORD wXcoordOfBeginOfYaxis
  , const std::set<VoltageAndFreq> & c_r_stdsetvoltageandfreq
  )
{
  LOGN(//"DrawOvervoltageProtectionCurve "
    "begin" )
  //May be NULL at startup.
  if( mp_i_cpucontroller
      //and ref clock <> 0
      && mp_i_cpucontroller->m_fReferenceClockInMHz )
  {
    float fVoltage ;
    WORD wYcoordinate ;
//    std::set<float> & r_stdset_fMultipliers = mp_model->m_cpucoredata.
//        m_stdset_floatAvailableMultipliers ;
//    std::set<float>::const_reverse_iterator c_iter_stdset_fMultipliers =
//        r_stdset_fMultipliers.rbegin() ;
//    if( c_iter_stdset_fMultipliers != r_stdset_fMultipliers.rend() )
//    float fMaxMulti = mp_model->m_cpucoredata.GetMaximumMultiplier() ;
//    if( fMaxMulti )
    {
      WORD wMaxFreqInMHz = //mp_i_cpucontroller->
        //mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
        //max. freq = max. multi * ref clock
        //*c_iter_stdset_fMultipliers
        //fMaxMulti * mp_i_cpucontroller->m_fReferenceClockInMHz ;
        m_wMaximumCPUcoreFrequency ;
      for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis
          //0
          //Begin with 1 to avoid div by zero.
          1 ;
          wCurrentXcoordinateInDiagram < //wxcoordWidth
          m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram )
      {
        //mp_i_cpucontroller->
        GetInterpolatedVoltageFromFreq(
          //Explicit cast to avoid (g++) compiler warning.
          (WORD)
           (
            (float) wMaxFreqInMHz /
             ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
           )
          , fVoltage
          , c_r_stdsetvoltageandfreq ) ;
        wYcoordinate = GetYcoordinateForVoltage(fVoltage);
        //p_wxpaintdc->DrawLine(
        //TODO draw lines from last point to current point. So gaps are avoided if
        //the angle is > 45 degrees
        wxdc.DrawLine(
          wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
          //wDiagramHeight -
          //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
          //* wDiagramHeight ,
          wYcoordinate ,
          wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
          //"+ 1" because:
          //http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
          //"Note that the point (x2, y2) is not part of the line and is
          //not drawn by this function (this is consistent with the
          //behaviour of many other toolkits)."
          + 1
          , wYcoordinate
          //"+ 1" because:
          //http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
          //"Note that the point (x2, y2) is not part of the line and is
          //not drawn by this function (this is consistent with the
          //behaviour of many other toolkits)."
          + 1
          ) ;
      }
    }
  } //if( mp_i_cpucontroller )
}

//Purpose: the output for _all_ cores should be (left-)aligned.
//So the max. width for the attribute values of the same attribute type
//(MHz,...) needs to be known.
void MainFrame::StoreCurrentVoltageAndFreqInArray(
  wxDC & r_wxdc
//  VoltageAndFreq * & r_ar_voltageandfreq
  , wxString r_ar_wxstrFreqInMHz []
  , wxString r_ar_wxstrVoltageInVolt []
//  , float r_ar_fTempInCelsius []
  , wxString r_ar_wxstrTempInCelsius []
  , I_CPUcontroller * p_cpucontroller
  )
{
  LOGN(//"StoreCurrentVoltageAndFreqInArray"
    "begin")
  if( //mp_i_cpucontroller
      p_cpucontroller )
  {
    int nWidth ;
    PerCPUcoreAttributes * arp_percpucoreattributes = mp_model->m_cpucoredata.
      m_arp_percpucoreattributes ;
//    WORD wFreqInMHz = 0 ;
    wxCoord wxcoordWidth, wxcoordHeight ;
    wxString wxstr ;
    wxSize wxsize ;
    float fVoltageInVolt = 0.0f ;
    float fTempInCelsius ;
    float fThrottleLevel;
    float fMultiplier ;
    float fReferenceClockInMHz ;
    WORD wFrequencyInMHz ;
//    WORD wCoreID ;
    //mp_cpucoredata->
    //respect # of cpu cores
    for( fastestUnsignedDataType CPUcoreID = 0 ; CPUcoreID <
        mp_cpucoredata->m_byNumberOfCPUCores ; ++ CPUcoreID )
    {
//      wFreqInMHz = 0 ;
//      fVoltageInVolt = 0.0f ;
//      if( mp_i_cpucontroller->GetCurrentPstate(wFreqInMHz, fVoltageInVolt,
//          byCPUcoreID )
      if( //mp_i_cpucontroller->GetCurrentVoltageAndFrequency(
          p_cpucontroller->//GetCurrentVoltageAndFrequency(
          //Storing in the array is needed for showing the multis in the task
          //bar icon.
          GetCurrentVoltageAndFrequencyAndStoreValues(
//          fVoltageInVolt
//          , fMultiplier
//          , fReferenceClockInMHz
          //,
          CPUcoreID
          )
        )
      {
        fVoltageInVolt = arp_percpucoreattributes[CPUcoreID].m_fVoltageInVolt;
        fThrottleLevel = arp_percpucoreattributes[CPUcoreID].m_fThrottleLevel;
        fMultiplier = arp_percpucoreattributes[CPUcoreID].m_fMultiplier;
        fReferenceClockInMHz = arp_percpucoreattributes[CPUcoreID].
          m_fReferenceClockInMhz;

        wFrequencyInMHz = (WORD) ( fReferenceClockInMHz * fMultiplier ) ;
//          arp_percpucoreattributes[CPUcoreID].GetFreqInMHz();
        if( wFrequencyInMHz > m_wMaximumCPUcoreFrequency )
          m_wMaximumCPUcoreFrequency = wFrequencyInMHz ;
        LOGN("fMultiplier:" << fMultiplier )
        mp_ar_voltage_and_multi[CPUcoreID ].m_fMultiplier = fMultiplier ;
        mp_ar_voltage_and_multi[CPUcoreID ].m_fVoltageInVolt = fVoltageInVolt;
        if(fThrottleLevel == -1.0f)
          r_ar_wxstrFreqInMHz[ CPUcoreID ] = wxT("?*");
        else
          r_ar_wxstrFreqInMHz[ CPUcoreID ] = wxString::Format(
            wxT("%.3f*"), fThrottleLevel);

        {
          if( fMultiplier == 0.0f)
            r_ar_wxstrFreqInMHz[ CPUcoreID ] += wxString::Format(
              wxT("?*%.3fMHz=? MHz "),
              fThrottleLevel,
              fReferenceClockInMHz);
          else if( fReferenceClockInMHz == 0.0f )
            r_ar_wxstrFreqInMHz[ CPUcoreID ] += wxString::Format(
              wxT("%.3f*?MHz=? MHz "),
              fThrottleLevel,
              fMultiplier);
          else
          {
            if(fThrottleLevel == -1.0f)
              r_ar_wxstrFreqInMHz[ CPUcoreID ] += wxString::Format(
                //Use wxT() to enable to compile with both unicode and ANSI.
    //            wxT("%u MHz "),
    //            wFreqInMHz ) ;
                wxT("%.3f*%.3fMHz=%.3f?MHz "),
    //            wxT("%.3g*%.3gMHz=%.3gMHz "),
    //            fThrottleLevel,
                fMultiplier ,
                fReferenceClockInMHz ,
                fMultiplier * fReferenceClockInMHz
                ) ;
            else
              r_ar_wxstrFreqInMHz[ CPUcoreID ] += wxString::Format(
                //Use wxT() to enable to compile with both unicode and ANSI.
    //            wxT("%u MHz "),
    //            wFreqInMHz ) ;
                wxT("%.3f*%.3fMHz=%.3fMHz "),
    //            wxT("%.3g*%.3gMHz=%.3gMHz "),
    //            fThrottleLevel,
                fMultiplier ,
                fReferenceClockInMHz ,
                fThrottleLevel * fMultiplier * fReferenceClockInMHz
                ) ;
          }
        }
        LOGN( "frequency string:\"" << wxWidgets::GetStdString(
          r_ar_wxstrFreqInMHz[ CPUcoreID ]) << "\"")
//        LOGN("r_ar_wxstrFreqInMHz[ CPUcoreID ]:" << GetStdString(
//          r_ar_wxstrFreqInMHz[ CPUcoreID ]) )
//        wxstr = wxString::Format( wxT("%u"), wFreqInMHz ) ;
//        wxsize = r_wxdc.GetTextExtent(//wxstr
//          r_ar_wxstrFreqInMHz[ CPUcoreID ] ) ;
//        nWidth = wxsize.GetWidth() ;
        r_wxdc.GetTextExtent( r_ar_wxstrFreqInMHz[ CPUcoreID ] ,
            & wxcoordWidth, & wxcoordHeight ) ;
        nWidth = wxcoordWidth ;
        //The max freq text widthcan not easyily be determined at startup:
        //if e.g. the documented max. multiplier is "10" and the reference
        //clock is 100MHz, then the freq can also get > 1000 MHz because the
        //reference clock can be a bittle higher frequented or increased via
        // BIOS etc.
        //So it's best to determine it at runtime.
        if( nWidth > m_wMaxFreqInMHzTextWidth )
          m_wMaxFreqInMHzTextWidth = nWidth ;

        if( fVoltageInVolt == 0.0 )
          r_ar_wxstrVoltageInVolt[ CPUcoreID ] = wxT("?V ") ;
        else
          r_ar_wxstrVoltageInVolt[ CPUcoreID ] = wxString::Format(
            //Use wxT() to enable to compile with both unicode and ANSI.
            wxT("%.4f"
              //"%.3g"
              "V "), fVoltageInVolt ) ;
        wxsize = r_wxdc.GetTextExtent(//wxstr
          r_ar_wxstrVoltageInVolt[ CPUcoreID ] ) ;
        nWidth = wxsize.GetWidth() ;
        if( nWidth > m_wMaxVoltageInVoltTextWidth )
          m_wMaxVoltageInVoltTextWidth = nWidth ;
//        r_ar_voltageandfreq[byCPUcoreID].m_fVoltageInVolt = fVoltageInVolt ;
//        r_ar_voltageandfreq[byCPUcoreID].m_wFreqInMHz = wFreqInMHz ;
      }
      else
      {
        LOGN_ERROR("error getting the current CPU core voltage and frequency "
          "CPU controller via the CPU controller->not using their results" )
      }
      fTempInCelsius = //mp_i_cpucontroller->
        p_cpucontroller->
        GetTemperatureInCelsius(CPUcoreID) ;
      s_arfTemperatureInDegreesCelsius[ CPUcoreID ] = fTempInCelsius ;
//      switch(fTempInCelsius)
//      {
//        case __FLT_MIN__ :
//          r_ar_wxstrTempInCelsius[ CPUcoreID ] = wxT("?°C ");
//          break;
//        case CPU_TEMP_IS_CRITICAL:
//          r_ar_wxstrTempInCelsius[ CPUcoreID ] = wxT("<C ");
//          break;
//        case CPU_TEMP_IS_BELOW_CRITICAL:
//          r_ar_wxstrTempInCelsius[ CPUcoreID ] = wxT(">C ");
//          break;
//        default:
//        //http://www.cplusplus.com/reference/clibrary/cstdio/printf/:
//        //"Use the shorter of %e or %f"
//        //-> if "%.3g" (max 3 digits after decimal point):
//        //    for "60.0" it is "60"
//        //   for  "60.1755" it is "60.175"
//        r_ar_wxstrTempInCelsius[ CPUcoreID ] = wxString::Format(
//          //Use wxT() to enable to compile with both unicode and ANSI.
//          wxT("%.3g°C "),
//          fTempInCelsius ) ;
//      }
      wxGetApp().GetTemperatureString(fTempInCelsius,
        r_ar_wxstrTempInCelsius[ CPUcoreID ]);
      r_ar_wxstrTempInCelsius[ CPUcoreID ] += wxT(" ");
//      LOGN("r_ar_wxstrTempInCelsius[ CPUcoreID ]:" << GetStdString(
//        r_ar_wxstrTempInCelsius[ CPUcoreID ]) )
      wxsize = r_wxdc.GetTextExtent(//wxstr
        r_ar_wxstrTempInCelsius[ CPUcoreID ] ) ;
      nWidth = wxsize.GetWidth() ;
      if( nWidth > m_wMaxTemperatureTextWidth )
        m_wMaxTemperatureTextWidth = nWidth ;
    }
  }
  LOGN( "end")
}

void MainFrame::DrawCPUcoreIDs(
  wxDC & r_wxdc,
  wxCoord & wxcoordX, //must be call by reference
  wxCoord wxcoordTextHeight
  )
{
  LOGN("before drawing the CPU core numbers")
  static wxString wxstr;
  int nWidth ;
  wxSize wxsize ;
  WORD wMaxCoreNumberTextWidth = 0 ;
  for ( WORD wCoreID = 0 ; wCoreID < mp_cpucoredata->m_byNumberOfCPUCores ;
    ++ wCoreID )
  {
    wxstr =  wxString::Format(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      wxT("core%u:")
      , wCoreID
      ) ;
    wxsize = r_wxdc.GetTextExtent( wxstr ) ;
    nWidth = wxsize.GetWidth() ;
    if( nWidth > wMaxCoreNumberTextWidth )
      wMaxCoreNumberTextWidth = nWidth ;
    r_wxdc.DrawText(
      wxstr
      , wxcoordX //x-coord
      , wCoreID * //m_wTextHeight //y-coord
      wxcoordTextHeight
      ) ;
  }
  wxcoordX += wMaxCoreNumberTextWidth ;
}

void MainFrame::DrawCurrentCPUcoreFrequency(
  wxDC & r_wxdc,
  const wxString ar_wxstrFreqInMHz [],
  wxCoord & wxcoordX,
  wxCoord wxcoordTextHeight
  )
{
  LOGN("before drawing the CPU core frequency")
  for ( fastestUnsignedDataType wCoreID = 0 ; wCoreID <
    mp_cpucoredata->m_byNumberOfCPUCores ; ++ wCoreID )
  {
#ifdef _DEBUG
    const wxString & r_wxstr = ar_wxstrFreqInMHz[ wCoreID ] ;
    //Avoid g++ warning "unused variable ‘r_wxstr’"
    SUPPRESS_UNUSED_VARIABLE_WARNING(r_wxstr)
#endif
    r_wxdc.DrawText(
      ar_wxstrFreqInMHz[ wCoreID ]
      , wxcoordX //x-coord
      , wCoreID * //m_wTextHeight //y-coord
        wxcoordTextHeight
      ) ;
  }
  wxcoordX += m_wMaxFreqInMHzTextWidth ;
}

void MainFrame::DrawCurrentCPUcoreTemperature(
  wxDC & r_wxdc,
  const wxString ar_wxstrCPUcoreTemperature [],
  wxCoord & wxcoordX,
  wxCoord wxcoordTextHeight
  )
{
  for ( WORD wCoreID = 0 ; wCoreID < mp_cpucoredata->m_byNumberOfCPUCores ;
    ++ wCoreID )
  {
    r_wxdc.DrawText(
      ar_wxstrCPUcoreTemperature[ wCoreID ]
      , wxcoordX //x-coord
      , wCoreID * //m_wTextHeight //y-coord
        wxcoordTextHeight
      ) ;
  }
  wxcoordX += m_wMaxTemperatureTextWidth ;
}

void MainFrame::DrawCPUcoreVoltage(
  wxDC & r_wxdc,
  const wxString ar_wxstrCPUcoreVoltageInVolt [],
  wxCoord & wxcoordX,
  wxCoord wxcoordTextHeight
  )
{
  LOGN("before drawing the CPU core voltage")
  for ( WORD wCoreID = 0 ; wCoreID < mp_cpucoredata->m_byNumberOfCPUCores ;
    ++ wCoreID )
  {
    r_wxdc.DrawText(
      ar_wxstrCPUcoreVoltageInVolt[ wCoreID ]
      , wxcoordX //x-coord
      , wCoreID * //m_wTextHeight //y-coord
      wxcoordTextHeight
      ) ;
  }
  wxcoordX += m_wMaxVoltageInVoltTextWidth ;
}

void MainFrame::DrawCPUcoreUsages(
  wxDC & r_wxdc,
  const ICPUcoreUsageGetter * p_cpucoreusagegetter,
  wxCoord wxcoordX,
  wxCoord wxcoordTextHeight
  )
{
 //mp_i_cpucontroller->
 if( //mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
     p_cpucoreusagegetter )
 {
   float fCPUload ;
   wxSize wxsize ;
   fCPUload = -1.0 ;
   static wxString wxstr;
   for ( WORD wCoreID = 0 ; wCoreID < mp_cpucoredata->m_byNumberOfCPUCores ;
     ++ wCoreID )
   {
     fCPUload = mp_cpucoredata->m_arfCPUcoreLoadInPercent[ wCoreID ] ;
     wxstr = wxString::Format(
//#ifdef _WINDOWS
#ifdef _MSC_VER //If MicroSoft compiler.
        _T("%.3f percent usage")
#else
        //when compiled with MSVC and running under WinXP the executable
        //crashes with this format string (surely because of the 1st "%")
        //http://www.cplusplus.com/reference/clibrary/cstdio/printf/:
        //"  A % followed by another % character will write % to stdout."
        wxT("%.3f%% usage")
#endif
      , fCPUload * 100.0f
      ) ;
     r_wxdc.DrawText(
       wxstr
       , wxcoordX //x-coord
       , wCoreID * //m_wTextHeight //y-coord
         wxcoordTextHeight
       ) ;
   }
 }
}

/**
 * Draws current CPU core data onto a wxDC:
 *  -core ID
 *  -frequency
 *  -voltage
 *  -temperature
 *  -usage
 */
void MainFrame::DrawCurrentCPUcoreInfo(
  wxDC & r_wxdc
  )
{
  LOGN(//"DrawCurrentCPUcoreData"
    "begin")
//  wxString wxstrCPUcoreUsage ;
  wxString wxstrCPUcoreVoltage ;
  wxString wxstrTemperature ;
  wxString wxstrFreqInMHz ;
  ICPUcoreUsageGetter * p_cpucoreusagegetter ;
  I_CPUcontroller * p_cpucontroller ;

  if( GetCPUcoreInfoDirectlyOrFromService(
      p_cpucoreusagegetter,
      p_cpucontroller //,
//      wxstrCPUcoreUsage
      , true
      )
    )
  {
//    DEBUGN("DrawCurrentCPUcoreInfo--Number of CPU cores:" <<
//        (WORD) mp_cpucoredata->m_byNumberOfCPUCores  )
    //TODO do not create arrays every time this func is called
    wxString ar_wxstrCPUcoreVoltage [ mp_cpucoredata->m_byNumberOfCPUCores] ;
    wxString ar_wxstrCPUcoreTemperature [ mp_cpucoredata->m_byNumberOfCPUCores] ;
    wxString ar_wxstrCPUcoreFreqInMHz [ mp_cpucoredata->m_byNumberOfCPUCores] ;

    const wxFont & wxfont = r_wxdc.GetFont();

//     int nFontPointSize = wxfont.GetPointSize();
    if( mp_model->m_userinterfaceattributes.mainframe.
        m_nCurrentCPUcoreInfoSizeInPoint)
    {
      //TODO make font a member var and move font creation to the c'to
      wxFont wxfont2(wxfont);
      wxfont2.SetPointSize(mp_model->m_userinterfaceattributes.mainframe.
        m_nCurrentCPUcoreInfoSizeInPoint);
      r_wxdc.SetFont(wxfont2);
    }
    //if( m_wxfontCPUcoreInfo.GetPointSize() )

    if( //mp_i_cpucontroller
        p_cpucontroller )
    {
//      VoltageAndFreq ar_voltageandfreq[mp_cpucoredata->m_byNumberOfCPUCores] ;
//      float ar_fTempInDegCelsius [ mp_cpucoredata->m_byNumberOfCPUCores] ;
      LOGN(//"DrawCurrentCPUcoreData "
        "before StoreCurrentVoltageAndFreqInArray" )
      StoreCurrentVoltageAndFreqInArray(
//        ar_voltageandfreq,
//        ar_fTempInDegCelsius
        r_wxdc
        , ar_wxstrCPUcoreFreqInMHz
        , ar_wxstrCPUcoreVoltage
        , ar_wxstrCPUcoreTemperature
        , p_cpucontroller
        ) ;
//      float fHighestTemperature = GetHighestTemperature(
//        s_arfTemperatureInDegreesCelsius) ;
#ifdef _WIN32
      //TODO use correct icon size for (both Windows and) Linux.
      ShowHighestCPUcoreTemperatureInTaskBar(p_cpucontroller) ;
      ShowCPUcoreUsagesInTaskBar(p_cpucontroller);
      ShowCPUcoresMultipliersInTaskBar(p_cpucontroller);
#endif
      LOGN(//"DrawCurrentCPUcoreData "
        "after StoreCurrentVoltageAndFreqInArray" )
    }
    else
    {
      for ( WORD wCoreID = 0 ; wCoreID < mp_cpucoredata->m_byNumberOfCPUCores ;
        ++ wCoreID )
      {
        ar_wxstrCPUcoreVoltage [ wCoreID ] = wxT("?");
        ar_wxstrCPUcoreTemperature [ wCoreID ] = wxT("?");
        ar_wxstrCPUcoreFreqInMHz [ wCoreID ] = wxT("?");
      }
    }
    LOGN(//"DrawCurrentCPUcoreData "
      "leaving IPC 2 in-program data crit sec")
    mp_cpucoredata->wxconditionIPC2InProgramData.Leave() ;
    LOGN(//"DrawCurrentCPUcoreData "
      "after leaving IPC 2 in-program data crit sec")

//#ifdef _DEBUG
//     if ( fCPUload == 0.0 )
//     {
////         //Breakpoint possibility
////         int i = 0 ;
//     }
//#endif
      //wxmemorydc
     wxString wxstr ;
     wxCoord wxcoordX ;
     wxcoordX = //45 ;
       m_uiRightmostEndOfVoltageString;

     wxCoord wxcoordTextWidth, wxcoordTextHeight;
     r_wxdc.GetTextExtent(
       wxT("I")
       , & wxcoordTextWidth
       , & wxcoordTextHeight
       //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
       ) ;
     DrawCPUcoreIDs(r_wxdc, wxcoordX, wxcoordTextHeight);
     if( //mp_i_cpucontroller
         p_cpucontroller )
     {
       DrawCPUcoreVoltage(r_wxdc, ar_wxstrCPUcoreVoltage, wxcoordX,
         wxcoordTextHeight);
       DrawCurrentCPUcoreFrequency(r_wxdc, ar_wxstrCPUcoreFreqInMHz, wxcoordX,
         wxcoordTextHeight);
       DrawCurrentCPUcoreTemperature(r_wxdc, ar_wxstrCPUcoreTemperature,
         wxcoordX, wxcoordTextHeight);
     }
     DrawCPUcoreUsages(r_wxdc, p_cpucoreusagegetter, wxcoordX,
       wxcoordTextHeight);
      //   } //for-loop
    if( mp_model->m_userinterfaceattributes.mainframe.m_nCurrentCPUcoreInfoSizeInPoint)
    {
//      wxfont.SetPointSize(nFontPointSize);
      r_wxdc.SetFont(wxfont);
    }
  }
  else
  {
    LOGN(//"DrawCurrentCPUcoreData "
      "before leaving IPC 2 in-program data crit sec")
    mp_cpucoredata->wxconditionIPC2InProgramData.Leave() ;
    LOGN(//"DrawCurrentCPUcoreData "
      "After leaving IPC 2 in-program data crit sec")
  }
  LOGN(//"DrawCurrentCPUcoreData "
    "end")
}

void MainFrame::DrawCurrentVoltageSettingsCurve(
    wxDC & wxdc
    , float fMaxVoltage
    )
{
  //May be NULL at startup.
  if( mp_i_cpucontroller )
  {
    float fVoltage ;
    WORD wYcoordinate ;
    WORD wMaxFreqInMHz = //mp_i_cpucontroller->
      //mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
      m_wMaximumCPUcoreFrequency ;
    if( //If max. freq is assigned
      wMaxFreqInMHz != 0 )
      for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis
          //0
          //Begin with 1 to avoid div by zero.
          1 ;
          wCurrentXcoordinateInDiagram < //wxcoordWidth
          m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram )
      {
        //mp_i_cpucontroller->GetMinFreqToPreventOvervoltage( iter ) ;
        if( //mp_i_cpucontroller->
            GetInterpolatedVoltageFromFreq(
            //Explicit cast to avoid (g++) compiler warning.
            (WORD)
             (
              (float) wMaxFreqInMHz /
               ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
             )
            , fVoltage
            , mp_cpucoredata->m_stdsetvoltageandfreqWanted )
          )
        {
          wYcoordinate = GetYcoordinateForVoltage(fVoltage);

          //p_wxpaintdc->DrawLine(
          wxdc.DrawLine(
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
            //wDiagramHeight -
            //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
            //* wDiagramHeight ,
            wYcoordinate ,
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html
            // #wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is
            //not drawn by this function (this is consistent with the
            //behaviour of many other toolkits)."
            + 1
            , wYcoordinate
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html
            // #wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is
            //not drawn by this function (this is consistent with the
            //behaviour of many other toolkits)."
            + 1
            ) ;
        }
      }//for-loop
  } // if( mp_i_cpucontroller )
}

void MainFrame::DrawFrequency(
  wxDC & r_wxdc ,
  WORD wFrequencyInMHz ,
//  WORD wMaxFreqInMHz ,
  wxCoord wxcoordWidth ,
  wxCoord wxcoordHeight ,
  WORD wLeftEndOfCurrFreqText ,
  wxString wxstrFreq ,
  WORD & wXcoordinate ,
  WORD & wYcoordinate ,
//  stdmapYcoord2RightEndOfFreqString
//  std::set<WORD> & setRightEndOfFreqString
//  std::map<WORD,WORD> & mapRightEndOfFreqString2yCoord ,
//  std::map<WORD,WORD>::const_reverse_iterator
//    r_iterstdmapRightEndOfFreqString2yCoord ,
  std::map<WORD,WORD>::iterator & r_iterstdmapYcoord2RightEndOfFreqString ,
  std::map<WORD,WORD> & stdmapYcoord2RightEndOfFreqString ,
  std::map<WORD,WORD>::iterator & r_iterstdmap_ycoord2rightendoffreqstringToUse
  , bool bCalculateMaxTextHeight
  )
{
  wXcoordinate = GetXcoordinateForFrequency(wFrequencyInMHz) ;
  //Draw frequency mark.
  //p_wxpaintdc->DrawText(
  //wxmemorydc.DrawText(

  wxstrFreq = wxString::Format(
    //We need a _T() macro (wide char-> L"", char->"") for EACH
    //line to make it compatible between char and wide char.
    wxT("%u") ,
//      (*r_iterstdsetvoltageandfreq).m_wFreqInMHz
    wFrequencyInMHz
    ) ;
  r_wxdc.GetTextExtent(
    wxstrFreq
    , & wxcoordWidth
    , & wxcoordHeight
    //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
    ) ;
  wLeftEndOfCurrFreqText = wXcoordinate
    //Position the line at the horizontal middle of the text.
    - ( wxcoordWidth / 2 ) ;
  //If the iterator is not set::end() then there is at least 1 strings that would
  //overlap with the current string if all were drawn with the same y coordinate.
  //iterstdsetword = setRightEndOfFreqString.
  //  //Finds the first element whose key greater than k.
  //  upper_bound(wLeftEndOfCurrFreqText) ;
  //r_iterstdmapRightEndOfFreqString2yCoord =
  //  mapRightEndOfFreqString2yCoord.rbegin() ;
  //while( r_iterstdmapRightEndOfFreqString2yCoord !=
  //  mapRightEndOfFreqString2yCoord.rend()
  //  )
  //{
  //  //If space between the right end is of a prev string and the left end
  //  //of this string.
  //  if( r_iterstdmapRightEndOfFreqString2yCoord->first <
  //    wLeftEndOfCurrFreqText )
  //    break ;
  //  ++ r_iterstdmapRightEndOfFreqString2yCoord ;
  //}

  ////Avoid overlapping of frequency strings.
  ////if( wLeftEndOfCurrFreqText < wRightEndOfPrevFreqText )
  ////  wYcoordinate = m_wDiagramHeight + wxcoordHeight ;
  //if( r_iterstdmapRightEndOfFreqString2yCoord !=
  //  mapRightEndOfFreqString2yCoord.rend()
  //  )
  //  wYcoordinate = r_iterstdmapRightEndOfFreqString2yCoord->second ;
  //else
  //  if( mapRightEndOfFreqString2yCoord.empty() )
  //    wYcoordinate = m_wDiagramHeight ;
  //  else // no right text end that is left from the current text to draw.
  //  {
  //    //wYcoordinate = m_wDiagramHeight + wxcoordHeight ;
  //    //e.g.:  500
  //    //         600
  //    //           650
  //    //Draw 1 line below the previous freq text.
  //    wYcoordinate = mapRightEndOfFreqString2yCoord.rbegin()->second +
  //      wxcoordHeight ;
  //  }

  r_iterstdmapYcoord2RightEndOfFreqString =
    stdmapYcoord2RightEndOfFreqString.begin() ;
  r_iterstdmap_ycoord2rightendoffreqstringToUse =
    stdmapYcoord2RightEndOfFreqString.end() ;
  while( r_iterstdmapYcoord2RightEndOfFreqString !=
    stdmapYcoord2RightEndOfFreqString.end()
    )
  {
    //If space between the right end is of a prev string and the left end
    //of this string.
    //The first entry is the topmost. This is also the entry
    if( r_iterstdmapYcoord2RightEndOfFreqString->second <
      wLeftEndOfCurrFreqText )
    {
      r_iterstdmap_ycoord2rightendoffreqstringToUse =
        r_iterstdmapYcoord2RightEndOfFreqString ;
      break ;
    }
    ++ r_iterstdmapYcoord2RightEndOfFreqString ;
  }
  if( r_iterstdmap_ycoord2rightendoffreqstringToUse !=
    stdmapYcoord2RightEndOfFreqString.end()
    )
  {
    //Update the new right end of string to draw.
    r_iterstdmap_ycoord2rightendoffreqstringToUse->second =
      wLeftEndOfCurrFreqText + wxcoordWidth ;
    wYcoordinate = r_iterstdmap_ycoord2rightendoffreqstringToUse->first ;
  }
  else
  {
    if( stdmapYcoord2RightEndOfFreqString.empty() )
      wYcoordinate = m_wDiagramHeight ;
    else // no right text end that is left from the current text to draw.
    {
      //wYcoordinate = m_wDiagramHeight + wxcoordHeight ;
      //e.g.:  500
      //         600
      //           650
      //Draw 1 line below the previous freq text.
      wYcoordinate = stdmapYcoord2RightEndOfFreqString.rbegin()->first +
        wxcoordHeight ;
    }
    stdmapYcoord2RightEndOfFreqString.insert(
      std::pair<WORD,WORD> ( wYcoordinate , wLeftEndOfCurrFreqText + wxcoordWidth ) ) ;
  }
  LOGN(//"DrawDiagramScale "
    "drawing " << wxWidgets::GetStdString( wxstrFreq )
    << " at " << wLeftEndOfCurrFreqText << ","
    << wYcoordinate )
  if( ! bCalculateMaxTextHeight)
    r_wxdc.DrawText(
      wxstrFreq
      ,
      //wXcoordOfBeginOfYaxis +
      //(float) wMaxFreqInMHz * wDiagramWidth
      wLeftEndOfCurrFreqText
      , //m_wDiagramHeight
      wYcoordinate
      ) ;
}

void MainFrame::DrawFrequencyMarksAndLines(wxDC & r_wxdc,
    //0=calculate max height
    WORD & wMaximumYcoordinateForFrequency)
{
  LOGN_DEBUG( "begin")
  WORD wInitialMaximumYcoordinateForFrequency = wMaximumYcoordinateForFrequency;
  wMaximumYcoordinateForFrequency = 0;
  //Initialize to avoid g++ warnings like
  //"'wLeftEndOfCurrFreqText' might be used uninitialized in this function"
  wxCoord wxcoordWidth = 0 ;
  wxCoord wxcoordHeight = 0 ;
  wxString wxstrFreq ;
  fastestUnsignedDataType wLeftEndOfCurrFreqText = 0 ;
  WORD wXcoordinate ;
  //Initialize to avoid g++ warning
  //"'wYcoordinate' may be used uninitialized in this function"
  WORD wYcoordinate = 0;
  std::map<WORD,WORD>::iterator
    r_iterstdmapYcoord2RightEndOfFreqString ;
  std::map<WORD,WORD> stdmapYcoord2RightEndOfFreqString ;
  std::map<WORD,WORD>::iterator
    r_iterstdmap_ycoord2rightendoffreqstringToUse ;

  float fReferenceClockInMHz = mp_i_cpucontroller->m_fReferenceClockInMHz ;
  WORD wFrequencyInMHz ;
  wxPen penLine( * wxLIGHT_GREY, 1); // pen of width 1
  const wxPen & c_r_penCurrent = r_wxdc.GetPen();
  
  const wxFont & wxfont = r_wxdc.GetFont();
//  int nFontPointSize = wxfont.GetPointSize();
  if( mp_model->m_userinterfaceattributes.mainframe.
      m_nCPUcoreFrequencyScaleSizeInPoint)
  { //TODO possibly create font in c'tor and not for every call to this function
    wxFont wxfont2(wxfont);
    wxfont2.SetPointSize(mp_model->m_userinterfaceattributes.mainframe.
      m_nCPUcoreFrequencyScaleSizeInPoint);
    r_wxdc.SetFont(wxfont2);
  }
//  wxCoord wxcoordTextWidth, wxcoordTextHeight;
//  r_wxdc.GetTextExtent(
//    wxT("I")
//    , & wxcoordTextWidth
//    , & wxcoordTextHeight
//    //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
//    ) ;
  const fastestSignedDataType numMultipliers = mp_cpucoredata->
    m_stdset_floatAvailableMultipliers.size();
  for( fastestUnsignedDataType multiplierArrayIndex = 0 ;
      multiplierArrayIndex < numMultipliers ; ++ multiplierArrayIndex )
  {
    wFrequencyInMHz =
      //Avoid g++ warning "converting to `WORD' from `float'" .
      (WORD)
      ( mp_cpucoredata->m_arfAvailableMultipliers[
        multiplierArrayIndex ] * fReferenceClockInMHz ) ;
    LOGN("should draw frequency "
      << wFrequencyInMHz
      << " for frequency scale")
    DrawFrequency(
      r_wxdc,
      wFrequencyInMHz ,
      wxcoordWidth ,
      wxcoordHeight ,
      wLeftEndOfCurrFreqText ,
      wxstrFreq ,
      wXcoordinate ,
      wYcoordinate ,
      r_iterstdmapYcoord2RightEndOfFreqString ,
      stdmapYcoord2RightEndOfFreqString ,
      r_iterstdmap_ycoord2rightendoffreqstringToUse,
      ! wInitialMaximumYcoordinateForFrequency
      ) ;
    if( wYcoordinate >
      wMaximumYcoordinateForFrequency )
      wMaximumYcoordinateForFrequency = wYcoordinate;
    if( wInitialMaximumYcoordinateForFrequency > 0)
    {
      r_wxdc.SetPen(penLine);
      r_wxdc.DrawLine( wXcoordinate, 0, wXcoordinate, wYcoordinate);
      r_wxdc.SetPen(c_r_penCurrent);
    }
  }
  if( mp_model->m_userinterfaceattributes.mainframe.
      m_nCPUcoreFrequencyScaleSizeInPoint)
  {
//    wxfont.SetPointSize(nFontPointSize);
    r_wxdc.SetFont(wxfont);
  }
}

void MainFrame::DrawVoltage(wxDC & r_wxdc , float fVoltageInVolt)
{
//  WORD wYcoordinate ;
  if( m_bRangeBeginningFromMinVoltage )
    m_wYcoordinate =
      //Explicit cast to avoid (g++) warning.
      (WORD)
      (
        m_wDiagramHeight -
        ( fVoltageInVolt - m_fMinVoltage )
        / m_fMaxMinusMinVoltage * m_wDiagramHeight
      ) ;
  else
    m_wYcoordinate =
      //Explicit cast to avoid (g++) warning.
      (WORD)
      (
        m_wDiagramHeight -
        //(*iterstdsetmaxvoltageforfreq).m_fVoltageInVolt
        fVoltageInVolt
        / m_fMaxVoltage * m_wDiagramHeight
        + m_wMinYcoordInDiagram
      ) ;
  //Draw voltage mark.
  //p_wxpaintdc->DrawText(
  //wxmemorydc.DrawText(
  r_wxdc.DrawText(
    wxString::Format(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      _T("%.3f") ,
        fVoltageInVolt
        ),
    5
    ,
    //wDiagramHeight -
    /// fMaxVoltage * wDiagramHeight
    m_wYcoordinate
    ) ;
}

unsigned MainFrame::GetYcoordinateForVoltage(float fVoltageInVolt//,
  // unsigned uiHalfTextHeightInPixels
  )
{
  WORD wYcoordinate ;
  if( m_bRangeBeginningFromMinVoltage )
    wYcoordinate =
      //Explicit cast to avoid (g++) warning.
      (WORD)
      (
        m_wDiagramHeight -
        ( fVoltageInVolt - m_fMinVoltage )
        / m_fMaxMinusMinVoltage * (m_wDiagramHeight
        //The highest voltage should start at the half pixel height of the
        //voltage string.
        - m_wMinYcoordInDiagram)
//        + //uiHalfTextHeightInPixels
//        m_wMinYcoordInDiagram
      ) ;
  else
    wYcoordinate =
      //Explicit cast to avoid (g++) warning.
      (WORD)
      (
        m_wDiagramHeight -
        //(*iterstdsetmaxvoltageforfreq).m_fVoltageInVolt
        fVoltageInVolt
        / m_fMaxVoltage * (m_wDiagramHeight
        //The highest voltage should start at the half pixel height of the
        //voltage string.
        - m_wMinYcoordInDiagram)
//        + m_wMinYcoordInDiagram
      ) ;
  return wYcoordinate;
}

wxCoord MainFrame::DrawVoltage(
  wxDC & r_wxdc ,
  float fVoltageInVolt,
  WORD wYcoordinate,
  unsigned uiXcoordinateInPixels
  )
{
  static wxCoord wxcoordTextWidth, wxcoordTextHeight;
  wxString wxstrVoltage = wxString::Format(
    //We need a _T() macro (wide char-> L"", char->"") for EACH
    //line to make it compatible between char and wide char.
    wxT("%.3f") ,
    fVoltageInVolt
    );
  unsigned uiBeginOfVoltageString = 3 + uiXcoordinateInPixels;

  //Draw voltage mark.
  //p_wxpaintdc->DrawText(
  //wxmemorydc.DrawText(
  r_wxdc.DrawText(
    wxstrVoltage ,
    uiBeginOfVoltageString
    ,
    //wDiagramHeight -
    /// fMaxVoltage * wDiagramHeight
    wYcoordinate
    ) ;
  r_wxdc.GetTextExtent(
      wxstrVoltage
    , & wxcoordTextWidth
    , & wxcoordTextHeight
    //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
    ) ;
  return wxcoordTextWidth + uiBeginOfVoltageString;
}

void MainFrame::DrawVoltageFreqCross(
  wxDC & r_wxdc
  , float fVoltageInVolt
  , WORD wFreqInMHz
  , const wxColor * cp_wxcolor 
  )
{
  LOGN("begin")
  WORD wXcoordinate = 
    //Explicit cast to avoid (g++) warning.
    (WORD)
    (
      m_wXcoordOfBeginOfYaxis +
      (float) //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
      //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
      wFreqInMHz /
      (float) //mp_cpucoredata->m_wMaxFreqInMHz
        m_wMaximumCPUcoreFrequency * m_wDiagramWidth
    ) ;
  WORD wYcoordinate = GetYcoordinateForVoltage(fVoltageInVolt);

  //wxPen pen(*wxBLUE, 3); // pen of width 3
  wxPen pen(*cp_wxcolor, 3); // pen of width 3
  r_wxdc.SetPen(pen);
  //Draw Cursor:
  r_wxdc.DrawLine(
    wXcoordinate - 3, wYcoordinate ,
    wXcoordinate + 4, wYcoordinate
    ) ;
  r_wxdc.DrawLine(
    wXcoordinate , wYcoordinate - 3,
    wXcoordinate , wYcoordinate + 4
    ) ;
}

void MainFrame::DrawVoltageScale(wxDC & r_wxdc )
{
  LOGN_DEBUG( "begin")
  WORD wPreviousYcoordinateForVoltage = MAXWORD ;

  const wxPen & c_r_penCurrent = r_wxdc.GetPen();
  //TODO possibly create pen in c'tor and not for every call to this function
  wxPen penLine( * wxLIGHT_GREY, 1); // pen of width 1
  r_wxdc.SetPen(penLine);

  const wxFont & cr_wxfontBefore = r_wxdc.GetFont();
  //wxWidgets runtime error when calling GetPointSize.
//  int nFontPointSize = cr_wxfontBefore.GetPointSize();
  if( mp_model->m_userinterfaceattributes.mainframe.m_nVoltageScaleSizeInPoint)
  {
//    cr_wxfontBefore.SetPointSize(mp_model->m_userinterfaceattributes.
//      m_nVoltageScaleSizeInPoint);
    //TODO possibly create font in c'tor and not for every call to this function
    //wxFont copy c'tor uses reference counting.
    wxFont wxfont(cr_wxfontBefore//.GetNativeFontInfoDesc()
      );
    wxfont.SetPointSize(mp_model->m_userinterfaceattributes.mainframe.
      m_nVoltageScaleSizeInPoint);
    r_wxdc.SetFont(//cr_wxfontBefore
      wxfont);
  }
  wxCoord wxcoordTextWidth, wxcoordTextHeight;
  wxCoord wxcoordVoltageStringWidth = 0;
  r_wxdc.GetTextExtent(
    wxT("I")
    , & wxcoordTextWidth
    , & wxcoordTextHeight
    //, wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL
    ) ;
  int nHalfTextHeightInPixels = wxcoordTextHeight/2;
  m_wMinYcoordInDiagram = nHalfTextHeightInPixels;

  unsigned uiBeginOfVoltageString = 0;
  m_uiRightmostEndOfVoltageString = 0;
  for( WORD wVoltageIndex = 0 ; wVoltageIndex < mp_cpucoredata->
    m_stdset_floatAvailableVoltagesInVolt.size() ; ++ wVoltageIndex )
  {
    LOGN("should draw voltage "
      << mp_cpucoredata->m_arfAvailableVoltagesInVolt[ wVoltageIndex ]
      << " for voltage scale")
    m_wYcoordinate = GetYcoordinateForVoltage(
      mp_cpucoredata->m_arfAvailableVoltagesInVolt[
      wVoltageIndex ]//, nHalfTextHeightInPixels
      );
//    wYoordinate = m_wYoordinate ;


    if( //If y coord of current voltage <= y coord of prev voltage - text h
        //=if the voltage string _does not overlap_ with the previous voltage
        //string.
      m_wYcoordinate <= wPreviousYcoordinateForVoltage - //m_wTextHeight
      wxcoordTextHeight)
    {
      //uiBeginOfVoltageString = wxcoordVoltageStringWidth;
//      //Next time start at x pos 0.
//      wxcoordVoltageStringWidth = 0;
      //Draw at x pos 0.
      wxcoordVoltageStringWidth = 0;
    }
    else //Strings would overlap-> place right of previous voltage string.
    {
//      wxcoordVoltageStringWidth = 0;
//      uiBeginOfVoltageString = 0;
//      uiBeginOfVoltageString
//      wxcoordVoltageStringWidth = DrawVoltage(
//        r_wxdc,
//        mp_cpucoredata->m_arfAvailableVoltagesInVolt[ wVoltageIndex ]
//        , m_wYcoordinate - nHalfTextHeightInPixels
//        , //wXcoordinateForVoltageStringBegin
//        wxcoordVoltageStringWidth
////        0
//        ) ;
    }
    //Next time start with x pos of end of this string.
//    wxcoordVoltageStringWidth =
    uiBeginOfVoltageString =
      DrawVoltage(
      r_wxdc,
      mp_cpucoredata->m_arfAvailableVoltagesInVolt[ wVoltageIndex ]
      , m_wYcoordinate - nHalfTextHeightInPixels
      , //wXcoordinateForVoltageStringBegin
//      0
        wxcoordVoltageStringWidth
//        uiBeginOfVoltageString
      ) ;
    if( wxcoordVoltageStringWidth )
      //Next time start with x pos 0.
      wxcoordVoltageStringWidth = 0;
    else
      wxcoordVoltageStringWidth = uiBeginOfVoltageString;
    wPreviousYcoordinateForVoltage = m_wYcoordinate ;
    if( uiBeginOfVoltageString
        //wxcoordVoltageStringWidth
        > m_uiRightmostEndOfVoltageString)
      m_uiRightmostEndOfVoltageString = uiBeginOfVoltageString;
      //wxcoordVoltageStringWidth;
    r_wxdc.DrawLine(
//      m_wXcoordOfBeginOfYaxis,
//      wxcoordVoltageStringWidth,
      uiBeginOfVoltageString,
      m_wYcoordinate, //+ nHalfTextHeightInPixels,
      m_wXcoordOfBeginOfYaxis + m_wDiagramWidth,
      m_wYcoordinate //+ nHalfTextHeightInPixels
      );
    if( uiBeginOfVoltageString)
      //Start at x pos 0 the next time.
      uiBeginOfVoltageString = 0;
  }
  r_wxdc.SetPen(c_r_penCurrent);
  if( mp_model->m_userinterfaceattributes.mainframe.m_nVoltageScaleSizeInPoint)
  {
//    cr_wxfontBefore.SetPointSize(nFontPointSize);
    r_wxdc.SetFont(cr_wxfontBefore);
  }
  LOGN_DEBUG( "end")
}

float MainFrame::GetClosestMultiplier(int nXcoordionate,
    float & fReferenceClockInMHz)
{
  LOGN( "begin--mp_i_cpucontroller:" << mp_i_cpucontroller)
  if( mp_i_cpucontroller && nXcoordionate >= m_wXcoordOfBeginOfYaxis )
  {
    unsigned uiFreqInMHz = (unsigned) (
      (float) (nXcoordionate - m_wXcoordOfBeginOfYaxis) /
      (float) m_wDiagramWidth * (float) m_wMaximumCPUcoreFrequency
      );

//    float fReferenceClockInMHz = mp_i_cpucontroller->m_fReferenceClockInMHz ;
    fReferenceClockInMHz = mp_i_cpucontroller->m_fReferenceClockInMHz ;

    float fMultiplierForYcoordinate = (float) uiFreqInMHz /
        fReferenceClockInMHz;

    float fAvailableMultiplier;
    float fLowerMultiplier = FLT_MIN, fHigherMultiplier = FLT_MAX;

    for( WORD wMultiplierIndex = 0 ; wMultiplierIndex < mp_cpucoredata->
      m_stdset_floatAvailableMultipliers.size() ; ++ wMultiplierIndex )
    {
      fAvailableMultiplier = mp_cpucoredata->m_arfAvailableMultipliers[
        wMultiplierIndex ] ;
      if( fAvailableMultiplier < fMultiplierForYcoordinate)
        fLowerMultiplier = fAvailableMultiplier;
      else
      {
        fHigherMultiplier = fAvailableMultiplier;
        break;
      }
    }
    float fClosestAvailabeMultiplier =
      (fMultiplierForYcoordinate - fLowerMultiplier) <
      fHigherMultiplier - fMultiplierForYcoordinate ?
      fLowerMultiplier : fHigherMultiplier;
    return fClosestAvailabeMultiplier;
  }
  return -1.0;
}

float MainFrame::GetClosestVoltageForYcoordinate(
  //Y coordinate starts at 0,0 at top left corner.
  int nYcoordinate)
{
  if( ! mp_cpucoredata->m_stdset_floatAvailableVoltagesInVolt.empty() )
  {
    const std::set<float> & c_r_std_set_float = mp_cpucoredata->
      m_stdset_floatAvailableVoltagesInVolt;
//    const float fLowestVoltage = * c_r_std_set_float.begin();
//    const float fHighestVoltage = * ( -- c_r_std_set_float.end() );

//    const float fVoltageDiff = fHighestVoltage - fLowestVoltage;

//    const float fYposToDiagramHeightRatio = (float) nYcoordinate / (float)
//      m_wDiagramHeight;

//    const float fVoltageAccordingToYcoordinate =
//      //The maximum voltage is at the top left corner.
//      m_fMaxVoltage -
//      ( fYposToDiagramHeightRatio * m_fMaxMinusMinVoltage) //fVoltageDiff
//      ;
//    GetVoltageAccordignToYcoordinate();
//    {
//      uiYcoordinateForAvailableVoltage = GetYcoordinateForVoltage(* c_iter);
//    }
  //    c_r_std_set_float.lower_bound(); upper_bound()

    float fClosestLowerVoltage = FLT_MIN, fClosestHigherVoltage =
      //If no voltage higher than "fVoltageAccordingToYcoordinate": then
      //fClosestHigherVoltage - fVoltageAccordingToYcoordinate is a very big
      //number.
      //http://www.cplusplus.com/reference/clibrary/cfloat/:
      FLT_MAX;
    std::set<float>::const_iterator c_iter = c_r_std_set_float.begin();
    unsigned uiYcoordinateForAvailableVoltage = 0;
    unsigned uiClosestLowerYcoordinate = 0;
    unsigned uiClosestHigherYcoordinate = 0;
    while( c_iter != c_r_std_set_float.end() )
    {
      uiYcoordinateForAvailableVoltage = GetYcoordinateForVoltage(* c_iter);
      if( //* c_iter < fVoltageAccordingToYcoordinate
          //Higher coordinates=lower voltages.
          uiYcoordinateForAvailableVoltage > nYcoordinate
        )
      {
        uiClosestHigherYcoordinate = uiYcoordinateForAvailableVoltage;
        fClosestLowerVoltage = * c_iter;
      }
      else
      {
        fClosestHigherVoltage = * c_iter;
        uiClosestLowerYcoordinate = uiYcoordinateForAvailableVoltage;
        break;
      }
      ++ c_iter;
    }
    if( uiClosestHigherYcoordinate)
    {
      if( uiClosestLowerYcoordinate)
      {
        const float fClosestVoltageToYcoordinate =
  //        ( fVoltageAccordingToYcoordinate - fClosestLowerVoltage ) <
  //        ( fClosestHigherVoltage - fVoltageAccordingToYcoordinate ) ?
          //Distance (in pixels) to higher voltage.
          ( nYcoordinate - uiClosestLowerYcoordinate ) >
          //Distance (in pixels) to lower voltage.
          ( uiClosestHigherYcoordinate - nYcoordinate ) ?
          fClosestLowerVoltage : fClosestHigherVoltage;
        return fClosestVoltageToYcoordinate;
      }
      else
        return fClosestLowerVoltage;
    }
    else
      return //Lowest voltage.
        * c_r_std_set_float.begin();
  }
  return -1.0;
}

/**
 * data provider: usually the x86I&C Windows service
 */
void MainFrame::GetCPUcoreInfoFromDataProvider(
  ICPUcoreUsageGetter * & p_cpucoreusagegetter ,
  I_CPUcontroller * & p_cpucontroller //,
  )
{
  LOGN("begin")
//  LOGN("DrawCurrentPstateInfo: connected to the service")
  //TODO possibly make IPC communication into a separate thread because it
  // may freeze the whole GUI.
  //    ::wxGetApp().m_ipcclient.SendCommandAndGetResponse(get_current_CPU_data) ;
  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;

  LOGN(//"MainFrame::DrawCurrentPstateInfo "
    "m_bCPUcoreUsageConsumed"
  #ifdef _DEBUG
    ": " << m_bCPUcoreUsageConsumed
  #endif
    )
  LOGN("mp_wxx86infoandcontrolapp->m_vbGotCPUcoreData:" <<
    mp_wxx86infoandcontrolapp->m_vbGotCPUcoreData )
//Do not run it more than once concurrently.
//    if( //m_bCPUcoreUsageConsumed
//        true
//  //    if(
//  //      //Do not call/ wait on the IPC thread if it is running right now.
//  //      && mp_wxx86infoandcontrolapp->m_vbGotCPUcoreData
//        )
//    {
//      //set to true when the thread has finished.
//      mp_wxx86infoandcontrolapp->m_vbGotCPUcoreData = false ;
  mp_wxx86infoandcontrolapp->GetCurrentCPUcoreDataViaIPCNonBlocking() ;
  m_bCPUcoreUsageConsumed = false ;
//    }
//    if( ::wxGetApp().m_ipcclient.m_arbyIPCdata )
//    {
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  p_cpucontroller = & mp_wxx86infoandcontrolapp->
      m_sax2_ipc_current_cpu_data_handler ;
  p_cpucoreusagegetter = & mp_wxx86infoandcontrolapp->
      m_sax2_ipc_current_cpu_data_handler ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  //Prevent the modification of in-program data of either the the CPU core usage
  // or CPU controller data
  // else this may happen:
  //  for some cores data may be from the previous retrieval.
  //  or even the CPU usage may be from previous and so not match the CPU
  //   controller data
  LOGN(//"DrawCurrent CPU core info: "
    "entering IPC 2 in-program data crit sec")
  //Prevent the concurrent modification of the # of log(ical?). cores in the
  //IPC data 2 in-program data thread.
  mp_cpucoredata->wxconditionIPC2InProgramData.Enter() ;
  LOGN(//"DrawCurrent CPU core info: "
    "After entering IPC 2 in-program data crit sec")

  //The number of CPU cores is known if the IPC data were got at first.
  WORD wNumCPUcores = p_cpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
  LOGN(//"DrawCurrentCPUcoreData "
    "after GetNumberOfLogicalCPUcores" )
//      if( wNumCPUcores > mp_cpucoredata->m_byNumberOfCPUCores )
    mp_cpucoredata->SetCPUcoreNumber( wNumCPUcores ) ;
  wxString wxstrTitle = mp_wxx86infoandcontrolapp->m_wxstrDataProviderURL + 
    (p_cpucontroller->m_bDVFSfromServiceIsRunning ? wxT(" [DVFS]--") : 
    wxT(" [no DVFS]--") );
  SetTitle( //wxT("--values from service")
    wxstrTitle + m_wxstrTitle
    ) ;
}

bool MainFrame::GetCPUcoreInfoDirectlyOrFromService(
  ICPUcoreUsageGetter * & p_cpucoreusagegetter ,
  I_CPUcontroller * & p_cpucontroller //,
//  wxString & wxstrCPUcoreUsage
  , bool bGetCPUcoreUsage
  )
{
  LOGN("begin")
  bool bReturn = false ;
  p_cpucoreusagegetter = mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter ;
  p_cpucontroller = mp_i_cpucontroller ;
  //  LOGN("DrawCurrentCPUcoreInfo")
  #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//  #ifdef _DEBUG
//    bool bIsGettingCPUcoreData = mp_wxx86infoandcontrolapp->m_ipcclient.
//      m_vbIsGettingCPUcoreData ;
//    SUPPRESS_UNUSED_VARIABLE_WARNING(bIsGettingCPUcoreData)
//  #endif

  if( //::wxGetApp().m_ipcclient.IsConnected()
      mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
    //This flag should be (set to) "true" as long as writing and reading data
    //to the service is successful.
  //      mp_wxx86infoandcontrolapp->m_ipcclient.m_vbIsGettingCPUcoreData
  //    false
    )
  {
    GetCPUcoreInfoFromDataProvider(
      p_cpucoreusagegetter,
      p_cpucontroller //,
      );
  }
  else
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  {
    SetTitle( m_wxstrTitle //+ wxT("--values from CPU controller")
      ) ;
    p_cpucoreusagegetter = mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter ;
    //Prevent the modification of in-program data of either the the CPU core usage
    // or CPU controller data
    // else this may happen:
    //  for some cores data may be from the previous retrieval.
    //  or even the CPU usage may be from previous and so not match the CPU
    //   controller data
    LOGN(//"DrawCurrent CPU core info: "
      "entering IPC 2 in-program data crit sec")
    mp_cpucoredata->wxconditionIPC2InProgramData.Enter() ;
    LOGN(//"DrawCurrent CPU core info: "
      "After entering IPC 2 in-program data crit sec")
  }
//  LOGN( p_cpucoreusagegetter << p_cpucontroller)
  //  DEBUGN("DrawCurrentCPUcoreInfo CPU controller address:" << mp_i_cpucontroller )
   //::wxGetApp().mp_cpucoreusagegetter->
  if( //mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
      p_cpucoreusagegetter
      // >= 1 CPU core
      && mp_cpucoredata->m_byNumberOfCPUCores )
  {
    PerCPUcoreAttributes * p_percpucoreattributes = & mp_cpucoredata->
        m_arp_percpucoreattributes[ //p_atts->m_byCoreID
        0 ] ;
    //DynFreqScalingThread * p_dynfreqscalingthread
    //If the drawing thread and the dyn freq scaling thread both try to get
    //the CPU usage they could interfere/ the delay between the usage retrieval
    //could be too short-> So only try to get usage here if no DVFS thread.
    if ( ! p_percpucoreattributes->mp_dynfreqscalingthread )
    {
      LOGN(//"DrawCurrentCPUcoreData "
        "before GetPercentalUsageForAllCores" )
      if( bGetCPUcoreUsage )
  //      mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter->
        p_cpucoreusagegetter->
          GetPercentalUsageForAllCores(
          mp_cpucoredata->m_arfCPUcoreLoadInPercent) ;
      m_bCPUcoreUsageConsumed = true ;
  //      DEBUGN("DrawCurrentCPUcoreInfo after GetPercentalUsageForAllCores" )
    }
  }
//  else
//    wxstrCPUcoreUsage = wxT("usage in percent: ?") ;
   #ifdef _DEBUG
   //::wxGetApp().mp_cpucoreusagegetter->Init() ;
   #endif
  //May be NULL at startup.
  if( //mp_i_cpucontroller
      ( p_cpucontroller || //mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
      p_cpucoreusagegetter )
      // >= 1 CPU core
      && mp_cpucoredata->m_byNumberOfCPUCores
    )
  {
    LOGN_WARNING( "before recreating the temperature array: mustn't be from another "
      "thread accessed during this time")
    if( s_arfTemperatureInDegreesCelsius )
      delete [] s_arfTemperatureInDegreesCelsius ;
    s_arfTemperatureInDegreesCelsius = new float[ mp_cpucoredata->
      m_byNumberOfCPUCores ] ;
    bReturn = true ;
  }
  LOGN("return " << bReturn << " I_CPUcontroller *:"
    << p_cpucontroller)
  return bReturn ;
}

WORD MainFrame::GetXcoordinateForFrequency(WORD wFrequencyInMHz)
{
  LOGN(//"GetXcoordinateForFrequency(" <<
    wFrequencyInMHz //<< ")"
    )
  return
    //Explicit cast to avoid (g++) warning.
    (WORD)
    (
     m_wXcoordOfBeginOfYaxis +
      (float) //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
      //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
      //(*iterstdsetmaxvoltageforfreq).m_wFreqInMHz /
    //      (*r_iterstdsetvoltageandfreq).m_wFreqInMHz /
      wFrequencyInMHz /
      (float) m_wMaximumCPUcoreFrequency * m_wDiagramWidth
    ) ;
}

//"Cross-Platform GUI Programming with wxWidgets"
// (Copyright © 2006 Pearson Education, Inc.)
// ISBN 0-13-147381-6  "First printing, July 2005"
//"CHAPTER 5 Drawing and Printing" ->
//"UNDERSTANDING DEVICE CONTEXTS" -> "Drawing on Windows with wxPaintDC" :
//  "[...] another thing you can do to make
//  drawing smoother (particularly when resizing) is to paint the background in
//  your paint handler, and not in an erase background handler. All the painting
//  will then be done in your buffered paint handler, so you don’t see the back-
//  ground being erased before the paint handler is called.

//  Add an empty erase background handler [...]"
// ->Empty implementation, to prevent flicker
void MainFrame::OnEraseBackground(wxEraseEvent& event)
{
}

void MainFrame::HandleResumeForAllVoltAndFreqDlgs()
{
  LOGN( "begin")

#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER
  m_crit_secVoltAndFreqDlgs.Enter();

  std::vector<FreqAndVoltageSettingDlg * >::const_iterator c_iter =
    m_stdvec_p_freqandvoltagesettingdlg.begin();
//  wxPowerEvent evt;
  while( c_iter != m_stdvec_p_freqandvoltagesettingdlg.end() )
  {
    ( * c_iter)->ResumendFromStandByOrHibernate();
    ++ c_iter;
  }
  m_crit_secVoltAndFreqDlgs.Leave();
#endif //#ifdef USE_CRIT_SEC_FOR_FREQ_AND_VOLT_SETTINGS_DLG_CONTAINER

  LOGN( "end")
}

void MainFrame::OnPaint(wxPaintEvent & r_wx_paint_event)
{
  LOGN("begin")
//  DEBUGN("OnPaint CPU controller address:" << mp_i_cpucontroller <<
//    "usage getter addr.:" << mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
//    << "mp_wxbitmap:" << mp_wxbitmap )
  I_CPUcontroller * p_cpucontroller ;
  ICPUcoreUsageGetter * p_cpucoreusagegetter ;
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  if( //::wxGetApp().m_ipcclient.IsConnected()
      mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
    )
  {
    p_cpucontroller = &
        mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler ;
    p_cpucoreusagegetter = &
        mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler ;
  }
  else
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  {
    p_cpucontroller = mp_i_cpucontroller ;
    p_cpucoreusagegetter = mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter ;
  }
  //May be NULL at startup.
  if( ( //mp_i_cpucontroller ||
      p_cpucontroller ||
      //mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
      p_cpucoreusagegetter
      ) &&
    //Is NULL if CPUcontroller is NULL at startup.
    mp_wxbitmap 
    )
  {
    //Control access to m_bAllowCPUcontrollerAccess between threads.
    //m_wxCriticalSectionCPUctlAccess.Enter() ;
    //wxCriticalSectionLocker wxcriticalsectionlocker( 
    //  m_wxcriticalsectionCPUctlAccess ) ;
//    bool bAllowCPUcontrollerAccess  = IsCPUcontrollerAccessAllowedThreadSafe() ;
//    if( //m_bAllowCPUcontrollerAccess
//      bAllowCPUcontrollerAccess )

    //TODO http://docs.wxwidgets.org/2.8/wx_wxbuffereddc.html:
    //"GTK+ 2.0 as well as OS X provide double buffering themselves natively"
    //if( IsDoubleBuffered() )

    wxPaintDC * p_wxpaintdc = new wxPaintDC(this);
      
    //wxMemoryDC wxmemorydc(wxbitmap) ;
    wxBufferedPaintDC wxmemorydc( this //, wxbitmap
      , *mp_wxbitmap
      ) ;

    //"Cross-Platform GUI Programming with wxWidgets"
    // (Copyright © 2006 Pearson Education, Inc.)
    // ISBN 0-13-147381-6  "First printing, July 2005"
    //"CHAPTER 5 Drawing and Printing" ->
    //"UNDERSTANDING DEVICE CONTEXTS" -> "Drawing on Windows with wxPaintDC" :
    // "Shifts the device origin so we don’t have to worry
    // about the current scroll position ourselves"
    PrepareDC(wxmemorydc);

    //http://docs.wxwidgets.org/stable/wx_wxmemorydc.html#wxmemorydc:
    //"Use the IsOk member to test whether the constructor was
    //successful in creating a usable device context."
    if( wxmemorydc.IsOk() )
    {
      wxCoord wxcoordCanvasWidth ;
      wxCoord wxcoordCanvasHeight ;
//        DEBUGN("OnPaint wxmemorydc.IsOk()" )
      p_wxpaintdc->GetSize( & wxcoordCanvasWidth , & wxcoordCanvasHeight ) ;

      //Clears the device context using the current background brush.
      //(else black background?)
      wxmemorydc.Clear();
      //TODO maybe "dc.Blit(dcDiagramScaleAndCurves)" is faster than DrawBitmap.
      //wxmemorydc.Blit(0, 0, wxcoordCanvasWidth , wxcoordCanvasHeight,
      //  & m_wxbufferedpaintdcStatic, 0, 0 );
      //if( mp_wxbufferedpaintdcStatic )
      wxmemorydc.Blit(0, 0, wxcoordCanvasWidth , wxcoordCanvasHeight,
        //mp_wxbufferedpaintdcStatic
        & m_wxmemorydcStatic , 0, 0 );
      //wxmemorydc.DrawBitmap( //mp_wxbitmapDiagramScaleAndCurves
      //  *mp_wxbitmapStatic , 0, 0) ;

  //    DrawCurrentVoltageSettingsCurve(
  //        wxmemorydc ,
  //        m_fMaxVoltage
  //        ) ;

#ifdef __WXGTK__
      /** The following lines are needed under Linux GTK for getting the text
      *   extent correctly? (else the text extent width is to small->
      *   if e.g. drawing multiple text from left to right then the text
      *   overlaps. */
//      wxFont wxfont ;
      /** This line caused a runtime error in wxDC::GetTextExtent() within 
       *   DrawCurrentCPUcoreInfo(...) :
       *   "../src/gtk/dcclient.cpp(1480): assert "fontToUse->IsOk()" failed in DoGetTextExtent(): invalid font"
       *   respectively then it did not show any CPU
       *   information like CPU core frequency.  */
//      wxmemorydc.SetFont( wxfont) ;
#endif //#ifdef __WXGTK__

      DrawCurrentCPUcoreInfo(wxmemorydc) ;

      //Just for testing:
      //wXcoordinate = wYcoordinate = 50 ;
      if( mp_i_cpucontroller )
      {
        bool bAllowCPUcontrollerAccess  =
          IsCPUcontrollerAccessAllowedThreadSafe() ;
        if( //m_bAllowCPUcontrollerAccess
          bAllowCPUcontrollerAccess )
        {
          if( //m_bDrawFreqAndVoltagePointForCurrCoreSettings
            m_vbAnotherWindowIsActive
            )
          {
            DrawVoltageFreqCross(
              wxmemorydc
              , m_fVoltageInVoltOfCurrentActiveCoreSettings
              , m_wFreqInMHzOfCurrentActiveCoreSettings
              , wxBLUE
              ) ;
          }
          else
          {
//              float fVoltageInVolt ;
//              WORD wFreqInMHz ;
            WORD wNumCPUcores = mp_cpucoredata->GetNumberOfCPUcores() ;
            float fRefClock = mp_i_cpucontroller->m_fReferenceClockInMHz ;
            for( //Use WORD data type for guaranteed future (if > 256 logical
              //CPU cores)
              WORD wCPUcoreID = 0 ; wCPUcoreID < wNumCPUcores ; ++ wCPUcoreID )
            {
//              mp_i_cpucontroller->GetCurrentPstate(wFreqInMHz,
//                fVoltageInVolt, byCoreID);
  //            PerCPUcoreAttributes & r_percpucoreattributes = mp_cpucoredata->
  //              m_arp_percpucoreattributes[ byCoreID] ;
              DrawVoltageFreqCross(
                wxmemorydc
                //, r_percpucoreattributes.m_fVoltageInVoltCalculatedFromCPUload
                , //fVoltageInVolt
                mp_ar_voltage_and_multi[wCPUcoreID ].m_fVoltageInVolt
                //, r_percpucoreattributes.m_wFreqInMHzCalculatedFromCPUload
                , //wFreqInMHz
                (WORD) ( mp_ar_voltage_and_multi[wCPUcoreID ].m_fMultiplier *
                  fRefClock )
                , wxBLUE
                ) ;
            }
          }
        } //if( bAllowCPUcontrollerAccess)
      }
      //m_bDrawFreqAndVoltagePointForCurrCoreSettings =
      //    ! m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
        //}
    }//if( wxmemorydc.IsOk() )
    else
    {
      DEBUGN("NOT wxmemorydc.IsOk()" )
    }
    delete p_wxpaintdc;
    //m_wxCriticalSectionCPUctlAccess.Leave() ;
  } // if( mp_i_cpucontroller )
  else
  {
    wxPaintDC wxpaintdc(this);
    //Clears the device context using the current background brush. 
    //(else black background?)
    wxpaintdc.Clear();
    wxpaintdc.DrawText( wxT("no CPU controller available->e.g. attach a DLL")
      , 0 , 0 ) ;
  }
  LOGN("end")
}

//order of submenus/ menu items of "core x" menus.
enum 
{
  Settings = 0,
  highLoadThread
  , highFPUloadThread
};

void MainFrame::DynVoltnFreqScalingEnabled()
{
  ////Stop the timer (else the timer redraws additionally to the scaling thread).
  //m_wxtimer.Stop() ;
  mp_wxmenuitemOwnDVFS->/*SetText*/SetItemLabel(
    /** We need a _T() macro (wide char-> L"", char->"") for EACH
     * line to make it compatible between char and wide char. */
    wxT("disable Own Dynamic Voltage and Frequency Scaling")
    ) ;
}

void MainFrame::EndDynVoltAndFreqScalingThread(
  PerCPUcoreAttributes * p_percpucoreattributes )
{
//  LOGN("ending Dynamic Voltage and Frequency scaling thread")
  LOGN("begin")
  p_percpucoreattributes->mp_dynfreqscalingthread->Stop() ;
  //p_percpucoreattributes->mp_dynfreqscalingthread->Delete() ;
  p_percpucoreattributes->mp_dynfreqscalingthread = NULL ;
  ////Start the timer (it should have been stopped before else the timer had redrawn 
  ////additonally to the scaling thread).
 // m_wxtimer.Start() ;
  mp_wxmenuitemOwnDVFS->/*SetText*/SetItemLabel(
    //We need a _T() macro (wide char-> L"", char->"") for EACH 
    //line to make it compatible between char and wide char.
    wxT("enable Own DVFS") 
    ) ;
  LOGN( "end")
}

void MainFrame::Notify() //overrides wxTimer::Notify()
{
  LOGN(/*"Notify"*/ "begin")
  m_bDrawFreqAndVoltagePointForCurrCoreSettings =
	! m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
    Refresh() ;
}

//Called by the destructor and by OnDetachCPUcontrollerDLL()
void MainFrame::PossiblyReleaseMemForCPUcontrollerUIcontrols()
{
//  BYTE byMenuPosFor1stCPUcore = //2
//      m_byIndexOf1stCPUcontrollerRelatedMenu ;
  LOGN(//"PossiblyReleaseMemForCPUcontrollerUIcontrols "
    "begin")
#ifdef ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
  //May be NULL if the CPU controller is NULL.
  if( m_arp_freqandvoltagesettingdlg )
  {
    //Release dynamically allocated memory (inside OnInit() ) :
    for( BYTE byCPUcoreID = 0 ; byCPUcoreID < mp_cpucoredata->
      m_byNumberOfCPUCores ; ++ byCPUcoreID )
    {
      if ( m_arp_freqandvoltagesettingdlg[ byCPUcoreID ] )
      {
        m_arp_freqandvoltagesettingdlg[ byCPUcoreID ]->
          //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindowdestroy:
          //"Use this function instead of the delete operator[...]"
          //"true if the window has either been successfully deleted, or it 
          //has been added to the list of windows pending real deletion."
          Destroy() ;
      }
    }
    //Delete the array containing the pointers
    delete [] m_arp_freqandvoltagesettingdlg ;
    m_arp_freqandvoltagesettingdlg = NULL ;
    LOGN(//"PossiblyReleaseMemForCPUcontrollerUIcontrols "
      "end")
  }
  //for( std::vector<wxMenu *>::const_iterator c_i = m_vecp_wxmenuCore.begin()
  //  ; c_i != m_vecp_wxmenuCore.end() ; ++ c_i )
  //{
  //  //(*c_i)->Destroy() ;
  //  mp_wxmenubar->Remove(2) ;
  //  delete (*c_i) ;
  //}
  //if( ! m_vecp_wxmenuCore.empty() )
  for( BYTE byCoreID = mp_cpucoredata->m_byNumberOfCPUCores - 1 ; 
    byCoreID != 
    //gets 255 if it was 0 before and then 1 is subtracted.
    255 ; -- byCoreID )
  {
    wxMenu * p_wxmenu = mp_wxmenubar->Remove( 
      byMenuPosFor1stCPUcore + byCoreID ) ;
    delete p_wxmenu ;
  }
  //for( std::vector<wxMenu *>::const_iterator c_i = m_stdvecp_wxmenuCore.begin()
  //  ; c_i != m_vecp_wxmenuCore.end() ; ++ c_i 
  //  )
  //{

  //}
#else //ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
  m_stdvec_p_freqandvoltagesettingdlg.clear() ;
  //TODO runtime error in Linux debug version
  const int numMenus = mp_wxmenubar->GetMenuCount();
  wxMenu * p_wxmenu = mp_wxmenubar->Remove(
    //Linux does not have a "service menu"-> 1 index less than under Windows.
    //Remove the "core(s)" menu.
//    3
    numMenus - 1
    ) ;
  delete p_wxmenu ;
#endif //ONE_P_STATE_DIALOG_FOR_EVERY_CPU_CORE
  m_vecp_wxmenuCore.clear() ;
}

void MainFrame::PossiblyAskForOSdynFreqScalingDisabling()
{
  LOGN("begin")
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  bool bOtherDVFSisEnabled =
    //mp_i_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled()
    //mp_i_cpucontroller->OtherPerfCtrlMSRwriteIsActive()
    mp_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->OtherDVFSisEnabled();
  LOGN("bOtherDVFSisEnabled: " << bOtherDVFSisEnabled)
  if( bOtherDVFSisEnabled )
  {
    if (::wxMessageBox(
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compatible between char and wide char.
        _T("The OS's dynamic frequency scaling must be disabled ")
        _T("in order that the p-state isn' changed by the OS afterwards.")
        _T("If the OS's dynamic frequency isn't disabled, should it be done now?")
        , 
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compatible between char and wide char.
        _T("Question")
        , wxYES_NO | wxICON_QUESTION )
        == wxYES
       )
      //mp_i_cpucontroller->DisableFrequencyScalingByOS();
      mp_wxx86infoandcontrolapp->mp_dynfreqscalingaccess->
        DisableFrequencyScalingByOS() ;
  }
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  LOGN("end")
}

//#ifdef _TEST_PENTIUM_M
#ifdef COMPILE_WITH_MSR_EXAMINATION
  void MainFrame::OnShowExamineCPUregistersDialog(wxCommandEvent& WXUNUSED(event))
  {
    ////May be NULL at startup.
    //if( mp_i_cpucontroller )
    //{
     wxExamineCPUregistersDialog * p_wxdlg = new wxExamineCPUregistersDialog(
       this , 
       //msrdata 
       //*mp_i_cpucontroller->mp_model,
       //mp_i_cpucontroller
       //mp_wxx86infoandcontrolapp->GetCPUaccess()
       //, 
       mp_wxx86infoandcontrolapp
       );
     //p_wxdlg->ShowModal() ;
     p_wxdlg->Show(true) ;
    //} //  if( mp_i_cpucontroller )
  }
#endif

void MainFrame::OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent)
{
  LOGN(//"on dyn created control"
    "begin")
  int nMenuEventID = wxevent.GetId() ;
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//  if( nMenuEventID == ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq )
//  {
//    //wxMessageDialog();
//    //wxMessageBox("hh") ;
//    //wxString strCurrentValue = wxT("50") ;
//    //wxString sNewValue = wxGetTextFromUser(wxT("Enter min CPU %for
//      CURRENT power profile (scheme)"),
////#ifndef _DEBUG
//    //  wxT("title"), strCurrentValue);
//    if( ::wxGetApp().mp_dynfreqscalingaccess->OtherDVFSisEnabled()
//      )
////#endif
//      ::wxGetApp().mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
////#ifndef _DEBUG
//    else
//      ::wxGetApp().mp_dynfreqscalingaccess->EnableFrequencyScalingByOS() ;
////#endif //#ifdef _DEBUG
//    mp_wxmenuitemOtherDVFS->SetText(//_T("")
//       wxString::Format(
//       //We need a _T() macro (wide char-> L"", char->"") for EACH 
//       //line to make it compatible between char and wide char.
//       _T("%sable DVFS") , 
//       ::wxGetApp().mp_dynfreqscalingaccess->OtherDVFSisEnabled() ? 
//       //We need a _T() macro (wide char-> L"", char->"") for EACH 
//       //line to make it compatible between char and wide char.
//       _T("dis") : 
//      //We need a _T() macro (wide char-> L"", char->"") for EACH 
//      //line to make it compatible between char and wide char.
//      _T("en")
//        )
//      ) ;
//    return ;
  //}
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//#endif
  BYTE byCoreID = ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) / 
    m_nNumberOfMenuIDsPerCPUcore ;
  //EventIDToDivisorIDandFrequencyID(nMenuEventID);
  if( //Array was successfully allocated.
    m_arp_freqandvoltagesettingdlg 
    //&&
    ////menu item "set frequency and voltage" for any CPU core menu was selected.
    //( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) % 
    //m_nNumberOfMenuIDsPerCPUcore == 0 
    )
  {
    //FreqAndVoltageSettingDlg freqandvoltagesettingdlg(this);
    //freqandvoltagesettingdlg.Show(true);
    //if( mp_freqandvoltagesettingdlg )
    //{
    //  //if(mp_freqandvoltagesettingdlg )
    //    mp_freqandvoltagesettingdlg->Show(true);
    //}
    //else
    //{
    //  //If created as local variable on stack the dialog would disappear
    //  //immediately.
    //  mp_freqandvoltagesettingdlg = new FreqAndVoltageSettingDlg(this);
    //  if(mp_freqandvoltagesettingdlg )
    //    mp_freqandvoltagesettingdlg->Show(true);
    //}
    BYTE bySubmenuIndexWithinPerCoreMenu = ( nMenuEventID - 
      m_nLowestIDForSetVIDnFIDnDID ) % m_nNumberOfMenuIDsPerCPUcore ;

    switch( bySubmenuIndexWithinPerCoreMenu )
    {
      case //0:
        Settings :
        if( mp_cpucoredata->m_arfAvailableMultipliers &&
            mp_cpucoredata->m_arfAvailableVoltagesInVolt )
        {
          if( m_arp_freqandvoltagesettingdlg[byCoreID] )
            m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
        }
        else
        {
          //TODO the dialog may be shown, but writing the p-state must be
          //prevented if there are no voltages and/ or multipliers because
          //_usually_ (voltage not for Nehalem/ i7 720 qm ) both are needed.
          if( ! mp_cpucoredata->m_arfAvailableMultipliers )
            wxMessageBox(
              wxT("no multipliers available->no setting possible") ) ;
          if( ! mp_cpucoredata->m_arfAvailableVoltagesInVolt )
            wxMessageBox(
              wxT("no multipliers available->no setting possible") ) ;
        }
//        else
//        {
//          //If created as local variable on stack the dialog would disappear
//          //immediately.
//          if( m_arp_freqandvoltagesettingdlg[byCoreID] )
//            m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
//        }
      break;
//      case setp_state2:
//        PossiblyAskForOSdynFreqScalingDisabling();
//        Set the register where the VID and multiplier values for p-state 2
//        reside as current (works at least for AMD Griffin)
//        mp_i_cpucontroller->SetPstate(2,
//          //1 = 1bin
//          1 <<
//          //=core ID:
//            //difference between current menu ID first menu ID for
//            //core settings
//            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//            / m_nNumberOfMenuIDsPerCPUcore);
//        break;
      case highLoadThread:
        {
        //m_byNumberOfSettablePstatesPerCore + 1:
//        BYTE byCoreID =
//          //=core ID:
//            //difference between current menu ID first menu ID for
//            //core settings
//            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//            / m_nNumberOfMenuIDsPerCPUcore ;
        //mp_i_cpucontroller->mp_calculationthread->StartCalculationThread(
        //  //=core ID:
        //    //difference between current menu ID first menu ID for
        //    //core settings
        //    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        //    / m_nNumberOfMenuIDsPerCPUcore
        //  );
        //TODO uncomment
        //BYTE byAction = mp_i_cpucontroller->StartOrStopCalculationThread(
        //    byCoreID
        //  );
        //BuildHighLoadThreadMenuText(
        //  " high load thread (e.g. for stability check)" , byAction ) ;
        }
        break;
      default:
        m_stdmapwmenuid2i_cpucontrolleraction.find(nMenuEventID)->
           second->Execute() ;
        //if( bySubmenuIndexWithinPerCoreMenu == m_byNumberOfSettablePstatesPerCore + 1)
        //{
        //BYTE byCoreID = 
        //  //=core ID:
        //    //difference between current menu ID first menu ID for
        //    //core settings
        //    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        //    / m_nNumberOfMenuIDsPerCPUcore ;
        ////mp_i_cpucontroller->mp_calculationthread->StartCalculationThread(
        ////  //=core ID:
        ////    //difference between current menu ID first menu ID for
        ////    //core settings
        ////    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        ////    / m_nNumberOfMenuIDsPerCPUcore
        ////  );
        //BYTE byAction = mp_i_cpucontroller->StartOrStopCalculationThread(
        //    byCoreID
        //  );
        //wxString wxstr = byAction == ENDED ? "Start" : "End" ;
        //////Invert the menu item's checked state.
        ////marp_wxmenuItemHighLoadThread[byCoreID]->Check(
        ////    ! marp_wxmenuItemHighLoadThread[byCoreID]->IsChecked () ) ;
        //marp_wxmenuItemHighLoadThread[byCoreID]->SetText( 
        //    wxstr + 
        //    " high load thread (e.g. for stability check)"
        //    );
        //}
        if( bySubmenuIndexWithinPerCoreMenu //> 0 
           >= m_byMenuIndexOf1stPstate &&
           bySubmenuIndexWithinPerCoreMenu < //bySubmenuIndexWithinPerCoreMenu 
           m_byMenuIndexOf1stPstate + m_byNumberOfSettablePstatesPerCore )
        {
//          BYTE byCoreID =
//          //=core ID:
//              //difference between current menu ID first menu ID for
//              //core settings
//              ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//              / m_nNumberOfMenuIDsPerCPUcore ;
//          BYTE byCoreBitMask =
//            //1 = 1bin
//            1 << byCoreID ;
          PossiblyAskForOSdynFreqScalingDisabling();
          //mp_i_cpucontroller->SetPstate(
          //  bySubmenuIndexWithinPerCoreMenu - m_byMenuIndexOf1stPstate ,
          //  //byCoreBitMask
          //  byCoreID
          //  );
        }
        break;
    }
  }
}

void MainFrame::OnFindLowestOperatingVoltage(wxCommandEvent& WXUNUSED(event))
{
  //TODO uncomment
  //mp_i_cpucontroller->FindLowestOperatingVoltage(
  //  mp_i_cpucontroller->GetCurrentPstate(1),0);
}

#ifdef PRIVATE_RELEASE //hide the other possibilities
void MainFrame::OnIncreaseVoltageForCurrentPstate(wxCommandEvent& WXUNUSED(event))
{
  mp_i_cpucontroller->IncreaseVoltageForCurrentPstate();
}
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities

  void MainFrame::OnRedrawEverything(wxCommandEvent & evt )
  {
    LOGN( "begin")
    RedrawEverything();
  }

#ifdef wxHAS_POWER_EVENTS
  void MainFrame::OnResume(wxPowerEvent & WXUNUSED(event) )
  {
    LOGN( "resumed from standby/ hibernate")

//    HandleResumeForAllVoltAndFreqDlgs();

    //May be NULL at startup.
    if( mp_i_cpucontroller )
    {
      //mp_pumastatectrl->ApplyAllPStates() ;
      mp_i_cpucontroller->ResumeFromS3orS4() ;
      //After a resume (from standby) the counter value for CPU core 1 did not
      //change.
      //So re-initialize it now.
      if(::wxGetApp().mp_cpucoreusagegetter)
        ::wxGetApp().mp_cpucoreusagegetter->Init() ;
      //wxLogMessage(_T("System resumed from suspend."));
    } // if( mp_i_cpucontroller )
    LOGN( "end")
  }
#endif // wxHAS_POWER_EVENTS

//void MainFrame::OnInitDialog(wxInitDialogEvent& event )
//{
//  LOGN("OnInitDialog")
////  RedrawEverything() ;
////  Update() ;
////  Refresh() ;
//}

void MainFrame::OnSaveVoltageForFrequencySettings(wxCommandEvent & WXUNUSED(event) )
{
  std::string std_strCPUtypeRelativeDirPath;
  if( //wxGetApp().m_maincontroller.GetPstatesDirPath(
      mp_wxx86infoandcontrolapp->m_maincontroller.GetPstatesDirPath(
      std_strCPUtypeRelativeDirPath )
    )
    mp_wxx86infoandcontrolapp->SaveVoltageForFrequencySettings(
      std_strCPUtypeRelativeDirPath);
}

void MainFrame::OnSaveAsCPUcontrollerDynLibForThisCPU(
  wxCommandEvent & WXUNUSED(event) )
{
  mp_wxx86infoandcontrolapp->SaveAsCPUcontrollerDynLibForThisCPU();
}

void MainFrame::OnSize( wxSizeEvent & //WXUNUSED(
                     sizeevent//)
                     )
{
  LOGN( "begin" )
  RedrawEverything() ;
}

void MainFrame::OnSizing(wxSizeEvent & wxSizeEvent)
{
  LOGN("begin")
}

#ifdef wxHAS_POWER_EVENTS
void MainFrame::OnSuspending(wxPowerEvent & WXUNUSED(event))
{
  LOGN( "suspending power event")
}

void MainFrame::OnSuspended(wxPowerEvent & WXUNUSED(event))
{
  LOGN( "suspended power event")
}

void MainFrame::OnSuspendCancel(wxPowerEvent & WXUNUSED(event))
{
  LOGN("cancelled suspend power event")
}
#endif //#ifdef wxHAS_POWER_EVENTS

//void GetCurrentPstateAndAddDefaultVoltage()
//{
//      //Do something
//      bool bNewVoltageAndFreqPair = false ;
//      float fVoltageInVolt ;
//      WORD wFreqInMHz ;
//      std::pair <std::set<VoltageAndFreq>::iterator, bool>
//        stdpairstdsetvoltageandfreq ;
//      for ( BYTE byCPUcoreID = 0 ; byCPUcoreID <
//        mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCPUcoreID )
//      {
//        if( mp_i_cpucontroller->GetCurrentPstate(wFreqInMHz, fVoltageInVolt,
//          byCPUcoreID ) )
//        {
//    #ifdef _DEBUG
//          if( wFreqInMHz > 1800 )
//            wFreqInMHz = wFreqInMHz ;
//    #endif
//          //stdpairstdsetvoltageandfreq = mp_model->m_cpucoredata.
//          //  m_stdsetvoltageandfreqDefault.insert(
//          //  VoltageAndFreq ( fVoltageInVolt , wFreqInMHz )
//          //  ) ;
//          if( mp_model->m_bCollectPstatesAsDefault )
//            bNewVoltageAndFreqPair = mp_model->m_cpucoredata.
//              AddDefaultVoltageForFreq(
//              fVoltageInVolt , wFreqInMHz ) ;
//          ////New p-state inserted.
//          //if( stdpairstdsetvoltageandfreq.second )
//          //  bNewVoltageAndFreqPair = true ;
//        }
//      }
//      if( bNewVoltageAndFreqPair )
//      {
//        std::set<VoltageAndFreq>::reverse_iterator reviter =
//          mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.rbegin() ;
//        //Need to set the max freq. Else (all) the operating points are
//        // drawn at x-coord. "0".
//        mp_cpucoredata->m_wMaxFreqInMHz = (*reviter).m_wFreqInMHz ;
//        if( mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.size() > 1
//          //&& ! mp_wxmenuitemOwnDVFS->IsEnabled()
//          )
//          mp_wxmenuitemOwnDVFS->Enable(true) ;
//        RedrawEverything() ;
//      }
//      else
//}

void MainFrame::OnTimerEvent(wxTimerEvent & event)
{
  LOGN( "begin")
//  DEBUGN("OnTimerEvent CPU controller pointer:" << mp_i_cpucontroller )
//  LOGN("OnTimerEvent "
//    "CPU controller pointer:" << mp_i_cpucontroller )
  //May be NULL at startup.
  I_CPUcontroller * p_cpucontroller ;
//  ICPUcoreUsageGetter * p_cpucoreusagegetter ;
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  if( //::wxGetApp().m_ipcclient.IsConnected()
      mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
    )
  {
    p_cpucontroller = &
        mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler ;
//    p_cpucoreusagegetter = &
//        mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler ;
    if( ! mp_wxbitmap )
      RecreateDisplayBuffers() ;
  }
  else
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  {
    p_cpucontroller = mp_i_cpucontroller ;
//    p_cpucoreusagegetter = mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter ;
  }
  if( //mp_i_cpucontroller
      p_cpucontroller )
  {
    bool bAllowCPUcontrollerAccess  = IsCPUcontrollerAccessAllowedThreadSafe() ;
//    DEBUGN("CPU controller access allowed:" << bAllowCPUcontrollerAccess )
    if( //m_bAllowCPUcontrollerAccess 
      bAllowCPUcontrollerAccess )
    {
      static bool isIconized, isVisible;
      isIconized = IsIconized();
      //TODO: returns true even if not visible (hidden bei other frames) ->
      // get window client update regions to decide whether to redraw.
      isVisible = IsVisible();
      //Even if this window is not visible (iconized/ hidden) the highest
      //temperature should be shown in the task bar/ system tray.
      if( //If the window is hidden, "IsIconized()" returns "false"?
          isIconized ||
//          !
//          //"Returns true if this window is currently active, i.e. if the user
//          //is currently working with it."
//          IsActive()
          ! //IsVisible() returns true (even) if the window is iconized.
          isVisible
        )
      {
        //If this window is iconized then OnPaint(...) isn't called and so
        //"StoreCurrentVoltageAndFreqInArray(...)" is _not_ being called
        //(indirectly), so get the current temperature here.
//        if( s_arfTemperatureInDegreesCelsius )
//          delete [] s_arfTemperatureInDegreesCelsius ;
//        s_arfTemperatureInDegreesCelsius = new float[ mp_cpucoredata->
//          m_byNumberOfCPUCores ] ;
        ICPUcoreUsageGetter * p_cpucoreusagegetter ;
        I_CPUcontroller * p_cpucontroller ;
        if( GetCPUcoreInfoDirectlyOrFromService(
            p_cpucoreusagegetter,
            p_cpucontroller //,
      //      wxstrCPUcoreUsage
            , //bool bGetCPUcoreUsage
//            false
            mp_model->m_userinterfaceattributes.
              m_bShowCPUcoreUsagesIconInTaskBar
            )
          )
        {
          LOGN(//"DrawCurrentCPUcoreData "
            "leaving IPC 2 in-program data crit sec")
          mp_cpucoredata->wxconditionIPC2InProgramData.Leave() ;
          LOGN(//"DrawCurrentCPUcoreData "
            "after leaving IPC 2 in-program data crit sec")
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
          if( mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon )
          {
            //respect # of cpu cores
            for ( WORD wCPUcoreID = 0 ; wCPUcoreID <
              mp_cpucoredata->m_byNumberOfCPUCores ; ++ wCPUcoreID )
            {
              s_arfTemperatureInDegreesCelsius[ wCPUcoreID ] = p_cpucontroller->
                GetTemperatureInCelsius(wCPUcoreID) ;
            }
            ShowHighestCPUcoreTemperatureInTaskBar(p_cpucontroller) ;
          }
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
          ShowCPUcoreUsagesInTaskBar(p_cpucontroller);
          if( mp_model->m_userinterfaceattributes.m_bShowCPUcoreUsagesIconInTaskBar )
          {
            //respect # of cpu cores
            for ( WORD wCPUcoreID = 0 ; wCPUcoreID <
              mp_cpucoredata->m_byNumberOfCPUCores ; ++ wCPUcoreID )
              p_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
                wCPUcoreID);
            ShowCPUcoresMultipliersInTaskBar(p_cpucontroller);
          }
        }
      }
      else // !IsIconized() && IsVisible()
      {
//        GetCurrentPstateAndAddDefaultVoltage();

        if( m_bDiagramNotDrawn //&& mp_i_cpucontroller
            && //mp_i_cpucontroller->m_fReferenceClockInMHz
            p_cpucontroller->m_fReferenceClockInMHz
            )
        {
          LOGN("diagram not already drawn and reference clock <> 0 ")
  //        bool bAllowCPUcontrollerAccess  =
  //            IsCPUcontrollerAccessAllowedThreadSafe() ;
  //        if( //bAllowCPUcontrollerAccess &&
  //            mp_i_cpucontroller->
  //            m_fReferenceClockInMHz )
  //        {
            //The diagram is based on the CPU core frequency and can be drawn at
            //first when the reference clock is known.
            RedrawEverything() ;
            m_bDiagramNotDrawn = false ;
  //        }
        }
        else
          //http://docs.wxwidgets.org/trunk/classwx_window.html
          // #29dc7251746154c821b17841b9877830:
          //"Causes this window, and all of its children recursively (except
          //under wxGTK1 where this is not implemented), to be repainted.
          //Note that repainting doesn't happen immediately but only during the
          //next event loop iteration, if you need to update the window
          //immediately you should use Update() instead."

          // -> EVT_PAINT -> MainFrame::OnPaint(...) is called.
          Refresh() ;
      }
    }
    //TRACE("OnTimerEvent\n") ;
  } // if( mp_i_cpucontroller )
  else //if( mp_i_cpucontroller )
  {
    if( mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter )
      //Also refresh if just a core usage getter (for showing the usage per
      //core)
      Refresh() ;
  }
  LOGN( "end")
}

void MainFrame::OnUpdateViewInterval(wxCommandEvent & WXUNUSED(event))
{
  wxString wxstrMessage = wxT("Input update view interval in milliseconds") ;
  wxString wxstrInput = ::wxGetTextFromUser( wxstrMessage , wxT("input"), 
    wxT("1000") ) ;
  //::wxGetNumberFromUser
  //If the user has input text (and has NOT pressed "cancel")
  if( ! wxstrInput.empty() )
  {
    unsigned long ulMs ;
    bool bSuccess = wxstrInput.ToULong(& ulMs) ;
    if ( bSuccess )
    {
      //ReadMsrEx() returned false results if used with a time and a 10 ms interval.
      if( ulMs < 100 )
        wxMessageBox( wxT("the number is too low. ")
          wxT("Getting the current CPU frequency returned wrong values with < 100 ms") ) ;
      else
      {
        m_dwTimerIntervalInMilliseconds = ulMs ;
        m_wxtimer.Stop() ;
        m_wxtimer.Start(ulMs) ;
//        m_p_wxtimer->Stop() ;
//        m_p_wxtimer->Start(ulMs) ;
      }
    }
    else
    {
      wxMessageBox( wxT("You did not enter a valid integer number") ) ;
    }
  }
}

  void MainFrame::OnShowCPUregistersReadAndWriteDialog( wxCommandEvent & WXUNUSED(event) )
  {
#ifndef _DEBUG
    //May be NULL at startup.
    if( mp_i_cpucontroller )
#endif
    {
#ifdef COMPILE_WITH_MSR_EXAMINATION
     CPUregisterReadAndWriteDialog * p_wxdlg = new
       CPUregisterReadAndWriteDialog(
       this ,
       //msrdata
       //*mp_i_cpucontroller->mp_model,
       * mp_model ,
       mp_i_cpucontroller
       //mp_i_cpucontroller->mp_cpuaccess
       //, mp_wxx86infoandcontrolapp
       );
     //p_wxdlg->ShowModal() ;
     p_wxdlg->Show(true) ;
#endif //#ifdef COMPILE_WITH_MSR_EXAMINATION
    }   //if( mp_i_cpucontroller )
  }

void MainFrame::RecreateDisplayBuffers()
{
  LOGN(//"RecreateDisplayBuffers "
    "begin")
  if( mp_wxbitmap )
    delete mp_wxbitmap ;
  wxRect rect = GetClientRect();

  //wxCoord wxcoordCanvasWidth ;
  //wxCoord wxcoordCanvasHeight ;
  //p_wxpaintdc->GetSize( & wxcoordCanvasWidth , & wxcoordCanvasHeight ) ;
  m_wDiagramHeight = rect.height
      //Space for drawing scale and scale values below the x-axis.
      - 50 ;
  //WORD wXcoordOfBeginOfYaxis = 50 ;
  m_wXcoordOfBeginOfYaxis = 50 ;
  //WORD wDiagramWidth = wxcoordCanvasWidth - wXcoordOfBeginOfYaxis - 30 ;
  m_wDiagramWidth = rect.width - m_wXcoordOfBeginOfYaxis - 30 ;

  mp_wxbitmap = new wxBitmap(
    rect.width,
    rect.height ,
    //http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapctor:
    //"A depth of -1 indicates the depth of the current screen or visual."
    -1) ;

  //if( mp_wxbufferedpaintdcStatic )
  //  delete mp_wxbufferedpaintdcStatic ;
  //create new objet so it uses internally an bitmap with the new client size.
  //mp_wxbufferedpaintdcStatic = new wxBufferedPaintDC(this) ;
  if( mp_wxbitmapStatic )
    delete mp_wxbitmapStatic ;
  mp_wxbitmapStatic = new wxBitmap(
    rect.width,
    rect.height ,
    //http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapctor:
    //"A depth of -1 indicates the depth of the current screen or visual."
    -1) ;
  m_wxmemorydcStatic.SelectObject(*mp_wxbitmapStatic) ;
  //Clears the device context using the current background brush.
  //(else black background?)
  m_wxmemorydcStatic.Clear();

  m_wMinYcoordInDiagram =
    //Let the diagram begin at the vertical middle of the topmost voltage value.
    m_wxmemorydcStatic.GetCharHeight() / 2 ;
}

//in order to draw voltage and freq aligned if more than 1 core/ to draw at
//the same pos for many intervals:
//core 0:  0.9  V 1100 MHz
//core 1:  1.11 V 550  MHz

void MainFrame::DetermineMaxVoltageAndMaxFreqDrawWidth(wxDC & r_wxdc)
{
  wxString wxstrMaxFreq = wxString::Format(
    //Use wxT() to enable to compile with both unicode and ANSI.
    wxT("%u"),
    mp_cpucoredata->m_wMaxFreqInMHz ) ;
  //the max. freq. usually has the max draw width.
  m_wMaxFreqWidth = r_wxdc.GetTextExtent(wxstrMaxFreq).GetWidth() ;
  //Because max. voltage ID is lowest voltage for AMD Griffin but highest
  //voltage for Pentium M.
  float fVoltageForMaxVoltageID = mp_i_cpucontroller->GetVoltageInVolt(
    mp_cpucoredata->m_byMaxVoltageID ) ;
  float fVoltageForMinVoltageID = mp_i_cpucontroller->GetVoltageInVolt(
    mp_cpucoredata->m_byMinVoltageID ) ;
  float fMaxVoltage = fVoltageForMaxVoltageID > fVoltageForMinVoltageID ?
      fVoltageForMaxVoltageID : fVoltageForMinVoltageID ;
  wxString wxstrMaxVolt = wxString::Format(
    //Use wxT() to enable to compile with both unicode and ANSI.
    wxT("%f"), fMaxVoltage ) ;
  //the max. voltage usually has the max draw width.
  m_wMaxVoltageWidth = r_wxdc.GetTextExtent(wxstrMaxFreq).GetWidth() ;
}

void MainFrame::DetermineTextHeight(wxDC & r_wxdc)
{
  wxCoord wxcoordWidth ;
  wxCoord wxcoordHeight ;
  wxCoord wxcoordDescent ;
  r_wxdc.GetTextExtent(
    wxT("|"),
    & wxcoordWidth , //wxCoord *w,
    & wxcoordHeight , //wxCoord *h,
    & wxcoordDescent //wxCoord *descent = NULL,
    //wxCoord *externalLeading = NULL, wxFont *font = NULL
    ) ;
  m_wTextHeight = wxcoordHeight + wxcoordDescent ;
}

void MainFrame::RedrawEverything()
{
  LOGN(//"RedrawEverything "
    "mp_i_cpucontroller:" << mp_i_cpucontroller)
  DetermineTextHeight(m_wxmemorydcStatic) ;
  //May be NULL at startup.
  if( mp_i_cpucontroller )
//  I_CPUcontroller * p_cpucontroller ;
//  if( ::wxGetApp().m_ipcclient.IsConnected() )
//  {
//    p_cpucontroller = &
//        mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler ;
//    if( !mp_wxbitmap )
//      RecreateDisplayBuffers() ;
//  }
//  else
//  {
//    p_cpucontroller = mp_i_cpucontroller ;
//  }
//  if( p_cpucontroller )
  {
    DetermineMaxVoltageAndMaxFreqDrawWidth(m_wxmemorydcStatic) ;
    //Control access to m_bAllowCPUcontrollerAccess between threads.
    //m_wxCriticalSectionCPUctlAccess.Enter() ;
    //wxCriticalSectionLocker wxcriticalsectionlocker( 
    //  m_wxcriticalsectionCPUctlAccess ) ;
    bool bAllowCPUcontrollerAccess  = IsCPUcontrollerAccessAllowedThreadSafe() ;
    LOGN(//"RedrawEverything "
      "bAllowCPUcontrollerAccess:" <<
      bAllowCPUcontrollerAccess)
    if( //m_bAllowCPUcontrollerAccess 
      bAllowCPUcontrollerAccess )
    {
//      int i = 0 ;
      m_fMinVoltage = //mp_i_cpucontroller->GetMinimumVoltageInVolt() ;
          mp_model->m_cpucoredata.GetMinimumVoltage() ;
      float fMaxMulti = mp_model->m_cpucoredata.GetMaximumMultiplier() ;
      m_wMaximumCPUcoreFrequency =
          (WORD) ( fMaxMulti *
          mp_i_cpucontroller->m_fReferenceClockInMHz ) ;
      LOGN(//"RedrawEverything "
        "m_wMaximumCPUcoreFrequency="
        << m_wMaximumCPUcoreFrequency
        << "=" << mp_i_cpucontroller->m_fReferenceClockInMHz
        << "*" << fMaxMulti
        )
      RecreateDisplayBuffers(//m_wxmemorydcStatic
        ) ;
      //m_wxbufferedpaintdcStatic.SelectObject(mp_wxbitmapStatic) ;
    //  if( mp_wxbufferedpaintdc
    //  wxBufferedPaintDC mp_wxbufferedpaintdc ( this ) ;
      //Drawing the curves (calculate and draw ~ 400 points) takes some time.
      //So do it only when the client size changes and store the drawn curves
      //into a image and DrawBitmap() or do "Blit()" with the DC drawn to and
      //the DC that shows it in the window.
      //if( mp_wxbufferedpaintdcStatic )
      {
        std::set<VoltageAndFreq> & r_setvoltageforfreq = //mp_i_cpucontroller->
          mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
    //    std::set<MaxVoltageForFreq>::iterator iterstdsetmaxvoltageforfreq = 
//        std::set<VoltageAndFreq>::reverse_iterator iterstdsetvoltageandfreq =
//          //m_setmaxvoltageforfreq.begin() ;
//          r_setvoltageforfreq.rbegin() ;
        if(
          //mp_i_cpucontroller->mp_model->
          //m_setmaxvoltageforfreq.end()
//          iterstdsetvoltageandfreq !=
//          r_setvoltageforfreq.rend()
           ! mp_cpucoredata->m_stdset_floatAvailableVoltagesInVolt.empty()
          )
        {
          m_fMaxVoltage = //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt ;
            ////P-state 0 usually has the highest voltage.
            //(*iterstdsetvoltageandfreq).m_fVoltageInVolt ;
            mp_cpucoredata->m_arfAvailableVoltagesInVolt[ mp_cpucoredata->
              //Last element/ highest voltage.
              m_stdset_floatAvailableVoltagesInVolt.size() - 1 ] ;
          //for g++: assign value to created iterator and pass this to the arg
          std::set<VoltageAndFreq>::iterator iter = r_setvoltageforfreq.begin() ;
          DrawDiagramScale( m_wxmemorydcStatic , //iterstdsetvoltageandfreq
            //r_setvoltageforfreq.begin()
            iter
            ) ;
        }
        m_fMaxMinusMinVoltage = m_fMaxVoltage - m_fMinVoltage ;
        DrawVoltageGraph(
          //m_wxbufferedpaintdcStatic
          //*mp_wxbufferedpaintdcStatic
          m_wxmemorydcStatic
          , m_fMaxVoltage
          , mp_cpucoredata->m_stdsetvoltageandfreqDefault
          ) ;
        DrawCurrentVoltageSettingsCurve(
          //m_wxbufferedpaintdcStatic
          //*mp_wxbufferedpaintdcStatic
          m_wxmemorydcStatic
          , m_fMaxVoltage
          //, mp_cpucoredata->m_stdsetvoltageandfreqWanted
          ) ;
        DrawLowestStableVoltageCurve(
          //m_wxbufferedpaintdcStatic
          //*mp_wxbufferedpaintdcStatic
          m_wxmemorydcStatic
          , m_fMaxVoltage
          ) ;
//        DrawAllPossibleOperatingPoints( m_wxmemorydcStatic ) ;
        DrawPerformanceStatesCrosses( m_wxmemorydcStatic , 
          mp_cpucoredata->m_stdsetvoltageandfreqDefault 
          , wxBLACK ) ;
        DrawPerformanceStatesCrosses( m_wxmemorydcStatic ,
          mp_cpucoredata->m_stdsetvoltageandfreqWanted
          //The wanted voltage may be above the default voltage because the
          //default voltage may be too low (e.g. for AMD Turion X2 Ultra
          //2400 MHz)
          , wxGREEN ) ;
        DrawPerformanceStatesCrosses( m_wxmemorydcStatic ,
          mp_cpucoredata->m_stdsetvoltageandfreqLowestStable
          , wxRED ) ;
      }
    } //if( bAllowCPUcontrollerAccess)
    //m_wxcriticalsectionCPUctlAccess.Leave() ;
  } // if( mp_i_cpucontroller )
  else
  {
    if(
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
       //::wxGetApp().m_ipcclient.IsConnected()
        mp_wxx86infoandcontrolapp->IPC_ClientIsConnected()
       ||
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
      mp_wxx86infoandcontrolapp->mp_cpucoreusagegetter
      )
      //Necessary for OnPaint() ;
      RecreateDisplayBuffers() ;
  }
  LOGN(//"RedrawEverything #"
    "end")
}

//Also called when CPU controller was changed.
void MainFrame::SetCPUcontroller(I_CPUcontroller * p_cpucontroller )
{
  LOGN(//"SetCPUcontroller "
    "begin")
  mp_i_cpucontroller = p_cpucontroller ;
  m_wMaxFreqInMHzTextWidth = 0 ;
  m_wMaxVoltageInVoltTextWidth = 0 ;
  m_wMaxTemperatureTextWidth = 0 ;
}

void MainFrame::ShowCPUcoreUsagesInTaskBar(
  I_CPUcontroller * p_i_cpucontroller)
{
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  if( mp_model->m_userinterfaceattributes.m_bShowCPUcoreUsagesIconInTaskBar )
  {
    LOGN_DEBUG("mp_taskbaricon:"
      << mp_wxx86infoandcontrolapp->m_p_CPUcoreUsagesTaskbarIcon )
    if( mp_wxx86infoandcontrolapp->m_p_CPUcoreUsagesTaskbarIcon )
    { //TODO make wxIcon (and other params) member variables of class TaskBarIcon
      //or wxicondrawer
      mp_wxx86infoandcontrolapp->m_p_CPUcoreUsagesTaskbarIcon->
        m_p_wxicon_drawer->DrawColouredBarsIcon(
        s_wxiconCPUcoreUsages,
        mp_cpucoredata->m_arfCPUcoreLoadInPercent,
        mp_cpucoredata->m_byNumberOfCPUCores
        );
      if( mp_wxx86infoandcontrolapp->m_p_CPUcoreUsagesTaskbarIcon->SetIcon(
          s_wxiconCPUcoreUsages, wxT("x86IandC--CPU cores usages") )
        )
      {
#ifdef _DEBUG
        wxBitmap & wxbmp //(s_wxiconCPUcoreUsages);
          = * mp_wxx86infoandcontrolapp->m_p_CPUcoreUsagesTaskbarIcon->
          m_p_wxicon_drawer->m_p_wxbitmapToDrawOn;
        wxbmp.SaveFile(
          // const wxString& name
          wxT("CPUcoreUsages.bmp")
          //wxBitmapType type
          , //wxBITMAP_TYPE_XPM  did work on Windows
          wxBITMAP_TYPE_BMP
          );
#endif
//        if( ! mp_wxmenuFile->IsEnabled() )
          //The menu item may be disabled if setting the icon failed for the
          //1st time (if started via the service on logon and the the task bar
          //was not ready).

        //TODO wx assert here if no menu item with ID "ID_MinimizeToSystemTray"
        //was added.
//          mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, true);
      }
      else
      {
        //::wxMessageBox( wxT("Could not set task bar icon."),
        //  getwxString(mp_wxx86infoandcontrolapp->m_stdtstrProgramName) ) ;
//        LOGN("Could not set task bar icon.")
        mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, false);
      }
    }
  }
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
}

void MainFrame::ShowCPUcoresMultipliersInTaskBar(
  I_CPUcontroller * p_i_cpucontroller)
{
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  if( mp_model->m_userinterfaceattributes.
    m_bShowCPUcoresMultipliersIconInTaskBar )
  {
    LOGN_DEBUG("mp_taskbaricon:" << mp_wxx86infoandcontrolapp->
      m_p_CPUcoresMultipliersTaskbarIcon )
    if( mp_wxx86infoandcontrolapp->m_p_CPUcoresMultipliersTaskbarIcon )
    {
      BYTE byNumberOfCPUcores = mp_cpucoredata->m_byNumberOfCPUCores;
      float * CPUcoreMultipliersInPercentOfMinAndMax = new float [
        byNumberOfCPUcores];

      LOGN( "Number Of CPU cores:" <<
         (WORD) byNumberOfCPUcores
        << "CPUcoreMultipliersInPercentOfMinAndMax:" <<
        CPUcoreMultipliersInPercentOfMinAndMax)

      if( CPUcoreMultipliersInPercentOfMinAndMax )
      {
        float fMaximumCPUcoreMultiplier = mp_cpucoredata->GetMaximumMultiplier();
        float fMinimumCPUcoreMultiplier = mp_cpucoredata->GetMinimumMultiplier();

        float fMaxMultiMinusMinMulti = fMaximumCPUcoreMultiplier -
          fMinimumCPUcoreMultiplier;
//        float fVoltageInVolt;
        float fMultiplier;
//        float fReferenceClockInMHz;
//        WORD wCPUcoreID;
        float currentMultiMinusMinMulti;
        float CPUcoreMultiplierInPercentOfMinAndMax;

        for( WORD wCPUcoreID = 0; wCPUcoreID < byNumberOfCPUcores; ++ wCPUcoreID)
        {
          fMultiplier = mp_cpucoredata->m_arp_percpucoreattributes[
            wCPUcoreID].m_fMultiplier;

          //GetCurrentVoltageAndFrequency(...) should have been called right before.
//          if( p_i_cpucontroller->GetCurrentVoltageAndFrequency(
//              fVoltageInVolt
//              , fMultiplier
//              , fReferenceClockInMHz
//              , wCPUcoreID
//              )
//            )
          {
            //possible_multis: [min_multi ... max_multi].
            //current_multi is_element_of possible_multis.
            //  max_minus_min_multi = max_multi - min_multi = 1.0 = 100%
            // current_multi_minus_min_multi = current_multi - min_multi.
            currentMultiMinusMinMulti = fMultiplier -
              fMinimumCPUcoreMultiplier;
            // current_multi in percent of possible_multis =
            //  current_multi_minus_min_multi / max_minus_min_multi
            CPUcoreMultiplierInPercentOfMinAndMax =
//              fMinimumCPUcoreMultiplier / currentMultiMinusMinMulti;
              currentMultiMinusMinMulti / fMaxMultiMinusMinMulti;

            if( currentMultiMinusMinMulti == 0.0f) // x / 0 = infinite
              CPUcoreMultipliersInPercentOfMinAndMax[wCPUcoreID] = 0;
            else
              CPUcoreMultipliersInPercentOfMinAndMax[wCPUcoreID] =
                CPUcoreMultiplierInPercentOfMinAndMax;
            LOGN( "multiplier for CPU core # "
              << (WORD) wCPUcoreID << " in percent in range of "
              "min and max=" << fMaxMultiMinusMinMulti << " / (" <<
              fMultiplier << " - " << fMinimumCPUcoreMultiplier << " ) = "
              << currentMultiMinusMinMulti << " / " <<
              fMaxMultiMinusMinMulti << " = "
              << CPUcoreMultiplierInPercentOfMinAndMax)
          }
        }
        mp_wxx86infoandcontrolapp->m_p_CPUcoresMultipliersTaskbarIcon->
          m_p_wxicon_drawer->DrawColouredBarsIcon(
          s_wxiconCPUcoresMultipliers,
          CPUcoreMultipliersInPercentOfMinAndMax,
          mp_cpucoredata->m_byNumberOfCPUCores
          );
        delete [] CPUcoreMultipliersInPercentOfMinAndMax;
      }
      if( mp_wxx86infoandcontrolapp->m_p_CPUcoresMultipliersTaskbarIcon->
          SetIcon(s_wxiconCPUcoresMultipliers,
            wxT("x86IandC--CPU cores multipliers in % of max - min multiplier") )
        )
      {
#ifdef _DEBUG
        wxBitmap & wxbmp //(s_wxiconCPUcoreUsages);
          = * mp_wxx86infoandcontrolapp->m_p_CPUcoresMultipliersTaskbarIcon->
          m_p_wxicon_drawer->m_p_wxbitmapToDrawOn;
        wxbmp.SaveFile(
          // const wxString& name
          wxT("CPUcoreMultipliers.bmp")
          //wxBitmapType type
          , //wxBITMAP_TYPE_XPM  did work on Windows
          wxBITMAP_TYPE_BMP
          );
#endif
  //        if( ! mp_wxmenuFile->IsEnabled() )
          //The menu item may be disabled if setting the icon failed for the
          //1st time (if started via the service on logon and the the task bar
          //was not ready).

        //TODO wxWidgets debug alert ("no such ID "ID_MinimizeToSystemTray"??)
        //here?!
//          mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, true);
      }
      else
      {
        //::wxMessageBox( wxT("Could not set task bar icon."),
        //  getwxString(mp_wxx86infoandcontrolapp->m_stdtstrProgramName) ) ;
  //        LOGN("Could not set task bar icon.")
        mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, false);
      }
    }
  }
  #endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
}

void MainFrame::ShowHighestCPUcoreTemperatureInTaskBar(
  I_CPUcontroller * p_i_cpucontroller)
{
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  LOGN_DEBUG("mp_taskbaricon:"
    << mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon)
  if( mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon )
  {
    static wxLongLong_t wxlonglong_tLocalTimeMillis;
    wxlonglong_tLocalTimeMillis = ::wxGetLocalTimeMillis().
      GetValue();
    static long long llDiffInMillis;
    llDiffInMillis = wxlonglong_tLocalTimeMillis -
//      mp_cpucoredata->m_llLastTimeTooHot;
      p_i_cpucontroller->m_llLastTimeTooHot;
    static std::basic_string<LOGGING_CHARACTER_TYPE> str;
    MAKE_STRING_FROM_STRING_STREAM( str,
      "diff between current time and last time too hot="
      << wxlonglong_tLocalTimeMillis << "-"
      <<  p_i_cpucontroller->m_llLastTimeTooHot << "="
      << llDiffInMillis
      )
//    g_logger.Log_inline( //FULL_FUNC_NAME,
//      str);
    LOGN_DEBUG( str)

    //Adapted from http://www.cppreference.com/wiki/valarray/max:
    std::valarray<float> stdvalarray_float(s_arfTemperatureInDegreesCelsius,
      mp_cpucoredata->m_byNumberOfCPUCores);
    float fHighestTemperature = stdvalarray_float.max() ;

    mp_wxx86infoandcontrolapp->GetTemperatureString(fHighestTemperature,
      s_wxstrHighestCPUcoreTemperative);

    //TODO
//    const wxFont & wxfontBefore = r_wxdc.GetFont();
////     int nFontPointSize = wxfont.GetPointSize();
//    if( mp_model->m_userinterfaceattributes.m_nCurrentCPUcoreInfoSizeInPoint)
//    {
//      wxFont wxfont2(wxfont);
//      wxfont2.SetPointSize(mp_model->m_userinterfaceattributes.
//        m_nCurrentCPUcoreInfoSizeInPoint);
//      mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->
//        m_wxdc.SetFont(wxfont2);
//    }
    if( llDiffInMillis < 5000 )
      mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->//m_wxicon_drawer.DrawText(
        DrawText(
        s_wxiconTemperature,
        s_wxstrHighestCPUcoreTemperative,
        wxRED//,
        //wxWHITE
        );
    else
//      CreateTextIcon( s_wxiconTemperature, s_wxstrHighestCPUcoreTemperative ) ;
      mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->//m_wxicon_drawer.DrawText(
        DrawText(
        s_wxiconTemperature,
        s_wxstrHighestCPUcoreTemperative,
        wxBLACK//,
//        wxWHITE
        );
    //TODO runtime error: "../src/gtk/bitmap.cpp(1328): assert "IsOk()" failed 
    //   in wxBitmap::GetPixbuf(): invalid bitmap"
    if( mp_wxx86infoandcontrolapp->m_p_HighestCPUcoreTemperatureTaskBarIcon->SetIcon(
        s_wxiconTemperature, s_wxstrTaskBarIconToolTip )
      )
    {
//        if( ! mp_wxmenuFile->IsEnabled() )
        //The menu item may be disabled if setting the icon failed for the
        //1st time (if started via the service on logon and the the task bar
        //was not ready).

      //TODO wxWidgets debug alert ("no such ID "ID_MinimizeToSystemTray"??)
      //here?!
//          mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, true);
    }
    else
    {
      //::wxMessageBox( wxT("Could not set task bar icon."),
      //  getwxString(mp_wxx86infoandcontrolapp->m_stdtstrProgramName) ) ;
//        LOGN("Could not set task bar icon.")
      mp_wxmenuFile->Enable(ID_MinimizeToSystemTray, false);
    }
  }
  LOGN("end")
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
}

//void MainFrame::UpdatePowerSettings(
//  wxPowerType powerType,
//  wxBatteryState batteryState
//  )
//{
//  LOGN("UpdatePowerSettings")
//    wxString powerStr;
//    switch ( m_powerType = powerType )
//    {
//        case wxPOWER_SOCKET:
//            powerStr = _T("wall");
//            break;
//        case wxPOWER_BATTERY:
//            powerStr = _T("battery");
//            break;
//        default:
//            wxFAIL_MSG(_T("unknown wxPowerType value"));
//            // fall through
//        case wxPOWER_UNKNOWN:
//            powerStr = _T("psychic");
//            break;
//    }
//
//    wxString batteryStr;
//    switch ( m_batteryState = batteryState )
//    {
//        case wxBATTERY_NORMAL_STATE:
//            batteryStr = _T("charged");
//            break;
//        case wxBATTERY_LOW_STATE:
//            batteryStr = _T("low");
//            break;
//        case wxBATTERY_CRITICAL_STATE:
//            batteryStr = _T("critical");
//            break;
//        case wxBATTERY_SHUTDOWN_STATE:
//            batteryStr = _T("empty");
//            break;
//        default:
//            wxFAIL_MSG(_T("unknown wxBatteryState value"));
//            // fall through
//        case wxBATTERY_UNKNOWN_STATE:
//            batteryStr = _T("unknown");
//            break;
//    }
//    SetStatusText( wxString::Format (
//      _T("System is on %s power, battery state is %s"),
//      powerStr.c_str(),
//      batteryStr.c_str()
//        )
//      );
//}
