#pragma once

#include <fstream> //for class ofstream
#include <windows.h> //for SERVICE_TABLE_ENTRY, ...
#include "ModelData/ModelData.hpp"
//#include "wxWidgets/DynFreqScalingThread.hpp"
//#include "PumaStateCtrl.h"
#include <DummyUserInterface.hpp>
#include <Controller/CPUcontrolBase.hpp>
#include <Controller/MainController.hpp>
#include <Controller/I_ServerProcess.hpp>
#include <Controller/stdtstr.hpp> //std::tstring

#ifdef COMPILE_WITH_CALC_THREAD
  #include <Windows/CalculationThread.hpp>
#endif
//#include <Windows/CPUcoreUsageGetterIWbemServices.hpp>
#include <Windows/PowerProf/PowerProfDynLinked.hpp>
//#include <Windows/DynFreqScalingAccess.hpp>
#ifdef COMPILE_WITH_IPC
  #include <Windows/NamedPipe/NamedPipeServer.hpp>
#endif //#ifdef COMPILE_WITH_IPC
#include <Windows/Service/ServiceBase.hpp>
//#include <Windows/WinRing0dynLinked.hpp>
#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#include <vector>

//#define _WIN32_WINNT 0x0400 //for MB_SERVICE_NOTIFICATION
//#include <winuser.h> //MB_SERVICE_NOTIFICATION
#define MB_SERVICE_NOTIFICATION 0x00200000L

class WinRing0DynLinked ;
class I_DynFreqScalingAccess ;
class DynFreqScalingThreadBase ;

class CPUcontrolService
  :
#ifdef COMPILE_WITH_IPC
    public I_ServerProcess
  ,
#endif //#ifdef COMPILE_WITH_IPC
  public CPUcontrolBase
  , public ServiceBase //for register service ctrl manager ease etc.
{
private :
  //Flag for ServiceCtrlHandlerEx to indicate whether the service is REALLY
  //running (the service report SERVICE_RUNNUNG immediately because the
  //Service Control Manager is blocked for other services until the service
  //reports SERVICE_RUNNUNG)
  bool m_vbServiceInitialized ;
  HANDLE m_handleMapFile;
  //LPSTR m_archServiceName ;
  std::string m_stdstrServiceName ;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
  //std::wstring m_stdwstrProgramName ;
  std::tstring m_stdtstrProgramName ;
  LPVOID mp_voidMappedViewStartingAddress ;
public:
    DWORD m_argc ;
    LPTSTR * m_argv ;
    LPTSTR * mar_tch ;
    //static 
      volatile bool m_bProcess ;
    //static 
      volatile bool m_bRun ;
    //static 
#ifdef COMPILE_WITH_CALC_THREAD
      CalculationThread m_calculationthread ;
#endif //COMPILE_WITH_CALC_THREADs
    //CPUcoreUsageGetterIWbemServices m_cpucoreusagegetteriwbemservices ;
    //Must be static. Else: runtime error when calling DummyUserInterface::Confirm(ostream)
    static 
      DummyUserInterface m_dummyuserinterface ;
    //Use non-objects rather than MFC objects because I don't want to be
    //dependand on MFC (porting is easier ).
    HANDLE m_hEndProcessingEvent ;
    //This method is form: http://msdn.microsoft.com/en-us/library/ms810429.aspx
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
    //static 
       //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
       IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
    PowerProfDynLinked m_powerprofdynlinked ;
    MainController m_maincontroller ;
    /*DynFreqScalingThread * mp_dynfreqscalingthread ;*/
    //static 
    //WinRing0_1_3RunTimeDynLinked m_winring0dynlinked ;
    //WinRing0_1_3RunTimeDynLinked * mp_winring0dynlinked ;
    static std::vector<std::string> m_vecstrCmdLineOptions ;
    std::tstring m_stdtstrProgramArg ;
    std::vector<std::string> m_vecstdstrCommandLineArgs ;
    ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
#ifdef COMPILE_WITH_IPC
    NamedPipeServer m_ipcserver ;
#endif //#ifdef COMPILE_WITH_IPC
    DynFreqScalingThreadBase * mp_dynfreqscalingthreadbase ;
public :
    CPUcontrolService(//const char* szServiceName
      //std::wstring & r_stdwstrProgramName
      std::tstring & r_stdtstrProgramName
      ) ;
    CPUcontrolService(
      DWORD argc, 
      LPTSTR *argv ,
      //std::wstring & r_stdwstrProgramName
      std::tstring & r_stdtstrProgramName
      ) ;
    ~CPUcontrolService() ;

    void Continue() ;
    //"static" because: there needn't be an object for putting this info out.
    static void CreateStringVector(
      //char arch[] , 
      std::string stdstrInput ,
      std::vector<std::string> & vecstdstrParams 
      ) ;
    static void FillCmdLineOptionsList() ;
    std::string GetLogFilePath() ;
    std::string GetValueIfHasPrefix( 
        const std::string & r_stdstrPrefix ) ;
    inline void HandlePowerEvent(DWORD dwEventType ) ;
    inline void HandleInitServiceFailed( DWORD dwStatus) ;
    inline void HandleStartDynVoltAndFreqScalingThread() ;
    void Initialize() ;
#ifdef COMPILE_WITH_IPC
    void IPC_Message(BYTE byCommand) ;
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
    void Pause() ;
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
    void StartDynVoltnFreqScaling() ;
    void Stop() ;
    static void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) ;
    void StartService() ;
};
