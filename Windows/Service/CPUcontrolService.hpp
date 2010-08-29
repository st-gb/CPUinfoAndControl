#pragma once

#include <Controller/CPUcontrolBase.hpp>
#include <Controller/MainController.hpp>
#include <Controller/I_ServerProcess.hpp>
#include <Controller/IPC/I_IPC_DataHandler.hpp>
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <ModelData/ModelData.hpp>
#include <UserInterface/DummyUserInterface.hpp>
//#include "wxWidgets/DynFreqScalingThread.hpp"

#ifdef COMPILE_WITH_CALC_THREAD
  #include <Windows/CalculationThread.hpp>
#endif
#include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
//#include <Windows/DynFreqScalingAccess.hpp>
#include <Windows/multithread/Thread.hpp>
#ifdef COMPILE_WITH_IPC
  #include <Windows/NamedPipe/NamedPipeServer.hpp>
#endif //#ifdef COMPILE_WITH_IPC
#include <Windows/Service/ServiceBase.hpp>
//#include <Windows/WinRing0dynLinked.hpp>
#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
////Also include the cpp file, else "used but never defined warning"
//#include <wxWidgets/multithread/wxConditionBasedI_Condition.cpp>
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>
#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>
#include <Xerces/IPC/IPCdataGenerator.hpp> //class Xerces::IPCdataHandler

#include <fstream> //for class ofstream
#include <vector>
#include <windows.h> //for SERVICE_TABLE_ENTRY, ...

//#define _WIN32_WINNT 0x0400 //for MB_SERVICE_NOTIFICATION
//#include <winuser.h> //MB_SERVICE_NOTIFICATION
#ifndef _WINUSER_H
  #define MB_SERVICE_NOTIFICATION 0x00200000L
#endif

class WinRing0DynLinked ;
class I_DynFreqScalingAccess ;
class DynFreqScalingThreadBase ;
//namespace Xerces
//{
//  class IPCdataHandler ;
//}

class CPUcontrolService
  :
#ifdef COMPILE_WITH_IPC
    public I_ServerProcess
  ,
