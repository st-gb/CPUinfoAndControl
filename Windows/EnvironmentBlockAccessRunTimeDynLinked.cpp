/*
 * EnvironmentBlockAccessRunTimeDynLinked.cpp
 *
 *  Created on: May 22, 2010
 *      Author: Stefan
 */
#include "EnvironmentBlockAccessRunTimeDynLinked.hpp"
#include <windows.h> //GetProcAddress()
#include <tchar.h> //_T() macro

//void CreateEnvironmentBlockFunctionExitsInDLL()
//{
//
//}

EnvironmentBlockAccessRunTimeDynLinked::EnvironmentBlockAccessRunTimeDynLinked()
{
  m_hUserEnvLib = ::LoadLibrary( _T("userenv.dll") );
  if ( m_hUserEnvLib )
  {
    m_lpfnCreateEnvironmentBlock = (LPFNCREATEENVIRONMENTBLOCK)
    GetProcAddress( m_hUserEnvLib, "CreateEnvironmentBlock" );

    m_lpfnDestroyEnvironmentBlock = (LPFNDESTROYENVIRONMENTBLOCK)
    GetProcAddress( m_hUserEnvLib, "DestroyEnvironmentBlock" );
  }
  else
  {
    m_lpfnCreateEnvironmentBlock = NULL ;
    m_lpfnDestroyEnvironmentBlock = NULL ;
  }
}

EnvironmentBlockAccessRunTimeDynLinked::~EnvironmentBlockAccessRunTimeDynLinked()
{
  if ( m_hUserEnvLib )
  {
    FreeLibrary( m_hUserEnvLib );
  }
}

//adapted from http://social.msdn.microsoft.com/Forums/en/windowssecurity/
//  thread/31bfa13d-982b-4b1a-bff3-2761ade5214f:
BYTE EnvironmentBlockAccessRunTimeDynLinked::CreateEnvironmentBlock(
  //__out
  LPVOID * lpEnvironment,
  //__in_opt
  HANDLE hToken,
  //__in
  BOOL bInherit
  )
{
  if ( m_lpfnCreateEnvironmentBlock )
  {
    if ( m_lpfnCreateEnvironmentBlock(
        lpEnvironment,
  //          hTokenDup,
        hToken,
        //"Specifies whether to inherit from the current process's environment."
        bInherit
        )
      )
    {
    }
    else
    {
      lpEnvironment = NULL;
//            OutputDebugString( _T(" CreateEnvironmentBlock() -- FAILED") );
//      DEBUGN("for spawning a process: CreateEnvironmentBlock() failed") ;
    }
  }
  else
  {
//        OutputDebugString(_T(" FAILED - GetProcAddress(CreateEnvironmentBlock)"));
//    DEBUGN( " FAILED - GetProcAddress(CreateEnvironmentBlock)") ;
  }
}

BOOL //WINAPI
  EnvironmentBlockAccessRunTimeDynLinked::DestroyEnvironmentBlock(
  //__in
  LPVOID lpEnvironment
  )
{
  if ( m_lpfnDestroyEnvironmentBlock )
    m_lpfnDestroyEnvironmentBlock( lpEnvironment );
}
