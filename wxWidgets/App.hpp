/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define _COMPILE_WITH_CALC_THREAD
#endif

#include <wx/app.h> //class wxApp

#include <Controller/CPUcontrolBase.hpp> //base class CPUcontrolBase
#include <Controller/IPC/I_IPC.hpp> //class IPC_Client
//#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
//#include <Controller/IDynFreqScalingAccess.hpp>
//#include <Controller/MSVC_adaption/tchar.h> //for TCHAR
#include <Controller/character_string/stdtstr.hpp> //std::tstring
//for CPU_TEMP_IS_BELOW_CRITICAL, CPU_TEMP_IS_CRITICAL
#include <Controller/CPU-related/CPU_core_temperature_defs.h>
//#include <Controller/multithread/native_API_event_type.hpp>
#include <Controller/multithread/condition_type.hpp>
#include <Controller/multithread/thread_type.hpp>
#include <Controller/IPC/IPC_data.hpp> //class IPC_data
//#include <Controller/MainController.hpp> //class MainController
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/UserInterface.hpp>//base class UserInterface
//#include <Windows/DynFreqScalingAccess.hpp>
//#include <Windows/PowerProfDynLinked.hpp>
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  #include <Windows/NamedPipe/NamedPipeClient.hpp> //class NamedPipeClient
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  //class SAX2IPCcurrentCPUdataHandler
  #include <Xerces/UserInterface/SAX2IPCcurrentCPUdataHandler.hpp>
#endif
//class Xerces::VoltageForFrequencyConfiguration
#include <Xerces/PStateConfig.hpp>
//for x86IandC::thread_type
//#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>

////Under Linux an error message (~"no image handler for type 3 defined") is shown
//#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #define COMPILE_WITH_SYSTEM_TRAY_ICON
//#endif

//#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  #include <wxWidgets/UserInterface/TaskBarIcon.hpp>
//#endif

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include "SystemTrayAccess.hpp"
#endif
//#include <wxWidgets/UserInterface/Mainframe.hpp>
#ifdef _MSC_VER //MicroSoft compiler
  #pragma  message( "including tchar.h" )
#endif
//Add "Controller/MSVC_adaption/" to the include dir under non-MSVC
#include <tchar.h> //for TCHAR
#ifdef _COMPILE_WITH_CALC_THREAD
  #include <Windows/CalculationThread.hpp>
#endif //#ifdef _COMPILE_WITH_CALC_THREAD
//#include <Windows/NamedPipeClient.hpp>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class FreqAndVoltageSettingDlg;
//class Windows_API::DynFreqScalingAccess ;
//class Model ;
class MainFrame ;
class TaskBarIcon ;
class UserInterface ;
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
class WinRing0_1_3RunTimeDynLinked ;
#else
  #include <Linux/MSRdeviceFile.hpp>
#endif
#ifdef COMPILE_WITH_CALC_THREAD
class CalculationThread ;
#endif //#ifdef COMPILE_WITH_CALC_THREAD
class DynFreqScalingThread ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;

//To avoid replacing "StartService" to "StartServiceA" / "StartServiceW" in
//"winsvc.h"
#ifdef StartService
  #undef StartService
#endif

#include <InstableCPUcoreVoltageDefintions.h>

DECLARE_LOCAL_EVENT_TYPE( wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE, wxNewEventType() )

enum FindInstableCPUcoreOpRetCodes
{
  settingVoltageFailed

};

