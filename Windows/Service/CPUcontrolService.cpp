/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "CPUcontrolService.hpp"
#include <conio.h> //for getche()
#include <iostream> //for std::cout, std::cin
#include <windows.h> //for SERVICE_TABLE_ENTRY, SERVICE_STATUS_HANDLE, ...
//#include <Controller/DynFreqScalingThreadBase.hpp>
//#include <Controller/CPU-related/I_CPUcontroller.hpp>
//for ICPUcoreUsageGetter::Init()
//#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/IDynFreqScalingAccess.hpp> //class IDynFreqScalingAccess
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
//::format_output_data
#include <Controller/character_string/format_as_string.hpp>
#include <Controller/character_string/stdtstr.hpp> //class std::tstring
#ifdef COMPILE_WITH_IPC
#include <Controller/IPC/I_IPC.hpp> //for enum "IPCcontrolCodes"
#endif //#ifdef COMPILE_WITH_IPC
//class Logger::StdCoutLogWriter
#include <Controller/Logger/OutputHandler/StdCoutLogWriter.hpp>
#include <Windows/Logger/ConsoleFormatter.hpp>
#include <Controller/Logger/Appender/RollingFileOutput.hpp>
//DEBUG(...), DEBUGN(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//template<typename charType> class CommandLineArgs
#include <Process/CommandLineArgs.hpp>
//for LocalLanguageMessageFromErrorCode(...)
#include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
//for int ::CallFromMainFunction(int argc, char * argv[])
#include <Windows/Service/CallFromMainFunction.h>
// for ::SetExePathAsCurrentDir()
#include <Windows/SetExePathAsCurrentDir/SetExePathAsCurrentDir.h>
//class WinRing0_1_3RunTimeDynLinked
#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
//GetStdString(wxString )
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
//class wxThreadBasedI_Thread
#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>
//WaitAsyncAndExec(...)
#include <Windows/multithread/WaitAndExecuteFunction.hpp>
//class XercesConfigurationHandler: for writing into memory buffer for IPC
#include <Xerces/PStateConfig.hpp>
#include <wx/apptrait.h> //class wxAppTraits
//for the #defines like "WTS_SESSION_LOGIN" in <wts32api.h>
//  ( is defined inside the #if (_WIN32_WINNT >= 0x0501)  )
#if _WIN32_WINNT < 0x0501
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
//#ifdef _MSC_VER
#include <wtsapi32.h>
//#else //WTSSendMessage is not available in MinGW's <wtsapi32.h>
//  #include <Windows/WTSSendMessageRunTimeDynLinked.hpp> //WTSSendMessage(...)
//#endif
//#include <windows.h>
#include <AccCtrl.h> //SE_SERVICE
//#define USE_WINDOWS_THREAD

//#include <Windows/GetWindowsVersion.h>
//#include <Windows/PowerProfFromWin6DynLinked.hpp>
//#include <Windows/PowerProfUntilWin6DynLinked.hpp>
//#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>

#include "StartGUI.hpp" //StartGUIprocessDelayedAsync(...)

//#include <wx/msw/winundef.h>
//To avoid replacing "StartService" to "StartServiceA" / "StartServiceW" in
//"winsvc.h"
#ifdef StartService
  #undef StartService
#endif

#define NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE 65535

//The static member must also be DEFINED here additional to the declaration 
//, else linker error LNK2019: Verweis auf nicht aufgelöstes externes Symbol
//""public: static class CPUcontrolService * CPUcontrolService::
// s_p_cpucontrolservice"
CPUcontrolService * CPUcontrolService::s_p_cpucontrolservice;
std::vector<std::string> CPUcontrolService::m_vecstrCmdLineOptions;
//DummyUserInterface CPUcontrolService::m_dummyuserinterface ;

extern CPUcontrolBase * gp_cpucontrolbase;

/** This function should be executed in a separate thread. */
DWORD WINAPI GetCurrentCPUcoreDataInLoopThreadFunc(void * p_v)
{
  LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "begin")
  CPUcontrolService * p_cpucontrolservice = (CPUcontrolService *) p_v;
  if (p_cpucontrolservice)
  {
    DWORD dwByteSize;
    LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "before Lock")
    //Wait until another function calls Leave().
    //    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Enter() ;
    //    DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Leave")
    //    //Let the other thread continue at its "Enter()"
    //    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;

    //    if( p_wxx86infoandcontrolapp->m_wxmutexIPCthread.TryLock() ==
    //        //http://docs.wxwidgets.org/2.6/wx_wxmutex.html#wxmutextrylock:
    //        //"The mutex is already locked by another thread."
    //        wxMUTEX_BUSY )
    //      return 0 ;
    //http://docs.wxwidgets.org/2.6/wx_wxcondition.html#wxconditionwait:
    //"it must be locked prior to calling Wait"
    p_cpucontrolservice->m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree. Lock();
    //    Sleep(4000) ;
    LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "after Lock()")
    //http://docs.wxwidgets.org/stable/wx_wxcondition.html:
    // "Waits until the condition is signaled.
    // This method atomically releases the lock on the mutex associated with
    // this condition (this is why it must be locked prior to calling Wait)
    // and puts the thread to sleep until Signal or Broadcast is called.
    // It then locks the mutex again and returns."
    p_cpucontrolservice->m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree. Wait();
    LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/
        "before the loop condition")
    //ok, at least 1 client thread wants to get CPU core data.
    //    LOGN(//"GetCurrentCPUcoreDataInLoopThreadFunc
    //      "retrieve CPU core data?"
    //      << p_wxx86infoandcontrolapp->m_vbRetrieveCPUcoreData )
    while ( //break condition
        p_cpucontrolservice->m_vbAlterCPUcoreDataForIPC)
    {
      LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/
        "after the loop condition")
      p_cpucontrolservice->m_ipc_datahandler.GetCurrentCPUcoreAttributeValues(
          dwByteSize, true);
      //Wake up all client threads waiting for IPC data.
      p_cpucontrolservice-> m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished. Broadcast();

      //      p_cpucontrolservice->m_vbGotCPUcoreData = true ;
      //      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Enter")
      //      //Wait until another function calls Leave().
      //      p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Enter() ;
      //      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Leave")
      //      //Let the other thread continue at its "Enter()"
      //      p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
      //    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
      //      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before TryLock")
      //      //MainFrame::Onclose() locks the mutex
      //      if( p_wxx86infoandcontrolapp->m_wxmutexIPCthread.TryLock() ==
      //          //http://docs.wxwidgets.org/2.6/wx_wxmutex.html#wxmutextrylock:
      //          //"The mutex is already locked by another thread."
      //          wxMUTEX_BUSY )
      //        return 0 ;

      //      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Lock")
      LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "before Lock" )
      //http://docs.wxwidgets.org/2.6/wx_wxcondition.html#wxconditionwait:
      //"it must be locked prior to calling Wait"
      p_cpucontrolservice-> m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Lock();
      if ( ! p_cpucontrolservice->m_vbAlterCPUcoreDataForIPC)
        break;
      //      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Wait()")
      LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "before Wait()" )

      p_cpucontrolservice-> m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Wait();
      LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "after Wait()")
    }
    LOGN("InterProcessCommunication client thread: ended get CPU core data loop")
  }
  else
    LOGN(/*"GetCurrentCPUcoreDataInLoopThreadFunc "*/ "app pointer == NULL")
  return 0;
}

#ifdef COMPILE_WITH_IPC
DWORD WINAPI
IPC_ServerThread(LPVOID lpParam)
{
  LOGN("IPC_ServerThread begin");
  I_IPC_Server * p_i_ipc_server = (I_IPC_Server *) lpParam;
  if (p_i_ipc_server)
    //while(1)
    //{
    //  Sleep(1000);
    //  MessageBeep( (WORD) -1 ) ;
    //}
    p_i_ipc_server->Init();
  return 0;
}
#endif //#ifdef COMPILE_WITH_IPC

// IDs for the controls and the menu commands
enum
{
  // id for sockets
  SERVER_ID = 100,
  SOCKET_ID
};

#include <wx/socket.h> //EVT_SOCKET

BEGIN_EVENT_TABLE(CPUcontrolService, wxApp)
  EVT_SOCKET(SERVER_ID,
      CPUcontrolService::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  //wxServiceSocketServer::
      CPUcontrolService::OnSocketEvent)
//  EVT_EV
END_EVENT_TABLE()

////Defines "WinMain(...)":
////Erzeugt ein wxAppConsole-Object auf dem Heap.
//IMPLEMENT_APP(CPUcontrolService)

//LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
//LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
std::wstring CPUcontrolService::s_std_wstrProgramName(//ptstrProgramName
  _T("X86_info_and_control") ) ;

CPUcontrolService::CPUcontrolService()
  //C++ style init.
  :
  CPUcontrolServiceBase( & m_dummyuserinterface),
  m_dwArgCount(argc)
#ifdef COMPILE_WITH_IPC
  //  , mr_ipc_datahandler(r_ipc_datahandler)
  ,
  m_ipc_datahandler(m_model, * this),
  m_ipcserver(this),
  m_wxservicesocketserver(this)
#endif //#ifdef COMPILE_WITH_IPC
  //  , m_powerprofdynlinked ( r_stdtstrProgramName )
  //  , m_stdtstrProgramName ( r_stdtstrProgramName )
  //  , m_maincontroller( & m_dummyuserinterface )
  , m_p_ptstrArgument(//argv),
      NULL),
  m_powerprofdynlinked(s_std_wstrProgramName),
  m_x86iandc_threadGetCurrentCPUcoreData(I_Thread::joinable)
{
//  LOGN("CPUcontrolService::CPUcontrolService(argc, argv, ...)")
  LOGN( "begin")
  m_stdtstrProgramName = Getstdtstring(s_std_wstrProgramName);
  //Calling the ctor inside another ctor created the object 2 times!
  //CPUcontrolService() ;
  InitializeMemberVariables();
}

CPUcontrolService::CPUcontrolService(
//const char * szServiceName
    std::wstring & r_stdwstrProgramName
//  std::tstring & r_stdtstrProgramName
//  , I_IPC_DataHandler & r_ipc_datahandler
  )
  //C++ style inits:
  :
  CPUcontrolServiceBase(& m_dummyuserinterface),
  //    m_bSyncGUIshowDataWithService ( false ) ,
#ifdef COMPILE_WITH_IPC
      //    mr_ipc_datahandler(r_ipc_datahandler) ,
      m_ipc_datahandler(m_model, * this), m_ipcserver(this), m_wxservicesocketserver(
          this)
#endif //#ifdef COMPILE_WITH_IPC
      //m_bProcess ( true )
      //, m_bRun ( true )
      //, mp_winring0dynlinked (NULL)
      //, mar_tch(NULL)
      , m_powerprofdynlinked(r_stdwstrProgramName)
      //,
      //    m_powerprofdynlinked ( r_stdtstrProgramName )
      //    , m_stdwstrProgramName ( r_stdwstrProgramName)
      //    , m_stdtstrProgramName ( r_stdtstrProgramName)
      , m_x86iandc_threadGetCurrentCPUcoreData(I_Thread::joinable)
{
  LOGN(/*"CPUcontrolService::CPUcontrolService()"*/ "begin")
  m_stdtstrProgramName = Getstdtstring(r_stdwstrProgramName);
  InitializeMemberVariables();
}

CPUcontrolService::CPUcontrolService(
  DWORD argc,
  LPTSTR * argv,
  std::wstring & r_stdwstrProgramName
//  std::tstring & r_stdtstrProgramName
//  , I_IPC_DataHandler & r_ipc_datahandler
)
  //C++ style init.
  :
    CPUcontrolServiceBase(& m_dummyuserinterface),
      m_cmdlineargs(argc, argv),
      m_dwArgCount(argc)
#ifdef COMPILE_WITH_IPC
      //  , mr_ipc_datahandler(r_ipc_datahandler)
      ,
      m_ipc_datahandler(m_model, * this),
      m_ipcserver(this),
      m_wxservicesocketserver(this)
#endif //#ifdef COMPILE_WITH_IPC
      //  , m_powerprofdynlinked ( r_stdtstrProgramName )
      //  , m_stdtstrProgramName ( r_stdtstrProgramName )
      //  , m_maincontroller( & m_dummyuserinterface )
      , m_p_ptstrArgument(//argv),
          NULL),
      m_powerprofdynlinked(r_stdwstrProgramName),
      m_x86iandc_threadGetCurrentCPUcoreData(I_Thread::joinable)
{
  //Time should be output (dumped) because the log time format may e.g. not the
  //contain year.
  LOGN( "current date/time: " << wxWidgets::GetStdString(::wxNow() ) )
  //  LOGN("CPUcontrolService::CPUcontrolService(argc, argv, ...)")
  LOGN( "begin")
  m_stdtstrProgramName = Getstdtstring(r_stdwstrProgramName);
  //Calling the ctor inside another ctor created the object 2 times!
  //CPUcontrolService() ;
  InitializeMemberVariables();
}

