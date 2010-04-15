#pragma once

#include <windows.h> //BOOL

class ConnectToSCMerror
{
public:
  DWORD m_dwErrorCode ;
  ConnectToSCMerror(DWORD dwErrorCode) { m_dwErrorCode = dwErrorCode ; }
};

class ServiceBase
{
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
  static DWORD DeleteService(
   const TCHAR * tchServiceName
   ) ;
  static DWORD PauseService(
   const TCHAR * tchServiceName
   ) ;
  static void PrintPossibleSolution(DWORD dwWinError , const TCHAR * tchServiceName) ;
  static DWORD StartService(
    LPCTSTR lpServiceName ) ;
  static DWORD StartService(
    SC_HANDLE schSCManager
    , LPCTSTR lpServiceName ) ;
};
