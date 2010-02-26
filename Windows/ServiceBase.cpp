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

DWORD ServiceBase::PauseService(const TCHAR * cp_tchServiceName
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
            SERVICE_CONTROL_PAUSE,   // control value to send 
            &ssStatus) )  // address of status info 
    {
        //printf("ControlService failed (%d)\n", GetLastError()); 
        return FALSE;
    }
  return 0 ;
}

DWORD ServiceBase::CreateService(
  const TCHAR * tchServiceName
  //, TCHAR * tchPathToServiceExe
  )
{ 
  BOOL boolReturn = FALSE ;
  SC_HANDLE schService ;
  SC_HANDLE schSCManager;
  TCHAR szPath[MAX_PATH]; 
  
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
    //    LocalLanguageMessageFromErrorCode(dwLastError)) ;
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
    if( !GetModuleFileName( 
      //If this parameter is NULL, GetModuleFileName retrieves the path 
      //of the executable file of the current process. 
      NULL, 
      szPath, MAX_PATH ) )
      {
          //DEBUG("GetModuleFileName failed (%d)\n", GetLastError()); 
          //LOG( 
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
              "Getting file name for THIS executable file failed: " << 
              LocalLanguageMessageFromErrorCode( ::GetLastError() ) << ")" //<< \n" 
              );
          //return FALSE;
      }
      //else
      //    //DEBUG("Path of this executable:%s\n", szPath);
      //    LOG("Path of this executable:" << szPath << "\n" );

      CreateService(
          schService
          , tchServiceName
          //, tchPathToServiceExe
          , szPath
          , schSCManager );

      if (schService == NULL) 
      {
          DWORD dwLastError = GetLastError() ;
          //DEBUG("Creating the service failed: error number:%d\n", dwLastError); 
          //LOG("Creating the service failed: error number:" << dwLastError 
          //    << "\n" );
          //std::cout 
          WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
              "Creating the service failed: error number:" << 
              dwLastError << "\nError: " << //"\n" 
              LocalLanguageMessageFromErrorCode( dwLastError ) 
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
              std::cout << "The specified service already exists in this database.\n" ;
              //DeleteService(schSCManager, tchServiceName);
              //CreateService(schService,tchServiceName,tchPathcToServiceExe);
              break;
          case ERROR_SERVICE_MARKED_FOR_DELETE:
              WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
                  "Stop the execution of the service: \"" << 
                  tchServiceName << "\"\n" << 
                  "-inside the service control manager\n"
                  "-via the task manager\n"
                  "-by another program/ etc.\n"
                  "and try again."
                  )
                  ;
          } //switch(dwLastError)
          //return FALSE;
      }//if (schService == NULL) 
      else
      {
          //printf("Creating service succeeded\n");
          std::cout << "Creating the service succeeded.\n" ;
          CloseServiceHandle(schService);
          //return TRUE;
      }
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
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/createservice.htm: 
    //"A service started automatically by the service control manager during system startup."
    SERVICE_AUTO_START,
    SERVICE_ERROR_NORMAL,      // error control type 
    // path to service's binary 
    //"Pointer to a null-terminated string that contains the fully qualified path to the service binary file. If the path contains a space, it must be quoted so that it is correctly interpreted. For example, "d:\\my share\\myservice.exe" should be specified as "\"d:\\my share\\myservice.exe\"". 
    //The path can also include arguments for an auto-start service. For example, "d:\\myshare\\myservice.exe arg1 arg2". These arguments are passed to the service entry point (typically the main function)."
    // "..\\GriffinControlService\\Release\\ 
    //"C:\\Dokumente und Einstellungen\\Stefan\\Eigene Dateien\\"
    //"Visual Studio 2005\\Projects\\GriffinControlService\\Release\\"
    //"GriffinControlService.exe",
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

//return: 0 = success
DWORD ServiceBase::DeleteService(
     //SC_HANDLE & r_schSCManager
     //, 
     const TCHAR * tchServiceName
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
      DWORD dwLastError = ::GetLastError() ;
      //return dwLastError ;
      //Throw exception because: an error code can be thrown for 
      //various reasons, so I do not need to return another value for 
      //the cause, e.g. connect to SCM error.
      throw ConnectToSCMerror( dwLastError ) ;
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
            DWORD dwErrorCode = ::GetLastError() ;
            std::cout << "Opening the service for service name \"" << 
                tchServiceName << "\" failed. Error code: " << 
                dwErrorCode << "\n" << "=>Message: " << 
                LocalLanguageMessageFromErrorCode(dwErrorCode) << "\n" ; 
            return FALSE;
        }
     
        if (! ::DeleteService(schService) ) 
        {
            DWORD dwErrorCode = ::GetLastError() ;
            std::cout << "Deleting service failed. Error code:  " << 
                dwErrorCode << "\n" << "=>Message: " << 
                LocalLanguageMessageFromErrorCode(dwErrorCode) << "\n" ;
            return FALSE;
        }
        else 
            std::cout << "Deleting service succeeded.\n" ; 
     
        CloseServiceHandle(schService); 
    }
    //return TRUE;
  return 0 ;
}

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