class wxX86InfoAndControlApp
  : public wxApp
  , public UserInterface
  //In order for the functions that are exported by this exe file to have 
  //the CPU access.
  , public CPUcontrolBase
{
private:
  TCHAR ** m_arartchCmdLineArgument ;
  fastestUnsignedDataType m_GUIthreadID;
//  wxThread m_wxthreadIPC ;
  void * m_systemtray_icon_notification_window ;
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  SystemTrayAccess m_systemtrayaccess ;
public:
  wxString m_wxstrDirectoryForLastSelectedInstableCPUcoreVoltageDynLib;

  std::vector<std::wstring> m_std_vec_std_wstrPowerSchemeName;
  uint16_t m_ui16ActivePowerSchemeIndex;
#endif
private:
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
  //PowerProfDynLinked m_powerprofdynlinked ;
//#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //I_CPUcontroller * mp_cpucontroller ;
  //e.g. point to console or GUI.
public:
  MainFrame * mp_frame ;
private:
//  UserInterface * mp_userinterface ;
  #ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  WinRing0_1_3RunTimeDynLinked * mp_winring0dynlinked ;
public:
private:
  #else
    //MSRdeviceFile m_MSRdeviceFile ;
  #endif
  ////This member needs to be created on runtime because it may throw
  ////an exception (that should be caught, else runtime error) when it is created.
  //I_CPUaccess * mp_i_cpuaccess ;
  Model * mp_modelData ;
  nativeCriticalSection_type m_critSecShowMessage;
public:
  IPC_data s_ipc_data;
  wxString m_wxstrCPUcontrollerDynLibFilePath ;
private:
#ifdef COMPILE_WITH_SHARED_MEMORY
  HANDLE m_handleMapFile;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
  LPVOID mp_voidMappedViewStartingAddress ;
#else
  //Model m_modelData ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY

  bool AbsoluteCPUspecificDirPathExists(
    wxString & wxstrAbsoluteCPUspecificDirPath);
  inline void CreateAndShowMainFrame() ;
  wxString GetAbsoluteCPUspecificDirPath(
    const std::string & cr_strCPUtypeRelativeDirPath,
    wxString & r_wxstrCurrentWorkingDir );
  void InitSharedMemory() ;
  //WinRing0_1_3RunTimeDynLinked m_winring0dynlinked ;

  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //If there are member objects they are destroyed from MyApp destructor. 
  //But if the MyApp object is deleted after wxWidgets structures that 
  //this member object depends on have been uninitialized: 
  //The solution is to destroy the member object in OnExit.
  virtual int OnExit();
  virtual bool OnInit();
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) ;
  #ifdef _COMPILE_WITH_CALC_THREAD
  //Windows_API::
    CalculationThread m_calculationthread ;
  #endif //#ifdef _COMPILE_WITH_CALC_THREAD
public:
//  Model m_model ;
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  SAX2IPCcurrentCPUdataHandler m_sax2_ipc_current_cpu_data_handler ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  //Must be created on heap, else left mouse clicks were not processed?
  TaskBarIcon * m_p_HighestCPUcoreTemperatureTaskBarIcon ;
  TaskBarIcon * m_p_CPUcoreUsagesTaskbarIcon ;
  TaskBarIcon * m_p_CPUcoresMultipliersTaskbarIcon;
#endif //#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
//  TaskBarIcon m_taskbaricon ;
  #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    //NamedPipeClient m_ipcclient ;
    //Must be a pointer because it is/ should be determined/ changeable at
    //_runtime_ .
    IPC_Client * m_p_i_ipcclient ;
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
//  MainController m_maincontroller ;
  //"volatile" because it is accessed from more than 1 thread.
  volatile bool m_vbRetrieveCPUcoreData ;
  volatile bool m_vbGotCPUcoreData ;
  std::tstring m_stdtstrProgramName ;
  wxCriticalSection m_wxcriticalsectionIPCthread ;
  wxCriticalSection m_wxcriticalsectionIPCobject ;
  wxMutex m_wxmutexIPCthread ;
  wxString m_wxstrDataProviderURL ;
//  wxCondition m_wxconditionIPCthread ;
//  x86IandC::native_API_event_type
  condition_type m_condition_type_eventIPCthread ;
  x86IandC::thread_type m_x86iandc_threadIPC ;
  FreqAndVoltageSettingDlg * m_p_freqandvoltagesettingdlgInstCPUcoreDetect;
  Xerces::VoltageForFrequencyConfiguration
    m_xerces_voltage_for_frequency_configuration ;
