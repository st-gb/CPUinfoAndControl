#include "CPUcontrolService.hpp"
#include <conio.h> //for getche()

#include <windows.h> //for SERVICE_TABLE_ENTRY, SERVICE_STATUS_HANDLE, ...

#include <Controller/DynFreqScalingThreadBase.hpp> //
#include <Controller/CPU-related/I_CPUcontroller.hpp>
//for ICPUcoreUsageGetter::Init()
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/stdtstr.hpp>
#ifdef COMPILE_WITH_IPC
  #include <Controller/IPC/I_IPC.hpp> //for enum ctrlcodes
#endif //#ifdef COMPILE_WITH_IPC
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <Xerces/SAX2ServiceConfigHandler.hpp>
#include <Xerces/XMLAccess.hpp> //readXMLconfig()
//for the #defines like "WTS_SESSION_LOGIN" in <wts32api.h>
//  ( is defined inside the #if (_WIN32_WINNT >= 0x0501)  )
#if _WIN32_WINNT < 0x0501
  #undef _WIN32_WINNT
  #define _WIN32_WINNT 0x0501
#endif
//#ifdef _MSC_VER
  #include <wtsapi32.h>
//#else //WTSSendMessage is not available in MinGW's <wtsapi32.h>
  #include <Windows/WTSSendMessageRunTimeDynLinked.hpp>
//#endif
//#include <windows.h>
#include <AccCtrl.h> //SE_SERVICE

typedef struct tagWTSSESSION_NOTIFICATION {
  DWORD cbSize;
  DWORD dwSessionId;
} WTSSESSION_NOTIFICATION, *PWTSSESSION_NOTIFICATION;

#define USE_WINDOWS_THREAD

#ifdef USE_WINDOWS_THREAD
  #include <Windows/DynFreqScalingThread.hpp>
#else //#ifdef USE_WINDOWS_THREAD
  #include <wxWidgets/DynFreqScalingThread.hpp>
#endif //#ifdef USE_WINDOWS_THREAD
//#include <Windows/GetWindowsVersion.h>
//#include <Windows/PowerProfFromWin6DynLinked.hpp>
//#include <Windows/PowerProfUntilWin6DynLinked.hpp>
//#include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#include <Windows/CreateProcess.hpp> //Invoke...

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2
#define NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE 65535

extern FILE * fileDebug ;
//The static member must also be DEFINED here additional to the declaration 
//, else linker error LNK2019: Verweis auf nicht aufgelöstes externes Symbol
//""public: static class CPUcontrolService * CPUcontrolService::
// msp_cpucontrolservice"
CPUcontrolService * CPUcontrolService::msp_cpucontrolservice ;
std::vector<std::string> CPUcontrolService::m_vecstrCmdLineOptions ;
DummyUserInterface CPUcontrolService::m_dummyuserinterface ;

#ifdef COMPILE_WITH_IPC
DWORD WINAPI IPC_ServerThread( LPVOID lpParam )
{
  LOGN("IPC_ServerThread");
  I_IPC_Server * p_i_ipc_server = (I_IPC_Server *) lpParam ;
  if( p_i_ipc_server )
  //while(1)
  //{
  //  Sleep(1000); 
  //  MessageBeep( (WORD) -1 ) ;
  //}
    p_i_ipc_server->Init() ;
  return 0; 
}
#endif //#ifdef COMPILE_WITH_IPC

CPUcontrolService::CPUcontrolService(
    //const char * szServiceName
  std::wstring & r_stdwstrProgramName
//  std::tstring & r_stdtstrProgramName
//  , I_IPC_DataHandler & r_ipc_datahandler
  )
  //C++ style inits:
  :
//    m_bSyncGUIshowDataWithService ( false ) ,
#ifdef COMPILE_WITH_IPC
//    mr_ipc_datahandler(r_ipc_datahandler) ,
  m_ipc_datahandler ( m_modelData ) ,
  m_ipcserver(this)
#endif //#ifdef COMPILE_WITH_IPC
    //m_bProcess ( true )
    //, m_bRun ( true ) 
    //, mp_winring0dynlinked (NULL)
    //, mar_tch(NULL)
  , m_maincontroller( & m_dummyuserinterface )
  , m_powerprofdynlinked ( r_stdwstrProgramName )
    //,
//    m_powerprofdynlinked ( r_stdtstrProgramName )
//    , m_stdwstrProgramName ( r_stdwstrProgramName)
//    , m_stdtstrProgramName ( r_stdtstrProgramName)
{
  m_stdtstrProgramName = Getstdtstring( r_stdwstrProgramName) ;
  Initialize() ;
}

CPUcontrolService::CPUcontrolService(
  DWORD argc , 
  LPTSTR *argv ,
  std::wstring & r_stdwstrProgramName
//  std::tstring & r_stdtstrProgramName
//  , I_IPC_DataHandler & r_ipc_datahandler
  )
  //C++ style init.
  :
  m_dwArgCount(argc)
#ifdef COMPILE_WITH_IPC
//  , mr_ipc_datahandler(r_ipc_datahandler)
  , m_ipc_datahandler ( m_modelData )
  , m_ipcserver(this)
#endif //#ifdef COMPILE_WITH_IPC
//  , m_powerprofdynlinked ( r_stdtstrProgramName )
//  , m_stdtstrProgramName ( r_stdtstrProgramName )
  , m_maincontroller( & m_dummyuserinterface )
  ,
    m_p_ptstrArgument(argv)
  , m_powerprofdynlinked ( r_stdwstrProgramName )
{
  m_stdtstrProgramName = Getstdtstring( r_stdwstrProgramName) ;
    //Calling the ctor inside another ctor created the object 2 times!
    //CPUcontrolService() ;
    Initialize() ;
}

CPUcontrolService::~CPUcontrolService()
{
  FreeRessources() ;
}

/**@ return true: already continued*/
bool CPUcontrolService::Continue()
{
  bool bAlreadyContinued = false ;
   msp_cpucontrolservice->
       m_bProcess = true ;
   msp_cpucontrolservice->
       m_servicestatus.dwCurrentState = SERVICE_RUNNING; 
   //If the service was paused before stand-by/ hibernation and
   //if continued after stand-by/ hibernation 
   //then for e.g. AMD Griffins the p-state
   //values have to be restored.
  msp_cpucontrolservice->mp_cpucontroller->
    ResumeFromS3orS4();
   //If the service was paused before stand-by/ hibernation and
   //if continued after stand-by/ hibernation 
   //then performance event select register values.
   //values have to be restored.
  msp_cpucontrolservice->mp_cpucoreusagegetter->Init() ;
  bAlreadyContinued = StartDynVoltnFreqScaling() ;
  SetServiceStatus() ;
  LOG("Service is continued.\n");
  return bAlreadyContinued ;
}

void CPUcontrolService::HandleInitServiceFailed( DWORD dwStatus)
{
  // Handle error condition
  if ( dwStatus == NO_ERROR )
  {
      LOG("The service was successfully initialized\n" ) ;
  }
  else
  {
      msp_cpucontrolservice->
          m_servicestatus.dwCurrentState       = SERVICE_STOPPED;
      msp_cpucontrolservice->
          m_servicestatus.dwCheckPoint         = 0;
      msp_cpucontrolservice->
          m_servicestatus.dwWaitHint           = 0;
      msp_cpucontrolservice->
          m_servicestatus.dwWin32ExitCode      = dwStatus;
      //msp_cpucontrolservice->
      //    m_servicestatus.dwServiceSpecificExitCode = specificError;

  #ifndef EMULATE_EXECUTION_AS_SERVICE
      ::SetServiceStatus (msp_cpucontrolservice->
          m_service_status_handle, & msp_cpucontrolservice->
          m_servicestatus);
  #endif //#ifndef EMULATE_EXECUTION_AS_SERVICE
      //DEBUG("error in initialization of the service--returning\n" )
  }
}

