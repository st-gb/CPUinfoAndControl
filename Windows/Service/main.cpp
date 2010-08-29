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
#include <Controller/Logger/log4cplus_Logger.hpp>
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include "Windows/Service/CPUcontrolService.hpp"
#include "Windows/Service/ServiceBase.hpp"
#include <Windows/SetExePathAsCurrentDir.h> // for SetExePathAsCurrentDir()
//#include <Xerces/IPC/IPCdataGenerator.hpp>
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

void OuputCredits()
{
  std::tstring stdtstr ;
  std::vector<std::tstring> stdvecstdtstring ;
  MainController::GetSupportedCPUs(stdvecstdtstring) ;
  for(BYTE by = 0 ; by < stdvecstdtstring.size() ; by ++ )
    stdtstr += _T("-") + stdvecstdtstring.at(by) + _T("\n") ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
    //"This program is an undervolting program for AMD family 17 CPUs.\n" 
    "This program is a CPU information and control program for (built-in):\n"
    //maincontroller.GetSupportedCPUtypes() ;
    + stdtstr +
    //"license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n" 
    "license/ info: http://www.trilobyte-se.de/x86iandc/\n" 
    )
  std::cout << 
    "This executable is both in one:\n"
    "-a(n) (de-)installer for the CPU controlling service\n"
    "-the CPU controlling service itself\n" ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    "Build time: " __DATE__ " " __TIME__ " (Greenwich Mean Time + 1)" );
}

void HandleErrorPlace(BYTE by, const std::string & cr_stdstr )
{
}

void CreateService( const TCHAR * const cpc_tchServiceName)
{
  try
  {
    BYTE by ;
    DWORD dwLastError = ServiceBase::CreateService( cpc_tchServiceName , by  ) ;
    if( dwLastError )
    {
      //DEBUG("Creating the service failed: error number:%d\n",
      // dwLastError);
      //LOG("Creating the service failed: error number:" << dwLastError
      //    << "\n" );
      //std::cout
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "Creating the service failed: error number:" <<
        dwLastError << "\nError: " << //"\n"
        LocalLanguageMessageFromErrorCodeA( dwLastError )
        )
        ;
      switch( by )
      {
      case ServiceBase::GetModuleFileNameFailed:
        //DEBUG("GetModuleFileName failed (%d)\n", GetLastError());
        //LOG(
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
          "Getting file name for THIS executable file failed: " <<
          LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n"
          );
        break ;
      }
      std::string stdstrPossibleSolution ;
      ServiceBase::GetPossibleSolution( dwLastError , cpc_tchServiceName ,
        stdstrPossibleSolution ) ;
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(stdstrPossibleSolution)
    }
    else
      //printf("Creating service succeeded\n");
      std::cout << "Creating the service succeeded.\n" ;
  }
  catch( ConnectToSCMerror & ctscme )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("") ;
  }
}

void DeleteService(const TCHAR * cp_tchServiceName
  , std::string & stdtstrProgramName )
{
  DWORD dwErrorCodeFor1stError ;
  BYTE by = ServiceBase::DeleteService(//"GriffinStateService"
    cp_tchServiceName
    , dwErrorCodeFor1stError
    ) ;
  if( by )
  {
//    HandleErrorPlace(by, "for deleting the service:") ;
//    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
//      ServiceBase::GetPossibleSolution(dwErrorCodeFor1stError)
//      ) ;
    std::string stdstr = "for deleting the service:" ;
    switch(by)
    {
    case ServiceBase::OpenSCManagerFailed:
    {
      std::string stdstrPossibleSolution ;
      ServiceBase::GetPossibleSolution(
        dwErrorCodeFor1stError ,
        cp_tchServiceName
        , stdstrPossibleSolution
        ) ;
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        stdstr
        << "opening the service control manager failed"
        << LocalLanguageMessageAndErrorCodeA(dwErrorCodeFor1stError)
        << stdstrPossibleSolution
        ) ;
    }
      break ;
    case ServiceBase::OpenServiceFailed:
    {
      std::string stdstrPossibleSolution ;
      ServiceBase::GetPossibleSolution(
        dwErrorCodeFor1stError ,
        cp_tchServiceName
        , stdstrPossibleSolution
        ) ;
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        stdstr
        << "Opening the service for service name \"" <<
          cp_tchServiceName << "\" failed."
        << LocalLanguageMessageAndErrorCodeA(dwErrorCodeFor1stError)
        << stdstrPossibleSolution
        )
    }
        break ;
    case ServiceBase::DeleteServiceFailed:
    {
      std::string stdstrPossibleSolution ;
      ServiceBase::GetPossibleSolution(
        dwErrorCodeFor1stError ,
        cp_tchServiceName
        , stdstrPossibleSolution
        ) ;
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "Deleting service failed."
        << LocalLanguageMessageAndErrorCodeA(dwErrorCodeFor1stError)
        << stdstrPossibleSolution
        )
    }
    break ;
    }
  }
  else
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "Deleting the service \""
      << cp_tchServiceName
      << "\" succeeded." )
  std::wstring stdwstr = GetStdWstring( stdtstrProgramName ) ;
  PowerProfDynLinked powerprofdynlinked( //stdtstrProgramName
    stdwstr ) ;
  powerprofdynlinked.DeletePowerScheme( stdtstrProgramName ) ;
  powerprofdynlinked.OutputAllPowerSchemes() ;
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
    g_logger.OpenFile( //std::string("GriffinControl_log.txt")
      stdstrLogFileName ) ;
    DEBUG("Begin of main program entry point\n");
    //Must set the exe path as current dir before (else the file is located in
    //: C:\WINDOWS\System32) !
    //PossiblyOutputUsage() ;
    OuputCredits() ;
    if( argc == 1 )
    {
      bool bStartService = true ;
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
          LOGN("This exe is started as service (and not as installer)");
      }
      else
      {
          DEBUG("Entered strings: ")
          for ( BYTE by = 0 ; by < vecstdstrParams.size() ; ++ by )
              //DEBUG("\"%s\" ", vecstdstrParams.at(by).c_str() );
              LOG("\"" << vecstdstrParams.at(by).c_str() << "\" " );
          DEBUG("\n" );
      }
      if( CPUcontrolService::ShouldDeleteService(vecstdstrParams) )
      {
        if( vecstdstrParams.size() > 1 )
        {
          DeleteService( vecstdstrParams.at(1).c_str(), stdtstrProgramName ) ;
        }
        bStartService = false ;
      }
      if( CPUcontrolService::ShouldCreateService(vecstdstrParams) )
      {
        if( vecstdstrParams.size() > 1 )
        {
          CreateService( vecstdstrParams.at(1).c_str() ) ;
        }
        bStartService = false ;
      }
      if( bStartService )
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
//            cpucontrolservice.m_modelData ) ;
          gp_cpucontrolbase = & cpucontrolservice ;
          cpucontrolservice.StartService();
      }
    }
    else
    {
        if( ShouldDeleteService(argc, argv) && argc > 2 )
        {
          DWORD dwErrorCodeFor1stError ;
            ServiceBase::DeleteService(//"GriffinStateService"
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
      << LocalLanguageMessageFromErrorCodeA(r_connect_to_scm_error.m_dwErrorCode)
      << PossibleSolution(r_connect_to_scm_error.m_dwErrorCode) )
  }
  std::cout << //"Waiting for input in order for the output to be readable."
    " Hit any key to exit this program\n" ;
  getche() ;
  DEBUG("end of main program entry point\n");
  return 0 ;
}