CPUcontrolService::~CPUcontrolService()
{
  LOGN(/*"~CPUcontrolService() "*/ "begin")
  //Already called in ~CPUcontrolBase()
  //  FreeRessources() ;
  ::DeleteTCHARarray(m_dwArgCount, m_p_ptstrArgument);
  EndAlterCurrentCPUcoreIPCdata();
  LOGN(/*"~CPUcontrolService() "*/ "end")
}

/**@ return true: already continued*/
bool
CPUcontrolService::Continue()
{
  bool bAlreadyContinued = false;
   m_bProcess = true;
   m_servicestatus.dwCurrentState = SERVICE_RUNNING;
  //If the service was paused before stand-by/ hibernation and
  //if continued after stand-by/ hibernation
  //then for e.g. AMD Griffins the p-state
  //values have to be restored.
  mp_cpucontroller-> ResumeFromS3orS4();
  //If the service was paused before stand-by/ hibernation and
  //if continued after stand-by/ hibernation
  //then performance event select register values.
  //values have to be restored.
  mp_cpucoreusagegetter->Init();
  bAlreadyContinued = StartDynVoltnFreqScaling();
  SetServiceStatus();
  LOG("Service is continued."//\n"
  )
  return bAlreadyContinued;
}

//void
//CPUcontrolService::CreateHardwareAccessObject()
//{
//  //Use parameterized constructor, because there were runtime errors with
//  //parameterless c'tor.
//  //mp_winring0dynlinked = new //WinRing0dynLinked(
//  mp_i_cpuaccess = new WinRing0_1_3RunTimeDynLinked( & m_dummyuserinterface);
//  LOGN( "end")
//}

void
CPUcontrolService::CreateService(const TCHAR * const cpc_tchServiceName)
{
  LOGN( //"CPUcontrolService::CreateService(" <<
    "begin--service name:" << GetStdString_Inline(cpc_tchServiceName) )
//  try
//  {
    BYTE by;
    DWORD dwLastError = ServiceBase::CreateService(cpc_tchServiceName, by);
    if (dwLastError)
      {
        //DEBUG("Creating the service failed: error number:%d\n",
        // dwLastError);
        //LOG("Creating the service failed: error number:" << dwLastError
        //    << "\n" );
        //std::cout
        /*WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE*/ LOGN_ERROR(
            "Creating the service failed: error number:" <<
            dwLastError << "\nError: " << //"\n"
            LocalLanguageMessageFromErrorCodeA( dwLastError )
        );
        switch (by)
        {
        case ServiceBase::GetModuleFileNameFailed:
          //DEBUG("GetModuleFileName failed (%d)\n", GetLastError());
          //LOG(
          LOGN_ERROR("Getting file name for THIS executable file failed: " <<
              LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n"
          )
          ;
          break;
        }
        std::string stdstrPossibleSolution;
        ServiceBase::GetPossibleSolution(dwLastError, cpc_tchServiceName,
            stdstrPossibleSolution);
        LOGN_INFO(stdstrPossibleSolution)
      }
    else
      //printf("Creating service succeeded\n");
      /*std::cout*/ LOGN_SUCCESS("Creating the service succeeded.");
//    }
//  catch (ConnectToSCMerror & ctscme)
//  {
//    LOGN_ERROR("");
//  }
  LOGN("end")
}

void CPUcontrolService::DeleteService(
  const TCHAR * cp_tchServiceName,
  const std::tstring & cr_std_tstrProgramName)
{
  DWORD dwErrorCodeFor1stError;
  /*BYTE by*/ fastestUnsignedDataType deleteServiceRetCode = ServiceBase::
    DeleteService(//"GriffinStateService"
      cp_tchServiceName, dwErrorCodeFor1stError);
  if (deleteServiceRetCode)
    {
      //    HandleErrorPlace(by, "for deleting the service:") ;
      //    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      //      ServiceBase::GetPossibleSolution(dwErrorCodeFor1stError)
      //      ) ;
      std::string stdstr = "for deleting the service:";
      std::string std_strSpecificErrorMessage;
      switch (deleteServiceRetCode)
        {
      case ServiceBase::OpenSCManagerFailed:
        std_strSpecificErrorMessage = "opening the service control manager "
          "failed";
        break;
      case ServiceBase::OpenServiceFailed:
        {
          std_strSpecificErrorMessage
              = "Opening the service for service name \""
                  + GetStdString_Inline(std::tstring(cp_tchServiceName))
                  + "\" failed.";
        }
        break;
      case ServiceBase::DeleteServiceFailed:
        std_strSpecificErrorMessage = "Deleting service failed:\n";
        break;
        }
      if ( ! std_strSpecificErrorMessage.empty() )
        {
          std::string stdstrPossibleSolution;
          ServiceBase::GetPossibleSolution(dwErrorCodeFor1stError,
              cp_tchServiceName, stdstrPossibleSolution);
//          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
          LOGN_ERROR(
              stdstr
              << std_strSpecificErrorMessage << ":"
              << ::LocalLanguageMessageAndErrorCodeA(dwErrorCodeFor1stError)
              << stdstrPossibleSolution
          );
        }
    }
    else
      /*WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE*/ LOGN_SUCCESS(
      "Deleting the service \"" << GetStdString_Inline(cp_tchServiceName)
      << "\" succeeded.")
  std::wstring std_wstrProgramName = GetStdWstring(cr_std_tstrProgramName);
  PowerProfDynLinked powerprofdynlinked( //stdtstrProgramName
    std_wstrProgramName);
  powerprofdynlinked.DeletePowerScheme(//cr_std_tstrProgramName
    std_wstrProgramName);
  powerprofdynlinked.OutputAllPowerSchemes();
}

void CPUcontrolService::DisableOtherVoltageOrFrequencyChange()
{
  m_powerprofdynlinked.DisableFrequencyScalingByOS();
}

void CPUcontrolService::EndAlterCurrentCPUcoreIPCdata()
{
  LOGN(/*"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "*/ "begin")
  m_vbAlterCPUcoreDataForIPC = false;
  //  LOGN("CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
  //    "before Lock")
  //Lock the mutex so that the thread that called Wait() on the condition
  // receives the wakeup by Signal() (else Signal may be called while the
  // other thread is not Wait() ing and so does not wake up.
  //  m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Lock() ;
  //  LOGN(//"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
  //    "before Signal")
  //  m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Signal() ;
  //  LOGN(//"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
  //    "before WaitForTermination of GetCurrentCPUcoreData")
  //  m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Unlock() ;
  LOGN(/*"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "*/ "before "
    "LockedBroadcast")
  m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.LockedBroadcast();
  LOGN(//"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
    "before waiting for termination of \"get current CPU core data\" thread")
  //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadwait:
  //"you must Wait() for a joinable thread or the system resources used by it
  //will never be freed,
  m_x86iandc_threadGetCurrentCPUcoreData.WaitForTermination();
  LOGN(//"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
    "after waiting for termination of \"get current CPU core data\" thread")
  LOGN(//"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "
    "before Delete")
  m_x86iandc_threadGetCurrentCPUcoreData.Delete();
  LOGN(/*"CPUcontrolService()::EndAlterCurrentCPUcoreIPCdata "*/ "end")
}

void CPUcontrolService::HandleInitServiceFailed(DWORD dwStatus)
{
  // Handle error condition
  if (dwStatus == NO_ERROR)
    {
      LOGN("The service was successfully initialized"//\n"
      )
    }
  else
    {
      s_p_cpucontrolservice-> m_servicestatus.dwCurrentState = SERVICE_STOPPED;
      s_p_cpucontrolservice-> m_servicestatus.dwCheckPoint = 0;
      s_p_cpucontrolservice-> m_servicestatus.dwWaitHint = 0;
      s_p_cpucontrolservice-> m_servicestatus.dwWin32ExitCode = dwStatus;
      //s_p_cpucontrolservice->
      //    m_servicestatus.dwServiceSpecificExitCode = specificError;

#ifndef EMULATE_EXECUTION_AS_SERVICE
    if( m_bStartedAsService )
      ::SetServiceStatus(s_p_cpucontrolservice-> m_service_status_handle,
          &s_p_cpucontrolservice-> m_servicestatus);
#endif //#ifndef EMULATE_EXECUTION_AS_SERVICE
      //DEBUG("error in initialization of the service--returning\n" )
    }
}

void CPUcontrolService::HandleLogonEvent(
  const PWTSSESSION_NOTIFICATION & pwtssession_notification) const
{
  LOGN("unlock or logon event--session ID is:" <<
      pwtssession_notification->dwSessionId )
  //            //If createing a
  //            r_service_attributes = mp_modelData->m_service_attributes ;
  //            if( r_service_attributes.m_bStartGUIonLogon )
  ServiceAttributes & r_service_attributes =
      s_p_cpucontrolservice->m_model.m_serviceattributes;
  LOGWN_WSPRINTF(L"GUI exe to start:%ls",
      r_service_attributes.m_stdwstrPathToGUIexe.c_str() )
  if( ! r_service_attributes.m_stdwstrPathToGUIexe.empty() )
  {
    //    StartGUIprocessDelayedSynchronous(r_security_attributes);
    StartGUIprocessDelayedAsync(
      r_service_attributes,
      pwtssession_notification->dwSessionId);
  }
}

void CPUcontrolService::HandlePowerEvent(DWORD dwEventType)
{
  LOGN("power event--type:" << dwEventType)
  //"If dwControl is SERVICE_CONTROL_POWEREVENT, this parameter can be one of
  //the values specified in the wParam parameter of the WM_POWERBROADCAST
  //message."
  //http://msdn.microsoft.com/en-us/library/aa373247%28v=VS.85%29.aspx:
  switch ( /*(int)*/  dwEventType)
  {
  //case PBT_APMPOWERSTATUSCHANGE:
  //http://msdn.microsoft.com/en-us/library/aa372720%28v=VS.85%29.aspx:
  //"Notifies applications that the system has resumed operation after being
  //suspended."
  //"An application can receive this event only if it received the
  //PBT_APMSUSPEND event before the computer was suspended. Otherwise, the
  //application will receive a PBT_APMRESUMECRITICAL event."
  // "If the system wakes due to user activity (such as pressing the power
  // button) or if the system detects the presence of a user after waking
  // unattended, the system will first broadcast the PBT_APMRESUMEAUTOMATIC
  // event followed by a PBT_APMRESUMESUSPEND event.
  //since Windows 2000
  case PBT_APMRESUMESUSPEND:
    LOGN("system has resumed operation after being suspended")
    //TODO timer that elapses after x seconds and sets back to the wanted voltage.
    //timer timer;
    //::CreateWaitableTimer()
    //OperatingSystem
    Windows_API::WaitAsyncAndExec(
      m_model.m_StableCPUcoreVoltageWaitTimeInMillis,
      SetToWantedVoltages);
    break;
  case PBT_APMRESUMEAUTOMATIC:
    LOGN("the computer has woken up automatically")
    break;
    //"An application can receive this event only if it received the
    //PBT_APMSUSPEND event before the computer was suspended. Otherwise, the
    //application will receive a PBT_APMRESUMECRITICAL event."

    //http://msdn.microsoft.com/en-us/library/aa372719%28VS.85%29.aspx:
    //for Win 2000-Win Server 2003
    //"Notifies applications that the system has resumed operation. This event
    //can indicate that some or all applications did not receive a PBT_APMSUSPEND
    //event. For example, this event can be broadcast after a critical suspension
    //caused by a failing battery.
  case PBT_APMRESUMECRITICAL:
    LOGN("The system has resumed operation")
    // handle event
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/handlerex.htm:
    //"If your service handles SERVICE_CONTROL_POWEREVENT, return NO_ERROR to
    //grant the request and an error code to deny the request."
    //for(unsigned char by = 0 ; by < 255 ; by ++ )
    //{
    //  ::MessageBeep(15000);
    //  ::Sleep(400);
    //}
    if (s_p_cpucontrolservice->m_bProcess)
      s_p_cpucontrolservice->mp_cpucontroller->ResumeFromS3orS4();
    //After ACPI S3/ S4 the performance event select
    //is cleared and has to be written again.
    s_p_cpucontrolservice->mp_cpucoreusagegetter->Init();
    //return NO_ERROR;
    break;
  case PBT_APMSUSPEND:
    LOGN("suspending")
    //Set to higher voltages before suspending so that a OS hangup / restart is
    //less probable.
//    mp_cpucontroller->m_p_std_set_voltageandfreqUseForDVFS =
//      & m_model.m_cpucoredata.m_stdsetvoltageandfreqDefault;
    SetToMaximumVoltages();
    break;
    //http://msdn.microsoft.com/en-us/library/windows/desktop/aa372716%28v=vs.85%29.aspx:
    // only Windows XP, Server 2003
  case PBT_APMQUERYSUSPEND:
    LOGN("querying suspend")
    //TODO Before a hibernation set core 1 to a low freq and voltage
    //to save current because I guess only the core 0 saves the
    //RAM onto disk:
    break;
  default:
    LOGN("other power event")
    break;
    }
}

