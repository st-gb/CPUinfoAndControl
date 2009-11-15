#include "global.h" //for BYTE
#include "CPUIDaccess.hpp"

#ifdef WIN32
  #ifndef WINRING_DLL_NAME 
    #define WINRING_DLL_NAME "WinRing0.dll"
  #endif
#endif
#ifdef X64
  #ifndef WINRING_DLL_NAME 
    #define WINRING_DLL_NAME "WinRing0x64.dll"
  #endif
#endif

CPUIDaccess::CPUIDaccess()
{
  m_wxdynamiclibraryHWaccess.Load(
    "WinRing0.dll"
    //WINRING_DLL_NAME
    //, int flags = wxDL_DEFAULT
    ) ;
}

BYTE CPUIDaccess::GetNumberOfPhysicalCores()
{
  //m_wxdynamiclibraryHWaccess.
  return 0 ;
}
