/*
 * CreateProcess.cpp
 *
 *  Created on: May 22, 2010
 *      Author: Stefan
 */

//for TokenElevation
#if _WIN32_WINNT < 0x0600
  #undef _WIN32_WINNT
  #define _WIN32_WINNT 0x0600
#endif
#include <windows.h>
#include <winnt.h> //TOKEN_ELEVATION_TYPE

//from http://social.msdn.microsoft.com/Forums/en/windowssecurity/thread/31bfa13d-982b-4b1a-bff3-2761ade5214f:
#include "userenv.h"
#include <Aclapi.h> //GetSecurityInfo()
#include <tchar.h>
#include <windef.h> //BOOL
#include <global.h>
#include <Windows/LocalLanguageMessageFromErrorCode.h>
//some MinGW installation's <wtsapi32.h> do not declare "WTSQueryUserToken"
#ifdef USE_WTSQueryUserToken
  #if _WIN32_WINNT < 0x0501
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
  #endif
  #include <wtsapi32.h>
#endif //#ifdef USE_WTSQueryUserToken

#include <ModelData/ServiceAttributes.hpp>
#include "CreateProcess.hpp"

#ifndef _MSC_VER // defined in winnt.h
typedef enum  {
  TokenElevationTypeDefault   = 1,
  TokenElevationTypeFull,
  TokenElevationTypeLimited
} TOKEN_ELEVATION_TYPE , *PTOKEN_ELEVATION_TYPE;
#endif

//for TokenElevation
#if _WIN32_WINNT < 0x0600
  #undef _WIN32_WINNT
  #define _WIN32_WINNT 0x0600
#endif

void LogTokenInfo(HANDLE TokenHandle)
{
  //elevation info is beginning from Win 6 (->not for XP):
//  DWORD dwReturnLength ;
//  TOKEN_ELEVATION_TYPE token_elevation_type ;
//  BOOL bool_ = GetTokenInformation(
//    TokenHandle, //__in       HANDLE TokenHandle,
//    TokenElevationType ,//__in       TOKEN_INFORMATION_CLASS TokenInformationClass,
//    & token_elevation_type ,//__out_opt  LPVOID TokenInformation,
//    sizeof(token_elevation_type) ,//__in       DWORD TokenInformationLength,
//    & dwReturnLength //__out      PDWORD ReturnLength
//  );
//  LOGN("GetTokenInformation: "
//    << " tokeninfo len:" << sizeof(token_elevation_type)
//    << " ret len: " << dwReturnLength )
//  if ( bool_ )
//  {
//    switch(token_elevation_type)
//    {
//    case TokenElevationTypeDefault:
//      LOGN("TokenElevationTypeDefault")
//      break;
//    case TokenElevationTypeFull:
//      LOGN("TokenElevationTypeFull")
//      break;
//    case TokenElevationTypeLimited :
//      LOGN("TokenElevationTypeLimited")
//      break;
//    }
//  }
//  else
//  {
//    DWORD dwErrorCode = 0;
//    dwErrorCode = GetLastError();
//    LOGN("GetTokenInformation error:" << dwErrorCode <<
//      LocalLanguageMessageFromErrorCode(dwErrorCode) );
//  }
}

DWORD GetProcessSecurityDescriptor(
  PSECURITY_DESCRIPTOR & rp_security_descriptor ,
  HANDLE handleProcess
  )
{
  //http://msdn.microsoft.com/en-us/library/aa446629%28v=VS.85%29.aspx:
  DWORD dwRtnCode = GetSecurityInfo(
    handleProcess ,//__in       HANDLE handle,
    //When SE_SERVICE: "GetSecurityInfo error:6The handle is invalid."
//        SE_SERVICE , //__in       SE_OBJECT_TYPE ObjectType,
    SE_KERNEL_OBJECT ,
    //"A set of bit flags that indicate the type of security information to retrieve."
    0 ,//__in       SECURITY_INFORMATION SecurityInfo,
//    OWNER_SECURITY_INFORMATION ,
//        NULL ,//__out_opt  PSID *ppsidOwner,
    NULL , //& pSidOwner ,
    NULL ,//__out_opt  PSID *ppsidGroup,
    NULL ,//__out_opt  PACL *ppDacl,
    NULL ,//__out_opt  PACL *ppSacl,
    //"When you have finished using the pointer, free the returned buffer
    //by calling the LocalFree function."
    & rp_security_descriptor //__out_opt  PSECURITY_DESCRIPTOR *ppSecurityDescriptor
  );
  return dwRtnCode ;
}

