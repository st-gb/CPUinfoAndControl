//Else: "fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]"
#define _AFXDLL 
#include <AfxWin.h> //for AfxMessageBox(...)->muss MultiThreaded DLL ("/MD") sein
#include <stdio.h> //for sprintf(...)
//#include "PumaStateCtrl.h"
#include <fstream> //for class ofstream
//#include "GriffinStateControlService.hpp"
#include <Controller/stdtstr.hpp> //std::tstring
#include "Windows/CPUcontrolService.hpp"
#include "Windows/ServiceBase.hpp"
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <Windows/CurrentDir.h> // for SetExePathAsCurrentDir()
#include <string>
#include <conio.h> //for getche()

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

FILE * fileDebug ;
Logger g_logger ;

//#include "wx/window.h"
//#include "wx/power.h" //for power mgmt notification (wxPowerType et.c), EVT_POWER_RESUME

//SERVICE_STATUS          g_servicestatus; 
//SERVICE_STATUS_HANDLE   g_servicestatushandle; 
//volatile bool g_bProcess = true ;
//volatile bool g_bRun = true ;
//
////from wx/event.h:
////error C2374: 'sm_eventTableEntries': Neudefinition; Mehrfachinitialisierung
////static const wxEventTableEntry sm_eventTableEntries[]; 
////static const wxEventTable        sm_eventTable; 
////error C2270: 'GetEventTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////virtual const wxEventTable*      GetEventTable() const; 
////error C2374: 'sm_eventHashTable': Neudefinition; Mehrfachinitialisierung
////static wxEventHashTable          sm_eventHashTable; 
////error C2270: 'GetEventHashTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////virtual wxEventHashTable&        GetEventHashTable() const;
//
////void OnResume(wxPowerEvent& WXUNUSED(event)) ;
////
////const wxEventTableEntry sm_eventTableEntries[] = { 
//// #ifdef wxHAS_POWER_EVENTS
////  //EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
////  //EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
////  //EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
////#endif // wxHAS_POWER_EVENTS
////  EVT_POWER_RESUME(OnResume)
////    END_EVENT_TABLE()
////    //DECLARE_EVENT_TABLE_ENTRY( wxEVT_NULL, 0, 0, 0, 0 ) 
////  //};
////const wxEventTable sm_eventTable = { //&wxWindow::sm_eventTable, 
////    sm_eventTableEntries[0] };
//////error C2270: 'GetEventTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
//////const wxEventTable GetEventTable() const 
//////{ 
//////    return & sm_eventTable; 
//////} 
////wxEventHashTable sm_eventHashTable( sm_eventTable); 
////error C2270: 'GetEventHashTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////wxEventHashTable & GetEventHashTable() const 
////{ 
////    return sm_eventHashTable; 
////}
//
//VOID SvcDebugOut(LPSTR String, DWORD Status);
//

//std::string LocalLanguageMessageFromErrorCode(DWORD dwErrorCode)
//{
//    std::string stdstrMessage ;
//    LPVOID lpMsgBuf;
//
//    ::FormatMessage(
//        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//        FORMAT_MESSAGE_FROM_SYSTEM,
//        NULL,
//        dwErrorCode,
//        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//        (LPTSTR) &lpMsgBuf,
//        0, NULL );
// 
//    stdstrMessage = std::string((char*)lpMsgBuf) ;
//    //Release memory allocated by "::FormatMessage(...)"?
//    ::LocalFree(lpMsgBuf);
//
//    return stdstrMessage ;
//}

bool IsWithinCmdLineArgs( int argc, char *  argv[], LPCTSTR p_tstr )
{
    bool bIsWithinCmdLineArgs = false ;
    BYTE byIndex = 1 ;
    for( ; byIndex < argc ; ++ byIndex )
        if ( strcmp(argv[byIndex], p_tstr) == 0 )
        {
            bIsWithinCmdLineArgs = true ;
            break ;
        }
    return bIsWithinCmdLineArgs ;
}