/** This method should be called by each constructor. */
void CPUcontrolService::InitializeMemberVariables()
{
  LOGN(/*"CPUcontrolServiceBase::InitializeMemberVariables()--"*/ "begin")
  m_bStartedAsService = false;
  gp_cpucontrolbase = this;
  m_vbAlterCPUcoreDataForIPC = true;
  m_vbServiceInitialized = false;
  m_bProcess = true;
  m_bRun = true;
  mp_cpucontroller = NULL;
  //mp_winring0dynlinked = NULL ;
  mp_i_cpuaccess = NULL;
  mar_tch = NULL;
  m_stdstrServiceName = "CPUcontrolService";
  mp_dynfreqscalingthreadbase = NULL;

  DEBUG("begin of constructor of service object\n");
  m_bProcess = true;
  m_bRun = true;
#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  mp_voidMappedViewStartingAddress = NULL;
  m_handleMapFile = NULL;
#endif //#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  //  mp_modelData = NULL ;
  m_stdstrSharedMemoryName = "CPUcontrolService";

  LOGN("creating the \"end service\" event")
  m_hEndProcessingEvent = ::CreateEvent(NULL, // default security attributes
      TRUE, // manual-reset event
      FALSE, // initial state is non-signaled
      _T("EndEvent") // object name
      );
  //This method (Windows service as an class / object and using a non-instance
  //(static) pointer to the object) is from(?):
  // http://msdn.microsoft.com/en-us/library/ms810429.aspx ?
  // now (22.06.2011 here:
  // http://code.msdn.microsoft.com/CppWindowsService-cacf4948/sourcecode?
  //  fileId=21604&pathId=2131191824
  s_p_cpucontrolservice = this;
  //m_winring0dynlinked.SetUserInterface(&m_dummyuserinterface);
  //DEBUG("end of constructor of service object\n");
  LOGN("before starting thread for GetCurrentCPUcoreDataInLoopThreadFunc")
}

void CPUcontrolService::FillCmdLineOptionsList()
{
  if (m_vecstrCmdLineOptions.empty())
    {
      m_vecstrCmdLineOptions.push_back("with no command line options: "
        "open console window requesting input for the desired operation\n"
        "(to install or deinstall/ delete)");
      m_vecstrCmdLineOptions.push_back("-d >>service name<< Deinstall/ Delete "
        "(this) Windows service named >>service name<<");
      m_vecstrCmdLineOptions.push_back("-i >>service name<< Install "
        "this Windows service as name >>service name<<");
    }
}

inline BYTE *
CPUcontrolService::GetCurrentCPUcoreData_Inline(DWORD & dwByteSize)
{
  BYTE * ar_byPipeDataToSend = NULL;
  LOGN(/*"IPC: get_current_CPU_data--"*/ "mp_dynfreqscalingthreadbase:"
    << mp_dynfreqscalingthreadbase )
  if (mp_dynfreqscalingthreadbase && mp_dynfreqscalingthreadbase-> IsStopped() )
  { //The DVFS thread does not collect CPU info-> collect it ourselves.
    //      r_arbyPipeDataToSend = m_ipc_datahandler.m_arbyData ;
    //      dwByteSize = 0 ;
    m_criticalsection_typeCPUcoreData.Enter();
    GetUsageAndVoltageAndFreqAndTempForAllCores(
//      GetUsageAndVoltageAndFreqForAllCores(
        m_model.m_cpucoredata.m_arfCPUcoreLoadInPercent,
        m_model.m_cpucoredata.GetNumberOfCPUcores() );
    //r_arbyPipeDataToSend =
    m_ipc_datahandler. GetCurrentCPUcoreAttributeValues(dwByteSize, false);
    ar_byPipeDataToSend = GetIPCdataThreadSafe(dwByteSize);
    m_criticalsection_typeCPUcoreData.Leave();
  }
  else
    {
      //    InterlockedIncrement()
      //TODO
      //    IncrementNumberOfClientsWantingCPUcoreDataThreadSafe() ;
      //TODO needs to be done only once for all client threads wanting CPU core
      //data.
      //    r_arbyPipeDataToSend = m_ipc_datahandler.GetCurrentCPUcoreAttributeValues(
      //      dwByteSize ) ;
      //    m_condition_typeGetCurrentCPUcoreData.

      WakeUpCreateIPCdataThread();
      ar_byPipeDataToSend = GetIPCdataThreadSafe(dwByteSize);
      //      dwByteSize = m_ipc_datahandler.m_dwByteSize ;
    }
  return ar_byPipeDataToSend;
}

//Use inline->faster
inline BYTE *
CPUcontrolService::GetIPCdataThreadSafe(DWORD & r_dwByteSize)
{
  //    m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished.Lock() ;
  //    LOGN("after locking the mutex for \"IPC data thread finished\" condition")
  //    //Wait for XML thread to finish altering CPU core data.
  //    m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished.Wait() ;
  //    LOGN("after waking up from \"IPC data thread finished\" condition")
  //    //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
  //    //"It then locks the mutex again and returns." -> unlock for other client
  //    //thread's Wait()
  //    m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished.m_wxmutex.
  //      Unlock( ) ;

  //Protect
  LOGN(/* "get IPC data: "*/ "before entering critical section for concurrent "
    "modification by in-program data to IPC data thread")
  //Assign the array pointer and array byte size _inside_ the critical section.
  m_model.m_cpucoredata.m_wxcriticalsectionIPCdata.Enter();

  LOGN(/*"get IPC data: " */ "after entering critical section for concurrent "
      "modification by in-program data to IPC data thread")
  BYTE * r_arbyPipeDataToSend = m_ipc_datahandler.m_arbyData;
  //  DWORD dwByteSize = m_ipc_datahandler.m_dwByteSize ;
  r_dwByteSize = m_ipc_datahandler.m_dwByteSize;
  if ( //if NULL if memory was released ( e.g. in order to reallocate)
  r_arbyPipeDataToSend && //dwByteSize
      r_dwByteSize)
    {
      //Copy the data before leaving the critical section: so it is ensured
      // that the data is valid independently of m_ipc_datahandler.m_arbyData.
      r_arbyPipeDataToSend = new BYTE[ //dwByteSize
          r_dwByteSize];
      memcpy(r_arbyPipeDataToSend, m_ipc_datahandler.m_arbyData, //dwByteSize
          r_dwByteSize);
    }
  else
    r_arbyPipeDataToSend = NULL;
  LOGN(/*"get IPC data: "*/ "before leaving critical section for concurrent "
    "modification by in-program data to IPC data thread")

  m_model.m_cpucoredata.m_wxcriticalsectionIPCdata.Leave();
  LOGN(/*"get IPC data: "*/ "after leaving critical section for concurrent "
    "modification by in-program data to IPC data thread")
  return r_arbyPipeDataToSend;
}

#ifdef COMPILE_WITH_IPC
DWORD
CPUcontrolService::IPC_Message(
//  BYTE byCommand
    ////  std::wstring & stdwstrMessage
    //  , BYTE * & r_arbyPipeDataToSend
    IPC_data & r_ipc_data)
{
  DWORD dwByteSize = 0;
  LOGN(/*"CPUcontrolService::IPC_Message(...)--"*/
    "size of data to read in byte:" << r_ipc_data.m_wPipeDataReadSizeInByte )

  //  LOGN("IPC message: " << (WORD) byCommand )
  //wide string because the power scheme may need it (e.g. for Chinese power
  // scheme names with > 256 chars in charset)
  std::wstring stdwstrMessage;
  switch (r_ipc_data.m_byCommand)
    {
  case get_configuration_data:
    { //Use a block here, else gcc 4.52: "error: jump to case label"
      LOGN("IPC: get_configuration_data")
      Xerces::VoltageForFrequencyConfiguration
          xerces_voltage_for_frequency_configuration( & m_model, mp_userinterface);
      r_ipc_data.m_ar_byDataToSend
          = xerces_voltage_for_frequency_configuration.SerializeConfigToMemoryBuffer(
              dwByteSize);
      LOGN("IPC: get_configuration_data end")
    }
    break;
  case get_current_CPU_data:
    r_ipc_data.m_ar_byDataToSend = GetCurrentCPUcoreData_Inline(dwByteSize);
    break;
  case stop_service:
    LOGN("IPC requested to stop the service")
    //s_p_cpucontrolservice->mp_dynfreqscalingbase->m_Stop() ;
    //if( mp_dynfreqscalingthreadbase ) 
    //   mp_dynfreqscalingthreadbase->Stop() ;
    //::SetEvent( s_p_cpucontrolservice->m_hEndProcessingEvent );
    Stop();
    break;
  case pause_service:
    { //Use a block here, else gcc 4.52: "error: jump to case label"
      LOGN("IPC requested to pause the service")
      bool bAlreadyPaused = Pause();
      if (bAlreadyPaused)
        stdwstrMessage
            = L"the dynamic voltage and frequency scaling thread is "
              "ALREADY stopped";
      else
        stdwstrMessage
            = L"the dynamic voltage and frequency scaling thread is "
              "stopped";
      LOGWN_WSPRINTF(stdwstrMessage.c_str() )
      dwByteSize = stdwstrMessage.size() * 2;
      //    r_arbyPipeDataToSend = new BYTE [ dwByteSize ] ;
      //    memcpy( r_arbyPipeDataToSend, stdwstrMessage.data(), dwByteSize ) ;
      r_ipc_data.m_ar_byDataToSend = new BYTE[dwByteSize];
      memcpy(r_ipc_data.m_ar_byDataToSend, stdwstrMessage.data(), dwByteSize);
    }
    break;
  case continue_service:
    { //Use a block here, else gcc 4.52: "error: jump to case label"
      LOGN("IPC requested to continue the service")
      bool bAlreadContinued = Continue();
      if (bAlreadContinued)
        stdwstrMessage
            = L"the dynamic voltage and frequency scaling thread is "
              "ALREADY running";
      else
        stdwstrMessage
            = L"the dynamic voltage and frequency scaling thread is "
              "running";
      LOGWN_WSPRINTF(stdwstrMessage.c_str() )
      dwByteSize = stdwstrMessage.size() * 2;
      //    r_arbyPipeDataToSend = new BYTE [ dwByteSize ] ;
      //    memcpy( r_arbyPipeDataToSend, stdwstrMessage.data(), dwByteSize ) ;
      r_ipc_data.m_ar_byDataToSend = new BYTE[dwByteSize];
      memcpy(r_ipc_data.m_ar_byDataToSend, stdwstrMessage.data(), dwByteSize);
      break;
    }
  case stop_DVFS:
    //if( mp_dynfreqscalingthreadbase ) 
    //   mp_dynfreqscalingthreadbase->Stop() ;
    Stop();
    break;
  case start_DVFS:
    StartDynVoltnFreqScaling();
    break;
  case setMaximumCPUcoreMultplier:
    { //Use a block here, else gcc 4.52: "error: jump to case label"
      LOGN("IPC: setMaximumCPUcoreMultplier begin")
      //    r_arbyPipeDataToSend = (dwByteSize) ;
      wxCriticalSectionLocker cs_locker(
          m_model.m_cpucoredata. m_wxcriticalsectionIPCdata);
      LOGN( "size of data to read: " << r_ipc_data.m_wPipeDataReadSizeInByte )
      if (r_ipc_data.m_wPipeDataReadSizeInByte >= sizeof(float))
        {
          LOGN( "size of data to read >=" << sizeof(float) )
          m_model.m_cpucoredata.m_fMaximumCPUcoreMultiplier
              = *((float *) r_ipc_data.m_ar_byPipeDataRead);
          LOGN( "IPC: setCPUcoreThrottleTemperature after setting the max. "
              "CPU core multiplier to "
              << m_model.m_cpucoredata.m_fMaximumCPUcoreMultiplier
              )
        }
      LOGN("IPC: setMaximumCPUcoreMultplier end")
    }
    break;
  case setCPUcoreThrottleTemperature:
    { //Use a block here, else gcc 4.52: "error: jump to case label"
      LOGN("IPC: setCPUcoreThrottleTemperature begin")
      //    r_arbyPipeDataToSend = (dwByteSize) ;
      wxCriticalSectionLocker cs_locker(
          m_model.m_cpucoredata. m_wxcriticalsectionIPCdata);
      if (r_ipc_data.m_wPipeDataReadSizeInByte >= sizeof(float))
        {
          LOGN( "size of data to read >=" << sizeof(float) )
          m_model.m_cpucoredata.m_fThrottleTempInDegCelsius
              = *((float *) r_ipc_data.m_ar_byPipeDataRead);
          LOGN( "IPC: setCPUcoreThrottleTemperature after setting the CPU core "
              "throttle temperature to "
              << m_model.m_cpucoredata.m_fThrottleTempInDegCelsius
              << " degrees Celsius")
        }
      LOGN("IPC: setCPUcoreThrottleTemperature end")
    }
    break;
  case setVoltageAndFrequency:
    //Setting the voltage and frequency means that it should not change. If
    //the DVFS thread would run it would surely be changed afterwards.
    //So stop the DVFS thread if it is running.
    if (mp_dynfreqscalingthreadbase)
      mp_dynfreqscalingthreadbase->Stop();
    //GetVoltage(fVoltage);
    //GetFrequency(wFrequency);
    break;
  case getAvailablePowerSchemeNames:
    {
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
      LOGN( "getAvailablePowerSchemeNames begin")
      std::set<std::wstring> stdset_std_wstrPowerSchemeName ;
      std::wstring wstrActivePowerScheme ;
      std::wstring std_wstrPowerSchemeNames;
      m_powerprofdynlinked.GetAllPowerSchemeNames(stdset_std_wstrPowerSchemeName) ;
      m_powerprofdynlinked.GetActivePowerSchemeName(wstrActivePowerScheme) ;

      uint16_t ui16PowerSchemeIndex = 0, ui16ActivePowerSchemeIndex = 0;
      for( std::set<std::wstring>::const_iterator
        c_iter_std_set_std_wstr_c_iterPowerSchemeName =
        stdset_std_wstrPowerSchemeName.begin() ;
        c_iter_std_set_std_wstr_c_iterPowerSchemeName !=
        stdset_std_wstrPowerSchemeName.end() ;
        ++ c_iter_std_set_std_wstr_c_iterPowerSchemeName
        )
      {
        if( wstrActivePowerScheme ==
            * c_iter_std_set_std_wstr_c_iterPowerSchemeName )
        {
          ui16ActivePowerSchemeIndex = ui16PowerSchemeIndex;
        }
        LOGN( "adding power scheme name \"" <<
          ::GetStdString(* c_iter_std_set_std_wstr_c_iterPowerSchemeName)
           << "\"." )
        std_wstrPowerSchemeNames +=
          * c_iter_std_set_std_wstr_c_iterPowerSchemeName + L"\n";
        ++ ui16PowerSchemeIndex;
      }
      dwByteSize = std_wstrPowerSchemeNames.length() * 2;
      r_ipc_data.m_ar_byDataToSend = new BYTE[dwByteSize
        //number for active scheme index
        + 2];
      memcpy(r_ipc_data.m_ar_byDataToSend, std_wstrPowerSchemeNames.//data(),
        c_str(),
        dwByteSize);
//      //Do the following because: every 2nd byte was 0x80 but should be "0x0" ?!
//      //: e.g. "B 80 a 80 l 80 a 80 n 80 c 80 e 80 d 80 8 10 80"
//      BYTE * p_by = r_ipc_data.m_ar_byDataToSend;
//      ++ p_by;
//      for( unsigned ui = 0; ui < dwByteSize; ++ ui)
//      {
////        if( * p_by == 0x80)
//        * p_by = //0x0;
//            0;
//        //Advance by 2 bytes.
//        p_by += 2;
//      }
      memcpy(r_ipc_data.m_ar_byDataToSend + dwByteSize,
        & ui16ActivePowerSchemeIndex, 2);
      //active scheme index number.
      dwByteSize += 2;
      LOGN( "data to send via IPC: " <<
        ::format_output_data(r_ipc_data.m_ar_byDataToSend,
          dwByteSize, 80)
        )
#endif
    }
    break;
  case setActivePowerScheme:
    LOGN("IPC: setActivePowerScheme begin")
    {
      std::wstring std_wstrActivePowerSchemeName(
          (wchar_t * ) r_ipc_data.m_ar_byPipeDataRead,
        //1 wchar_t has 2 bytes.
        r_ipc_data.m_wPipeDataReadSizeInByte / 2);
      LOGN("power scheme name from IPC: \"" <<
        ::GetStdString(std_wstrActivePowerSchemeName) << "\"")
      m_powerprofdynlinked.SetActivePowerScheme(std_wstrActivePowerSchemeName);
    }
    break;
  case sync_GUI_show_data_with_service:
    m_model.m_bSyncGUIshowDataWithService = true;
    break;
    }
  //  m_ipc_datahandler.GetResponse( byCommand ) ;
  r_ipc_data.m_wDataToWriteSizeInByte = dwByteSize;
  return dwByteSize;
}
#endif //#ifdef COMPILE_WITH_IPC