void CPUcontrolService::HandlePowerEvent(DWORD dwEventType )
{
  LOGN("power event")
  //"If dwControl is SERVICE_CONTROL_POWEREVENT, this parameter can be one of
  //the values specified in the wParam parameter of the WM_POWERBROADCAST
  //message."
  //http://msdn.microsoft.com/en-us/library/aa373247%28v=VS.85%29.aspx:
  switch( //(int)
      dwEventType )
  {
    //case PBT_APMPOWERSTATUSCHANGE:
  case PBT_APMRESUMESUSPEND:
     //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/pbt_apmresumesuspend.htm:
     //"An application can receive this event only if it received the PBT_APMSUSPEND event before the computer was suspended. Otherwise, the application will receive a PBT_APMRESUMECRITICAL event."
  case PBT_APMRESUMECRITICAL:
       // handle event
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/handlerex.htm:
    //"If your service handles SERVICE_CONTROL_POWEREVENT, return NO_ERROR to grant the request and an error code to deny the request."
      //for(unsigned char by = 0 ; by < 255 ; by ++ )
      //{
      //  ::MessageBeep(15000);
      //  ::Sleep(400);
      //}
    if( msp_cpucontrolservice->
        m_bProcess
      )
      msp_cpucontrolservice->mp_cpucontroller->
        ResumeFromS3orS4();
      //After ACPI S3/ S4 the performance event select
      //is cleared and has to be written again.
      msp_cpucontrolservice->mp_cpucoreusagegetter->Init() ;
      //return NO_ERROR;
      break ;
  case PBT_APMQUERYSUSPEND:
   LOGN("querying suspend")
     //TODO Before a hibernation set core 1 to a low freq and voltage
     //to save current because I guess only the core 0 saves the
     //RAM onto disk:
     break;
  default:
   LOGN("other power event")
   break ;
  }
}