HANDLE GetHandleForCurrentProcess()
{
  DWORD dwProcID = GetCurrentProcessId() ;
  LOGN("this process' ID:" << dwProcID ) ;
  //    GetSecurityDescriptorSacl()()
  //      OpenProcessToken() ;
  //      HANDLE hProc = GetCurrentProcess() ;
  HANDLE handleCurrentProcess = OpenProcess(
    PROCESS_ALL_ACCESS, //__in  DWORD dwDesiredAccess,
    FALSE, //__in  BOOL bInheritHandle,
    dwProcID // __in  DWORD dwProcessId
    );
  LOGN("handle to this process: " << handleCurrentProcess )
  if( handleCurrentProcess == NULL )
  {
    DWORD dwErrorCode = 0;
    dwErrorCode = GetLastError();
    LOGN("OpenProcess error:" << dwErrorCode <<
      LocalLanguageMessageFromErrorCode(dwErrorCode) );
  }
  return handleCurrentProcess ;
}

bool GetTokenHandleForThisProcess(
  HANDLE handleThisProcess ,
  HANDLE & r_handleProcessToken
  )
{
  bool bSuccess = false ;
  {
    if( OpenProcessToken(
        handleThisProcess,
        TOKEN_ALL_ACCESS,
        & r_handleProcessToken
        )
      )
    {
      bSuccess = true ;
//      LOGN("OpenProcessToken for service succ:")
//      LogTokenInfo(r_handleProcessToken) ;
    }
    else
    {
      DWORD dwErrorCode = 0;
      dwErrorCode = GetLastError();
      LOGN("error opening a token for this process:"
        << dwErrorCode
        << LocalLanguageMessageFromErrorCode(dwErrorCode) );
    }
  }
  return bSuccess ;
}

BOOL SetSessionForToken(HANDLE hTokenDup, DWORD dwSessionID)
{
//  DWORD dwReturnLength ;
//  TOKEN_ELEVATION_TYPE token_elevation_type = TokenElevationTypeDefault ;
  BOOL bool_ =
    //"If the function succeeds, the function returns nonzero."
    SetTokenInformation(
    hTokenDup, //__in       HANDLE TokenHandle,
    //__in       TOKEN_INFORMATION_CLASS TokenInformationClass
//        TokenElevationType ,
    //This causes the process to start in the Desktop of the user.
    TokenSessionId ,
//        & token_elevation_type ,//__in  LPVOID TokenInformation,
    & dwSessionID ,
    //__in       DWORD TokenInformationLength,
//        sizeof(token_elevation_type)
    sizeof(dwSessionID)
    );
//  LOGN("SetTokenInformation: "
//    << " tokeninfo len:" << sizeof(token_elevation_type)
    //<< " ret len: " << dwReturnLength
//    )
  if ( bool_ )
  {
//    LOGN("set token info succeeded")
//    LogTokenInfo(hTokenDup) ;
  }
  else
  {
    DWORD dw = GetLastError() ;
    LOGN("setting token information failed:"
      <<  dw << LocalLanguageMessageFromErrorCode(dw) ) ;
  }
  return bool_ ;
}

BOOL DuplicateTokenAsNeeded(HANDLE hToken, HANDLE & r_hTokenDup)
{
  BOOL boolSuccess =
    //http://msdn.microsoft.com/en-us/library/aa446617%28VS.85%29.aspx:
    DuplicateTokenEx(
  //        handleThisProcess ,
      hToken,
      //dwDesiredAccess
      //"To request all access rights that are valid for the caller, specify
      //MAXIMUM_ALLOWED."
      MAXIMUM_ALLOWED |
  //        TOKEN_IMPERSONATE |
      //For CreateProcessAsUser:
      //". The handle must have the TOKEN_QUERY, TOKEN_DUPLICATE, and
      //TOKEN_ASSIGN_PRIMARY access rights. "
      TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY
      | DELETE | READ_CONTROL | WRITE_DAC
      //| WRITE_OWNER
      //"to get or set the SACL in the object's security descriptor."
      //this value caused "[...] is not held by the client"(?)
  //        | ACCESS_SYSTEM_SECURITY
      ,
  //      TOKEN_IMPERSONATE| TOKEN_READ| TOKEN_ASSIGN_PRIMARY| TOKEN_DUPLICATE,
      //lpTokenAttributes [in, optional] :
      //". If lpTokenAttributes is NULL, the token gets a default security
      //descriptor and the handle cannot be inherited. "
      NULL,
  //      & sa ,
      //"Impersonation is the ability of a process to take on the security
      //attributes of another process."
  //        SecurityImpersonation,
      SecurityIdentification ,
  //        SecurityAnonymous ,
      //TokenPrimary: "The new token is a primary token that you can use in the
      //CreateProcessAsUser function."
      TokenPrimary,
      //phNewToken [out]
      //"A pointer to a HANDLE variable that receives the new token.
      //When you have finished using the new token, call the CloseHandle
      //function to close the token handle."
      & r_hTokenDup
      )
    ;
  if(boolSuccess)
  {
//    LOGN("DuplicateTokenEx succeeded")
//    LogTokenInfo(r_hTokenDup) ;
  }
  else
  {
    DWORD dw = GetLastError() ;
    LOGN("Duplicating token failed:"
      <<  dw << LocalLanguageMessageFromErrorCode(dw) ) ;
  }
  return boolSuccess ;
}

