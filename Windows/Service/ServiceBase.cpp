#include "ServiceBase.hpp"

#include <windows.h> //for SERVICE_TABLE_ENTRY, ... CreateService
#include <iostream> //std::cout
#include <Windows/LocalLanguageMessageFromErrorCode.h>

DWORD ServiceBase::ContinueService(
  const TCHAR * cp_tchServiceName
  )
{
  SC_HANDLE schService ;
  SC_HANDLE schSCManager;
  SERVICE_STATUS ssStatus; 
  //DWORD dwStartTickCount, dwWaitTime;

  schSCManager = OpenSCManager( 
    NULL,                    // local machine 
    NULL,                    // ServicesActive database 
    //SC_MANAGER_ALL_ACCESS
    //STANDARD_RIGHTS_REQUIRED | SC_MANAGER_CREATE_SERVICE
    ////Required to call the DeleteService function to delete the service.
    //| DELETE
    //SC_MANAGER_ALL_ACCESS
    //Combines the following access types: 
    // -STANDARD_RIGHTS_EXECUTE
    // -SC_MANAGER_CONNECT
    // -SC_MANAGER_LOCK
    GENERIC_EXECUTE 
    );  // full access rights 
     
  if ( schSCManager == NULL ) 
  {
    DWORD dwLastError = ::GetLastError() ;
    //return dwLastError ;
    //Throw exception because: an error code can be thrown for 
    //various reasons, so I do not need to return another value for 
    //the cause, e.g. connect to SCM error.
    throw ConnectToSCMerror( dwLastError ) ;
  }
  // Open a handle to the service. 
  schService = OpenService( 
    schSCManager,        // SCManager database 
    //TEXT("Sample_Srv"),  // name of service 
    cp_tchServiceName ,
    SERVICE_PAUSE_CONTINUE );          // specify access 
  if (schService == NULL) 
  {
    //printf("OpenService failed (%d)\n", GetLastError()); 
    return FALSE;
  }

  // Send a control value to the service. 
  if (! ControlService( 
          schService,   // handle to service 
          SERVICE_CONTROL_CONTINUE,   // control value to send 
          &ssStatus) )  // address of status info 
  {
      //printf("ControlService failed (%d)\n", GetLastError()); 
      return FALSE;
  }
  return FALSE;
}

void ServiceBase::GetErrorDescriptionFromStartServiceCtrlDispatcherErrCode(
  DWORD dwLastError ,
  std::string & r_stdstrErrorDescription
  )
{
  switch(dwLastError)
  {
  case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
    r_stdstrErrorDescription =
      "Typically, this error indicates that the program is being run as a "
      "console application rather than as a service. "
      "If the program will be run as a console application for debugging "
      "purposes, structure it such that service-specific code is not called "
      "when this error is returned." ;
    break;
  case ERROR_INVALID_DATA:
    r_stdstrErrorDescription =
      "The specified dispatch table contains entries that are not in the "
      "proper format." ;
    break;
  case ERROR_SERVICE_ALREADY_RUNNING:
    r_stdstrErrorDescription =
      "The process has already called StartServiceCtrlDispatcher. "
      "Each process can call StartServiceCtrlDispatcher only one time." ;
    break;
//  default:
//    DEBUG("error code was not set by the service control manager directly\n");
  }
}

void ServiceBase::GetErrorDescriptionFromRegSvcCtrlHandlerExErrCode(
  DWORD dwLastError ,
  std::string & r_stdstrErrorDescription
  )
{
  switch( dwLastError )
  {
  //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
    case ERROR_NOT_ENOUGH_MEMORY:
      r_stdstrErrorDescription =
        "Not enough memory is available to convert an ANSI string parameter "
        "to Unicode. This error does not occur for Unicode string "
        "parameters." ;
      break;
    case ERROR_SERVICE_NOT_IN_EXE:
      r_stdstrErrorDescription =
        "The service entry was specified incorrectly when the process called "
        "the StartServiceCtrlDispatcher function." ;
      break;
    case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
      r_stdstrErrorDescription =
        "Typically, this error indicates that the program is "
        "being run as a console application rather than as a "
        "service. If the program will be run as a console "
        "application for debugging purposes, structure it such "
        "that service-specific code is not called when this "
        "error is returned." ;
      break;
    case ERROR_INVALID_DATA:
      r_stdstrErrorDescription =
        "The specified dispatch table contains entries that are "
        "not in the proper format." ;
      break ;
    case ERROR_SERVICE_ALREADY_RUNNING:
      r_stdstrErrorDescription =
        "The process has already called "
        "StartServiceCtrlDispatcher. Each process can call "
        "StartServiceCtrlDispatcher only one time." ;
      break;
    default:
      r_stdstrErrorDescription =
          LocalLanguageMessageFromErrorCodeA( dwLastError ) ;
  }
}

