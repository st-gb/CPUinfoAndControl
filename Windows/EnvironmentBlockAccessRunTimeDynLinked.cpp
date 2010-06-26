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
  BOOL bool_ = FALSE ;
  if ( m_lpfnCreateEnvironmentBlock )
  {
    bool_ =
      //http://msdn.microsoft.com/en-us/library/bb762270%28VS.85%29.aspx:
      //"TRUE if successful; otherwise, FALSE."
      m_lpfnCreateEnvironmentBlock(
      lpEnvironment,
//          hTokenDup,
      hToken,
      //"Specifies whether to inherit from the current process's environment."
      bInherit
      ) ;
    if( bool_ )
    {
    }
    else
    {
      lpEnvironment = NULL;
//            OutputDebugString( _T(" CreateEnvironmentBlock() -- FAILED") );
//      DEBUGN("for spawning a process: CreateEnvironmentBlock() failed") ;
    }
    return bool_ ;
  }
  else
  {
//        OutputDebugString(_T(" FAILED - GetProcAddress(CreateEnvironmentBlock)"));
//    DEBUGN( " FAILED - GetProcAddress(CreateEnvironmentBlock)") ;
  }
  return bool_ ;
}

BOOL //WINAPI
  EnvironmentBlockAccessRunTimeDynLinked::DestroyEnvironmentBlock(
  //__in
  LPVOID lpEnvironment
  )
{
  if ( m_lpfnDestroyEnvironmentBlock )
    return
      //http://msdn.microsoft.com/en-us/library/bb762274%28v=VS.85%29.aspx:
      //"TRUE if successful; otherwise, FALSE."
      m_lpfnDestroyEnvironmentBlock( lpEnvironment );
  return FALSE ;
}