BOOL CreateGUIprocess::CreateProcessAsNeeded(
  //Use param here for less error-prone change ( so the param value
  // can be switched easily)
  HANDLE handleToken
  )
{
  LOGWN_WSPRINTF(L"before starting process %ls with current dir %ls",
    m_r_service_attrs.m_stdwstrPathToGUIexe.c_str() ,
    m_r_service_attrs.m_stdwstrGUICurrentDirFullPathTo.c_str()
    )
//  std::stdwstrPathToGUIexe = m_r_service_attrs.m_stdwstrPathToGUIexe ;
  return
  //info from http://msdn.microsoft.com/en-us/library/ms682429%28VS.85%29.aspx:
  //Must use CreateProcessAsUser because we need to specify the token modified
  // to run on the specific session.
  ::CreateProcessAsUserW(
    //"A handle to the
    //primary token
    //that represents a user."
    //". The handle must have the TOKEN_QUERY, TOKEN_DUPLICATE, and
    //TOKEN_ASSIGN_PRIMARY access rights. "
  //  hTokenDup,
    handleToken ,
  //      hTokenService,
  //      hToken,
  //      CreateProcess(
    //LPCTSTR lpApplicationName
    NULL,
    //lpCommandLine [in, out, optional]
  //      _T("c:\\windows\\notepad.exe"),
  //  _T("C:\\Users\\Stefan\\x86InfoAndControl\\"
  //    "x86InfoAndControl_GUI_MinGW32bit.exe") ,
  //  Getstdtstring( m_r_service_attrs.m_stdstrPathToGUIexe )
    (WCHAR*) m_r_service_attrs.m_stdwstrPathToGUIexe.c_str() ,
  //"The default security descriptor is that of the user referenced in the
  //hToken  parameter"
     NULL,
//     & sa ,
   //"The default security descriptor is that of the user referenced in the
   //hToken  parameter."
     NULL,
//     & sa ,
    //"If this parameter is TRUE, each inheritable handle in the calling
    //process is inherited by the new process. If the parameter is FALSE,
    //the handles are not inherited. Note that inherited handles have the
    //same value and access rights as the original handles."
    //Do NOT inherit handles (else handle to the service's log file etc. is
    //inherited).
    FALSE,
    //     TRUE ,
    //dwCreationFlags [in]
    //     dwCreationFlag,
    0 ,
    //"A pointer to an environment block for the new process. If this
    //parameter is NULL, the new process uses the environment of the
    //calling process."
    //     pEnvironmentUser,
    NULL ,
    //"The full path to the current directory for the process.
    //The string can also specify a UNC path. "
    //       NULL,
    //   _T("C:\\Users\\Stefan\\x86InfoAndControl\\") ,
    m_r_service_attrs.m_stdwstrGUICurrentDirFullPathTo.c_str() ,
    & startupinfo,
    & process_information
    );
}

#ifdef USE_WTSQueryUserToken
BOOL GetUserTokenForSession( DWORD dwSessionID, HANDLE & r_hToken )
{
  BOOL bool_ = WTSQueryUserToken(dwSessionID, & r_hToken ) ;
  if( bool_ )
  {
    LOGN("WTSQueryUserToken succeeded")
//    LogTokenInfo(hToken) ;
  }
  else
    LOGN("WTSQueryUserToken failed")
}
#endif //#ifdef USE_WTSQueryUserToken

DWORD GetSecurityDescriptorAndAssignToSecurityAttributes(
  SECURITY_ATTRIBUTES & r_security_attributes ,
  PSECURITY_DESCRIPTOR & rp_security_descriptor ,
  HANDLE handleProcess
  )
{
//  SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION ;
//  SECURITY_ATTRIBUTES sa ;
//  PSID pSidOwner = NULL;
//  PSECURITY_DESCRIPTOR p_security_descriptor = NULL;
  DWORD dwRtnCode = GetProcessSecurityDescriptor(
    rp_security_descriptor,
    handleProcess ) ;
  // Check GetLastError for GetSecurityInfo error condition.
  if ( dwRtnCode == ERROR_SUCCESS)
  {
    //http://msdn.microsoft.com/en-us/library/aa379560%28VS.85%29.aspx:
    //"A pointer to a security descriptor for the object that controls the
    //sharing of it. If NULL is specified for this member, the object is
    //assigned the default security descriptor of the calling process."
    r_security_attributes.lpSecurityDescriptor = rp_security_descriptor ;
    r_security_attributes.bInheritHandle = TRUE ;
    r_security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES) ;
    LOGN("GetSecurityInfo succ"
      << " sec desc:" << rp_security_descriptor )
  }
  else
  {
    DWORD dwErrorCode = 0;
    dwErrorCode = GetLastError();
    LOGN("GetSecurityInfo error:" << dwErrorCode <<
      LocalLanguageMessageFromErrorCode(dwErrorCode) );
//    return ;
  }
  return dwRtnCode ;
}