void ServiceBase::GetErrorDescriptionFromOpenServiceErrCode(
  DWORD dwLastError ,
  std::string & r_stdstrErrorDescription
  )
{
  switch( dwLastError )
  {
    //http://msdn.microsoft.com/en-us/library/ms684330%28VS.85%29.aspx:
    case ERROR_INVALID_NAME:
      r_stdstrErrorDescription =
        "The specified service name is invalid." ;
      break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
      r_stdstrErrorDescription =
        "The specified service does not exist." ;
      break;
    default:
      r_stdstrErrorDescription =
          LocalLanguageMessageFromErrorCodeA( dwLastError ) ;
  }
}

DWORD ServiceBase::PauseService(
  const TCHAR * cp_tchServiceName
  , std::string & r_stdstrMsg
  )
{
  SC_HANDLE schService ;
  SC_HANDLE schSCManager;
  SERVICE_STATUS ssStatus; 
  //DWORD dwStartTickCount, dwWaitTime;

  schSCManager = OpenSCManager( 
    NULL,                    // local machine 
    NULL,                    // ServicesActive database 
    //SC_MANAGER_ALL_ACCESS
    //STANDARD_RIGHTS_REQUIRED | SC_MANAGER_CREATE_SERVICE
    ////Required to call the DeleteService function to delete the service.
    //| DELETE
    //SC_MANAGER_ALL_ACCESS
    //Combines the following access types: 
    // -STANDARD_RIGHTS_EXECUTE
    // -SC_MANAGER_CONNECT
    // -SC_MANAGER_LOCK
    GENERIC_EXECUTE 
    );  // full access rights 
     
    if ( schSCManager == NULL ) 
    {
      DWORD dwLastError = ::GetLastError() ;
      //return dwLastError ;
      //Throw exception because: an error code can be thrown for 
      //various reasons, so I do not need to return another value for 
      //the cause, e.g. connect to SCM error.
      throw ConnectToSCMerror( dwLastError ) ;
    }
    // Open a handle to the service. 
    schService =
      //http://msdn.microsoft.com/en-us/library/ms684330%28VS.85%29.aspx:
      //"If the function fails, the return value is NULL.
      //To get extended error information, call GetLastError."
      ::OpenService(
      schSCManager,        // SCManager database 
      //TEXT("Sample_Srv"),  // name of service 
      cp_tchServiceName ,
      SERVICE_PAUSE_CONTINUE
      );          // specify access
    if ( schService == NULL)
    {
        //printf("OpenService failed (%d)\n", GetLastError());
      DWORD dw = ::GetLastError() ;
      GetErrorDescriptionFromOpenServiceErrCode( dw , r_stdstrMsg ) ;
      return FALSE;
    }
 
    // Send a control value to the service. 
    if (! ControlService( 
            schService,   // handle to service 
            SERVICE_CONTROL_PAUSE,   // control value to send 
            &ssStatus) )  // address of status info 
    {
        //printf("ControlService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
  return TRUE ;
}

void ServiceBase::GetPossibleSolution(
  DWORD dwWinError ,
  const TCHAR * tchServiceName
  , std::string & r_stdstrPossibleSolution
  )
{
  switch(dwWinError)
  {
    case ERROR_SERVICE_MARKED_FOR_DELETE:
//      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      //Use std::stringstream (instead of widestring version) for
      //MinGW compatibility
        std::stringstream strstream ;
        strstream <<
        "possible solution:\n"
        "-close the service control manager in order to let the service be "
            "deleted\n"
        "-if you want to install a service and a service with the same name "
          "is still running->stop the service with the same name first\n"
        "\n-Stop the execution of the service: \""
        << tchServiceName
        << "\"\n"
        << " -inside the service control manager\n"
        " -via the task manager\n"
        " -by another program/ etc.\n"
        " and try again."
//        )
        ;
        r_stdstrPossibleSolution = strstream.str() ;
    break ;
  }
}

DWORD ServiceBase::CreateService(
  const TCHAR * tchServiceName
  //, TCHAR * tchPathToServiceExe
  )
{ 
//  BOOL boolReturn = FALSE ;
  SC_HANDLE schSCManager;
  
  schSCManager = OpenSCManager( 
    NULL,                    // local machine 
    NULL,                    // ServicesActive database 
    //SC_MANAGER_ALL_ACCESS
    STANDARD_RIGHTS_REQUIRED | SC_MANAGER_CREATE_SERVICE
    ////Required to call the DeleteService function to delete the service.
    //| DELETE
    );  // full access rights 
   
  if ( schSCManager == NULL ) 
  {
    DWORD dwLastError = GetLastError() ;
    ////DEBUG("OpenSCManager failed: error code: \n", dwLastError);
    ////WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("Opening Service Control "
    ////    "Manager failed: error code: %d", dwLastError);
    //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("Opening Service Control "
    //    "Manager failed: error message: " << //dwLastError 
    //    LocalLanguageMessageFromErrorCodeA(dwLastError)) ;
    //switch(dwLastError)
    //{
    //case ERROR_ACCESS_DENIED:
    //    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    //        "The requested access was denied.\n");
    //    break;
    //}
    //Throw exception because: an error code can be thrown for 
    //various reasons, so I do not need to return another value for 
    //the cause, e.g. connect to SCM error.
    throw ConnectToSCMerror( dwLastError ) ;
  }
  else
  {
    TCHAR szPath[MAX_PATH];
    if( GetModuleFileName(
      //If this parameter is NULL, GetModuleFileName retrieves the path 
      //of the executable file of the current process. 
      NULL, 
      szPath, MAX_PATH )
      )
    {
      SC_HANDLE schService ;
      CreateService(
          schService
          , tchServiceName
          //, tchPathToServiceExe
          , szPath
          , schSCManager );

      if (schService == NULL) 
      {
          DWORD dwLastError = GetLastError() ;
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
          switch(dwLastError)
          {
          case ERROR_ACCESS_DENIED:
              DEBUG("The handle to the SCM database does not have the "
                  "SC_MANAGER_CREATE_SERVICE access right.\n");
              std::cout << "The handle to the SCM database does not have "
                  "the SC_MANAGER_CREATE_SERVICE access right.\n" ;
              break;
          case ERROR_DUPLICATE_SERVICE_NAME:
              DEBUG("The display name already exists in the service "
                  "control manager database either as a service name "
                  "or as another display name.\n");
              std::cout << "The display name already exists in the service "
                  "control manager database either as a service name "
                  "or as another display name.\n" ;
              break;
          case ERROR_SERVICE_EXISTS:
              DEBUG("The specified service already exists in this database.\n");
              std::cout << "The specified service already exists in this "
                  "database.\n" ;
              //DeleteService(schSCManager, tchServiceName);
              //CreateService(schService,tchServiceName,tchPathcToServiceExe);
              break;
          } //switch(dwLastError)
          std::string stdstrPossibleSolution ;
          GetPossibleSolution( dwLastError , tchServiceName ,
            stdstrPossibleSolution ) ;
          //return FALSE;
      }//if (schService == NULL) 
      else
      {
          //printf("Creating service succeeded\n");
          std::cout << "Creating the service succeeded.\n" ;
          CloseServiceHandle(schService);
          //return TRUE;
      }
    }
    else

      {
          //DEBUG("GetModuleFileName failed (%d)\n", GetLastError());
          //LOG(
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
              "Getting file name for THIS executable file failed: " <<
              LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n"
              );
          //return FALSE;
      }
      //else
      //    //DEBUG("Path of this executable:%s\n", szPath);
      //    LOG("Path of this executable:" << szPath << "\n" );
  }//if ( schSCManager == NULL ) 
  //return boolReturn ;
  return 0 ;
}