void CPUcontrolService::HandleStartDynVoltAndFreqScalingThread()
{
  msp_cpucontrolservice->m_powerprofdynlinked.DisableFrequencyScalingByOS();
  //Windows_API::DynFreqScalingThread dynfreqscalingthread(
  msp_cpucontrolservice->mp_dynfreqscalingthreadbase =
  #ifdef USE_WINDOWS_THREAD
    new Windows_API::DynFreqScalingThread(
  #else //USE_WINDOWS_THREAD
    new wxWidgets::DynFreqScalingThread(
  #endif //USE_WINDOWS_THREAD
    msp_cpucontrolservice->mp_cpucontroller->mp_icpucoreusagegetter
    , msp_cpucontrolservice->mp_cpucontroller
    , msp_cpucontrolservice->mp_modelData->m_cpucoredata
    ) ;
  bool bContinue = false ;
  if( msp_cpucontrolservice->mp_dynfreqscalingthreadbase )
  {
    LOGN("trying to start the Dynamic Voltage and Frequency Scaling thread.")
    //dynfreqscalingthread.Run() ;
    DWORD dwRet = msp_cpucontrolservice->mp_dynfreqscalingthreadbase->Start() ;
    if( dwRet == ERROR_SUCCESS )
    {
      LOGN( "Successfully started the Dynamic Voltage and Frequency Scaling "
          "thread.")
      bContinue = true ;
    }
    else
    {
      LOGN( "error starting Dynamic Voltage and Frequency Scaling thread :"
        << LocalLanguageMessageFromErrorCodeA(dwRet) )
    }
  }
  if( bContinue )
  {
    LOGN("Waiting for the stop service condition to become true")
    LOGN("service main--current thread id:" << ::GetCurrentThreadId() )
    msp_cpucontrolservice->m_vbServiceInitialized = true ;
    //If not wait the thread object is destroyed at end of block.
    ::WaitForSingleObject( msp_cpucontrolservice->
      m_hEndProcessingEvent, INFINITE) ;
  }
}

void CPUcontrolService::Initialize()
{
  m_vbServiceInitialized = false ;
  m_bProcess = true ;
  m_bRun = true ;
  mp_cpucontroller = NULL ;
  //mp_winring0dynlinked = NULL ;
  mp_i_cpuaccess = NULL ;
  mar_tch = NULL ;
  m_stdstrServiceName = "CPUcontrolService" ;
  mp_dynfreqscalingthreadbase = NULL ;

  DEBUG("begin of constructor of service object\n");
  m_bProcess = true ;
  m_bRun = true ;
#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  mp_voidMappedViewStartingAddress = NULL ;
  m_handleMapFile = NULL ;
#endif //#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
  mp_modelData = NULL ;
  m_stdstrSharedMemoryName = "CPUcontrolService" ;

  m_hEndProcessingEvent = CreateEvent( 
      NULL,         // default security attributes
      TRUE,         // manual-reset event
      FALSE,         // initial state is non-signaled
      "EndEvent"  // object name
      ); 
  //This method is form: http://msdn.microsoft.com/en-us/library/ms810429.aspx
  // Copy the address of the current object so we can access it from
  // the static member callback functions.
  // WARNING: This limits the application to only one CNTService object. 
  msp_cpucontrolservice = this;
  //m_winring0dynlinked.SetUserInterface(&m_dummyuserinterface);
  //DEBUG("end of constructor of service object\n");
}

//Pass ERROR_SUCCESS (0L) or NO_ERROR (0L) if no error
// Stub initialization function. 
DWORD CPUcontrolService::MyServiceInitialization(
  DWORD   argc, 
  LPTSTR  * argv
  ) 
{ 
  //DWORD dwReturnValue = ERROR_SUCCESS ;
  DWORD dwReturnValue = ! NO_ERROR ;
  //argv; 
  LPTSTR  ** ppartch ;
  //std::vector<std::string> stdvecstdstrFurtherCmdLineArgs ;
  //argc; 
  //DEBUG("initializing the service--begin\n",argc)
  LOGN( "initializing the service--begin. argument count:" << argc )
  //specificError; 
  if( argc < 2 )
  {
    LPTSTR ptchFirstArg
      //Initialize to avoid g++ warning (too many warnings are unuebersichtlich)
      = NULL ;
      //TCHAR artchDefaultArg[] = "-config=GriffinControl_config.xml" ;
          //"-config=C:\\Users\\Public\\config.xml" ;
      //for( BYTE by = 0 ; by < argc ; ++by )

      if( argc == 1 )
          ptchFirstArg = argv[0] ;
      //free(argv[by]);
      //It crashed here: tried to free an array that hasn't been allocated 
      //on the heap.
      //free(argv);
      //argv = new char * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
      mar_tch = new char * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
      argc = NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ;
      m_stdtstrProgramArg = std::tstring("-config=") + //m_stdwstrProgramName + 
        m_stdtstrProgramName + 
        std::tstring("_config.xml") ;
      //if( argc == 1 )
          //argv[0] = ptchFirstArg ;
          mar_tch[0] = ptchFirstArg ;
      //argv[1] = new char [ strlen(artchDefaultArg) + 1 ] ;
      //mar_tch[1] = new char [ strlen(artchDefaultArg) + 1 ] ;
      //mar_tch[1] = "-config=CPUcontrol_config.xml" ;
      //mar_tch[1] = "-config=GriffinControl_config.xml" ;
      mar_tch[1] = (LPTSTR) m_stdtstrProgramArg.c_str() ;
      //strcpy(argv[1],artchDefaultArg);
      //strcpy(mar_tch[1],artchDefaultArg);
      ppartch = & mar_tch ;
      //stdvecstdstrFurtherCmdLineArgs.push_back(
  }
  else
      ppartch = & argv ;
  //Intension for this try-block is: 
  //The program should not continue if initializing the CPU acces failed.
  try 
  {
    //LOG here because: 
    //A user sent me a log file because the service crashed around this
    //place.
    LOG("Before initializing the CPU access\n") ;
    //msp_cpucontrolservice->
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
    //::SetServiceStatus (msp_cpucontrolservice->
    //    m_service_status_handle, & msp_cpucontrolservice->
    //    m_servicestatus); 
    //Use parametrized constructor, because there were runtime errors with 
    //parameterless c'tor.
    //mp_winring0dynlinked = new //WinRing0dynLinked(
    mp_i_cpuaccess = new
      WinRing0_1_3RunTimeDynLinked(
      & msp_cpucontrolservice->m_dummyuserinterface ) ;
    DEBUGN("MyServiceInitialization() CPU access address: " << mp_i_cpuaccess )
    mp_modelData = //new Model() ;
        & m_modelData ;
    LOGN("Address of service attributes: " << mp_modelData)
    if( mp_modelData )
    {
      //Important! because the DLLs assign their pointers to the model
      //from the I_CPUaccess::mp_model
      mp_i_cpuaccess->mp_model = mp_modelData ;

      //This assignment is needed for "CPUID" calls
      m_maincontroller.SetCPUaccess( //mp_winring0dynlinked) ;
        mp_i_cpuaccess ) ;
      m_maincontroller.Init( 
        //m_modelData, 
        * mp_modelData 
        , & msp_cpucontrolservice->m_dummyuserinterface );
      std::string stdstrServiceCfgFile = "service.xml" ;
      SAX2ServiceConfigHandler sax2serviceconfighandler(
        * mp_modelData ,
        & msp_cpucontrolservice->m_dummyuserinterface );
      if( readXMLConfig(
          stdstrServiceCfgFile.c_str()
          , *mp_modelData
//          , p_userinterface
          , & msp_cpucontrolservice->m_dummyuserinterface
          //Base class of implementing Xerces XML handlers.
          //This is useful because there may be more than one XML file to read.
          //So one calls this functions with different handlers passed.
          //DefaultHandler & r_defaulthandler
          , sax2serviceconfighandler
          )
        )
      {
//        byRet = 1 ;
      }
      else
      {
        //std::tstring tstr(
        //  _T("Running this program is unsafe because theres was an error ") ) ;
        //tstr +=  _T("with the file containg the maximum voltages (") ;
        //tstr = tstr + strProcessorFilePath ;
        //tstr += _T(")") ;
        //throw VoltageSafetyException(
        //  tstr ) ;
//        byRet = 1 ;
      }
      if( mp_modelData->m_cpucoredata.
        m_stdsetvoltageandfreqDefault.size() == 0
        //mp_stdsetvoltageandfreqDefault->size() == 0
        )
      {
        LOGN("no default voltages specified->no overvoltage protection->exiting");
        return 1 ;
      }
      if( mp_modelData->m_cpucoredata.
        m_stdsetvoltageandfreqWanted.size() == 0 
        //mp_stdsetvoltageandfreqWanted->size() == 0 
        )
      {
        LOGN("no preferred voltages specified->exiting");
        return 1 ;
      }
      //mp_userinterface = //p_frame ;
      //  & msp_cpucontrolservice->m_dummyuserinterface ;
      //The controller must be created before the main frame because 
      //its view depends of values retrieved from the controller
      //(e.g. for every core a single menu)
      //msp_cpucontrolservice->
      //  mp_cpucontroller = 
      //  m_maincontroller.CreateCPUcontrollerAndUsageGetter(
      //    mp_cpucoreusagegetter ) ;
      //if( !
        m_maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter( 
          mp_cpucontroller 
          , mp_cpucoreusagegetter
          ) ;
        //)
      if( ! mp_cpucontroller )
      {
        if( ! mp_cpucoreusagegetter )
        {
          LOGN("no CPU controller and no usage getter->exiting");
          return 1 ;
        }
        else
        {
          LOGN("no CPU controller ->exiting");
          return 1 ;
        }
      }
      if( ! mp_cpucoreusagegetter )
      {
        LOGN("no CPU controller and no usage getter->exiting");
        return 1 ;
      }
      SetCPUcontroller(mp_cpucontroller) ;
      mp_cpucontroller->SetCmdLineArgs(
        NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
        *ppartch ) ;
      mp_cpucontroller->SetUserInterface( 
        & msp_cpucontrolservice->m_dummyuserinterface ) ;
      //Set the CPU access BEFORE getting number of CPU cores in
      //SetModelData(...) .
      mp_cpucontroller->SetCPUaccess( //mp_winring0dynlinked) ;
        mp_i_cpuaccess ) ;
      mp_cpucontroller->SetModelData( //& m_modelData
        mp_modelData ) ;
      //mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      //mp_cpucontroller->SetOtherDVFSaccess(& m_powerprofdynlinked) ;

      //mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) 
      mp_cpucontroller->SetOtherDVFSaccess( & m_powerprofdynlinked ) ;
      //m_modelData.SetGriffinController(mp_pstatectrl) ;
      //m_modelData.SetCPUcontroller( mp_cpucontroller);
      mp_modelData->SetCPUcontroller( mp_cpucontroller);

      //mp_cpucontroller->SetCmdLineArgs(
      //  NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
      //  m_arartchCmdLineArgument
      //  ) ;

      //DEBUG("initializing the service--end\n",argc)
      LOG( "End of initializing the service\n" //<< argc 
          )
//      if( byReturn == SUCCESS )
//        //It does not make much sense to run this service if not voltages
//        //are changed/ set due to an error.
//        dwReturnValue = NO_ERROR ;
      dwReturnValue = NO_ERROR ;
      }
  }
  //catch(DLLnotLoadedException e)
  catch(CPUaccessException cpuaccessexception)
  {
    //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    //    "Exception: DLL was not loaded\n" ) ;
    dwReturnValue = ! NO_ERROR ;
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/interactive_services.htm:
    //"Display a message box by calling the MessageBox function with
    //MB_SERVICE_NOTIFICATION. This is recommended for displaying simple 
    //status messages. Do not call MessageBox during service initialization 
    //or from the HandlerEx routine, unless you call it from a separate 
    //thread, so that you return to the SCM in a timely manner."

    ::MessageBox(
      //If this parameter is NULL, the message box has no owner window.
      NULL
      , cpuaccessexception.m_stdstrErrorMessage.c_str()
      //title bar
      //, "error initializing the CPU access"
      //, strProgramName.c_str()
//      , getstdstring( m_stdwstrProgramName ).c_str()
      , m_stdtstrProgramName.c_str()
      //, "CPUcontrol service error"
      , MB_SERVICE_NOTIFICATION
      ) ;
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
    //delete e ;
  }
  return dwReturnValue; 
}

void CPUcontrolService::FillCmdLineOptionsList()
{
  if( m_vecstrCmdLineOptions.empty() )
  {
    m_vecstrCmdLineOptions.push_back("with no command line options: "
        "open console window requesting input for the desired operation\n"
        "(to install or deinstall/ delete)") ;
    m_vecstrCmdLineOptions.push_back("-d >>service name<< Deinstall/ Delete "
        "(this) Windows service named >>service name<<");
    m_vecstrCmdLineOptions.push_back("-i >>service name<< Install "
        "this Windows service as name >>service name<<");
  }
}

#ifdef COMPILE_WITH_IPC
DWORD CPUcontrolService::IPC_Message(
  BYTE byCommand
//  std::wstring & stdwstrMessage
  , BYTE * & r_arbyPipeDataToSend
 )
{
  DWORD dwByteSize = 0 ;
  LOGN("IPC message: " << (WORD) byCommand )
  //wide string because the power scheme may need it (e.g. for Chinese power
  // scheme names with > 256 chars in charset)
  std::wstring stdwstrMessage ;
  switch(byCommand)
  {
  case get_current_CPU_data :
    LOGN("IPC: get_current_CPU_data")
    r_arbyPipeDataToSend = m_ipc_datahandler.GetCurrentCPUcoreAttributeValues(
      dwByteSize ) ;
    break ;
  case stop_service:
    LOGN("IPC requested to stop the service")
    //msp_cpucontrolservice->mp_dynfreqscalingbase->m_Stop() ;
    //if( mp_dynfreqscalingthreadbase ) 
    //   mp_dynfreqscalingthreadbase->Stop() ;
    //::SetEvent( msp_cpucontrolservice->m_hEndProcessingEvent );
    Stop() ;
    break ;
  case pause_service:
    LOGN("IPC requested to pause the service")
    bool bAlreadyPaused = Pause() ;
    if( bAlreadyPaused )
      stdwstrMessage = L"the dynamic voltage and frequency scaling thread is "
        "ALREADY stopped" ;
    else
      stdwstrMessage = L"the dynamic voltage and frequency scaling thread is "
        "stopped" ;
    LOGWN_WSPRINTF(stdwstrMessage.c_str() )
    dwByteSize = stdwstrMessage.size() * 2 ;
    r_arbyPipeDataToSend = new BYTE [ dwByteSize ] ;
    memcpy( r_arbyPipeDataToSend, stdwstrMessage.data(), dwByteSize ) ;
    break;
  case continue_service:
    LOGN("IPC requested to continue the service")
    bool bAlreadContinued = Continue() ;
    if( bAlreadContinued )
      stdwstrMessage = L"the dynamic voltage and frequency scaling thread is "
        "ALREADY running" ;
    else
      stdwstrMessage = L"the dynamic voltage and frequency scaling thread is "
        "running" ;
    LOGWN_WSPRINTF(stdwstrMessage.c_str() )
    dwByteSize = stdwstrMessage.size() * 2 ;
    r_arbyPipeDataToSend = new BYTE [ dwByteSize ] ;
    memcpy( r_arbyPipeDataToSend, stdwstrMessage.data(), dwByteSize ) ;
    break;
  case stop_DVFS:
    //if( mp_dynfreqscalingthreadbase ) 
    //   mp_dynfreqscalingthreadbase->Stop() ;
    Stop() ;
    break ;
  case start_DVFS:
    StartDynVoltnFreqScaling() ;
    break ;
  case setVoltageAndFrequency:
    //Setting the voltage and frequency means that it should not change. If
    //the DVFS thread would run it would surely be changed afterwards.
    //So stop the DVFS thread if it is running.
    if( mp_dynfreqscalingthreadbase ) 
       mp_dynfreqscalingthreadbase->Stop() ;
    //GetVoltage(fVoltage);
    //GetFrequency(wFrequency);
    break ;
  case sync_GUI_show_data_with_service:
    mp_modelData->m_bSyncGUIshowDataWithService = true ;
    break ;
  }
//  m_ipc_datahandler.GetResponse( byCommand ) ;
  return dwByteSize ;
}
#endif //#ifdef COMPILE_WITH_IPC

SERVICE_STATUS_HANDLE CPUcontrolService::RegSvcCtrlHandlerAndHandleError()
{
  DWORD dwLastError ;
  //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
  //"The ServiceMain function should immediately call the
  //RegisterServiceCtrlHandlerEx function to specify a HandlerEx
  //function to handle control requests.
  msp_cpucontrolservice->
    //"If the function fails, the return value is zero."
    m_service_status_handle =
    //RegisterServiceCtrlHandler(
    //"GriffinControlService",
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
    RegSvcCtrlHandlerExAndGetErrMsg ( //"GriffinControlService",
      msp_cpucontrolservice->m_stdtstrProgramName.c_str() ,
      ServiceCtrlHandlerEx,
      //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
      //"lpContext [in, optional]
      //Any user-defined data. This parameter, which is passed to the
      //handler function, can help identify the service when multiple
      //services share a process."
      (LPVOID) 2 ,
      //stdstrErrorDescripition
      dwLastError
      );
  if ( msp_cpucontrolservice->m_service_status_handle ==
      (SERVICE_STATUS_HANDLE) 0 )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "Registering the service contrl handler failed; "
      "error code: " << dwLastError );
    std::string stdstrErrorDescription ;
    ServiceBase::GetErrorDescriptionFromRegSvcCtrlHandlerExErrCode(
      dwLastError ,
      stdstrErrorDescription ) ;
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      stdstrErrorDescription )
  }
  return msp_cpucontrolservice->m_service_status_handle ;
}