void CreateGUIprocess::StartProcess(DWORD dwSessionID)
{
  // Local Variable Declarations
//  HANDLE hToken    = NULL;
  HANDLE hTokenDup = NULL;

  ZeroMemory( & startupinfo, sizeof( STARTUPINFOW ) );
  ZeroMemory( & process_information, sizeof( PROCESS_INFORMATION ) );

  startupinfo.cb = sizeof( STARTUPINFOW );
//  startupinfo.lpDesktop = _T("Winsta0\\Default");
  startupinfo.lpDesktop = L"Winsta0\\Default" ;

//  DWORD  dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

//  LPVOID pEnvironmentUser = NULL;
  //not needed?!:
  //EnvironmentBlockAccessRunTimeDynLinked env_block_dyn_linked ;

//    OpenThreadToken( GetCurrentThread(), TOKEN_DUPLICATE, TRUE, &hToken );

  HANDLE handleThisProcess = GetHandleForCurrentProcess() ;
  if( ! handleThisProcess )
    return ;
  HANDLE hTokenService ;
  if( ! GetTokenHandleForThisProcess(handleThisProcess,hTokenService)
    )
    return ;

//  SECURITY_ATTRIBUTES security_attributes ;
//  PSECURITY_DESCRIPTOR p_security_descriptor = NULL;
//  GetSecurityDescriptorAndAssignToSecurityAttributes(
//    security_attributes ,
//    p_security_descriptor ,
//    handleThisProcess
//    ) ;

  BOOL boolSetSessionForToken = FALSE ;
  if( ! DuplicateTokenAsNeeded(hTokenService, hTokenDup)  )
    return ;
  boolSetSessionForToken = SetSessionForToken(hTokenDup,dwSessionID) ;
  if( ! boolSetSessionForToken )
    return ;
//    SetTokenInformation()

//    GetTokenInformation(hTokenDup) ;
//    RevertToSelf( );

//  env_block_dyn_linked.CreateEnvironmentBlock(
//    & pEnvironmentUser,
////          hTokenDup,
//    hToken,
//    //"Specifies whether to inherit from the current process's environment."
//    FALSE
//    ) ;
//  if( pEnvironmentUser )
//    //http://msdn.microsoft.com/en-us/library/bb762270%28VS.85%29.aspx
//    //"If the environment block is passed to CreateProcessAsUser, you
//    //must also specify the CREATE_UNICODE_ENVIRONMENT flag"
//    dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;   // must specify

//    GetSecurityDescriptorSacl()()

  BOOL bool_ = CreateProcessAsNeeded(hTokenDup) ;

//  CloseHandle( hToken );
  CloseHandle(hTokenService) ;
//    CloseHandle( hTokenDup );
  if( bool_ )
  {
    LOGN("creating process succeeded.") ;
  }
  else
  {
    DWORD dw = GetLastError() ;
    LOGN("creating process failed:"
      <<  dw << LocalLanguageMessageFromErrorCode(dw) ) ;
  }
//  LocalFree(p_security_descriptor) ;

//  env_block_dyn_linked.DestroyEnvironmentBlock() ;
  CloseHandle( hTokenDup );
}

DWORD WINAPI StartProcessThread( LPVOID lpParam )
{
  LOGN("StartProcessThread");
  //while(1)
  //{
  //Sleep some seconds, else the system tray icon can't be set if the logon
  //has started immediately before.
//    Sleep(5000);
//  CreateGUIprocess create_gui_process ;
//  create_gui_process.StartProcess((DWORD)lpParam) ;
  //}
  return 0;
}

void InvokeApp(DWORD dwSessionID)
{
  DWORD dwThreadId ;
  HANDLE handleThread =
    ::CreateThread(
    NULL,                   // default security attributes
    0,                      // use default stack size
    StartProcessThread,       // thread function name
    (void*)dwSessionID ,// argument to thread function
    0,                      // use default creation flags
    & dwThreadId
    );   // returns the thread identifier
  if ( handleThread == NULL )
  {
     LOGN("creating the start process thread failed");
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
}