DWORD ServiceBase::CreateService(
  SC_HANDLE & r_schService 
  , const TCHAR * tchServiceName
  , TCHAR * tchPathcToServiceExe
  , SC_HANDLE schSCManager
  )
{
  r_schService = ::CreateService( 
    schSCManager,              // SCManager database 
    // name of service 
    //TEXT("GriffinControlService"),
    tchServiceName,
    // service name to display 
    //lpszDisplayName,
    //"GriffinControlService",
    tchServiceName ,
    SERVICE_ALL_ACCESS,        // desired access 
    //Service that runs in its own process.
    SERVICE_WIN32_OWN_PROCESS, // service type 
    //SERVICE_DEMAND_START,      // start type 
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/
    // createservice.htm:
    //"A service started automatically by the service control manager during
    // system startup."
    SERVICE_AUTO_START,
    SERVICE_ERROR_NORMAL,      // error control type 
    // path to service's binary 
    //"Pointer to a null-terminated string that contains the fully qualified
    //path to the service binary file. If the path contains a space, it must
    //be quoted so that it is correctly interpreted. For example,
    //"d:\\my share\\myservice.exe" should be specified as
    //"\"d:\\my share\\myservice.exe\"".
    //The path can also include arguments for an auto-start service.
    //For example, "d:\\myshare\\myservice.exe arg1 arg2".
    //These arguments are passed to the service entry point
    //(typically the main function)."
    //Use C comment, else compiler warning: multi-line comment because of "\" at
    // line end.
    /* "..\\GriffinControlService\\Release\\
    //"C:\\Dokumente und Einstellungen\\Stefan\\Eigene Dateien\\"
    //"Visual Studio 2005\\Projects\\GriffinControlService\\Release\\"
    //"GriffinControlService.exe", */
    tchPathcToServiceExe,
    NULL,                      // no load ordering group 
    NULL,                      // no tag identifier 
    NULL,                      // no dependencies 
    NULL,                      // LocalSystem account 
    NULL     // NULL = no password 
    );                     
    //if (r_schService == NULL)
    //{
    //  printf("Creating the service failed\n"); 
    //}
    //else
    //  printf("Creating the service succeeded\n"); 
  return 0 ;
}