SERVICE_STATUS_HANDLE CPUcontrolService::RegSvcCtrlHandlerAndHandleError()
{
  LOGN("begin")
  DWORD dwLastError;
  //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
  //"The ServiceMain function should immediately call the
  //RegisterServiceCtrlHandlerEx function to specify a HandlerEx
  //function to handle control requests.
  //s_p_cpucontrolservice->
    //"If the function fails, the return value is zero."
    m_service_status_handle =
    //RegisterServiceCtrlHandler(
      //ServiceCtrlHandler);
      //Use RegisterServiceCtrlHandlerEx  because:
      //http://msdn.microsoft.com/en-us/library/ms810440.aspx:
      //"In addition, HandlerEx [RegisterServiceCtrlHandlerEx(...) is meant?]
      //gets extra messages unique to itself. These messages help a service
      //react appropriately to changes in the hardware configuration or the
      //state of the machine. These service control messages are:
      //[...] SERVICE_CONTROL_POWEREVENT
      //To accept these new messages, the appropriate flags must be submitted using SetServiceStatus. In the SERVICE_STATUS structure, the dwControlsAccepted member should be updated to reflect the desired messages. The bitwise flags are:
      //[...] SERVICE_ACCEPT_POWEREVENT
      //[...] SERVICE_ACCEPT_POWEREVENT is the service equivalent of the WM_POWERBROADCAST message; dwEventType is the power event identifier, and lpEventData is optional data [Code 2]. This is the WPARAM and LPARAM of WM_POWERBROADCAST, respectively."
      //If the function fails, the return value is zero.
      RegSvcCtrlHandlerExAndGetErrMsg(
        //s_p_cpucontrolservice->m_stdtstrProgramName.c_str(),
        m_stdtstrProgramName.c_str(),
        ServiceCtrlHandlerEx,
        //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
        //"lpContext [in, optional]
        //Any user-defined data. This parameter, which is passed to the
        //handler function, can help identify the service when multiple
        //services share a process."
        (LPVOID) 2,
        //stdstrErrorDescripition
        dwLastError);
  if (s_p_cpucontrolservice->m_service_status_handle
      == (SERVICE_STATUS_HANDLE) 0)
    {
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
          "Registering the service control handler failed; "
          "error code: " << dwLastError );
      std::string stdstrErrorDescription;
      ServiceBase::GetErrorDescriptionFromRegSvcCtrlHandlerExErrCode(
          dwLastError, stdstrErrorDescription);
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
          stdstrErrorDescription )
    }
  return s_p_cpucontrolservice->m_service_status_handle;
}

bool
CPUcontrolService::IsWithinStrings(
    const std::vector<std::string> & vecstdstrParams,
    const std::string & cr_stdstrToCompare)
{
  bool bIsWithinStrings = false;
  for (WORD wIndex = 0; wIndex < vecstdstrParams.size(); ++wIndex)
    {
      if (vecstdstrParams.at(wIndex) == cr_stdstrToCompare)
        {
          bIsWithinStrings = true;
          break;
        }
    }
  return bIsWithinStrings;
}

void
CPUcontrolService::outputUsage()
{
  //std::cout << "This program is an undervolting program for AMD family 17 CPUs.\n"
  //    "This executable is both in one:
  //    "-an installer for the undervolting service\n"
  //    "-the undervolting service itself\n" ;
  FillCmdLineOptionsList();
  std::cout << "Usage"
    "(pass this as command line options)"
    ":\n";
  for (WORD wIndex = 0; wIndex < m_vecstrCmdLineOptions.size(); ++wIndex)
    {
      std::cout << m_vecstrCmdLineOptions[wIndex] << "\n";
    }
}

//void CPUcontrolService::CreateStringVector(
//    stdstrInput = std::string(arch) ;
//{
//
//}

void
CPUcontrolService::CreateStringVector(
//char arch[] ,
    std::string stdstrInput, std::vector<std::string> & vecstdstrParams)
{
  bool bAnfZeichen = false;
  bool bStringSeperator = false;
  bool bWhitespace = false;
  WORD wBeginOfSubstring = NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE;
  WORD wNumberOfSubstringChars = 0;

  //DEBUG("whole string length: %u\n", stdstrInput.length() );
  LOG( "whole string length: " << stdstrInput.length() //<< "\n"
  )
  for (WORD wIndex = 0; wIndex < stdstrInput.length(); ++wIndex)
  {
    switch (stdstrInput[wIndex])
    {
      case ' ':
      case '\t':
        bWhitespace = true;
        ////Only if NOT the 1st of 2 AnfZeichen take a substring.
        ////(-> a anf-zeichen is there to allow a single string that
        ////even includes white spaces)
        //if( ! bAnfZeichen )
        //    vecstdstrParams.push_back(
        //        stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
        //        wIndex - wBeginOfSubstring + 1 )
        //      ) ;
        bStringSeperator = true;
        break;
      case '\"':
        if (
        //If already an AnfZeichen, then this AnfZeichen is the closing
        //AnfZeichen.
        bAnfZeichen)
          {
            //vecstdstrParams.push_back(
            //    stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
            //    wIndex - wBeginOfSubstring + 1 );
            bStringSeperator = true;
          }
        else
          // 1st Anfzeichen of pair of 2 Anfzeichen.
          //The substring starts directly after this Anfuehrungs-zeichen.
          wBeginOfSubstring = wIndex + 1;
        //Invert value of variable.
        //bAnfZeichen != bAnfZeichen ;
        bAnfZeichen = !bAnfZeichen;
        break;
      default:
        bWhitespace = false;
        bStringSeperator = false;
        if (wBeginOfSubstring == NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE)
          wBeginOfSubstring = wIndex;
        break;
    }// switch (stdstrInput[wIndex])
      //DEBUG("character: %c index:%u\n", stdstrInput[ wIndex ], wIndex ) ;
      LOG( "character: " << stdstrInput[ wIndex ] << "index:" << wIndex <<
          "\n" );
      if (bStringSeperator)
        wNumberOfSubstringChars = wIndex - wBeginOfSubstring;
      if (
      //Last string character
      wIndex == stdstrInput.length() - 1)
        wNumberOfSubstringChars = wIndex - wBeginOfSubstring + 1;
      if (wNumberOfSubstringChars)
        //Only if NOT the 1st of 2 AnfZeichen take a substring.
        //(-> a anf-zeichen is there to allow a single string that
        //even includes white spaces)
        if ( ! bAnfZeichen)
        {
          DEBUG("string seperator or end of whole string\n");
          vecstdstrParams.push_back(stdstrInput.substr(wBeginOfSubstring, //wNumberOfChanrs
              //wIndex - wBeginOfSubstring + 1
              wNumberOfSubstringChars));
          wBeginOfSubstring = NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE;
          wNumberOfSubstringChars = 0;
        }
  } // "for" loop
}

std::string
CPUcontrolService::GetValueIfHasPrefix(const std::string & r_stdstrPrefix)
{
  std::string stdstrValue;
  for (BYTE byCommandLineArgumentIndex = 0; byCommandLineArgumentIndex
      < m_vecstdstrCommandLineArgs.size(); ++byCommandLineArgumentIndex)
    {
      const std::string & cr_stdstrCommandlineArgument =
          m_vecstdstrCommandLineArgs.at(byCommandLineArgumentIndex);
      std::string::size_type stdstrsize_typeStartPos =
      //m_vecstdstrCommandLineArgs.at( byCommandLineArgumentIndex ).
          cr_stdstrCommandlineArgument.
          //Returns: The position of the first occurrence in the string of the
          //searched content.
          find(r_stdstrPrefix);
      if (stdstrsize_typeStartPos
      // The string was found at index 0.
          == 0)
        {
          stdstrValue = cr_stdstrCommandlineArgument.substr(
          //Copy from 1st char after prefix until the end of string.
              r_stdstrPrefix.length());
          break;
        }
      if (
      //Because this is a single command line argument that is a
      //combination of attribute name
      //and attribute value (e.g. "path=C:\Program Files\Griffin_svc.txt"
      //a '"' may precede the prefix.
      stdstrsize_typeStartPos == 1 && stdstrValue[0] == '\"')
        {
          stdstrValue = cr_stdstrCommandlineArgument.substr(
              //Copy from 1st char after prefix until the end of string.
              r_stdstrPrefix.length() + 1,
              cr_stdstrCommandlineArgument.length() -
              //Because there is a beginning '"' I also assume an ending
                  //'"' ->
                  // e.g.:  >>"path=C:\out .txt"<< = 18 chars
                  //   >>path=C:\out .txt<< = 16 chars.
                  //   prefix >>path=<< = 5 chars.
                  //   18 - ( 5 + 2 ) = 18 - 7 = 11 <=> >>C:\out .txt<<
                  (r_stdstrPrefix.length()
                  //For the '"'
                      + 2));
          break;
        }
    }
  return stdstrValue;
}

