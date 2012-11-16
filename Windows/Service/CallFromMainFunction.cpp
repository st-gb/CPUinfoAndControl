/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CallFromMainFunction.cpp
 *
 *  Created on: 20.09.2011
 *      Author: Stefan
 */
//#include <stdio.h> //for ::sprintf(...)
//#include <fstream> //for class ofstream
#include <Controller/character_string/stdtstr.hpp> //std::tstring
//#include <Controller/Logger/Logger.hpp> //class Logger
//#include <Controller/Logger/log4cplus/log4cplus_Logger.hpp>
// ::GetErrorMessageFromLastErrorCodeA()
//#include <Windows/ErrorCode/GetErrorMessageFromLastErrorCode.h>
#include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
#define wxCHECK_W32API_VERSION(maj, min) (0) //for "StartDoc" in "winundef.h"
#include <wx/msw/winundef.h> // undefine "CreateServiceA" etc. for the
// CPUcontrolService class
#include "Windows/Service/CPUcontrolService.hpp"
#include "Windows/Service/ServiceBase.hpp"
// for ::SetExePathAsCurrentDir()
#include <Windows/SetExePathAsCurrentDir/SetExePathAsCurrentDir.h>
//#include <Xerces/IPC/IPCdataGenerator.hpp>
//#include <string> //class std::string
#include <conio.h> //for ::getche()
//#include <pstypes.h> //PROCESS_QUERY_LIMITED_INFORMATION
//#include <tchar.h>

//Logger g_logger ;

CPUcontrolBase * gp_cpucontrolbase ;

//#ifdef UNICODE
//  #define TCHAR wchar_t
//#else
//  #define TCHAR char
//#endif