bool ShouldCreateService(int argc, char *  argv[])
{
    bool bShouldCreateService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-i" )
    if ( IsWithinCmdLineArgs(argc, argv, "-i" ) 
        )
       bShouldCreateService = true ;

    if( ! bShouldCreateService )
    {
      std::cout << "\n"
          "Should the undervolting service be installed now? [Y]es/ "
        "other key: no\n"//; finish with ENTER/RETURN key\n" 
        ;
        char ch ;
        //std::cin >> ch ;
        ch = getche() ;
        if( ch == 'Y' )
            bShouldCreateService = true ;
    }
    return bShouldCreateService ;
}

bool ShouldDeleteService(int argc, char *  argv[])
{
    bool bShouldDeleteService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-d" )
    if ( IsWithinCmdLineArgs(argc, argv, "-d" ) 
        )
       bShouldDeleteService = true ;

    return bShouldDeleteService ;
}

void OuputCredits()
{
  std::tstring stdtstr ;
  std::vector<std::tstring> stdvecstdtstring ;
  MainController::GetSupportedCPUs(stdvecstdtstring) ;
  for(BYTE by = 0 ; by < stdvecstdtstring.size() ; by ++ )
    stdtstr += stdvecstdtstring.at(by) + _T(" ") ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
    //"This program is an undervolting program for AMD family 17 CPUs.\n" 
    "This program is CPU information and control program for .\n" 
    //maincontroller.GetSupportedCPUtypes() ;
    + stdtstr +
    "license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n" )
  std::cout << 
    "This executable is both in one:\n"
    "-an (de-)installer for the undervolting service\n"
    "-the undervolting service itself\n" ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    "Build time: " __DATE__ " " __TIME__ " (Greenwich Mean Time + 1)" );
}