std::string
CPUcontrolService::GetLogFilePath()
{
  return GetValueIfHasPrefix( //_T("log_file_path=")
      "log_file_path=");

  //The log file path may contain spaces. So it is easier to pass the
  //log file path as a separate command line argument (e.g. as
  //"C:\Program Files\Griffin_svc.txt" ) than to extract the path from
  //a single command line argument that is a combination of attribute name
  //and attribute value (e.g. "path=C:\Program Files\Griffin_svc.txt"
  //IsWithinStrings( m_vecstdstrCommandLineArgs , "-log_file_path" )
  //    )
}

//bool CPUcontrolService::HasLogFilePathOption( //int argc, char *  argv[] 
//  )
//{
//    if( HasPrefix( _T("log_file_path=") )
//        return true ;
//}
//
//bool CPUcontrolService::HasPrefix( 
//    const std::string & stdstr )
//{
//    bool bHasPrefix = false ;
//    for( BYTE byCommandLineArgumentIndex = 0 ; 
//        byCommandLineArgumentIndex < m_vecstdstrCommandLineArgs.size() ;
//        ++ byCommandLineArgumentIndex )
//        if( 
//            m_vecstdstrCommandLineArgs.at( byCommandLineArgumentIndex ).
//            //Returns: The position of the first occurrence in the string of
//            // the searched content.
//            find(stdstr) 
//            // The string was found at index 0.
//            == 0 
//          )
//        {
//            bHasPrefix = true ;
//            break ;
//        }
//    return bHasPrefix ;
//}

//http://svn.wxwidgets.org/svn/wx/wxWidgets/trunk/docs/changes.txt:
//"Processing of pending events now requires a running event loop.
//  Thus initialization code (e.g. showing a dialog) previously done in wxApp::OnRun()
//  or equivalent function should now be done into wxApp::OnEventLoopEnter().
//  See wxApp::OnEventLoopEnter() and wxApp::OnEventLoopExit() docs for more info."
void CPUcontrolService::OnEventLoopEnter( wxEventLoopBase *   loop)
{

}

bool CPUcontrolService::OnInit()
{
  LOGN( "begin")
//  std::cout << "# prog args:" << argc << "\n";
////  ::OpenLogFile(argv);
//  std::cout << "after OpenLogFile\n";

  //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "before CallFromMainFunction")
//  if( CallFromMainFunction(argc,argv, this) == 0)
  {
    //OnInit() must return true, else no wxWidgets socket events can be processed?!
//    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "return true")
    return true ;
  }
//  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "return false")
  return false ;
}

void CPUcontrolService::OnServerEvent(wxSocketEvent & event)
{
  LOGN( "begin")
}
void CPUcontrolService::OnSocketEvent(wxSocketEvent & event)
{
  LOGN( "begin")
}

void CPUcontrolService::AddConsoleLogEntryWriter()
{
//  /*Logger::*/StdCoutLogWriter * logentryoutputter = new
//    /*Logger::*/StdCoutLogWriter();
////    I_LogFormatter logformatter = new I_LogFormatter::CreateFormatter();
//  RollingFileOutput * logfileappender = new RollingFileOutput(
//    g_logger,
//    logentryoutputter,
//    //logformatter,
//    NULL,
//    100,
//    LogLevel::warning
////      LogLevel::GetAsNumber(m_model.m_logfileattributes.m_std_strLevel)
//    );
////    logfileappender->CreateFormatter("txt"/*, std_strLogTimeFormatString*/ );
//  /*Logger::*/ConsoleFormatter * logformatter = new /*Logger::*/ConsoleFormatter(
//    logfileappender);
//  logfileappender->SetFormatter(logformatter);
//  g_logger.AddFormattedLogEntryProcessor( logfileappender);

  std::string std_strDummyLogFilePath = "dummy.txt";
  /*Logger::*/StdCoutLogWriter * std_cout_logentryoutputter = new
    /*Logger::*/StdCoutLogWriter();
  //    I_LogFormatter logformatter = new I_LogFormatter::CreateFormatter();
  RollingFileOutput * consoleappender = new RollingFileOutput(
    g_logger,
    std_strDummyLogFilePath,
    std_cout_logentryoutputter,
    "txt",
  //      consoleformatter,
    100,
    //Use minimum log level "success" to also output coloured success messages.
    LogLevel::success/*warning*/
  //      LogLevel::GetAsNumber(m_model.m_logfileattributes.m_std_strLevel)
    );
  //    logfileappender->CreateFormatter("txt"/*, std_strLogTimeFormatString*/ );
  Windows::ConsoleFormatter * consoleformatter = new Windows::ConsoleFormatter(
    consoleappender);
  consoleappender->Open(std_strDummyLogFilePath);
  consoleappender->SetFormatter(consoleformatter);
  g_logger.AddFormattedLogEntryProcessor( consoleappender);
}

bool CPUcontrolService::OpenLogFile(TCHAR * argv[])
{
#ifdef UNICODE
//  std::wcout << L"1st program arg:" << argv[0] << L"\n";
#else
  std::cout << L"1st program arg:" << argv[0] << L"\n";
#endif

  std::string std_strLogFileName =
    //CPUcontrolBase::m_model.m_stdstrLogFilePath + "/" +
    ::GetStdString_Inline( argv[0] );
//  std::cout << "std_strLogFileName:" << std_strLogFileName << "\n";


  //  std::tstring std_tstrLogFileName;
  #if defined(__GNUC__) && __GNUC__ > 3 //GCC 3.4.5 does not have "psapi.a" lib.
  if( ServiceBase::IsStartedAsService() )
    //std::string std_strLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
    std_strLogFileName += //std::string( argv[0]) +
      std::string("_log.") ;
  //    std_tstrLogFileName = std::tstring( argv[0]) +
  //      std::tstring( L"_log.txt") ;
  else
    std_strLogFileName += //std::string( argv[0]) +
      //to not use the same log file as the possibly running service.
      std::string("_TUI_log.") ; //Textual User Interface
  //    std_tstrLogFileName = std::tstring( argv[0]) +
  //      std::tstring(L"_execute_actions_log.txt") ;
  #else
  std_strLogFileName += //std::string( argv[0]) +
    std::string("_log.") ;
  //  std_tstrLogFileName = std::tstring( argv[0]) +
  //    std::tstring( L"_log.txt") ;
  #endif
  std::wstring std_wstrLogFileName = ::GetStdWstring( std_strLogFileName );

  //The path must be set before the config files are accessed.
  if( ! ::SetExePathAsCurrentDir() )
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "Getting file path for THIS executable file failed: " <<
      LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n"
      );

  std::wstring std_wstrLogFileFolderPath;
  std::wstring std_wstrFileNameWithoutDirs = GetExecutableFileName(
    std_wstrLogFileName.c_str() );
  bool logFileIsOpen = CPUcontrolBase::GetLogFilePropertiesAndOpenLogFile(
    std_wstrLogFileFolderPath,
    /*std_wstrLogFileName*/ std_wstrFileNameWithoutDirs
    );
//  if( logFileIsOpen)
//  {
//    AddConsoleLogEntryWriter();
//  }
  return logFileIsOpen;
}

/**@ return true: already paused*/
bool CPUcontrolService::Pause()
{
  LOGN( "begin")
  bool bAlreadyPaused = false;
  m_servicestatus.dwCurrentState = SERVICE_PAUSED;
  m_bProcess = false;
  if (mp_dynfreqscalingthreadbase)
    {
      if (mp_dynfreqscalingthreadbase->IsStopped() )
        {
          bAlreadyPaused = true;
          LOGN("Service is ALREADY paused."//\n"
            )
        }
      else
        {
          mp_dynfreqscalingthreadbase->Stop();
          LOGN("Service is paused."//\n"
          )
        }
    }
  SetServiceStatus();
  return bAlreadyPaused;
}

//ReadOptionsAsStringLine()
//{
//std::cout << "select your option now: >>i/I<<nstall this program as service\n" ;
////std::cin >> stdstrInput ;
//std::cin.getline( //::getline(std::cin,
//    //stdstrInput
//    arch , 100 ) ; //.getline(
//CreateStringVector(
//  arch ,
//  r_vecstdstrParams
//  ) ;
//stdstrInput = std::string(arch) ;
//DEBUG("whole string length: %u\n", stdstrInput.length() );
//for( WORD wIndex = 0 ; wIndex < stdstrInput.length(); ++ wIndex )
//{
//    switch ( stdstrInput[ wIndex ] )
//    {
//    case ' ':
//    case '\t':
//        bWhitespace = true ;
//        ////Only if NOT the 1st of 2 AnfZeichen take a substring.
//        ////(-> a anf-zeichen is there to allow a single string that
//        ////even includes white spaces)
//        //if( ! bAnfZeichen )
//        //    vecstdstrParams.push_back(
//        //        stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
//        //        wIndex - wBeginOfSubstring + 1 )
//        //      ) ;
//        bStringSeperator = true ;
//        break ;
//    case '\"':
//        if(
//            //If already an AnfZeichen, then this AnfZeichen is the closing
//            //AnfZeichen.
//            bAnfZeichen )
//        {
//            //vecstdstrParams.push_back(
//            //    stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
//            //    wIndex - wBeginOfSubstring + 1 );
//            bStringSeperator = true ;
//        }
//        else // 1st Anfzeichen of pair of 2 Anfzeichen.
//            //The substring starts directly after this Anfuehrungs-zeichen.
//            wBeginOfSubstring = wIndex + 1 ;
//        //Invert value of variable.
//        bAnfZeichen != bAnfZeichen ;
//        break ;
//    default:
//        bWhitespace = false ;
//        bStringSeperator = false ;
//        if ( wBeginOfSubstring ==
//            NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE )
//            wBeginOfSubstring = wIndex ;
//    }
//    DEBUG("character: %c index:%u\n", stdstrInput[ wIndex ], wIndex ) ;
//    if( bStringSeperator )
//        wNumberOfSubstringChars = wIndex - wBeginOfSubstring ;
//    if(
//        //Last string character
//        wIndex == stdstrInput.length() - 1
//        )
//        wNumberOfSubstringChars = wIndex - wBeginOfSubstring + 1;
//    if( wNumberOfSubstringChars )
//        //Only if NOT the 1st of 2 AnfZeichen take a substring.
//        //(-> a anf-zeichen is there to allow a single string that
//        //even includes white spaces)
//        if( ! bAnfZeichen )
//        {
//            DEBUG("string seperator or end of whole string\n");
//            vecstdstrParams.push_back(
//                stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
//                //wIndex - wBeginOfSubstring + 1
//                wNumberOfSubstringChars )
//              ) ;
//            wBeginOfSubstring = NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE ;
//            wNumberOfSubstringChars = 0 ;
//        }
//}
//}

void CPUcontrolService::RemoveConsoleLogWriter()
{
  std::vector<FormattedLogEntryProcessor *> & formattedLogEntryProcessors =
    g_logger.GetFormattedLogEntryProcessors();
  LOGN_DEBUG( "# log entry processors:" << formattedLogEntryProcessors.size() )
  std::vector<FormattedLogEntryProcessor *>::iterator
    firstFormattedLogEntryProcessor =
    //(std::vector<FormattedLogEntryProcessor *>::iterator)
    formattedLogEntryProcessors.begin();
  FormattedLogEntryProcessor * p_formattedLogEntryProcessor =
    * firstFormattedLogEntryProcessor;
  FormattedLogEntryProcessor & r_formattedLogEntryProcessor =
    ** firstFormattedLogEntryProcessor;
//  LOGN_DEBUG( "should delete formatted log entry processor:" <<
//    (const FormattedLogEntryProcessor &) r_formattedLogEntryProcessor )
  /** Remove console log writer */
  //( (std::vector<FormattedLogEntryProcessor *>)
     formattedLogEntryProcessors
    //)
    .erase(firstFormattedLogEntryProcessor);
}