bool CPUcontrolService::IsWithinStrings(
    const std::vector<std::string> & vecstdstrParams
    , const std::string & cr_stdstrToCompare )
{
    bool bIsWithinStrings = false ;
    for( WORD wIndex = 0 ; wIndex < vecstdstrParams.size() ; ++ wIndex )
    {
        if( vecstdstrParams.at(wIndex) == cr_stdstrToCompare )
        {
            bIsWithinStrings = true ;
            break ;
        }
    }
    return bIsWithinStrings ;
}

void CPUcontrolService::outputUsage()
{
    //std::cout << "This program is an undervolting program for AMD family 17 CPUs.\n"
    //    "This executable is both in one: 
    //    "-an installer for the undervolting service\n"
    //    "-the undervolting service itself\n" ;
    FillCmdLineOptionsList() ;
    std::cout << "Usage"
        "(pass this as command line options)"
        ":\n" ;
    for(WORD wIndex = 0 ; wIndex < m_vecstrCmdLineOptions.size() ; ++ wIndex )
    {
        std::cout << m_vecstrCmdLineOptions[wIndex] << "\n";
    }
}

//void CPUcontrolService::CreateStringVector(
//    stdstrInput = std::string(arch) ;
//{
//
//}

void CPUcontrolService::CreateStringVector(
  //char arch[] , 
  std::string stdstrInput ,
  std::vector<std::string> & vecstdstrParams 
  )
{
    bool bAnfZeichen = false ;
    bool bStringSeperator = false ;
    bool bWhitespace = false ;
    WORD wBeginOfSubstring = NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE ;
    WORD wNumberOfSubstringChars = 0 ;

    //DEBUG("whole string length: %u\n", stdstrInput.length() );
    LOG( "whole string length: " << stdstrInput.length() << "\n"  );
    for( WORD wIndex = 0 ; wIndex < stdstrInput.length(); ++ wIndex )
    {
        switch ( stdstrInput[ wIndex ] )
        {
        case ' ':
        case '\t':
            bWhitespace = true ;
            ////Only if NOT the 1st of 2 AnfZeichen take a substring.
            ////(-> a anf-zeichen is there to allow a single string that 
            ////even includes white spaces)
            //if( ! bAnfZeichen )
            //    vecstdstrParams.push_back( 
            //        stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
            //        wIndex - wBeginOfSubstring + 1 ) 
            //      ) ;
            bStringSeperator = true ;
            break ;
        case '\"':
            if( 
                //If already an AnfZeichen, then this AnfZeichen is the closing
                //AnfZeichen.
                bAnfZeichen )
            {
                //vecstdstrParams.push_back( 
                //    stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
                //    wIndex - wBeginOfSubstring + 1 );
                bStringSeperator = true ;
            }
            else // 1st Anfzeichen of pair of 2 Anfzeichen.
                //The substring starts directly after this Anfuehrungs-zeichen.
                wBeginOfSubstring = wIndex + 1 ;
            //Invert value of variable.
            //bAnfZeichen != bAnfZeichen ;
            bAnfZeichen = !bAnfZeichen ;
            break ;
        default:
            bWhitespace = false ;
            bStringSeperator = false ;
            if ( wBeginOfSubstring == 
                NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE )
                wBeginOfSubstring = wIndex ;
        }
        //DEBUG("character: %c index:%u\n", stdstrInput[ wIndex ], wIndex ) ;
        LOG( "character: " << stdstrInput[ wIndex ] << "index:" << wIndex << 
            "\n" ) ;
        if( bStringSeperator )
            wNumberOfSubstringChars = wIndex - wBeginOfSubstring ;
        if( 
            //Last string character 
            wIndex == stdstrInput.length() - 1 
            )
            wNumberOfSubstringChars = wIndex - wBeginOfSubstring + 1;
        if( wNumberOfSubstringChars )
            //Only if NOT the 1st of 2 AnfZeichen take a substring.
            //(-> a anf-zeichen is there to allow a single string that 
            //even includes white spaces)
            if( ! bAnfZeichen )
            {
                DEBUG("string seperator or end of whole string\n");
                vecstdstrParams.push_back( 
                    stdstrInput.substr( wBeginOfSubstring , //wNumberOfChanrs
                    //wIndex - wBeginOfSubstring + 1 
                    wNumberOfSubstringChars ) 
                  ) ;
                wBeginOfSubstring = NO_BEGIN_OF_STRING_FOR_16BIT_UNSIGNED_DATATYPE ;
                wNumberOfSubstringChars = 0 ;
            }
    }
}

