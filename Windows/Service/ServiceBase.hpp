#pragma once

//Must be included BEFORE Windows-specific inlcudes (else: comp. errors like:
//C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/istream.tcc:1164: error: expected primary-expression before '.' token

#include <string> //
#include <windows.h> //BOOL
//#include <specstrings.h> // "__in" etc.

enum errorCodes
{
  OpenSCManagerFailed = 1,
  OpenServiceFailed,
  DeleteServiceFailed
};

class ConnectToSCMerror
{
public:
  DWORD m_dwErrorCode ;
  ConnectToSCMerror(DWORD dwErrorCode) { m_dwErrorCode = dwErrorCode ; }
};

class ServiceBase
{
protected:
  SERVICE_STATUS_HANDLE m_service_status_handle ;
public:
  static DWORD ContinueService(
   const TCHAR * tchServiceName
   ) ;
  static DWORD CreateService(
    const TCHAR * tchServiceName
    ) ;
  static DWORD CreateService(
    SC_HANDLE & r_schService 
    , const TCHAR * tchServiceName
    , TCHAR * tchPathcToServiceExe
    , SC_HANDLE schSCManager
    ) ;
  //return: 0 = success
  static //DWORD
    BYTE DeleteService(
      const TCHAR * tchServiceName
      , DWORD & dwErrorCodeFor1stError
   ) ;
  static void GetErrorDescriptionFromRegSvcCtrlHandlerExErrCode(
    DWORD dwLastError ,
    std::string & r_stdstrErrorDescription
    ) ;
  static void GetErrorDescriptionFromOpenServiceErrCode(
    DWORD dwLastError ,
    std::string & r_stdstrErrorDescription
    ) ;
  static DWORD PauseService(
   const TCHAR * tchServiceName
   , std::string & r_stdstrMsg
   ) ;
  static void GetPossibleSolution(
    DWORD dwWinError ,
    const TCHAR * tchServiceName
    //use as reference param and do not return because so it is not copied
    //->faster
    , std::string & r_stdstrPossibleSolution
    ) ;
  //from http://msdn.microsoft.com/en-us/library/ms685058%28VS.85%29.aspx:
  SERVICE_STATUS_HANDLE //WINAPI
    RegSvcCtrlHandlerExAndGetErrMsg(
    //__in
      LPCTSTR lpServiceName,
    //__in
      LPHANDLER_FUNCTION_EX lpHandlerProc,
    //__in_opt
      LPVOID lpContext ,
//      std::string & r_stdstrErrorDescription
      DWORD & r_dwErrorCode
    );
//  SERVICE_STATUS_HANDLE //WINAPI
//    RegSvcCtrlHandlerExAndGetErrMsg(
//    //__in
//      LPCTSTR lpServiceName,
//    //__in_opt
//      LPVOID lpContext ,
////      std::string & r_stdstrErrorDescription
//      DWORD & r_dwErrorCode
//    );
//  //from http://msdn.microsoft.com/en-us/library/ms683241%28v=VS.85%29.aspx:
//  //overwrite in subclasses
//  //MUST be static (=without "this" pointer ) because this is a
//  //callback function from Windows API.
//  static virtual DWORD WINAPI ServiceCtrlHandlerEx(
//    __in  DWORD dwControl,
//    __in  DWORD dwEventType,
//    __in  LPVOID lpEventData,
//    __in  LPVOID lpContext
//    ) { return 0 ; } ;
  static DWORD StartService(
    LPCTSTR lpServiceName ) ;
  static DWORD StartService(
    SC_HANDLE schSCManager
    , LPCTSTR lpServiceName ) ;
};