void CPUcontrolService::RenameLogOutputNames()
{
  const std::vector<FormattedLogEntryProcessor *> & formattedLogEntryProcessors
    = g_logger.GetFormattedLogEntryProcessors();
  std::vector<FormattedLogEntryProcessor *>::const_iterator c_iter =
    formattedLogEntryProcessors.begin();
  //    g_logger.
  std::tstring std_tstrLogFileName(m_cmdlineargs.GetStringArray()
    /*m_stringArray*/[0]);
  std_tstrLogFileName += _T("_TUI_log");
  std::string std_strLogFileName;
  std::string::size_type lastDot;
  FormattedLogEntryProcessor * p_formattedlogentryprocessor;
  while( c_iter != formattedLogEntryProcessors.end() )
  {
    p_formattedlogentryprocessor = (* c_iter);
    std_strLogFileName = GetStdString(std_tstrLogFileName);
    lastDot = p_formattedlogentryprocessor->GetFilePath().rfind('.');
    if( lastDot != std::string::npos )
      std_strLogFileName += p_formattedlogentryprocessor->GetFilePath().substr(
        lastDot);
    //Rename to ->"[...]TUI" so that the log file is not overwritten by the
    //service after starting this program as a service.
    p_formattedlogentryprocessor->RenameThreadSafe( std_strLogFileName);
    ++ c_iter;
  }
}

int
CPUcontrolService::requestOption(
  //Make as parameter as reference: more ressource-saving than
  //to return (=a copy).
  std::vector<std::string> & r_vec_std_strParams,
  const std::tstring & r_tstrProgName)
{
  bool bContinue = false;
  char arch[101];
  std::string stdstrInput;
  std::string stdstrDefaultProcessName = //"CPUcontrolService" ;
      GetStdString(r_tstrProgName);
  //std::cout << "You can input your choice (deinstall/ install) now: "
  //    "same format as if command line option:\n" ;
  std::cout << "\nSelect your option now:\n"
    "Start as normal [a/A]pplication (-> not as service)\n"
    "[i/I]nstall this program as service\n"
    "[d/D]einstall/ [d/D]elete (this) service\n"
    "[r/R]einstall (this) service (delete->install)\n"
    "[s/S]tart (this) service\n"
    "Sto[p/P] (this) service\n"
    "[q/Q]uit\n";
  std::cout.flush();
#ifdef _DEBUG
  char ch;// = 'R';
  std::cin >> ch;
  int nChar = ch;
#else
  int nChar = ::getche();
#endif
  std::cout << "\n";
  switch ( ::toupper(nChar))
    {
  case 'A':
    std::cout << "You chose to start the application as normal program\n";
    r_vec_std_strParams.push_back( //_T("-i")
        "-a");
    break;
  case 'I':
    std::cout << "You chose to install the service\n";
    std::cout << "Now input the name for the service. "
      "Input no text to choose the default name \"" << stdstrDefaultProcessName
        << "\"\nPress ENTER/ Return to finish.\n";
    r_vec_std_strParams.push_back( //_T("-i")
        "-i");
    //Valid input char->continue.
    bContinue = true;
    break;
  case 'D':
    std::cout << "You chose to delete the service\n";
    std::cout << "Now input the name for the service to delete. "
      "Input no text to choose the default name \"" << stdstrDefaultProcessName
        << "\"\nPress ENTER/ Return to finish.\n";
    r_vec_std_strParams.push_back( //_T("-d")
        "-d");
    //Valid input char->continue.
    bContinue = true;
    break;
  case 'R':
    std::cout << "You chose to reinstall the service\n";
    std::cout << "Now input the name for the service to delete and install "
      "afterwards. "
      "Input no text to choose the default name \"" << stdstrDefaultProcessName
        << "\"\nPress ENTER/ Return to finish.\n";
    r_vec_std_strParams.push_back( //_T("-d")
        "-d");
    //Valid input char->continue.
    bContinue = true;
    break;
  case 'S':
    std::cout << "You chose to start the service\n";
    std::cout << "Now input the name for the service to start. "
      "Input no text to choose the default name \"" << stdstrDefaultProcessName
        << "\"\nPress ENTER/ Return to finish.\n";
    r_vec_std_strParams.push_back( //_T("-s")
        "-s");
    bContinue = true;
    break;
  case 'P':
    std::cout << "You chose to stop the service\n";
    std::cout << "Now input the name for the service to start. "
      "Input no text to choose the default name \"" << stdstrDefaultProcessName
        << "\"\nPress ENTER/ Return to finish.\n";
    r_vec_std_strParams.push_back( //_T("-p")
        "-p");
    bContinue = true;
    break;
    //Just for exclusion of the "default" switch.
  case 'Q':
    break;
  default:
    std::cout << "You did not input a valid option.\n";
    break;
  }
  if (bContinue)
    {
      //std::cin >> stdstrInput ;
      std::cin.getline( //::getline(std::cin,
          //stdstrInput
          arch, 100);
      stdstrInput = std::string(arch);
      //        if( stdstrInput.empty() )
      //            r_vecstdstrParams.push_back( stdstrDefaultProcessName ) ;
      //        else
      //            r_vecstdstrParams.push_back( stdstrInput ) ;
      std::string & r_stdstrServiceName =
          stdstrInput.empty() ? stdstrDefaultProcessName : stdstrInput;
      r_vec_std_strParams.push_back(r_stdstrServiceName);
      if (toupper(nChar) == 'R')
        {
          r_vec_std_strParams.push_back( //_T("-i")
              "-i");
          r_vec_std_strParams.push_back(r_stdstrServiceName);
        }
    }
  //ReadOptionsAsStringLine() ;
  return nChar;
}

//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/
// startservicectrldispatcher.htm:
//"All initialization tasks are done in the service's ServiceMain function
//when the service is started."
void WINAPI
CPUcontrolService::ServiceMain(DWORD argc, LPTSTR *argv)
{
  I_Thread::SetCurrentThreadName("service main");
  LOGN( "begin")
  BYTE byArgIndex = 0;
  DWORD dwStatus;
  std::string stdstrErrorDescripition;
  //    DWORD specificError;
#ifndef EMULATE_EXECUTION_AS_SERVICE
  if( s_p_cpucontrolservice->m_bStartedAsService )
    if ( //if error occurred
      ! s_p_cpucontrolservice->RegSvcCtrlHandlerAndHandleError() )
      return;
#endif //#ifndef EMULATE_EXECUTION_AS_SERVICE
  //http://msdn.microsoft.com/en-us/library/ms687414%28v=VS.85%29.aspx:
  //  "For best system performance, your application should enter the running
  //  state within 25-100 milliseconds."
  //Initialization not complete, but report "running" status in order to
  // let the service control manager continue.
  s_p_cpucontrolservice->SetInitialServiceStatusAttributeValues(
    SERVICE_WIN32,
    SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE
    //http://msdn.microsoft.com/en-us/library/ms810440.aspx:
    //"To accept these new messages[SERVICE_CONTROL_POWEREVENT], the
    //appropriate flags must be submitted using SetServiceStatus. In the
    //SERVICE_STATUS structure, the dwControlsAccepted member should be
    //updated to reflect the desired messages. The bitwise flags are:
    //[...] SERVICE_ACCEPT_POWEREVENT"
    | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_SESSIONCHANGE
    );

#if defined( _DEBUG ) && !defined(EMULATE_EXECUTION_AS_SERVICE)
//  if( argc > 2 )
  {
//    CommandLineArgs<TCHAR> cmdlineargs(argc, argv);
    unsigned index = s_p_cpucontrolservice->m_cmdlineargs.contains(
      (TCHAR *) _T("-delay") );
    if( index != UINT_MAX && ++ index < argc)
    {
      std::tstring std_tstrDelay(argv[index]);
      std::string std_strDelay = GetStdString(std_tstrDelay);
      DWORD dwSleepTimeInMs;
      if( ConvertStdStringToTypename(dwSleepTimeInMs, std_strDelay) )
        //Give time to attach a debugger to THIS process if it was not started
        //from Visual Studio.
        ::Sleep( //wait time in milliseconds
          dwSleepTimeInMs);
    }
  }
#endif
  //DEBUG("ServiceMain--argument count: %u\n",argc)
  LOGN( "Service main--argument count: " << argc //<< "\n"
    )
  for (; byArgIndex < argc; ++ byArgIndex)
    //DEBUG("argument %u:%s\n", (WORD) byArgIndex, argv[byArgIndex]);
    LOG("argument " << (WORD) byArgIndex << ": " << argv[byArgIndex] <<
        "\n" );

  //  BYTE byVoltageAndFrequencyScalingType;
  // Initialization code goes here.
  //"After these calls[...,ServiceStatus()], the function should complete the
  //initialization of the service."
  dwStatus = (
  //Tell g++ that CPUcontrolServiceBase's Initialize is meant.
    (CPUcontrolServiceBase *) s_p_cpucontrolservice)->Initialize(
      argc,
      argv //&specificError
      , s_p_cpucontrolservice->m_byVoltageAndFrequencyScalingType);
  LOGN("Should use DVFS type:" << (WORD) s_p_cpucontrolservice->
      m_byVoltageAndFrequencyScalingType)
  s_p_cpucontrolservice->HandleInitServiceFailed(dwStatus);
  if (dwStatus != NO_ERROR)
    {
      LOGN("error in initialization of the service--returning"//\n"
        )
      return;
    }

  //s_p_cpucontrolservice->m_ipcserver.Init() ;
  //if( s_p_cpucontrolservice->mp_cpucontroller )
  //  s_p_cpucontrolservice->mp_cpucontroller->EnableOwnDVFS() ;
#ifdef COMPILE_WITH_IPC
  s_p_cpucontrolservice->m_x86iandc_threadGetCurrentCPUcoreData.start(
      GetCurrentCPUcoreDataInLoopThreadFunc, s_p_cpucontrolservice,
      "GetCurrentCPUcoreInfo");
//  StartwxSocketServerThread();
  StartIPCserverThread();
#endif

  LOGN("service main--CPU controller: "
      << s_p_cpucontrolservice->mp_cpucontroller
      << "CPU usage getter: " << s_p_cpucontrolservice->mp_cpucoreusagegetter )
  if (s_p_cpucontrolservice->HandleStartDynVoltAndFreqScalingThread(
      //byVoltageAndFrequencyScalingType
      ) == EXIT_SUCCESS)
    {
//      LOGN("Waiting for the stop service condition to become true")
      LOGN("service main--current thread ID:" << ::GetCurrentThreadId() )
      s_p_cpucontrolservice->m_vbServiceInitialized = true;

      LOGN( "before waiting for the end condition to "
        "become signalled." )
      //If not waiting the thread object is destroyed at end of block.
      ::WaitForSingleObject(s_p_cpucontrolservice->m_hEndProcessingEvent,
          INFINITE);
      //    WaitForEndOfDVFSthread() ;
      LOGN( "after waiting for the end condition to "
        "become signalled." )
    }

  // This is where the service does its work.
  //SvcDebugOut(" From the service: Returning the main thread \n",0);
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "From the service: Returning the main thread" );
  //"[...] your service should use this thread to complete whatever tasks
  //it was designed to do.
  //If a service does not need a thread to do its work (such as a service
  // that only processes RPC requests), its ServiceMain function should
  //return control to the caller. It is important for the function to return,
  //rather than call the ExitThread function, because returning allows for
  //cleanup of the memory allocated for the arguments.

  ////You can provide for additional cleanup by calling the
  // RegisterWaitForSingleObject function on an event before returning.
  // The thread that is running the ServiceMain function terminates, but the
  // service itself continues to run. The service control handler can then
  //signal your event when the service stops, and a thread from the thread pool
  //executes your callback to perform any additional cleanup you need and to
  //call SetServiceStatus with SERVICE_STOPPED.
  //This technique is not illustrated in the sample code below, however."

  //synchr. with "ServiceCtrlHandlerEx"
  //p_eventStopped.Set();
  //DEBUG("Service main function--end\n",argc)
  LOGN( "Service main function--end"//\n"
  )
  LOGN( "return")
  return;
}

void
CPUcontrolService::SetCommandLineArgs(int argc, char * argv[])
{
  for (BYTE byCommandLineArgumentIndex = 0; byCommandLineArgumentIndex < argc; ++byCommandLineArgumentIndex)
    m_vecstdstrCommandLineArgs.push_back(std::string(argv[argc]));
}