//Use number because that enables language-independent messaged
//return: 0 = success
//DWORD
BYTE ServiceBase::DeleteService(
   //SC_HANDLE & r_schSCManager
   //,
   const TCHAR * tchServiceName
   //Use number because that enables language-independent messages
   , DWORD  & dwErrorCodeFor1stError
   )
{ 
  SC_HANDLE schSCManager = OpenSCManager( 
    NULL,                    // local machine 
    NULL,                    // ServicesActive database 
    //SC_MANAGER_ALL_ACCESS
    STANDARD_RIGHTS_REQUIRED //| SC_MANAGER_CREATE_SERVICE
    //Required to call the DeleteService function to delete the service.
    | DELETE
    );  // full access rights 
     
  if ( schSCManager == NULL )
  {
    dwErrorCodeFor1stError = ::GetLastError() ;
    //return dwLastError ;
    //Throw exception because: an error code can be thrown for
    //various reasons, so I do not need to return another value for
    //the cause, e.g. connect to SCM error.
//      throw ConnectToSCMerror( dwLastError ) ;
    return OpenSCManagerFailed ;
  }
  else
  {
    SC_HANDLE schService = OpenService(
        schSCManager,       // SCManager database
        // name of service
        //TEXT("Sample_Srv"),
        tchServiceName ,
        DELETE);            // only need DELETE access

    if ( schService == NULL)
    {
      dwErrorCodeFor1stError = ::GetLastError() ;
//            return FALSE;
      return OpenServiceFailed ;
    }

    if ( ::DeleteService(schService) )
//       std::cout << "Deleting service succeeded.\n" ;
      ;
    else
    {
      dwErrorCodeFor1stError = ::GetLastError() ;
      return DeleteServiceFailed ;
    }
    ::CloseServiceHandle(schService);
  }
    //return TRUE;
  return 0 ;
}

SERVICE_STATUS_HANDLE //WINAPI
  ServiceBase::RegSvcCtrlHandlerExAndGetErrMsg(
  //__in
    LPCTSTR lpServiceName,
  //__in
    LPHANDLER_FUNCTION_EX lpHandlerProc,
  //__in_opt
    LPVOID lpContext,
//    std::string & r_stdstrErrorDescription
    DWORD & r_dwErrorCode
  )
{
  m_service_status_handle =
    ::RegisterServiceCtrlHandlerEx ( //"GriffinControlService",
      lpServiceName ,
      lpHandlerProc,
      //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
      //"lpContext [in, optional]
      //Any user-defined data. This parameter, which is passed to the
      //handler function, can help identify the service when multiple
      //services share a process."
      lpContext
      );
  //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
  //"If the function fails, the return value is zero. To get extended error
  //information, call GetLastError."
  if ( m_service_status_handle == (SERVICE_STATUS_HANDLE) 0 )
  {
    //Get error code for RegisterServiceCtrlHandler(...).
//    DWORD dwLastError = ::GetLastError();
    r_dwErrorCode = ::GetLastError();
  }
  return m_service_status_handle ;
}