std::string CPUcontrolService::GetValueIfHasPrefix( 
    const std::string & r_stdstrPrefix
    )
{
    std::string stdstrValue ;
    for( BYTE byCommandLineArgumentIndex = 0 ; 
        byCommandLineArgumentIndex < m_vecstdstrCommandLineArgs.size() ;
        ++ byCommandLineArgumentIndex )
    {
        const std::string & cr_stdstrCommandlineArgument = 
            m_vecstdstrCommandLineArgs.at( byCommandLineArgumentIndex ) ;
        std::string::size_type stdstrsize_typeStartPos =
            //m_vecstdstrCommandLineArgs.at( byCommandLineArgumentIndex ).
            cr_stdstrCommandlineArgument.
            //Returns: The position of the first occurrence in the string of the searched content.
            find(r_stdstrPrefix) ;
        if( stdstrsize_typeStartPos
            // The string was found at index 0.
            == 0 
            )
        {
            stdstrValue = cr_stdstrCommandlineArgument.substr(
                //Copy from 1st char after prefix until the end of string.
                r_stdstrPrefix.length() 
                ) ;
            break ;
        }
        if(
            //Because this is a single command line argument that is a 
            //combination of attribute name 
            //and attribute value (e.g. "path=C:\Program Files\Griffin_svc.txt"
            //a '"' may precede the prefix.
            stdstrsize_typeStartPos == 1 && stdstrValue[0] == '\"'
          )
        {
            stdstrValue = cr_stdstrCommandlineArgument.substr(
                //Copy from 1st char after prefix until the end of string.
                r_stdstrPrefix.length() + 1
                , cr_stdstrCommandlineArgument.length() - 
                //Because there is a beginning '"' I also assume an ending
                //'"' -> 
                // e.g.:  >>"path=C:\out .txt"<< = 18 chars
                //   >>path=C:\out .txt<< = 16 chars.
                //   prefix >>path=<< = 5 chars.
                //   18 - ( 5 + 2 ) = 18 - 7 = 11 <=> >>C:\out .txt<<
                ( r_stdstrPrefix.length() 
                    //For the '"'
                    + 2 )
                ) ;
            break ;
        }
    }
    return stdstrValue ;
}