void
CPUcontrolService::SetCPUcontroller(I_CPUcontroller * p_cpucontrollerNew)
{
  if (p_cpucontrollerNew)
    {
      //        if( mp_cpucontroller )
      //        {
      //          //Release memory.
      //          delete mp_cpucontroller ;
      //          LOGN(" current CPU controller deleted")
      //        }
      LOGN("address of model: " << & m_model )
      mp_cpucontroller = p_cpucontrollerNew;
      //May be NULL at startup.
      if (mp_cpucoreusagegetter)
        mp_cpucoreusagegetter->SetCPUcontroller(p_cpucontrollerNew);
      LOGN("after setting CPU controller for usage getter")
      mp_cpucontroller->SetModelData(&m_model);
      LOGN("before GetMaximumFrequencyInMHz. number of CPU cores: " <<
          (WORD) m_model.m_cpucoredata.GetNumberOfCPUcores() )
      //Needed for drawing the voltage-frequency curves.
      WORD w = mp_cpucontroller->GetMaximumFrequencyInMHz();
      LOGN("after GetMaximumFrequencyInMHz: " << w )
    }
}

void
CPUcontrolService::SetInitHardwareAccessWaitHint()
{
  //s_p_cpucontrolservice->
  //  //Estimated time required for a pending start, stop, pause, or
  //  //continue operation, in milliseconds. Before the specified amount
  //  //of time has elapsed, the service should make its next call to the
  //  //SetServiceStatus function with either an incremented dwCheckPoint
  //  //value or a change in dwCurrentState. If the amount of time
  //  //specified by dwWaitHint passes, and dwCheckPoint has not been
  //  //incremented or dwCurrentState has not changed, the service
  //  //control manager or service control program can assume that an error
  //  //has occurred and the service should be stopped.
  //  m_servicestatus.dwWaitHint =
  //  //calling "InitializeOls" sometimes took 1:30 (min:s) ro even longer
  //  //(seen in the log file) so set 360 s = 6 min as a wait hint
  //  360000 ;
  //SetServiceStatus();
}

void
CPUcontrolService::SetServiceStatus()
{
#ifndef EMULATE_EXECUTION_AS_SERVICE
  if ( m_bStartedAsService && ! ::SetServiceStatus(
      //s_p_cpucontrolservice->
  //this handle is the return value of "RegisterServiceCtrlHandlerEx()"
        m_service_status_handle,
        & //s_p_cpucontrolservice->
        m_servicestatus)
      )
  {
    DWORD dwStatus = ::GetLastError();
    //SvcDebugOut(" [MY_SERVICE] SetServiceStatus error %ld\n",
    //   status);
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "SetServiceStatus error " << dwStatus //<< "\n"
    );
  }
#endif //#ifndef EMULATE_EXECUTION_AS_SERVICE
}

bool
CPUcontrolService::ShouldCreateService(
    const std::vector<std::string> & cr_vecstdstrParams)
{
  bool bShouldCreateService = false;
  //if( argc > 1 )
  //    //if( strcmp(argv[1],"-i") == 0 )
  //    if( std::string(argv[1]) == "-i" )
  if (IsWithinStrings(cr_vecstdstrParams, "-i"))
    bShouldCreateService = true;

  //if( ! bShouldCreateService )
  //{
  //  cout << "\n"
  //      "Should the undervolting service be installed now? [Y]es/ "
  //    "other key: no\n"//; finish with ENTER/RETURN key\n"
  //    ;
  //    char ch ;
  //    //std::cin >> ch ;
  //    ch = getche() ;
  //    if( ch == 'Y' )
  //        bShouldCreateService = true ;
  //}
  return bShouldCreateService;
}

bool
CPUcontrolService::ShouldDeleteService(
    const std::vector<std::string> & vecstdstrParams)
{
  bool bShouldDeleteService = false;
  //if( argc > 1 )
  //    //if( strcmp(argv[1],"-i") == 0 )
  //    if( std::string(argv[1]) == "-d" )
  if (IsWithinStrings(vecstdstrParams, "-d"))
    bShouldDeleteService = true;

  return bShouldDeleteService;
}

bool
CPUcontrolService::ShouldStartService(
    const std::vector<std::string> & vecstdstrParams)
{
  bool bShouldStartService = false;
  //if( argc > 1 )
  //    //if( strcmp(argv[1],"-i") == 0 )
  //    if( std::string(argv[1]) == "-d" )
  if (IsWithinStrings(vecstdstrParams, "-s"))
    bShouldStartService = true;

  return bShouldStartService;
}

bool
CPUcontrolService::ShouldStartAsNormalApp(
    const std::vector<std::string> & vecstdstrParams)
{
  bool bShouldStartAsNormalApp = false;
  //if( argc > 1 )
  //    //if( strcmp(argv[1],"-i") == 0 )
  //    if( std::string(argv[1]) == "-d" )
  if (IsWithinStrings(vecstdstrParams, "-a") )
    bShouldStartAsNormalApp = true;

  LOGN( "return" << bShouldStartAsNormalApp )
  return bShouldStartAsNormalApp;
}

bool
CPUcontrolService::ShouldStopService(
    const std::vector<std::string> & vecstdstrParams)
{
  bool bShouldStopService = false;
  //if( argc > 1 )
  //    //if( strcmp(argv[1],"-i") == 0 )
  //    if( std::string(argv[1]) == "-d" )
  if (IsWithinStrings(vecstdstrParams, "-p"))
    bShouldStopService = true;

  return bShouldStopService;
}

void
CPUcontrolService::StartIPCserverThread()
{
  DWORD dwThreadId;
  s_p_cpucontrolservice->m_ipc_datahandler.m_p_i_cpucontroller
      = s_p_cpucontrolservice->mp_cpucontroller;
  //IPC_servers wait for client and are often BLOCKING, so THIS
  //block would not continue execution->start client
  //connection listening in dedicated thread.
  HANDLE handleThread = ::CreateThread(NULL, // default security attributes
      0, // use default stack size
      IPC_ServerThread, // thread function name
      & s_p_cpucontrolservice->m_ipcserver,// argument to thread function
      0, // use default creation flags
      &dwThreadId); // returns the thread identifier
  if (handleThread == NULL)
    {
      LOGN("creating the IPC server thread failed");
      return;
    }
  else
    {
      //http://msdn.microsoft.com/en-us/library/ms686724%28v=VS.85%29.aspx:
      //"When a thread terminates, its thread object is not freed until all open
      //handles to the thread are closed."
      //http://msdn.microsoft.com/en-us/library/ms724211%28v=VS.85%29.aspx:
      //"Closing a thread handle does not terminate the associated thread or
      //remove the thread object."
      //Close the thread handle here (waiting for the end of the thread via
      // WaitForSingleObject() would need another thread->not so good.)
      ::CloseHandle(handleThread);
    }
}

void
CPUcontrolService::StartwxSocketServerThread()
{
  DWORD dwThreadId;
  LOGN( "begin")
  s_p_cpucontrolservice->m_wxservicesocketserver.SetServerPortNumber(5000);
  //  s_p_cpucontrolservice->m_wxservicesocketserver.Init() ;
  //IPC_servers wait for client and are often BLOCKING, so THIS
  //block would not continue execution->start client
  //connection listening in dedicated thread.
  HANDLE handleThread = ::CreateThread(
      NULL, // default security attributes
      0, // use default stack size
      IPC_ServerThread, // thread function name
      // argument to thread function
      & s_p_cpucontrolservice->m_wxservicesocketserver,
      0, // use default creation flags
      & dwThreadId // returns the thread identifier
      );
  if (handleThread == NULL)
    {
      LOGN("creating the IPC server thread failed");
      return;
    }
  else
    {
      //http://msdn.microsoft.com/en-us/library/ms686724%28v=VS.85%29.aspx:
      //"When a thread terminates, its thread object is not freed until all open
      //handles to the thread are closed."
      //http://msdn.microsoft.com/en-us/library/ms724211%28v=VS.85%29.aspx:
      //"Closing a thread handle does not terminate the associated thread or
      //remove the thread object."
      //Close the thread handle here (waiting for the end of the thread via
      // WaitForSingleObject() would need another thread->not so good.)
      ::CloseHandle(handleThread);
    }
  //http://docs.wxwidgets.org/trunk/classwx_app_traits.html
  // #11205b08706b4d83bc83d0170511c909:
  // "The version in wxConsoleAppTraits returns a console-specific event loop
  // which can be used to handle timer and socket events in console programs
  // under Unix and MSW"
  //  wxAppTraits::CreateEventLoop() ;
  //The socket events need to be processed in a thread!
  //  wxEventLoopBase * p = wxConsoleAppTraits::CreateEventLoop() ;
  LOGN( "end")
}

void CPUcontrolService::StartAsNonService()
{
  LOGN( "begin")

#ifdef COMPILE_WITH_IPC
  s_p_cpucontrolservice->m_wxservicesocketserver.SetServerPortNumber(5000);
  //wxWidgets seockets must be initialized in the main thread!
  s_p_cpucontrolservice->m_wxservicesocketserver.Init();
#endif //#ifdef COMPILE_WITH_IPC

  ServiceMain(
      //m_dwArgCount,
      1,
      //m_p_lptstrArgument
      argv
    );
  LOGN( "end")
}

void CPUcontrolService::StartServiceCtrlDispatcherInSeparateThread()
{
  LOGN( "begin")
  Windows_API::Thread x86iandc_threadServiceMain;
  x86iandc_threadServiceMain.start(
    CPUcontrolService::StartServiceCtrlDispatcher_static, NULL);

#ifdef COMPILE_WITH_IPC
  s_p_cpucontrolservice->m_wxservicesocketserver.SetServerPortNumber(5000);
  //wxWidgets sockets must be initialized in the main thread!
  s_p_cpucontrolservice->m_wxservicesocketserver.Init();
#endif //#ifdef COMPILE_WITH_IPC

  LOGN( "Waiting for the stop service condition to become "
    "true")

  //http://msdn.microsoft.com/en-us/library/ms686324%28v=VS.85%29.aspx
  // (StartServiceCtrlDispatcher function):
  //"Connects the main thread of a service process to the service control
  //manager, which causes the thread to be the service control dispatcher
  //thread for the calling process."

  //If not waiting the service control dispatcher thread ends!.
  ::WaitForSingleObject(s_p_cpucontrolservice->m_hEndProcessingEvent,
      INFINITE);
  LOGN( "end")
}

DWORD THREAD_FUNCTION_CALLING_CONVENTION CPUcontrolService::
  StartServiceCtrlDispatcher_static(void * p_v)
{
  LOGN( "begin")
  //SERVICE_TABLE_ENTRYA ("char") or SERVICE_TABLE_ENTRYW ( wchar_t )
  SERVICE_TABLE_ENTRY ar_service_table_entry[] =
    {
      {
      //"Pointer to a null-terminated string that specifies the name of
          //a service to be run in this service process.
          //If the service is installed with the SERVICE_WIN32_OWN_PROCESS
          //service type, this member is ignored, but cannot be NULL.
          //This member can be an empty string ("").
          //If the service is installed with the SERVICE_WIN32_SHARE_PROCESS
          //service type, this member specifies the name of the service that
          //uses the ServiceMain function pointed to by the lpServiceProc member."
          //m_stdstrServiceName.c_str()
          //"CPUcontrolService"
          //Convert "const char *" -> "char *", "const wchar_t *" -> "wchar_t *".
          (TCHAR *) s_p_cpucontrolservice->m_stdtstrProgramName.
          //returns "const char *" (ANSI) or "const wchar_t *" (wide char)
          c_str()
          //Pointer to a ServiceMain function.
          //"All initialization tasks are done in the service's ServiceMain
          //function when the service is started."
          , ServiceMain },
    //NULL entries mark the end of the SERVICE_TABLE_ENTRY array.
          { NULL, NULL } };
#ifdef EMULATE_EXECUTION_AS_SERVICE
  LPTSTR argv[1];
  argv[0] = "CPUcontrolService";
  ServiceMain(
      //m_dwArgCount,
      1,
      //m_p_lptstrArgument
      argv
  );
#else
  LOGN_WARNING("Before starting service ctrl dispatcher--current thread id:" <<
      ::GetCurrentThreadId()
      << "\nNote: it may take 2 minutes or even more (especially for a debug "
      "version) until the service control "
      "dispatcher has finished to start")
  //The StartServiceCtrlDispatcher function connects the main thread of a 
  //service process to the service control manager, which causes the thread 
  //to be the service control dispatcher thread for the calling process.
  //In DIESEM Thread wird dann "ServiceCtrlHandlerEx" ausgefuehrt.
  //When the service control manager starts a service process, it waits for
  // the process to call the StartServiceCtrlDispatcher function.
  //The main thread of a service process should make this call as soon as
  //possible after it starts up.
  //Starts the "ServiceMain" function in a new thread.
  if ( ! ::StartServiceCtrlDispatcher(ar_service_table_entry))
    {
      DWORD dw = ::GetLastError();
      std::string stdstr;
      LOGN_ERROR("StartServiceCtrlDispatcher failed:" <<
        ::LocalLanguageMessageAndErrorCodeA( dw ) )
      ServiceBase::GetErrorDescriptionFromStartServiceCtrlDispatcherErrCode(dw,
          stdstr);
      LOGN(stdstr)
      return 1;
    }
#endif //#ifdef EMULATE_EXECUTION_AS_SERVICE
  LOGN( "return 0")
  return 0;
}

