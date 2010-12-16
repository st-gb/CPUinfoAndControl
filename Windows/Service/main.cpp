//Else: "fatal error C1189: #error :  Building MFC application with /MD[d]
// (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or
// do not use /MD[d]"
//#define _AFXDLL
//#include <AfxWin.h> //for AfxMessageBox(...)->muss MultiThreaded DLL ("/MD")
// sein
#include <stdio.h> //for sprintf(...)
#include <fstream> //for class ofstream
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <Controller/Logger/Logger.hpp>
//#include <Controller/Logger/log4cplus/log4cplus_Logger.hpp>
#include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
#include "Windows/Service/CPUcontrolService.hpp"
#include "Windows/Service/ServiceBase.hpp"
#include <Windows/SetExePathAsCurrentDir/SetExePathAsCurrentDir.h> // for SetExePathAsCurrentDir()
//#include <Xerces/IPC/IPCdataGenerator.hpp>
#include <string>
#include <conio.h> //for getche()

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

Logger g_logger ;

CPUcontrolBase * gp_cpucontrolbase ;

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
          "Should the CPU controlling service be installed now? [Y]es/ "
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

void HandleErrorPlace(BYTE by, const std::string & cr_stdstr )
{
}

void HandleProgramOptions(
  const std::vector<std::string> & vecstdstrParams ,
  const std::tstring & stdtstrProgramName
  )
{
  try//ConnectToSCMerror
  {
    if( CPUcontrolService::ShouldDeleteService(vecstdstrParams) )
    {
      if( vecstdstrParams.size() > 1 )
      {
        CPUcontrolService::DeleteService( vecstdstrParams.at(1).c_str(),
          stdtstrProgramName ) ;
      }
//      bStartServiceWithinThisProcess = false ;
    }
    if( CPUcontrolService::ShouldCreateService(vecstdstrParams) )
    {
      if( vecstdstrParams.size() > 1 )
      {
        CPUcontrolService::CreateService( vecstdstrParams.at(1).c_str() ) ;
      }
//      bStartServiceWithinThisProcess = false ;
    }
    if( CPUcontrolService::ShouldStartService(vecstdstrParams) )
    {
      if( vecstdstrParams.size() > 1 )
      {
        //CPUcontrolService::StartService( vecstdstrParams.at(1).c_str() ) ;
        if( ServiceBase::StartService( GetStdTstring_Inline(
          vecstdstrParams.at(1) ).c_str() ) == ERROR_SUCCESS )
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
            "Starting the service succeeded.")
        else
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
            "Starting the service failed:" <<
            LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << "."
            )
      }
//      bStartServiceWithinThisProcess = false ;
    }
    if( CPUcontrolService::ShouldStopService(vecstdstrParams) )
    {
      if( vecstdstrParams.size() > 1 )
      {
        if( ServiceBase::StopService( GetStdTstring_Inline(
          vecstdstrParams.at(1) ).c_str() ) == ERROR_SUCCESS )
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
            "Stopping the service succeeded.")
        else
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
            "Stopping the service failed:" <<
            LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << "."
            )
      }
    }
  }
  catch( ConnectToSCMerror & r_connect_to_scm_error )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "Error connecting to the service control manager:\n"
      << LocalLanguageMessageFromErrorCodeA(
        r_connect_to_scm_error.m_dwErrorCode)
      << PossibleSolution(r_connect_to_scm_error.m_dwErrorCode)
      )
  }
}

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main( int argc, char *  argv[] )
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring("-config=") + ptstrProgramName + 
    std::tstring("_config.xml") ;
  //std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  std::string stdstrLogFileName = std::string(argv[0]) + std::tstring("_log.txt") ;
  try
  {
    if( ! SetExePathAsCurrentDir() )
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "Getting file path for THIS executable file failed: " <<
        LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n"
        );
#ifdef USE_LOG4CPLUS
    init_log4cplus() ;
#endif
    g_logger.OpenFile( stdstrLogFileName ) ;
    DEBUG("Begin of main program entry point\n");
