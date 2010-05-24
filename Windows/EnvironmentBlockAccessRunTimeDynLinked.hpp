/*
 * EnvironmentBlockAccessRunTimeDynLinked.hpp
 *
 *  Created on: May 22, 2010
 *      Author: Stefan
 */

#ifndef ENVIRONMENTBLOCKACCESSRUNTIMEDYNLINKED_HPP_
#define ENVIRONMENTBLOCKACCESSRUNTIMEDYNLINKED_HPP_
#include <windows.h>

// Global Typedefs for function pointers in USERENV.DLL
typedef BOOL ( STDMETHODCALLTYPE FAR * LPFNCREATEENVIRONMENTBLOCK)
    ( LPVOID  *lpEnvironment,
      HANDLE  hToken,
      BOOL    bInherit );
typedef BOOL (STDMETHODCALLTYPE FAR * LPFNDESTROYENVIRONMENTBLOCK)
    ( LPVOID lpEnvironment );

class EnvironmentBlockAccessRunTimeDynLinked
{
  HMODULE m_hUserEnvLib ;
  LPFNCREATEENVIRONMENTBLOCK  m_lpfnCreateEnvironmentBlock ;
  LPFNDESTROYENVIRONMENTBLOCK m_lpfnDestroyEnvironmentBlock ;
public:
  EnvironmentBlockAccessRunTimeDynLinked() ;
  ~EnvironmentBlockAccessRunTimeDynLinked() ;
  //http://msdn.microsoft.com/en-us/library/bb762270%28VS.85%29.aspx:
  BYTE //WINAPI
    CreateEnvironmentBlock(
    //__out
      LPVOID *lpEnvironment,
    //__in_opt
      HANDLE hToken,
    //__in
      BOOL bInherit
    );
  //http://msdn.microsoft.com/en-us/library/bb762274%28v=VS.85%29.aspx:
  BOOL //WINAPI
    DestroyEnvironmentBlock(
    //__in
    LPVOID lpEnvironment
    );
} ;

#endif /* ENVIRONMENTBLOCKACCESSRUNTIMEDYNLINKED_HPP_ */
