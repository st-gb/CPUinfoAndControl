/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //Include guard
#ifndef CPUCONTROLSERVICE_HPP_ //Include guard
#define CPUCONTROLSERVICE_HPP_

//#include <Controller/CPUcontrolBase.hpp> //Base class CPUcontrolBase
//Base class CPUcontrolServiceBase
#include <Controller/CPUcontrolServiceBase.hpp>
#include <Controller/I_ServerProcess.hpp> //Base class I_ServerProcess
//#include <Controller/IPC/I_IPC_DataHandler.hpp>
#include <Controller/character_string/stdtstr.hpp> //std::tstring
//THREAD_FUNCTION_CALLING_CONVENTION
#include <Controller/multithread/thread_function_calling_convention.h>
//#include <ModelData/ModelData.hpp>
//Member variable of class DummyUserInterface
#include <UserInterface/DummyUserInterface.hpp>
//#include "wxWidgets/DynFreqScalingThread.hpp"

#ifdef COMPILE_WITH_CALC_THREAD
  #include <Windows/CalculationThread.hpp>
#endif
///Member variable of class PowerProfDynLinked
#include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
//#include <Windows/DynFreqScalingAccess.hpp>
//Member variable of class Windows_API::Thread
#include <Windows/multithread/Thread.hpp>
#ifdef COMPILE_WITH_IPC
  //Member variable of class NamedPipeServer
  #include <Windows/NamedPipe/NamedPipeServer.hpp>
#endif //#ifdef COMPILE_WITH_IPC
#include <Windows/Service/ServiceBase.hpp> //Base class ServiceBase
//#include <Windows/WinRing0dynLinked.hpp>
//#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
//class wxServiceSocketServer
#include <wxWidgets/Controller/non-blocking_socket/server/\
wxServiceSocketServer.hpp>
////Also include the cpp file, else "used but never defined warning"
//#include <wxWidgets/multithread/wxConditionBasedI_Condition.cpp>
//Member variable of class wxConditionBasedI_Condition
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>
//#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>
//Member variable of class Xerces::IPCdataHandler
#include <Xerces/service/IPCdataGenerator.hpp>

//#include <fstream> //for class std::ofstream
#include <vector> //class std::vector
#include <windows.h> //for SERVICE_TABLE_ENTRY, ...
#include <wx/app.h> //for class wxApp

//#define _WIN32_WINNT 0x0400 //for MB_SERVICE_NOTIFICATION
//#include <winuser.h> //MB_SERVICE_NOTIFICATION
#ifndef _WINUSER_H
  #define MB_SERVICE_NOTIFICATION 0x00200000L
#endif

typedef struct tagWTSSESSION_NOTIFICATION {
  DWORD cbSize;
  DWORD dwSessionId;
} WTSSESSION_NOTIFICATION, *PWTSSESSION_NOTIFICATION;

//Forward declarations (faster than #include)
class I_DynFreqScalingAccess ;
class WinRing0DynLinked ;
class wxEventLoopBase;
//namespace Xerces
//{
//  class IPCdataHandler ;
//}

//#include <wx/msw/winundef.h>
//To avoid replacing "StartService" to "StartServiceA" / "StartServiceW"
#ifdef StartService
  #undef StartService
#endif

class CPUcontrolService
  :
#ifdef COMPILE_WITH_IPC
    public I_ServerProcess
  ,