#endif //#ifdef COMPILE_WITH_IPC
  public CPUcontrolBase
  , public ServiceBase //for register service ctrl manager ease etc.
{
public:
  DWORD m_dwArgCount ;
private :
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
  Xerces::IPCdataHandler m_ipc_datahandler ;
private:
  std::string m_stdstrServiceName ;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
//  std::wstring m_stdwstrProgramName ;
  std::tstring m_stdtstrProgramName ;
#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  LPVOID mp_voidMappedViewStartingAddress ;
#endif
  //this might be changed to a pointer later
//  I_IPC_DataHandler m_ipc_datahandler ;
public:
    LPTSTR * mar_tch ;
    //static 
      volatile bool m_bProcess ;
    //static 
      volatile bool m_bRun ;
    //static 
#ifdef COMPILE_WITH_CALC_THREAD
      CalculationThread m_calculationthread ;
#endif //COMPILE_WITH_CALC_THREADs
    //Must be static. Else: runtime error when calling
    // DummyUserInterface::Confirm(ostream)
    static 
      DummyUserInterface m_dummyuserinterface ;
    //Use non-objects rather than MFC objects because I don't want to be
    //dependent on MFC (porting is easier ).
    HANDLE m_hEndProcessingEvent ;
    //This method (C++ class for a service) is from:
    //http://msdn.microsoft.com/en-us/library/ms810429.aspx
    static 
      CPUcontrolService * 
        msp_cpucontrolservice;   // nasty hack to get object ptr
    //static 
       SERVICE_STATUS          m_servicestatus; 
    //static 
//       SERVICE_STATUS_HANDLE   m_service_status_handle;
    //static 
//       I_CPUcontroller * mp_cpucontroller ;
    //static 
       std::ofstream m_ofstream ;
    //static 
       //Model m_modelDataForCopying ;
       Model * mp_modelData ;
       Model m_modelData ;
    //static 
       //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
       IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
#ifdef COMPILE_WITH_IPC
    NamedPipeServer m_ipcserver ;
#endif //#ifdef COMPILE_WITH_IPC
    MainController m_maincontroller ;
    PTSTR * m_p_ptstrArgument ;
    PowerProfDynLinked m_powerprofdynlinked ;
    /*DynFreqScalingThread * mp_dynfreqscalingthread ;*/
    //static 
    //WinRing0_1_3RunTimeDynLinked m_winring0dynlinked ;
    //WinRing0_1_3RunTimeDynLinked * mp_winring0dynlinked ;
    static std::vector<std::string> m_vecstrCmdLineOptions ;
    std::tstring m_stdtstrProgramArg ;
    std::vector<std::string> m_vecstdstrCommandLineArgs ;
    ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
    DynFreqScalingThreadBase * mp_dynfreqscalingthreadbase ;
    wxConditionBasedI_Condition
      m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtree ;
    wxConditionBasedI_Condition
      m_wxconditionbasedi_conditionAlterCPUcoreDataDOMtreeFinished ;
//    x86IandC::thread_type m_x86iandc_threadGetCurrentCPUcoreData ;
    Windows_API::Thread m_x86iandc_threadGetCurrentCPUcoreData ;
    volatile bool m_vbAlterCPUcoreDataForIPC ;
public :
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
    //"static" because: there needn't be an object for putting this info out.
    static void CreateStringVector(
      //char arch[] , 
      std::string stdstrInput ,
      std::vector<std::string> & vecstdstrParams 
      ) ;
    void EndAlterCurrentCPUcoreIPCdata() ;
    static void FillCmdLineOptionsList() ;
    inline BYTE * GetIPCdataThreadSafe(DWORD & r_dwByteSize) ;
    std::string GetLogFilePath() ;
    std::string GetValueIfHasPrefix( 
        const std::string & r_stdstrPrefix ) ;
    inline void HandlePowerEvent(DWORD dwEventType ) ;
    inline void HandleInitServiceFailed( DWORD dwStatus) ;
    inline void HandleStartDynVoltAndFreqScalingThread() ;
    void Initialize() ;
#ifdef COMPILE_WITH_IPC
    DWORD IPC_Message(
      BYTE byCommand
//      //wide string because the string may be a chinese string for a power
//      scheme
//      , std::wstring & stdwstrMessage
      , BYTE * & r_arbyPipeDataToSend
      ) ;
#endif //#ifdef COMPILE_WITH_IPC
    ////static 
    //    bool HasLogFilePathOption( int argc, char *  argv[] ) ;
    static void outputUsage();
    //static 
    DWORD MyServiceInitialization(
      DWORD   argc, 
      LPTSTR  * argv
        //DWORD *specificError
      //, ICalculationThread * p_icalculationthread
      //, ISpecificController * p_ispecificcontroller
      //, UserInterface * p_userinterface
      ) ;
    bool Pause() ;
    SERVICE_STATUS_HANDLE RegSvcCtrlHandlerAndHandleError() ;
    static void requestOption(
      //Make as parameter as reference: more ressource-saving than
      //to return (=a copy).
      std::vector<std::string> & vecstdstrParams 
      , std::tstring & r_tstrProgName ) ;
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

    void SetServiceStatus () ;
    void SetServiceStatusAttValues() ;
    static bool ShouldCreateService(
        const std::vector<std::string> & cr_vecstdstrParams ) ;
    static bool ShouldDeleteService(
        const std::vector<std::string> & cr_vecstdstrParams );
    static VOID SvcDebugOut(LPSTR String, DWORD Status);
    bool StartDynVoltnFreqScaling() ;
    void Stop() ;
    static void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) ;
    void StartService() ;
    inline void WakeUpCreateIPCdataThread() ;
};