bool IsWithinCmdLineArgs( int argc, //char
  TCHAR * argv[], //LPCTSTR p_tstr
  //LPCSTR
  TCHAR * pc_chCompareTo )
{
    bool bIsWithinCmdLineArgs = false ;
    BYTE byIndex = 1 ;
    for( ; byIndex < argc ; ++ byIndex )
    {
      if (
#if defined(_UNICODE) || defined(UNICODE)
        ::wcscmp(
#else
        ::strcmp(
#endif
          argv[byIndex], pc_chCompareTo) == 0
        )
        {
            bIsWithinCmdLineArgs = true ;
            break ;
        }
    }
    return bIsWithinCmdLineArgs ;
}

bool ShouldCreateService(int argc, //char
    TCHAR * argv[] )
{
    bool bShouldCreateService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-i" )
    if ( IsWithinCmdLineArgs(argc, argv, L"-i" )
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

bool ShouldDeleteService(int argc, //char
    TCHAR * argv[] )
{
    bool bShouldDeleteService = false ;
    //if( argc > 1 )
    //    //if( strcmp(argv[1],"-i") == 0 )
    //    if( std::string(argv[1]) == "-d" )
    if ( IsWithinCmdLineArgs(argc, argv, L"-d" )
        )
       bShouldDeleteService = true ;

    return bShouldDeleteService ;
}

void HandleErrorPlace(BYTE by, const std::string & cr_stdstr )
{
}

void StartAsNormalApp(
    const std::tstring & cr_std_tstrProgramName,
    CPUcontrolService * p_cpucontrolservice)
{
  if( ! p_cpucontrolservice)
  {
    std::wstring std_wstrProgramName = GetStdWstring(cr_std_tstrProgramName);
    CPUcontrolService cpucontrolservice(
  //          argc,
      0,
  //          GetTCHARarray_Inline( (const char **) argv, argc),
      NULL,
  //            Get_wchar_t_Array_Inline( (const char **) argv, argc),
  //            std_tstrProgramName
      std_wstrProgramName
  //            , xerces_ipc_data_handler
      ) ;
    cpucontrolservice.StartAsNonService();
  }
  else
    p_cpucontrolservice->StartAsNonService();
}

BYTE HandleProgramOptions(
  const std::vector<std::string> & std_vec_std_strParams ,
  const std::tstring & cr_std_tstrProgramName,
  CPUcontrolService * p_cpucontrolservice
  )
{
  BYTE byRet = 1;
  try//ConnectToSCMerror
  {
    if( CPUcontrolService::ShouldStartAsNormalApp(std_vec_std_strParams) )
    {
      StartAsNormalApp(cr_std_tstrProgramName, p_cpucontrolservice);
      byRet = 0;
    }
    if( CPUcontrolService::ShouldDeleteService(std_vec_std_strParams) )
    {
      if( std_vec_std_strParams.size() > 1 )
      {
        std::tstring std_tstrServiceName = GetStdTstring_Inline(
          std_vec_std_strParams.at(1) );
        CPUcontrolService::DeleteService(
          //std_vec_std_strParams.at(1).c_str(),
          std_tstrServiceName.c_str(),
          cr_std_tstrProgramName
          ) ;
      }
//      bStartServiceWithinThisProcess = false ;
    }
    if( CPUcontrolService::ShouldCreateService(std_vec_std_strParams) )
    {
      LOGN("Should create the service")
      if( std_vec_std_strParams.size() > 1 )
      {
        CPUcontrolService::CreateService( //std_vec_std_strParams.at(1).c_str()
          GetStdTstring_Inline( std_vec_std_strParams.at(1) ).c_str()
          ) ;
      }
//      bStartServiceWithinThisProcess = false ;
    }
    if( CPUcontrolService::ShouldStartService(std_vec_std_strParams) )
    {
      if( std_vec_std_strParams.size() > 1 )
      {
        //CPUcontrolService::StartService( vecstdstrParams.at(1).c_str() ) ;
        if( ServiceBase::Start( GetStdTstring_Inline(
          std_vec_std_strParams.at(1) ).c_str() ) == ERROR_SUCCESS )
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
    if( CPUcontrolService::ShouldStopService(std_vec_std_strParams) )
    {
      if( std_vec_std_strParams.size() > 1 )
      {
        if( ServiceBase::Stop( GetStdTstring_Inline(
          std_vec_std_strParams.at(1) ).c_str() ) == ERROR_SUCCESS )
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
  return byRet;
}

void PossiblyInteractWithUser(int argc, TCHAR * argv[],
  CPUcontrolService * p_cpucontrolservice,
  const std::tstring & std_tstrProgramName,
  bool & bStartServiceWithinThisProcess
  )
{
  int nChar ;
  std::vector<std::string> vec_std_strParams ;
//  std::tstring stdtstrProgramArg = std::tstring( _T("-config=") )
//    + ptstrProgramName + std::tstring( _T("_config.xml") ) ;
  int nOptionInUpperCase = 0;
  do
  {
    nChar = CPUcontrolService::requestOption( vec_std_strParams
      , std_tstrProgramName );
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
      for ( BYTE by = 0 ; by < vec_std_strParams.size() ; ++ by )
          //DEBUG("\"%s\" ", vecstdstrParams.at(by).c_str() );
          LOG("\"" << vec_std_strParams.at(by).c_str() << "\" " );
      DEBUG("\n" );
    }
    //nRet =
      HandleProgramOptions(vec_std_strParams, std_tstrProgramName,
      p_cpucontrolservice) ;
    //Clear the vector for the next call to CPUcontrolService::
    //requestOption(...).
    vec_std_strParams.clear() ;
    nOptionInUpperCase = ::toupper( nChar );
  }
  while( nOptionInUpperCase != 'Q' && nOptionInUpperCase != 'A') ;
}

void NoProgramArgumentsSpecified(int argc, TCHAR * argv[],
  CPUcontrolService * p_cpucontrolservice,
  std::tstring & std_tstrProgramName
  )
{
  bool bStartServiceWithinThisProcess = //true ;
    false ;
  //IDEA: to distinguish between if called as service or by a user:
  //get the parent process name of this process.
  //Programs that run as service have "services.exe" as their parent
  //process.
  CPUcontrolService::outputUsage();
  //Useful for debugging purposes.
  #ifdef EMULATE_EXECUTION_AS_SERVICE
  #else
  PossiblyInteractWithUser(argc, argv, p_cpucontrolservice,
    std_tstrProgramName, bStartServiceWithinThisProcess);
  #endif //#ifdef EMULATE_EXECUTION_AS_SERVICE

  if( bStartServiceWithinThisProcess )
  {
    LOGN("Starting service inside _this_ process.")
//    if( p_cpucontrolservice == NULL )
    {
//      std::wstring std_wstrProgramName = GetStdWstring( std_tstrProgramName ) ;
//      CPUcontrolService cpucontrolservice(
//        argc,
//        GetTCHARarray_Inline( (const char **) argv, argc),
//  //            Get_wchar_t_Array_Inline( (const char **) argv, argc),
//  //            std_tstrProgramName
//        std_wstrProgramName
//  //            , xerces_ipc_data_handler
//        ) ;
//  //          Xerces::IPCdataHandler xerces_ipc_data_handler(
//  //            cpucontrolservice.m_model ) ;
//      gp_cpucontrolbase = & cpucontrolservice ;
      p_cpucontrolservice->OnInit();
      p_cpucontrolservice->StartService();
          //StartServiceCtrlDispatcherInSeparateThread();
    }
//    else
//    {
//      p_cpucontrolservice->//StartService();
//        StartServiceCtrlDispatcherInSeparateThread();
//      gp_cpucontrolbase = p_cpucontrolservice;
//    }
  //         if( bStartAsService)
  //         {
  //
  //         }
  //         else
  //           cpucontrolservice.StartAsNormalApp();
//     nRet = 0;
  }
}

void AtLeast1ProgramArgumentSpecified(int argc, TCHAR * argv[],
  CPUcontrolService * p_cpucontrolservice,
  const std::tstring & std_tstrProgramName
  )
{
  if( argc > 1 && ::IsWithinCmdLineArgs(argc, argv, _T("-a") ) )
    ::StartAsNormalApp(std_tstrProgramName, p_cpucontrolservice);
  if( argc > 2 && ShouldDeleteService(argc, argv) )
  {
    DWORD dwErrorCodeFor1stError ;
      ServiceBase::DeleteService( //argv[2]
        GetStdTstring_Inline( argv[2]).c_str()
        , dwErrorCodeFor1stError
        ) ;
  }
  if( argc > 2 && ShouldCreateService(argc, argv) )
  {
    BYTE by ;
  //    CreateService(
  //if( //::AfxMessageBox("Should the undervolting service be installed now?",
  //    //IDYES | IDNO ) == IDYES  )
    ServiceBase::CreateService( //argv[2]
      GetStdTstring_Inline( argv[2]).c_str(), by ) ;
  }
  else
      std::cout << "\nNOT installing the service\n" ;
}

//Call this function e.g. from "int main()" or from "wxApp::OnInit()"
int CallFromMainFunction(
    int argc, //char *
    TCHAR * argv[],
    CPUcontrolService * p_cpucontrolservice
   )
{
  int nRet = 1;
  LOGN( FULL_FUNC_NAME << "--begin")
  try
  {
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
    LPTSTR ptstrProgramName = _T("X86_info_and_control") ;

    std::tstring std_tstrProgramName(ptstrProgramName) ;
    if( argc == 1 ) // No program arguments passed.
    {
      NoProgramArgumentsSpecified(argc, argv, p_cpucontrolservice,
        std_tstrProgramName);
    }
    else
    {
      AtLeast1ProgramArgumentSpecified(argc, argv, p_cpucontrolservice,
        std_tstrProgramName);
    }
    DEBUG("Run this program as \"local system account\" account within the "
        "service's properties in Windows->control panel->management->services "
        "else errors may occur\n");
    nRet = 0;
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
  ::getche() ;
  LOGN("end of main program entry point--return " << nRet);
  return //0 ;
    nRet;
}