#ifdef COMPILE_WITH_CPU_SCALING
  //wxDynFreqScalingTimer * mp_wxdynfreqscalingtimer ;
  //#include "wxDynLinkedCPUcoreUsageGetter.hpp"
  //wxDynLinkedCPUcoreUsageGetter * mp_wxDynLinkedCPUcoreUsageGetter ;
  //ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  //http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadctor:
  //"Because the detached threads delete themselves, they can only be
  //allocated on the heap."
  DynFreqScalingThread * mp_dynfreqscalingthread ;
  //Windows_API::DynFreqScalingThread m_dynfreqscalingthread ;
  //DynFreqScalingThread m_dynfreqscalingthread ;
#endif //#ifdef COMPILE_WITH_CPU_SCALING
  wxX86InfoAndControlApp() ;
  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //"You should delete all wxWidgets object that you created by 
  //the time OnExit finishes.
  //In particular, do not destroy them from application class' destructor!"
  ~wxX86InfoAndControlApp() ;
  void CheckForChangedVoltageForFrequencyConfiguration();
  bool Confirm(const std::string & str) ;
  bool Confirm(const std::wstring & str) ;
//  bool Confirm(std::ostrstream & r_ostrstream ) ;
  bool Confirm( std::ostringstream & r_stdostringstream ) ;
  bool ContinueServiceViaIPC(wxString & r_wxstrMessage);
  void CPUcontrollerChanged() ;
  void CPUcontrollerDeleted() ;
  void CPUcoreUsageGetterDeleted() ;
  bool ConnectToDataProviderAndShowResult() ;
  bool ConnectIPCclient(
    const wxString & cr_wxstrIPCclientURL
    //, std::string & r_stdstrMessage
    , bool bShowResultViaUserInterface = false
    ) ;