#endif //#ifdef COMPILE_WITH_IPC
//  public CPUcontrolBase
  public CPUcontrolServiceBase
  , public ServiceBase //for register service ctrl manager ease etc.
  //For receiving events for non-blocking sockets.
  , public wxApp
{
public:
  static std::wstring s_std_wstrProgramName;
  DWORD m_dwArgCount ;
private :
  bool m_bStartedAsService;
  //Flag for ServiceCtrlHandlerEx to indicate whether the service is REALLY
  //running (the service report SERVICE_RUNNUNG immediately because the
  //Service Control Manager is blocked for other services until the service
  //reports SERVICE_RUNNUNG)
  bool m_vbServiceInitialized ;
//  bool m_bSyncGUIshowDataWithService ;
#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  HANDLE m_handleMapFile;
#endif //#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  //LPSTR m_archServiceName ;
public:
  //For generating XML data from the CPU core voltage, multiplier, usage etc.
  Xerces::IPCdataHandler m_ipc_datahandler ;
private:
  std::string m_stdstrServiceName ;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
//  std::wstring m_stdwstrProgramName ;
#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  LPVOID mp_voidMappedViewStartingAddress ;
#endif
  //this might be changed to a pointer later
//  I_IPC_DataHandler m_ipc_datahandler ;
public:
  //static
    volatile bool m_bProcess ;
  //static
    volatile bool m_bRun ;
  //static
#ifdef COMPILE_WITH_CALC_THREAD
  CalculationThread m_calculationthread ;
#endif //COMPILE_WITH_CALC_THREADs
  //Use non-objects rather than MFC objects because I don't want to be
  //dependent on MFC (porting is easier ).
  HANDLE m_hEndProcessingEvent ;
  //This method (C++ class for a service) is from:
  //http://msdn.microsoft.com/en-us/library/ms810429.aspx
  static
    CPUcontrolService * s_p_cpucontrolservice; // nasty hack to get object ptr
  static void SetToWantedVoltages()
  {
    LOGN(FULL_FUNC_NAME << "--begin")
    //wxCriticalSectionLocker
    s_p_cpucontrolservice->m_model.m_cpucoredata.
      m_mutexDVFSthreadMayChangeData.Lock();
    LOGN(FULL_FUNC_NAME << "--after locking the mutex")
    s_p_cpucontrolservice->mp_cpucontroller->
      m_p_std_set_voltageandfreqUseForDVFS = & s_p_cpucontrolservice->m_model.
      m_cpucoredata.m_stdsetvoltageandfreqWanted;
    s_p_cpucontrolservice->m_model.m_cpucoredata.
      m_mutexDVFSthreadMayChangeData.Unlock();
    LOGN(FULL_FUNC_NAME << "--end")
  }
  static void SetToMaximumVoltages()
  {
    LOGN(FULL_FUNC_NAME << "--begin")
    //wxCriticalSectionLocker
    //Sync access to the variable with the DVFS thread.
    s_p_cpucontrolservice->m_model.m_cpucoredata.
      m_mutexDVFSthreadMayChangeData.Lock();
    LOGN(FULL_FUNC_NAME << "--after locking the mutex")
    s_p_cpucontrolservice->mp_cpucontroller->
      m_p_std_set_voltageandfreqUseForDVFS = & s_p_cpucontrolservice->m_model.
      m_cpucoredata.m_stdsetvoltageandfreqDefault;
    s_p_cpucontrolservice->m_model.m_cpucoredata.
      m_mutexDVFSthreadMayChangeData.Unlock();
    LOGN(FULL_FUNC_NAME << "--end")
  }
  //static
//       SERVICE_STATUS_HANDLE   m_service_status_handle;
//     Model * mp_modelData ;
  //static
   //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
//   IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
#ifdef COMPILE_WITH_IPC
  //For sending data to clients like a graphical user interface.
  NamedPipeServer m_ipcserver ;
  wxServiceSocketServer m_wxservicesocketserver ;
#endif //#ifdef COMPILE_WITH_IPC
  PTSTR * m_p_ptstrArgument ;
  //For (possibly) disabling Windows' dynamic voltage and frequency scaling.
  PowerProfDynLinked m_powerprofdynlinked ;
  /*DynFreqScalingThread * mp_dynfreqscalingthread ;*/
  //static
  //WinRing0_1_3RunTimeDynLinked m_winring0dynlinked ;
  //WinRing0_1_3RunTimeDynLinked * mp_winring0dynlinked ;
  static std::vector<std::string> m_vecstrCmdLineOptions ;
  std::vector<std::string> m_vecstdstrCommandLineArgs ;
//    ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  wxConditionBasedI_Condition
    m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree ;
  wxConditionBasedI_Condition
    m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished ;
//    x86IandC::thread_type m_x86iandc_threadGetCurrentCPUcoreData ;
  Windows_API::Thread m_x86iandc_threadGetCurrentCPUcoreData ;
  volatile bool m_vbAlterCPUcoreDataForIPC ;
public :
  CPUcontrolService();
  CPUcontrolService(//const char* szServiceName
    std::wstring & r_stdwstrProgramName
//      std::tstring & r_stdtstrProgramName
//      , I_IPC_DataHandler & r_ipc_datahandler
    ) ;
  CPUcontrolService(
    DWORD argc,
    LPTSTR *argv ,
    std::wstring & r_stdwstrProgramName
//      std::tstring & r_stdtstrProgramName
//      , I_IPC_DataHandler & r_ipc_datahandler
    ) ;
  //"virtual" too avoid g++ warning: "warning: `class PowerProfDynLinked' has
  //  virtual functions but non-virtual destructor"
  virtual ~CPUcontrolService() ;

  bool Continue() ;
  void CreateHardwareAccessObject() ;
  static void CreateService( const TCHAR * const cpc_tchServiceName) ;
  //"static" because: there needn't be an object for putting this info out.
  static void CreateStringVector(
    //char arch[] ,
    std::string stdstrInput ,
    std::vector<std::string> & vecstdstrParams
    ) ;
  static void DeleteService(const TCHAR * cp_tchServiceName
    , const std::tstring & cr_std_tstrProgramName ) ;
  void DisableOtherVoltageOrFrequencyChange() ;
  void EndAlterCurrentCPUcoreIPCdata() ;
  static void FillCmdLineOptionsList() ;
  inline BYTE * GetCurrentCPUcoreData_Inline(DWORD & dwByteSize);
  inline BYTE * GetIPCdataThreadSafe(DWORD & r_dwByteSize) ;
  std::string GetLogFilePath() ;
  std::string GetValueIfHasPrefix(
    const std::string & r_stdstrPrefix ) ;
  inline void HandleLogonEvent(
    PWTSSESSION_NOTIFICATION pwtssession_notification) ;
  inline void HandlePowerEvent(DWORD dwEventType ) ;
  inline void HandleInitServiceFailed( DWORD dwStatus) ;
  void InitializeMemberVariables() ;
#ifdef COMPILE_WITH_IPC
  DWORD IPC_Message(
//    BYTE byCommand
////      //wide string because the string may be a chinese string for a power
////      scheme
////      , std::wstring & stdwstrMessage
//    , BYTE * & r_arbyPipeDataToSend
    IPC_data & r_ipc_data
    ) ;
#endif //#ifdef COMPILE_WITH_IPC
  ////static
  //    bool HasLogFilePathOption( int argc, char *  argv[] ) ;
  static void outputUsage();
  //Overrides wxAppConsole::OnEventLoopEnter
  void OnEventLoopEnter(wxEventLoopBase *   loop);
  //Overwrites wxApp::OnInit() ;
  bool OnInit();
  void OnServerEvent(wxSocketEvent & event);
  void OnSocketEvent(wxSocketEvent & event);

  bool Pause() ;
  SERVICE_STATUS_HANDLE RegSvcCtrlHandlerAndHandleError() ;
  static int requestOption(
    //Make as parameter as reference: more resource-saving than
    //to return (=a copy).
    std::vector<std::string> & vecstdstrParams
    , const std::tstring & r_tstrProgName ) ;
  static bool IsWithinStrings(
    const std::vector<std::string> & vecstdstrParams
    , const std::string & cr_stdstrToCompare ) ;
  //MUST be static (=without "this" pointer ) because this is a
  //callback function from Windows API.
  static
    DWORD WINAPI ServiceCtrlHandlerEx (
      DWORD dwControl,
      DWORD dwEventType,
      LPVOID lpEventData,
      LPVOID lpContext
      ) ;
  void SetCommandLineArgs( int argc, char *  argv[] ) ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew ) ;
  inline void SetInitHardwareAccessWaitHint() ;
  void SetServiceStatus () ;
  static bool ShouldCreateService(
    const std::vector<std::string> & cr_vecstdstrParams ) ;
  static bool ShouldDeleteService(
    const std::vector<std::string> & cr_vecstdstrParams );
  static bool ShouldStartService(
    const std::vector<std::string> & cr_vecstdstrParams );
  static bool ShouldStartAsNormalApp(
    const std::vector<std::string> & cr_vecstdstrParams );
  static bool ShouldStopService(
    const std::vector<std::string> & cr_vecstdstrParams );
  static VOID SvcDebugOut(LPSTR String, DWORD Status);
  inline void ShowMessage( const std::tstring & cr_std_tstrMessage ) ;
  void Stop() ;
  //Must be "static" because no "this" (would be class-specific) pointer is
  //allowed(?).
  //Function signature is defined here:
  // http://msdn.microsoft.com/en-us/library/ms685138%28VS.85%29.aspx
  static void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) ;
  static void StartIPCserverThread() ;
  static void StartwxSocketServerThread() ;
  void StartAsNonService();
  void StartService() ;
  void StartServiceCtrlDispatcherInSeparateThread();
  static DWORD THREAD_FUNCTION_CALLING_CONVENTION
    StartServiceCtrlDispatcher_static(void * p_v);
  inline void WakeUpCreateIPCdataThread() ;

//  //For receiving wxWidgets (socket) events.
  DECLARE_EVENT_TABLE()
};

//
//DECLARE_APP(CPUcontrolService)

#endif //#ifndef CPUCONTROLSERVICE_HPP_