void CPUcontrolService::StartService()
{
  LOGN_DEBUG("begin");
  m_bStartedAsService = true;

#ifdef COMPILE_WITH_IPC
//  s_p_cpucontrolservice->m_wxservicesocketserver.SetServerPortNumber(5000);
//  //wxWidgets seockets must be initialized in the main thread!
//  s_p_cpucontrolservice->m_wxservicesocketserver.Init();
#endif //#ifdef COMPILE_WITH_IPC

  StartServiceCtrlDispatcher_static(NULL);

  //The execution continues here if the service was stopped.
  //DEBUG("end of starting service\n");
  LOGN("after starting service ctrl dispatcher--current thread id:" <<
      ::GetCurrentThreadId() )
  //  EndAlterCurrentCPUcoreIPCdata() ;
}

/** This method is called by the Windows Service Control Manager.
* It is executed in the same thread that called
* "StartServiceCtrlDispatcher". */
DWORD WINAPI
CPUcontrolService::ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType,
    LPVOID lpEventData, LPVOID lpContext)
{
  LOGN(/*"service control handler "*/ "begin control code:" << dwControl
    /* << " current thread id:" << ::GetCurrentThreadId() */ );
  //  LOGN("service ctrl handler--current thread id:" << ::GetCurrentThreadId() )
  //DWORD status;
  // Local variables.
  static DWORD CachedState;
  //LPEVENTINFO pEventInfo;
  //   DWORD dwBuffer;
  if (s_p_cpucontrolservice->m_vbServiceInitialized)
    {
      // Do not replicate current state.
      // Controls codes greater than 127 are programmer
      //      defined service control codes.
      if ((CachedState == dwControl) && (dwControl < 128))
        return NO_ERROR;

      switch (dwControl)
        {
      case SERVICE_CONTROL_PAUSE:
        // Do whatever it takes to pause here.
        s_p_cpucontrolservice->Pause();
        //break;
        return NO_ERROR;

      case SERVICE_CONTROL_CONTINUE:
        // Do whatever it takes to continue here.
        s_p_cpucontrolservice->Continue();
        //break; 
        return NO_ERROR;

        //if( s_p_cpucontrolservice->mp_cpucontroller )
        //  s_p_cpucontrolservice->mp_cpucontroller->EnableOwnDVFS() ;
      case SERVICE_CONTROL_STOP:
        LOGN(/*"service control handler--"*/ "received stop control code")
        // Do whatever it takes to stop here. 
        s_p_cpucontrolservice->Stop();
        //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/handlerex.htm
        //"For all other control codes your service handles, return NO_ERROR."
        return NO_ERROR;

      case SERVICE_CONTROL_INTERROGATE:
        // Fall through to send current status.
        break;

      case SERVICE_CONTROL_POWEREVENT:
        {
          s_p_cpucontrolservice->HandlePowerEvent(dwEventType);
        }
        break;
      case SERVICE_CONTROL_SESSIONCHANGE:
        //        LOGN("session change event")
        //http://msdn.microsoft.com/en-us/library/ms683241%28v=VS.85%29.aspx:
        //"If dwControl is SERVICE_CONTROL_SESSIONCHANGE, this parameter can be
        //one of the values specified in the wParam parameter of the
        //WM_WTSSESSION_CHANGE message."
        //http://msdn.microsoft.com/en-us/library/aa383828%28v=VS.85%29.aspx:
        switch (dwEventType)
          {
        case WTS_SESSION_LOGON:
          LOGN("Logon event")
          //          case WTS_SESSION_UNLOCK:
          //http://msdn.microsoft.com/en-us/library/ms683241%28v=VS.85%29.aspx:
          //"If dwControl is SERVICE_CONTROL_SESSIONCHANGE, this parameter
          // is a pointer to a WTSSESSION_NOTIFICATION  structure."
          PWTSSESSION_NOTIFICATION pwtssession_notification =
              (PWTSSESSION_NOTIFICATION) lpEventData;
          s_p_cpucontrolservice->HandleLogonEvent(pwtssession_notification);
          break; //WTS_SESSION_LOGON
          //TODO ? If locked, the GUI does not need to to the work to show
          // anything.
          //          case WTS_SESSION_LOCK:
          }
        break;
      default:
        LOGN(//"service control handler--"
            "call not impl.")
        //SvcDebugOut(" [MY_SERVICE] Unrecognized opcode %ld\n",
        //    dwControl);
        //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        //    "Unrecognized service opcode / control code received from the "
        //    "OS/ service control manager: " << dwControl //<< "\n"
        //    );
        return ERROR_CALL_NOT_IMPLEMENTED;
        }
      LOGN("setting service status")
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
      //"Next, it should call the SetServiceStatus function to send
      //status information to the service control manager."
      s_p_cpucontrolservice->SetServiceStatus();
      //return;
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/
      // handlerex.htm
      //"For all other control codes your service handles, return NO_ERROR. "
      return NO_ERROR;
    }
  else
    //"If your service does not handle the control, return
    // ERROR_CALL_NOT_IMPLEMENTED."
    return ERROR_CALL_NOT_IMPLEMENTED;
}

void
CPUcontrolService::ShowMessage(const std::tstring & cr_std_tstrMessage)
{
  ::MessageBox(
  //If this parameter is NULL, the message box has no owner window.
      NULL, cr_std_tstrMessage.c_str()
      //title bar
      //, "error initializing the CPU access"
      //, strProgramName.c_str()
      //      , getstdstring( m_stdwstrProgramName ).c_str()
      , m_stdtstrProgramName.c_str()
      //, "CPUcontrol service error"
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/
      // interactive_services.htm:
      //"Display a message box by calling the MessageBox function with
      //MB_SERVICE_NOTIFICATION. This is recommended for displaying simple
      //status messages. Do not call MessageBox during service initialization
      //or from the HandlerEx routine, unless you call it from a separate
      //thread, so that you return to the SCM in a timely manner."
      , MB_SERVICE_NOTIFICATION);
  //TODO http://msdn.microsoft.com/en-us/library/ms683502%28v=VS.85%29.aspx:
  // MessageBox with MB_SERVICE_NOTIFICATION works only for
  // for Windows Server 2003, Windows XP, and Windows 2000->
  // Better use WTSSendMessage(...) ?!
  //TODO WTSSendMessage is not in MinGW's <wtsapi.h>
  //    DWORD dwResponse ;
  //    DWORD dwTitleBarStringLenInBytes = m_stdtstrProgramName.length() ;
  //    //TODO correct macros used (also for MSVC)?
  //#if defined _UNICODE || defined UNICODE
  //    dwTitleBarStringLenInBytes *= 2 ;
  //#endif
  //    DWORD dwMessageStringLenInBytes =
  //        cpuaccessexception.m_stdstrErrorMessage.length() ;
  //#if defined _UNICODE || defined UNICODE
  //    dwMessageStringLenInBytes *= 2 ;
  //#endif
  //    //Citations from:
  //    //http://msdn.microsoft.com/en-us/library/aa383842%28VS.85%29.aspx:
  //    ::WTSSendMessage(
  //      //"WTS_CURRENT_SERVER_HANDLE to indicate the RD Session Host server on "
  //      //"which your application is running."
  //      WTS_CURRENT_SERVER_HANDLE
  //      //"To indicate the current session, specify WTS_CURRENT_SESSION"
  //      , WTS_CURRENT_SESSION
  //      , //getstdstring( m_stdtstrProgramName.c_str() )
  //      //(TCHAR*) m_stdtstrProgramName.c_str()
  //      "hh"
  //      , //"The length, in bytes, of the title bar string."
  //        //dwTitleBarStringLenInBytes
  //        2
  //      //"A pointer to a null-terminated string that contains the message to
  //      //display."
  //      , //cpuaccessexception.m_stdstrErrorMessage.c_str()
  //      //(TCHAR*) Getstdtstring( cpuaccessexception.m_stdstrErrorMessage ).c_str()
  //      "jj"
  //      , //"The length, in bytes, of the message string."
  //      //cpuaccessexception.m_stdstrErrorMessage.size()
  ////      dwMessageStringLenInBytes
  //      2
  //      , MB_OK
  //      //"The time, in seconds[...]
  //      //If the Timeout parameter is zero, WTSSendMessage  waits indefinitely
  //      //for the user to respond."
  //      , 10
  //      , & dwResponse
  //      //"If TRUE, WTSSendMessage does not return until the user responds or the
  //      //time-out interval elapses."
  //      , //FALSE
  //      TRUE
  //      ) ;
}

void
CPUcontrolService::Stop()
{
  LOGN(/*"CPUcontrolService::Stop() "*/ "begin")
  if (mp_dynfreqscalingthreadbase)
    mp_dynfreqscalingthreadbase->Stop();
  //TODO implement
  //  EndAllClientThreads() ;
  mp_dynfreqscalingthreadbase->WaitForTermination();
  //::SetEvent( s_p_cpucontrolservice->m_hEndProcessingEvent );
  //   s_p_cpucontrolservice->
  m_servicestatus.dwWin32ExitCode = 0;
  //   s_p_cpucontrolservice->
  m_servicestatus.dwCurrentState = SERVICE_STOPPED;
  //   s_p_cpucontrolservice->
  m_servicestatus.dwCheckPoint = 0;
  //   s_p_cpucontrolservice->
  m_servicestatus.dwWaitHint = 0;

  LOGN(/*"CPUcontrolService::Stop()--"*/ "signalling the main thread to continue")
  //WaitForSingleObject(m_eventStopped);
  ::SetEvent(m_hEndProcessingEvent);

  //Inform the Service Control Manager that we have stopped.
  SetServiceStatus();
  EndAlterCurrentCPUcoreIPCdata();

  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "Leaving service" );
}

VOID
CPUcontrolService::SvcDebugOut(LPSTR String, DWORD Status)
{
  CHAR Buffer[1024];
  if (strlen(String) < 1000)
    {
      sprintf(Buffer, String, Status);
      OutputDebugStringA(Buffer);
      //DEBUG("%s",Buffer);
      LOG( Buffer );
      //printf("%s",Buffer);
      //m_ofstream << std::string(Buffer) ;
      //m_ofstream.flush() ;
    }
}

inline void
CPUcontrolService::WakeUpCreateIPCdataThread()
{
  //Send message to thread to alter XML DOM tree for CPU core data.
  //    LOGN("DynFreqScalingThreadBase::Entry(): before "
  //        "mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
  //    //Let all threads waiting (that called condition.Wait() ) on the
  //    // condition continue.
  //    //"Wait() atomically unlocks the mutex
  //    // which allows the thread to continue and starts waiting"
  //    //blocks here when using wxCondition::Broadcast() (wxWidgets bug?)
  ////      mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast();
  //
  ////    mp_cpucoredata->m_win32eventCPUdataCanBeSafelyRead.Broadcast() ;
  ////    mp_cpucoredata->m_win32eventCPUdataCanBeSafelyRead.ResetEvent() ;
  //    LOGN(//"DynFreqScalingThreadBase::Entry():"
  //      "after mp_cpucoredata->m_conditionCPUdataCanBeSafelyRead.Broadcast()")
  LOGN("before waking up the CPU core data to IPC data thread")

  //This would happen if the pipe client thread would Lock() the mutex
  //  before calling "Signal(...)":
  //-the pipe client thread would block its execution until creating the IPC
  // data has finished (this also depends on the variable wait time in the
  //  "dyn volt 'n freq scaling" thread)
  //-the CPU data->IPC data thread would probably would receive the wakeup, but
  //-other pipe clients would continue to wait until the mutex is locked.
  //
  // IPC=InterProcessCommunication

  // | create_IPC_data_thread   pipe_client_thread1    pipe_client_thread2
  // |     |                      mutex.Lock()                 |
  // |     |                        |                      mutex.Lock()
  // +-condition::Wait()            |                          |
  //    for: create Inter           |
  //    Process Communication       |
  //    data                        |
  //   releases mutex lock ---> gets mutex' lock
  //                                |
  //                                v
  // +--wakes up<---------------condition::Signal()            |
  //       |
  //       v
  // +-creates IPC data         condition::Wait() on:
  //       |                      creating IPC data
  //       v                      finished
  // +-condition::
  //    Broadcast():
  //    IPC data ready     ->   wakes up
  // |
  // v
  //time

  //The other thread may not wake up by "Signal(...)" because the mutex is not
  // locked before calling "Signal(...)".
  // In this case use the previously generated IPC data.
  m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree.Signal();
  LOGN("after waking up the CPU core data to IPC data thread")
}