//  //Created an object of subclass of I_CPUacces for CPU and PCIconfig etc.
//  //access .
//  inline void CreateHardwareAccessObject() ;
  void CurrenCPUfreqAndVoltageUpdated() ;
  void CreateLogFileFormatter(const std::string & c_r_std_strFileExt);
  void DeleteCPUcontroller() ;
  void DynVoltnFreqScalingEnabled() ;
  void EndAllAccessToCPUcontroller();
  void EndDVFS() ;
  void EndGetCPUcoreDataViaIPCthread() ;
  void ExitFindLowestStableVoltageThread();
  I_CPUcontroller * GetCPUcontroller()
  {
    return mp_cpucontroller ;
  }
  inline TaskBarIcon * CreateTaskBarIcon(
    TaskBarIcon * & r_p_taskbaricon,
    const char * p_chTaskBarIconName
    );
  inline void DeleteTaskBarIcon(
    TaskBarIcon * & r_p_taskbaricon,
    const char * p_chTaskBarIconName
    );
  void DeleteTaskBarIcons();
  I_CPUaccess * GetCPUaccess()
  {
    return mp_i_cpuaccess ;
  }
  bool GetAvailablePowerSchemesViaIPC(
    std::vector<std::wstring> & r_std_vec_std_wstrPowerSchemeName,
    uint16_t & r_ui16ActivePowerSchemeIndex
    );
  void InitUnstableCPUcoreOperationDetection();
  BYTE InitUnstableVoltageDetectionDynLibAccess();
  bool SetPowerSchemeViaIPC(
    const std::wstring & r_std_wstrPowerSchemeName);
  BYTE GetConfigDataViaInterProcessCommunication() ;
  void GetCurrentCPUcoreDataViaIPCNonBlocking() ;
  void GetCurrentCPUcoreDataViaIPCNonBlockingCreateThread() ;
  void GetTemperatureString(const float fTemperature,
    wxString & r_wxstrTemperative)
  {
    //E.g. the Pentium M has no temperature sensor.
    if( fTemperature >= //0.0f
        O_KELVIN_IN_DEG_CELSIUS )
    {
      r_wxstrTemperative = wxString::Format( //wxT("%u"), (WORD) fTemperature
        wxT("%.3g°C"), fTemperature
        ) ;
    }
    else if( fTemperature == CPU_TEMP_IS_BELOW_CRITICAL )
    {
      r_wxstrTemperative = wxT("<C");
    }
    else if( fTemperature == CPU_TEMP_IS_CRITICAL )
    {
      r_wxstrTemperative = wxT("C");
    }
  }
  bool GetX86IandCiconFromFile(wxIcon & r_wxicon );
  Model * GetModel()
  {
    return mp_modelData ;
  }
  void IPCclientDisconnect() ;
  bool IPCclientConnectToDataProvider( std::string & r_stdstrMessage ) ;
  inline bool IPCclientConnect_Inline(std::string & r_stdstrMessage) ;
  bool IPC_ClientIsConnected() ;
  inline bool IPC_ClientIsConnected_Inline() ;
  inline BYTE IPC_ClientSendCommandAndGetResponse_Inline(
    BYTE byCommand,
    DWORD dwSizeOfDataToSendInBytes =
      //A command usually needs 1 byte
      1,
    BYTE ar_byDataToSend [] = NULL) ;
  BYTE IPC_ClientSendCommandAndGetResponse(
    BYTE byCommand,
    DWORD dwSizeOfDataToSendInBytes =
      //A command usually needs 1 byte
      1,
    BYTE ar_byDataToSend [] = NULL);
  void MessageWithTimeStamp(const std::wstring & cr_stdwstr,
    unsigned flags = 0)
    /** Cannot be "const", else the "wchar_t" version is not called? */
    /*const*/ ;
  void MessageWithTimeStamp(//const LPWSTR
      const wchar_t * cp_lpwstrMessage, unsigned flags = 0);
  void MessageWithTimeStampGUIthreadUnsafe(
    const wchar_t * const c_wch_wstrMessage,
    unsigned flags = 0
    ) const;
  bool OpenLogFile(//std::tstring & r_std_tstrLogFilePath
    std::wstring & r_std_wstrLogFilePath,
    std::wstring & std_wstrExecutableFileName,
    bool bAppendProcessID, bool bRolling);
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  void PauseService(
    wxString & r_wxstrMessageFromService,
    bool bTryToPauseViaServiceControlManagerIfViaIPCfails = true ) ;
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  void PossiblyAskForOSdynFreqScalingDisabling() ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  bool PreventVoltageBelowLowestStableVoltage();
  void RedrawEverything() ;
  void SaveAsCPUcontrollerDynLibForThisCPU();
  void SaveVoltageForFrequencySettings(
      const std::string & cr_std_strCPUtypeRelativeDirPath);
  void SetAsMinimumVoltage(float fVoltage, WORD wFrequencyInMHz);
  void SetAsWantedVoltage(float fVoltage, WORD wFrequencyInMHz);
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller ) ;
  bool ShowTaskBarIcon(MainFrame * p_mf) ;
  inline void ShowTaskBarIconViaWindowsAPI();
  inline bool ShowTaskBarIconUsingwxWidgets();
  void ShowMessageGUIsafe(const wxString &);
  void ShowMessageGUIsafe(const wchar_t *);
  void StabilizeVoltage(
    const float fVoltageInVolt,
    const float fMultiplier,
    const float fReferenceClockInMHz);
  void StabilizeVoltageAndShowVoltageSettingsChanges(
    const float fVoltageInVolt
    ,const float fMultiplier,
    const float fReferenceClockInMHz);
  BYTE StartInstableCPUcoreVoltageDetection(const FreqAndVoltageSettingDlg *);
  void SetStartFindingLowestStableVoltageButton();
  void StopInstableCPUcoreVoltageDetection();
  void StartService() ;
  void StopService() ;
  void LoadDetectInstableCPUcoreVoltageDynLib();
#ifdef _WIN32
  void UpdateInstableCPUcoreOpDetectInfo();
#endif
  void UnloadDetectInstableCPUcoreVoltageDynLib();

public:
  void OnShowMessage(wxCommandEvent &);
  DECLARE_EVENT_TABLE()
};



BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_LOCAL_EVENT_TYPE( wxEVT_COMMAND_SHOW_MESSAGE, wxNewEventType() )
//"EVT_BUTTON" also expands to "DECLARE_EVENT_TYPE"
END_DECLARE_EVENT_TYPES()

DECLARE_APP(wxX86InfoAndControlApp)