std::string CPUcontrolService::GetLogFilePath()
{
    return GetValueIfHasPrefix( _T("log_file_path=") ) ;

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
//            //Returns: The position of the first occurrence in the string of the searched content.
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

/**@ return true: already paused*/
bool CPUcontrolService::Pause()
{
  bool bAlreadyPaused = false ;
   msp_cpucontrolservice->m_servicestatus.dwCurrentState = 
     SERVICE_PAUSED; 
   msp_cpucontrolservice->m_bProcess = false ;
  if( mp_dynfreqscalingthreadbase )
  {
    if( mp_dynfreqscalingthreadbase->IsStopped()
        )
    {
      bAlreadyPaused = true ;
      LOG("Service is ALREADY paused.\n");
    }
    else
    {
      mp_dynfreqscalingthreadbase->Stop() ;
      LOG("Service is paused.\n");
    }
  }
   SetServiceStatus() ;
   return bAlreadyPaused ;
}

void CPUcontrolService::requestOption(
    //Make as parameter as reference: more ressource-saving than
    //to return (=a copy).
    std::vector<std::string> & r_vecstdstrParams 
    , std::tstring & r_tstrProgName )
{
    bool bContinue = false ;
    char arch[101] ;
    std::string stdstrInput ;
    //std::string  stdstrDefaultProcessName = "GriffinStateControl" ;
    std::string stdstrDefaultProcessName = //"CPUcontrolService" ;
      GetStdString( r_tstrProgName ) ;
    //std::cout << "You can input your choice (deinstall/ install) now: "
    //    "same format as if command line option:\n" ;
    std::cout << "\nSelect your option now:\n"
        "[i/I]nstall this program as service\n" 
        "[d/D]einstall/ [d/D]elete (this) service\n"
        "[r/R]einstall (this) service (delete->install)\n"
        ;
    int nChar = getche();
    std::cout << "\n" ;
    switch( toupper(nChar) )
    {
    case 'I':
        std::cout << "You choosed to install the service\n" ;
        std::cout << "Now input the name for the service. "
          "Input no text to choose the default name \"" << 
          stdstrDefaultProcessName << 
          "\"\nPress ENTER/ Return to finish.\n" ;
        r_vecstdstrParams.push_back(_T("-i") );
        //Valid input char->continue.
        bContinue = true ;
        break ;
    case 'D':
        std::cout << "You choosed to delete the service\n" ;
        std::cout << "Now input the name for the service to delete. "
          "Input no text to choose the default name \"" << 
          stdstrDefaultProcessName << 
          "\"\nPress ENTER/ Return to finish.\n" ;
        r_vecstdstrParams.push_back(_T("-d") );
        //Valid input char->continue.
        bContinue = true ;
        break ;
    case 'R':
      std::cout << "You choosed to reinstall the service\n" ;
      std::cout << "Now input the name for the service to delete and install afterwards. "
        "Input no text to choose the default name \"" <<
        stdstrDefaultProcessName <<
        "\"\nPress ENTER/ Return to finish.\n" ;
      r_vecstdstrParams.push_back(_T("-d") );
      //Valid input char->continue.
      bContinue = true ;
      break ;
    default: 
        std::cout << "You did not input a valid option\n" ;
    }
    if( bContinue )
    {
        //std::cin >> stdstrInput ;
        std::cin.getline( //::getline(std::cin, 
            //stdstrInput 
            arch , 100 ) ; 
        stdstrInput = std::string ( arch ) ;
//        if( stdstrInput.empty() )
//            r_vecstdstrParams.push_back( stdstrDefaultProcessName ) ;
//        else
//            r_vecstdstrParams.push_back( stdstrInput ) ;
        std::string & r_stdstrServiceName = stdstrInput.empty() ?
          stdstrDefaultProcessName : stdstrInput ;
        r_vecstdstrParams.push_back( r_stdstrServiceName ) ;
        if ( toupper(nChar) == 'R' )
        {
          r_vecstdstrParams.push_back( _T("-i") ) ;
          r_vecstdstrParams.push_back( r_stdstrServiceName ) ;
        }
    }

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
}

//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/startservicectrldispatcher.htm:
//"All initialization tasks are done in the service's ServiceMain function
//when the service is started."
void WINAPI //MyServiceStart 
    CPUcontrolService::ServiceMain(DWORD argc, LPTSTR *argv) 
{ 
    BYTE byArgIndex = 0 ;
    DWORD dwStatus; 
    std::string stdstrErrorDescripition ;
//    DWORD specificError;
#ifndef EMULATE_EXECUTION_AS_SERVICE
    if( //if error occurred
        ! msp_cpucontrolservice->RegSvcCtrlHandlerAndHandleError() )
      return ;
#endif //#ifndef EMULATE_EXECUTION_AS_SERVICE
 
    msp_cpucontrolservice->SetServiceStatusAttValues() ;

#if defined( _DEBUG ) && !defined(EMULATE_EXECUTION_AS_SERVICE)
  //Give time to attach a debugger to THIS process if it was not started 
  //from Visual Studio.
  ::Sleep( //wait time in milliseconds
    100000) ;
#endif
    //DEBUG("ServiceMain--argument count: %u\n",argc)
    LOG( "Service main--argument count: " << argc << "\n" )
    for( ; byArgIndex < argc ; ++ byArgIndex )
      //DEBUG("argument %u:%s\n", (WORD) byArgIndex, argv[byArgIndex]);
      LOG("argument " << (WORD) byArgIndex << ": " << argv[byArgIndex] << 
        "\n" );

    // Initialization code goes here. 
    //"After these calls[...,ServiceStatus()], the function should complete the initialization of the service."
    dwStatus = msp_cpucontrolservice->MyServiceInitialization(
      argc
      , argv //&specificError
      ); 
 
    msp_cpucontrolservice->HandleInitServiceFailed(dwStatus ) ;
    if( dwStatus != NO_ERROR )
    {
      LOG("error in initialization of the service--returning\n" )
      return;
    }
 
    //// Initialization complete - report running status. 
    ////ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
    ////"The Service Control Manager (SCM) waits until the service reports a status of SERVICE_RUNNING. It is recommended that the service reports this status as quickly as possible, as other components in the system that require interaction with SCM will be blocked during this time."
    ////"Furthermore, you should not call any system functions during service initialization. The service code should call system functions only after it reports a status of SERVICE_RUNNING."
    //msp_cpucontrolservice->
    //    m_servicestatus.dwCurrentState       = SERVICE_RUNNING; 
    //msp_cpucontrolservice->
    //    m_servicestatus.dwCheckPoint         = 0; 
    //msp_cpucontrolservice->
    //    m_servicestatus.dwWaitHint           = 0; 
 
    //msp_cpucontrolservice->SetServiceStatus() ;
 
    //msp_cpucontrolservice->m_ipcserver.Init() ;
    //if( msp_cpucontrolservice->mp_cpucontroller )
    //  msp_cpucontrolservice->mp_cpucontroller->EnableOwnDVFS() ;
#ifdef COMPILE_WITH_IPC
    DWORD dwThreadId ;
    //IPC_servers wait for client and are often BLOCKING, so THIS
    //block would not continue execution->start client
    //connection listening in dedicated thread.
    HANDLE handleThread =
      ::CreateThread(
      NULL,                   // default security attributes
      0,                      // use default stack size  
      IPC_ServerThread,       // thread function name
      & msp_cpucontrolservice->m_ipcserver ,// argument to thread function 
      0,                      // use default creation flags 
      & dwThreadId
      );   // returns the thread identifier
      if ( handleThread == NULL )
      {
         LOGN("creating the IPC server thread failed");
         return ;
      }
      else
      {
        //http://msdn.microsoft.com/en-us/library/ms686724%28v=VS.85%29.aspx:
        //"When a thread terminates, its thread object is not freed until all open
        //handles to the thread are closed."
        //http://msdn.microsoft.com/en-us/library/ms724211%28v=VS.85%29.aspx:
        //"Closing a thread handle does not terminate the associated thread or remove
        //the thread object."
        //Close the thread handle here (waiting for the end of the thread via
        // WaitForSingleObject() would need another thread->not so good.)
        ::CloseHandle(handleThread ) ;
      }
#endif

//    //Create after IPC was initialzed by the service. So the GUI can connect to
//    //it.
//    CreateProcess() ;
//    CreateProcessAsUser() ;
    msp_cpucontrolservice->HandleStartDynVoltAndFreqScalingThread() ;
    //CreateProcessAsUser()
    //while(1)
    //{
    //  Sleep(1000);
    //}
    // This is where the service does its work. 
    //SvcDebugOut(" From the service: Returning the main thread \n",0); 
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
        "From the service: Returning the main thread \n" ); 
    //"[...] your service should use this thread to complete whatever tasks 
    //it was designed to do. 
    //If a service does not need a thread to do its work (such as a service that only processes RPC requests), its ServiceMain function should return control to the caller. It is important for the function to return, rather than call the ExitThread function, because returning allows for cleanup of the memory allocated for the arguments.

    ////You can provide for additional cleanup by calling the RegisterWaitForSingleObject function on an event before returning. The thread that is running the ServiceMain function terminates, but the service itself continues to run. The service control handler can then signal your event when the service stops, and a thread from the thread pool executes your callback to perform any additional cleanup you need and to call SetServiceStatus with SERVICE_STOPPED. This technique is not illustrated in the sample code below, however."
    //while( WaitForSingleObject( msp_cpucontrolservice->
    //    m_hEndProcessingEvent,1000) == WAIT_TIMEOUT )
    //{
    //    //if(msp_cpucontrolservice->
    //    //    m_bProcess)
    //    //    ::MessageBeep(5000);
    //    //::Sleep(1000);
    //    LOG( "Ending the main service thread\n" )
    //}
    //WaitForSingleObject( msp_cpucontrolservice->
    //    m_hEndProcessingEvent,INFINITE);
    //synchr. with "ServiceCtrlHandlerEx"
    //p_eventStopped.Set();
    //DEBUG("Service main function--end\n",argc)
    LOG( "Service main function--end\n" )
    return; 
}

void CPUcontrolService::SetCommandLineArgs( int argc, char *  argv[] )
{
    for( BYTE byCommandLineArgumentIndex = 0 ; 
        byCommandLineArgumentIndex < argc ;
        ++ byCommandLineArgumentIndex )
        m_vecstdstrCommandLineArgs.push_back(std::string(argv[argc] ) ) ;
}