void main( int argc, char *  argv[] ) 
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring("-config=") + ptstrProgramName + 
    std::tstring("_config.xml") ;
  std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  try
  {
    ////TODO make file path variable.
    //fileDebug = fopen ("C:\\Temp\\GriffinStateControlSvc_debug.txt","w");
      //= fopen("GriffinStateControlSvc_debug.txt","w");
    //g_logger.OpenFile( std::string("C:\\Temp\\GriffinStateControlSvc_log.txt") ) ;

    //GriffinStateControlService griffinstatecontrolservice ;
    //griffinstatecontrolservice.SetCommandLineArgs( argc, argv ) ;
    //std::string stdstrLogFilePath = 
    //    griffinstatecontrolservice.GetLogFilePath() ;
    //if( //argc > 1 && //GriffinStateControlService::PathOption(argc, argv) 
    //    stdstrLogFilePath != _T("")
    //  )
    //  g_logger.OpenFile(//griffinstatecontrolservice.GetLogFilePa 
    //        stdstrLogFilePath) ;
    //ofstream ofstreamLogFile;
    //if( 
        //ofstreamLogFile.open("GriffinContro_log.txt")
        //if( ofstreamLogFile.good() )
            
    DEBUG("Begin of main program entry point\n");
    SetExePathAsCurrentDir() ;
    //Must set the exe path as current dir before (else the file is located in
    //: C:\WINDOWS\System32) !
    g_logger.OpenFile( //std::string("GriffinControl_log.txt") 
      stdstrLogFileName ) ;

    //PossiblyOutputUsage() ;
    OuputCredits() ;
    if( argc == 1 )
    {
        //IDEA: to distinguish between if called as service or by a user:
        //get the parent process name of this process.
        //Programs that run as service have "services.exe" as their parent 
        //process.
        CPUcontrolService::outputUsage();
        std::vector<std::string> vecstdstrParams ;
//Useful for debugging purposes.
#ifdef EMULATE_EXECUTION_AS_SERVICE
#else
        CPUcontrolService::requestOption( vecstdstrParams
          , stdtstrProgramName );
#endif //#ifdef EMULATE_EXECUTION_AS_SERVICE
        if( vecstdstrParams.empty() )
        {
            LOG("This exe is started as service (and not as installer)\n");
        }
        else
        {
            DEBUG("Entered strings: ")
            for ( BYTE by = 0 ; by < vecstdstrParams.size() ; ++ by )
                //DEBUG("\"%s\" ", vecstdstrParams.at(by).c_str() );
                LOG("\"" << vecstdstrParams.at(by).c_str() << "\" " );
            DEBUG("\n" );
        }
        if( CPUcontrolService::ShouldCreateService(vecstdstrParams) )
        {
            if( vecstdstrParams.size() > 1 )
                ServiceBase::CreateService( vecstdstrParams.at(1).c_str() ) ;
        }
        else if( CPUcontrolService::ShouldDeleteService(vecstdstrParams) )
        {
            if( vecstdstrParams.size() > 1 )
            {
                ServiceBase::DeleteService(//"GriffinStateService"
                   vecstdstrParams.at(1).c_str() ) ;
                PowerProfDynLinked powerprofdynlinked( stdtstrProgramName ) ;
                powerprofdynlinked.DeletePowerScheme( stdtstrProgramName ) ;
            }
        }
        else
        {
            //GriffinStateControlService griffinstatecontrolservice ;
            CPUcontrolService cpucontrolservice(argc,argv,
              stdtstrProgramName ) ;
            cpucontrolservice.StartService();
        }
    }
    else
    {
        if( ShouldDeleteService(argc, argv) && argc > 2 )
            ServiceBase::DeleteService(//"GriffinStateService"
                argv[2] ) ;
        if( ShouldCreateService(argc, argv) && argc > 2 )
        //    CreateService(
        //cout << "hhjhj\n" ;
        //if( //::AfxMessageBox("Should the undervolting service be installed now?",
        //    //IDYES | IDNO ) == IDYES  )
           ServiceBase::CreateService(//"GriffinControlService"
                argv[2]) ;
        else
            std::cout << "\nNOT installing the service\n" ;
    }
    DEBUG("Run this program as \"local system account\" account within the "
        "service's properties in Windows->control panel->management->services "
        "else errors may occur\n");
    if( argc == 1 )
    {
        //GriffinStateControlService::outputUsage();
      //GriffinStateControlService griffinstatecontrolservice ;
       //SERVICE_TABLE_ENTRY ar_service_table_entry[] = 
       //{ 
       //   { 
       //       //Pointer to a null-terminated string that specifies the name of a service to be run in this service process. 
       //       //If the service is installed with the SERVICE_WIN32_OWN_PROCESS service type, this member is ignored, but cannot be NULL. This member can be an empty string ("").
       //       //If the service is installed with the SERVICE_WIN32_SHARE_PROCESS service type, this member specifies the name of the service that uses the ServiceMain function pointed to by the lpServiceProc member.
       //       "GriffinControlService", 
       //       //Pointer to a ServiceMain function. 
       //       //MyServiceStart
       //       ServiceMain
       //   },
       //   //NULL entries mark the end of the SERVICE_TABLE_ENTRY array.
       //   { NULL,              NULL          }
       //}; 
     
       //if ( !StartServiceCtrlDispatcher( ar_service_table_entry)
       //    ) 
       //{ 
       //    SvcDebugOut("error: StartServiceCtrlDispatcher (%d)\n", 
       //      GetLastError()); 
       //}
       //griffinstatecontrolservice.StartService();
    }
  }
  catch( ConnectToSCMerror e )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("Error connecting to the service control manager:\n"
      << LocalLanguageMessageFromErrorCode(e.m_dwErrorCode) 
      << PossibleSolution(e.m_dwErrorCode) )
  }
  std::cout << //"Waiting for input in order for the output to be readable."
     " Hit any key to exit this program\n" ;
   getche() ;
   DEBUG("end of main program entry point\n");
}