//SERVICE_STATUS_HANDLE //WINAPI
//  ServiceBase::RegSvcCtrlHandlerExAndGetErrMsg(
//  //__in
//    LPCTSTR lpServiceName,
//  //__in_opt
//    LPVOID lpContext ,
////    std::string & r_stdstrErrorDescription
//    DWORD & r_dwErrorCode
// )
//{
//  m_service_status_handle =
//    RegSvcCtrlHandlerExAndGetErrMsg ( //"GriffinControlService",
//      lpServiceName ,
//      (LPHANDLER_FUNCTION_EX) ServiceCtrlHandlerEx,
//      //http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
//      //"lpContext [in, optional]
//      //Any user-defined data. This parameter, which is passed to the
//      //handler function, can help identify the service when multiple
//      //services share a process."
//      lpContext ,
//      //r_stdstrErrorDescription
//      r_dwErrorCode
//      );
//  return m_service_status_handle ;
//}

DWORD ServiceBase::StartService(
  LPCTSTR lpServiceName )
{
  SC_HANDLE schSCManager;

  // Open a handle to the SC Manager database. 
  schSCManager = 
    //Call OpenSCManager for the spec. methods because:
    //Open with LEAST access rights for the SPECIFIC operation.
    OpenSCManager( 
      NULL,                    // local machine 
      NULL,                    // ServicesActive database 
      //SC_MANAGER_ALL_ACCESS
      //Combines the following access types: 
      // -STANDARD_RIGHTS_EXECUTE
      // -SC_MANAGER_CONNECT
      // -SC_MANAGER_LOCK
      GENERIC_EXECUTE 
      );  // full access rights 
   
  if (NULL == schSCManager) 
  {
    //printf("OpenSCManager failed (%d)\n", GetLastError());
    //return 0 ;
    DWORD dwLastError = ::GetLastError() ;
    //Throw exception because: an error code can be thrown for 
    //various reasons, so I do not need to return another value for 
    //the cause, e.g. connect to SCM error.
    throw ConnectToSCMerror( dwLastError ) ;
  }
  else
    return StartService(schSCManager,lpServiceName) ;
  return 0 ;
}

DWORD ServiceBase::StartService(
  SC_HANDLE schSCManager
  , LPCTSTR lpServiceName )
{
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS ssStatus; 
    DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
 
    schService = ::OpenService( 
        schSCManager,          // SCM database 
        lpServiceName ,          // service name
        SERVICE_ALL_ACCESS); 
 
    if (schService == NULL) 
    { 
        return 0; 
    }
 
    if ( ! ::StartService(
            schService,  // handle to service 
            0,           // number of arguments 
            NULL) )      // no arguments 
    {
        return 0; 
    }
    else 
    {
        printf("Service start pending.\n"); 
    }
 
    // Check the status until the service is no longer start pending. 
 
    if (!QueryServiceStatusEx( 
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE) &ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
    {
        return 0; 
    }
 
    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
    { 
        // Do not wait longer than the wait hint. A good interval is 
        // one tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if ( dwWaitTime > 10000 )
            dwWaitTime = 10000;

        Sleep( dwWaitTime );

        // Check the status again. 
 
    if (!QueryServiceStatusEx( 
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE) &ssStatus,              // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // if buffer too small
            break; 
 
        if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
        {
            // The service is making progress.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint
                break;
            }
        }
    } 

    CloseServiceHandle(schService); 

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
    {
        //printf("StartService SUCCESS.\n"); 
        return 1;
    }
    else 
    { 
        //printf("\nService not started. \n");
        //printf("  Current State: %d\n", ssStatus.dwCurrentState); 
        //printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
        //printf("  Service Specific Exit Code: %d\n", 
        //    ssStatus.dwServiceSpecificExitCode); 
        //printf("  Check Point: %d\n", ssStatus.dwCheckPoint); 
        //printf("  Wait Hint: %d\n", ssStatus.dwWaitHint); 
        return 0;
    } 
  return 0 ;
}