//    LOGN( "stdin:" << stdin << " stdout:" << stdout << " argc:" << argc )
//    if( stdin )
//      LOGN("Executed as service.")
//    else
//      LOGN("Not executed as service.")
    //Must set the exe path as current dir before (else the file is located in
    //: C:\WINDOWS\System32) !
    //PossiblyOutputUsage() ;
    CPUcontrolBase::OuputCredits() ;
    if( argc == 1 )
    {
      bool bStartServiceWithinThisProcess = //true ;
        false ;
      //IDEA: to distinguish between if called as service or by a user:
      //get the parent process name of this process.
      //Programs that run as service have "services.exe" as their parent
      //process.
      CPUcontrolService::outputUsage();
      std::vector<std::string> vecstdstrParams ;
      //Useful for debugging purposes.
      #ifdef EMULATE_EXECUTION_AS_SERVICE
      #else
      int nChar ;
      do
      {
        nChar = CPUcontrolService::requestOption( vecstdstrParams
          , stdtstrProgramName );
        //LOGN("character entered:" << nChar )
//        //The vector is empty when this program was executed/ invoked via the
//        //service control manager.
//        if( vecstdstrParams.empty() )
        //The character is "-1" when this program was executed/ invoked via the
        //service control manager.
        if( nChar == -1 )
        {
          LOGN("This exe is started as service (and not "//"as installer)"
            "for executing actions)");
          bStartServiceWithinThisProcess = true ;
          break ; //Leave this loop.
        }
        else
        {
          DEBUG("Entered strings: ")
          for ( BYTE by = 0 ; by < vecstdstrParams.size() ; ++ by )
              //DEBUG("\"%s\" ", vecstdstrParams.at(by).c_str() );
              LOG("\"" << vecstdstrParams.at(by).c_str() << "\" " );
          DEBUG("\n" );
        }
        HandleProgramOptions(vecstdstrParams, stdtstrProgramName) ;
        //Clear the vector for the next call to CPUcontrolService::
        //requestOption(...).
        vecstdstrParams.clear() ;
      }
      while( toupper( nChar ) != 'Q' ) ;
      #endif //#ifdef EMULATE_EXECUTION_AS_SERVICE
      if( bStartServiceWithinThisProcess )
      {
         std::wstring stdwstr = GetStdWstring( stdtstrProgramName ) ;
          CPUcontrolService cpucontrolservice(
            argc,
            argv,
//            stdtstrProgramName
            stdwstr
//            , xerces_ipc_data_handler
            ) ;
//          Xerces::IPCdataHandler xerces_ipc_data_handler(
//            cpucontrolservice.m_model ) ;
          gp_cpucontrolbase = & cpucontrolservice ;
          cpucontrolservice.StartService();
      }
    }
    else
    {
        if( ShouldDeleteService(argc, argv) && argc > 2 )
        {
          DWORD dwErrorCodeFor1stError ;
            ServiceBase::DeleteService(
              argv[2]
              , dwErrorCodeFor1stError
              ) ;
        }
        if( ShouldCreateService(argc, argv) && argc > 2 )
        {
          BYTE by ;
        //    CreateService(
        //if( //::AfxMessageBox("Should the undervolting service be installed now?",
        //    //IDYES | IDNO ) == IDYES  )
          ServiceBase::CreateService( argv[2], by ) ;
        }
        else
            std::cout << "\nNOT installing the service\n" ;
    }
    DEBUG("Run this program as \"local system account\" account within the "
        "service's properties in Windows->control panel->management->services "
        "else errors may occur\n");
  }
  catch( ConnectToSCMerror & r_connect_to_scm_error )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "Error connecting to the service control manager:\n"
      << LocalLanguageMessageFromErrorCodeA(
        r_connect_to_scm_error.m_dwErrorCode)
      << PossibleSolution(r_connect_to_scm_error.m_dwErrorCode)
      )
  }
  std::cout << //"Waiting for input in order for the output to be readable."
    " Hit any key to exit this program\n" ;
  getche() ;
  DEBUG("end of main program entry point\n");
  return 0 ;
}
