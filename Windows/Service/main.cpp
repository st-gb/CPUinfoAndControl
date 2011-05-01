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
// ::GetErrorMessageFromLastErrorCodeA()
//#include <Windows/ErrorCode/GetErrorMessageFromLastErrorCode.h>
#include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
#define wxCHECK_W32API_VERSION(maj, min) (0) //for "StartDoc" in "winundef.h"
#include <wx/msw/winundef.h> // undefine "CreateServiceA" etc. for the
// CPUcontrolService class
#include "Windows/Service/CPUcontrolService.hpp"
#include "Windows/Service/ServiceBase.hpp"
#include <Windows/SetExePathAsCurrentDir/SetExePathAsCurrentDir.h> // for SetExePathAsCurrentDir()
//#include <Xerces/IPC/IPCdataGenerator.hpp>
#include <string>
#include <conio.h> //for getche()
//#include <pstypes.h> //PROCESS_QUERY_LIMITED_INFORMATION

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

//Logger g_logger ;

CPUcontrolBase * gp_cpucontrolbase ;

bool IsWithinCmdLineArgs( int argc, char *  argv[], //LPCTSTR p_tstr
  LPCSTR pc_chCompareTo )
{
    bool bIsWithinCmdLineArgs = false ;
    BYTE byIndex = 1 ;
    for( ; byIndex < argc ; ++ byIndex )
        if ( strcmp(argv[byIndex], pc_chCompareTo) == 0 )
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
  const std::vector<std::string> & std_vec_std_strParams ,
  const std::tstring & cr_std_tstrProgramName
  )
{
  try//ConnectToSCMerror
  {
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
        if( ServiceBase::StartService( GetStdTstring_Inline(
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
        if( ServiceBase::StopService( GetStdTstring_Inline(
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
}

//from http://stackoverflow.com/questions/185254/how-can-a-win32-process-get-the-pid-of-its-parent:
// / see http://msdn.microsoft.com/en-us/library/ms684280%28v=vs.85%29.aspx:
ULONG_PTR GetParentProcessIDviaNtQueryInformationProcess()
{
  ULONG_PTR pbi[6];
  ULONG ulSize = 0;
  LONG (WINAPI *NtQueryInformationProcess)(HANDLE ProcessHandle, ULONG ProcessInformationClass,
   PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
  *(FARPROC *)&NtQueryInformationProcess =
   GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
  if(NtQueryInformationProcess)
  {
   if(NtQueryInformationProcess(
       GetCurrentProcess(),
       0, //0ProcessBasicInformation
       &pbi,
       sizeof(pbi),
       &ulSize) >= 0 && ulSize == sizeof(pbi))
      return pbi[5];
  }
  return (ULONG_PTR)-1;
}

//DWORD GetParentProcessIDviaProcess32First()
//{
//  HANDLE h = CreateToolhelp32Snapshot(
//    TH32CS_SNAPPROCESS, //__in  DWORD dwFlags,
//    0 //__in  DWORD th32ProcessID
//  );
//  if( h != INVALID_HANDLE_VALUE )
//  {
//    PROCESSENTRY32 pe32;
//    ::Process32First( h);
//    ::Process32Nex(h, & pe32);
//  }
//}

DWORD GetParentProcessId() // By Napalm @ NetCore2K
{
  //http://msdn.microsoft.com/en-us/library/ms684868%28v=vs.85%29.aspx:
  // "A process can use the Process32First function to obtain the process identifier of its parent process."
//  return GetParentProcessIDviaProcess32First
  return GetParentProcessIDviaNtQueryInformationProcess();
}

bool IsStartedAsService()
{
  ULONG_PTR dwParentProcessID = GetParentProcessId();
  LOGN("parent PID:" << dwParentProcessID)
  if( dwParentProcessID != (ULONG_PTR)-1 )
  {
      HANDLE handleProcess =//WINAPI
       ::OpenProcess(
           //PROCESS_QUERY_LIMITED_INFORMATION
        0x1000, //__in  DWORD dwDesiredAccess,
        FALSE, //__in  BOOL bInheritHandle,
        dwParentProcessID //__in  DWORD dwProcessId
      );
      //http://msdn.microsoft.com/en-us/library/ms684320(v=vs.85).aspx:
      //"If the function fails, the return value is NULL."
      if( handleProcess)
      {
        TCHAR lpFilename [MAX_PATH];
        char ar_chFilename [MAX_PATH];
        DWORD dw =//WINAPI
          ::GetModuleFileNameA(
          (HMODULE) handleProcess, //__in_opt  HMODULE hModule,
//          lpFilename, //__out     LPTSTR lpFilename,
          ar_chFilename,
          MAX_PATH //__in      DWORD nSize
        );
        if( dw )
          LOGN( "file name for PID:" << ar_chFilename)
        else
          LOGN( //::GetErrorMessageFromLastErrorCodeA()
              "GetModuleFileNameA failed:" <<
              LocalLanguageMessageFromErrorCodeA( ::GetLastError() )
              )
        //QueryFullProcessImageName
      }
      else
        LOGN( //::GetErrorMessageFromLastErrorCodeA()
            LocalLanguageMessageFromErrorCodeA( ::GetLastError() )
            )
  }
}

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main( int argc, char * argv[]
  //TCHAR *  argv[]
  )
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring( _T("-config=") )
    + ptstrProgramName + std::tstring( _T("_config.xml") ) ;

  IsStartedAsService();
  //std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  std::string stdstrLogFileName = std::string( argv[0]) +
    std::string("_log.txt") ;
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
    g_logger.OpenFile2( stdstrLogFileName ) ;
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
         LOGN("Starting service inside _this_ process.")
         std::wstring stdwstr = GetStdWstring( stdtstrProgramName ) ;
          CPUcontrolService cpucontrolservice(
            argc,
            GetTCHARarray_Inline( (const char **) argv, argc),
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
            ServiceBase::DeleteService( //argv[2]
              GetStdTstring_Inline( argv[2]).c_str()
              , dwErrorCodeFor1stError
              ) ;
        }
        if( ShouldCreateService(argc, argv) && argc > 2 )
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