void CPUcontrolService::SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew )
{
  if( p_cpucontrollerNew )
  {
//        if( mp_cpucontroller )
//        {
//          //Release memory.
//          delete mp_cpucontroller ;
//          LOGN(" current CPU controller deleted")
//        }
    LOGN("address of model: " << mp_modelData )
    mp_cpucontroller = p_cpucontrollerNew ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( p_cpucontrollerNew ) ;
    LOGN("after setting CPU controller for usage getter")
    mp_cpucontroller->SetModelData( //& m_modelData
       mp_modelData ) ;
    LOGN("before GetMaximumFrequencyInMHz. number of CPU cores: " <<
        (WORD) mp_modelData->m_cpucoredata.GetNumberOfCPUcores() )
    //Needed for drawing the voltage-frequency curves.
    WORD w = mp_cpucontroller->GetMaximumFrequencyInMHz() ;
    LOGN("after GetMaximumFrequencyInMHz: " << w )
  }
}

void CPUcontrolService::SetServiceStatus()
{
#ifndef EMULATE_EXECUTION_AS_SERVICE
   if ( ! ::SetServiceStatus (
     msp_cpucontrolservice->
     //this handle is the return value of "RegisterServiceCtrlHandlerEx()"
     m_service_status_handle, 
     & msp_cpucontrolservice->m_servicestatus )
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

void CPUcontrolService::SetServiceStatusAttValues()
{
  msp_cpucontrolservice->
      m_servicestatus.dwServiceType        = SERVICE_WIN32;
  msp_cpucontrolservice->
      m_servicestatus.dwCurrentState       = //SERVICE_START_PENDING;
      SERVICE_RUNNING ;
  msp_cpucontrolservice->
      m_servicestatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP
      | SERVICE_ACCEPT_PAUSE_CONTINUE
  //http://msdn.microsoft.com/en-us/library/ms810440.aspx:
  //"To accept these new messages[SERVICE_CONTROL_POWEREVENT], the
  //appropriate flags must be submitted using SetServiceStatus. In the
  //SERVICE_STATUS structure, the dwControlsAccepted member should be
  //updated to reflect the desired messages. The bitwise flags are:
  //[...] SERVICE_ACCEPT_POWEREVENT"
      | SERVICE_ACCEPT_POWEREVENT
      | SERVICE_ACCEPT_SESSIONCHANGE
      ;
  msp_cpucontrolservice->
      m_servicestatus.dwWin32ExitCode      = 0;
  msp_cpucontrolservice->
      m_servicestatus.dwServiceSpecificExitCode = 0;
  msp_cpucontrolservice->
    //Check-point value the service increments periodically to report its
    //progress during a lengthy start, stop, pause, or continue operation.
    //For example, the service should increment this value as it completes
    //each step of its initialization when it is starting up. The user
    //interface program that invoked the operation on the service uses this
    //value to track the progress of the service during a lengthy operation.
    //This value is not valid and should be zero when the service does not
    //have a start, stop, pause, or continue operation pending.
      m_servicestatus.dwCheckPoint         = 0;
  msp_cpucontrolservice->
    //Estimated time required for a pending start, stop, pause, or
    //continue operation, in milliseconds. Before the specified amount
    //of time has elapsed, the service should make its next call to the
    //SetServiceStatus function with either an incremented dwCheckPoint
    //value or a change in dwCurrentState. If the amount of time
    //specified by dwWaitHint passes, and dwCheckPoint has not been
    //incremented or dwCurrentState has not changed, the service
    //control manager or service control program can assume that an error
    //has occurred and the service should be stopped.
    m_servicestatus.dwWaitHint           = 0;

  //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
  //The Service Control Manager (SCM) waits until the service reports a
  //status of SERVICE_RUNNING. It is recommended that the service
  //reports this status as quickly as possible, as other components in
  //the system that require interaction with SCM will be blocked during this time.
  msp_cpucontrolservice->SetServiceStatus() ;
}

bool CPUcontrolService::ShouldCreateService(
    const std::vector<std::string> & cr_vecstdstrParams )
{
    bool bShouldCreateService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-i" )
    if ( IsWithinStrings( cr_vecstdstrParams , "-i" ) 
        )
        bShouldCreateService = true ;

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
    return bShouldCreateService ;
}

bool CPUcontrolService::ShouldDeleteService(
    const std::vector<std::string> & vecstdstrParams )
{
    bool bShouldDeleteService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-d" )
    if ( IsWithinStrings(vecstdstrParams, "-d" ) 
        )
       bShouldDeleteService = true ;

    return bShouldDeleteService ;
}

bool CPUcontrolService::StartDynVoltnFreqScaling()
{
  bool bAlreadyContinued = false ;
  if( ! mp_dynfreqscalingthreadbase ) 
    mp_dynfreqscalingthreadbase = new
#ifdef USE_WINDOWS_THREAD
    Windows_API::DynFreqScalingThread(
#else //#ifdef USE_WINDOWS_THREAD
      wxWidgets::DynFreqScalingThread(
#endif //#ifdef USE_WINDOWS_THREAD
      mp_cpucontroller->mp_icpucoreusagegetter
      , mp_cpucontroller
      , mp_modelData->m_cpucoredata
      ) ;
  //If allocating memory succeeded.
  if( mp_dynfreqscalingthreadbase ) 
  {
    if( mp_dynfreqscalingthreadbase->m_vbRun )
      bAlreadyContinued = true ;
    else
      mp_dynfreqscalingthreadbase->Start() ;
  }
  return bAlreadyContinued ;
}

void CPUcontrolService::StartService()
{
  DEBUG("begin of starting service\n");
  //SERVICE_TABLE_ENTRYA ("char") or SERVICE_TABLE_ENTRYW ( wchar_t )
  SERVICE_TABLE_ENTRY ar_service_table_entry[] = 
  { 
    { 
      //Pointer to a null-terminated string that specifies the name of 
      //a service to be run in this service process. 
      //If the service is installed with the SERVICE_WIN32_OWN_PROCESS 
      //service type, this member is ignored, but cannot be NULL. 
      //This member can be an empty string ("").
      //If the service is installed with the SERVICE_WIN32_SHARE_PROCESS 
      //service type, this member specifies the name of the service that 
      //uses the ServiceMain function pointed to by the lpServiceProc member.
      //m_stdstrServiceName.c_str()
      //"CPUcontrolService"
      //Convert "const char *" -> "char *", "const wchar_t *" -> "wchar_t *".
      (TCHAR *) m_stdtstrProgramName.
        //returns "const char *" (ANSI) or "const wchar_t *" (wide char)
        c_str()
      //Pointer to a ServiceMain function. 
      //MyServiceStart
      //All initialization tasks are done in the service's ServiceMain 
      //function when the service is started.
      , ServiceMain
    },
    //NULL entries mark the end of the SERVICE_TABLE_ENTRY array.
    { NULL,              NULL          }
   }; 
#ifdef EMULATE_EXECUTION_AS_SERVICE
  LPTSTR argv[1] ;
  argv[0] = "GriffinControlService" ;
  ServiceMain( 
     //m_dwArgCount,
     1,
     //m_p_lptstrArgument
     argv
     ) ;
#else
  LOGN("Before starting service ctrl dispatcher--current thread id:" <<
    ::GetCurrentThreadId()
     << "\nNote: it may take 2 minutes or even more until the service control "
     "dispatcher has finished to start")
  //The StartServiceCtrlDispatcher function connects the main thread of a 
  //service process to the service control manager, which causes the thread 
  //to be the service control dispatcher thread for the calling process.
  //In DIESEM Thread wird dann "ServiceCtrlHandlerEx" ausgeführt.
  //When the service control manager starts a service process, it waits for the process to call the StartServiceCtrlDispatcher function. The main thread of a service process should make this call as soon as possible after it starts up.
  //Starts the "ServiceMain" function in a new thread.
  if ( ! ::StartServiceCtrlDispatcher( ar_service_table_entry)
     )
  { 
    DWORD dw = ::GetLastError() ;
    std::string stdstr ;
    LOGN("StartServiceCtrlDispatcher failed:" <<
      LocalLanguageMessageAndErrorCodeA( dw ) )
    ServiceBase::GetErrorDescriptionFromStartServiceCtrlDispatcherErrCode(
      dw , stdstr) ;
    LOGN(stdstr)
  }
#endif //#ifdef EMULATE_EXECUTION_AS_SERVICE
  //The execution continues here if the service was stopped.
  //DEBUG("end of starting service\n");
  LOGN("after staring service ctrl dispatcher--current thread id:" << 
    ::GetCurrentThreadId() )
}

//This method is called by the Windows Service Control Manager.
//It is executed in the same thread that called 
//"StartServiceCtrlDispatcher".
DWORD WINAPI CPUcontrolService::ServiceCtrlHandlerEx ( 
  DWORD dwControl,  
  DWORD dwEventType, 
  LPVOID lpEventData, 
  LPVOID lpContext 
  )
{ 
  LOGN("service ctrl handler--begin control code:" << dwControl );
  LOGN("service ctrl handler--current thread id:" << 
    ::GetCurrentThreadId() )
   //DWORD status; 
   // Local variables.
   static DWORD CachedState;
   //LPEVENTINFO pEventInfo; 
//   DWORD dwBuffer;
  if( msp_cpucontrolservice->m_vbServiceInitialized )
  {
   // Do not replicate current state.
   // Controls codes greater than 127 are programmer
   //      defined service control codes.
   if(( CachedState == dwControl) && ( dwControl < 128))
      return NO_ERROR;

   switch(dwControl) 
   { 
      case SERVICE_CONTROL_PAUSE: 
      // Do whatever it takes to pause here. 
        msp_cpucontrolservice->Pause() ;
        //break;
        return NO_ERROR; 
 
      case SERVICE_CONTROL_CONTINUE: 
      // Do whatever it takes to continue here. 
        msp_cpucontrolservice->Continue() ;
        //break; 
        return NO_ERROR; 

      //if( msp_cpucontrolservice->mp_cpucontroller )
      //  msp_cpucontrolservice->mp_cpucontroller->EnableOwnDVFS() ;
      case SERVICE_CONTROL_STOP: 
        // Do whatever it takes to stop here. 
        msp_cpucontrolservice->Stop() ;
        //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/handlerex.htm
        //"For all other control codes your service handles, return NO_ERROR."
        return NO_ERROR; 
 
      case SERVICE_CONTROL_INTERROGATE: 
      // Fall through to send current status. 
         break; 
 
      case SERVICE_CONTROL_POWEREVENT:
      {
        msp_cpucontrolservice->HandlePowerEvent(dwEventType) ;
      }
      break ;
      case SERVICE_CONTROL_SESSIONCHANGE:
//        LOGN("session change event")
        //http://msdn.microsoft.com/en-us/library/ms683241%28v=VS.85%29.aspx:
        //"If dwControl is SERVICE_CONTROL_SESSIONCHANGE, this parameter can be
        //one of the values specified in the wParam parameter of the
        //WM_WTSSESSION_CHANGE message."
        //http://msdn.microsoft.com/en-us/library/aa383828%28v=VS.85%29.aspx:
        switch( dwEventType )
        {
          case WTS_SESSION_LOGON:
            LOGN("Logon event")
//          case WTS_SESSION_UNLOCK:
            //http://msdn.microsoft.com/en-us/library/ms683241%28v=VS.85%29.aspx:
            //"If dwControl is SERVICE_CONTROL_SESSIONCHANGE, this parameter is a pointer to a WTSSESSION_NOTIFICATION  structure."
            PWTSSESSION_NOTIFICATION pwtssession_notification =
                (PWTSSESSION_NOTIFICATION) lpEventData ;
            LOGN("unlock or logon event--session ID is:" <<
              pwtssession_notification->dwSessionId )
//            //If createing a
//            r_service_attributes = mp_modelData->m_service_attributes ;
//            if( r_service_attributes.m_bStartGUIonLogon )
            ServiceAttributes & r_sa =
                msp_cpucontrolservice->mp_modelData->m_serviceattributes ;
            LOGWN_WSPRINTF(L"GUI exe to start:%ls",
              r_sa.m_stdwstrPathToGUIexe.c_str() )
            if ( ! r_sa.m_stdwstrPathToGUIexe.empty() )
            {
              CreateGUIprocess create_gui_process(
                r_sa ) ;
              create_gui_process.StartProcess(
                pwtssession_notification->dwSessionId) ;
            }
            break ; //WTS_SESSION_LOGON
          //If locked, the GUI does not need to to the work to show anything.
//          case WTS_SESSION_LOCK:
        }
        break ;
     default: 
       LOGN("service control handler--call not impl.")
         //SvcDebugOut(" [MY_SERVICE] Unrecognized opcode %ld\n", 
         //    dwControl); 
         //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
         //    "Unrecognized service opcode / control code received from the "
         //    "OS/ service control manager: " << dwControl //<< "\n"
         //    ); 
         return ERROR_CALL_NOT_IMPLEMENTED ;
   } 
   LOGN("setting service status")
   //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/servicemain.htm:
   //"Next, it should call the SetServiceStatus function to send 
   //status information to the service control manager."
   msp_cpucontrolservice->SetServiceStatus() ;
   //return; 
   //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/handlerex.htm
   //"For all other control codes your service handles, return NO_ERROR. "
   return NO_ERROR ;
  }
  else
    //If your service does not handle the control, return ERROR_CALL_NOT_IMPLEMENTED. 
    return ERROR_CALL_NOT_IMPLEMENTED ;
}

void CPUcontrolService::Stop()
{
  if( mp_dynfreqscalingthreadbase ) 
     mp_dynfreqscalingthreadbase->Stop() ;
  //::SetEvent( msp_cpucontrolservice->m_hEndProcessingEvent );
   msp_cpucontrolservice->
       m_servicestatus.dwWin32ExitCode = 0; 
   msp_cpucontrolservice->
       m_servicestatus.dwCurrentState  = SERVICE_STOPPED; 
   msp_cpucontrolservice->
       m_servicestatus.dwCheckPoint    = 0; 
   msp_cpucontrolservice->
       m_servicestatus.dwWaitHint      = 0; 

   //WaitForSingleObject(m_eventStopped);
   ::SetEvent(
    msp_cpucontrolservice->m_hEndProcessingEvent 
    );

   //Inform the Service Control Manager that we have stopped.
  SetServiceStatus() ;

   //SvcDebugOut(" [MY_SERVICE] Leaving MyService \n",0); 
   WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "Leaving service" ); 
}

VOID CPUcontrolService::SvcDebugOut(LPSTR String, DWORD Status) 
{ 
   CHAR  Buffer[1024]; 
   if (strlen(String) < 1000) 
   { 
      sprintf(Buffer, String, Status); 
      OutputDebugStringA(Buffer); 
      //DEBUG("%s",Buffer);
      LOG( Buffer );
      //printf("%s",Buffer);
      //fprint()
      //m_ofstream << std::string(Buffer) ;
      //m_ofstream.flush() ;
   } 
}
