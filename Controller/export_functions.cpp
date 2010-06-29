//#include <wxWidgets/App.hpp>
#include <Controller/CPUcontrolBase.hpp>
#include <Controller/exported_functions.h> //for AM_LIB_EXPORT
#include <Controller/I_CPUaccess.hpp>
#include "exported_functions.h"
//#include <windef.h>
//#include <Windows_compatible_typedefs.h> //DWORD etc.

//class wx86InfoAndControlApp ;
//extern wx86InfoAndControlApp & wxGetApp();
//class CPUcontrolBase ;

extern CPUcontrolBase * gp_cpucontrolbase ;

//from http://stackoverflow.com/questions/2094427/dll-export-as-a-c-c-function:
#ifdef _WIN32
#define AM_LIB_EXPORT __declspec(dllexport)
#else
#define AM_LIB_EXPORT
#endif // _WIN32

//from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
//("Calling an Exported Function in an EXE from Within a DLL"):
// Do exactly as you would export a DLL...
#ifdef  __cplusplus
extern "C"
{
#endif //#ifdef __cplusplus
//  //EXPORT
//  AM_LIB_EXPORT BOOL ReadMSR(
//    DWORD dwIndex,		// MSR index
//    PDWORD p_dweax,			// bit  0-31
//    PDWORD p_dwedx,			// bit 32-63
//    DWORD_PTR affinityMask	// Thread Affinity Mask
//  )
//  {
//    //MessageBox(NULL,lpszMessage,"From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
//    return p_cpuaccess->RdmsrEx(
//      dwIndex,
//      p_dweax,// bit  0-31 (register "EAX")
//      p_dwedx,
//      //m_dwAffinityMask
//      affinityMask
//      ) ;
//  }

  //EXPORT
  AM_LIB_EXPORT BOOL ReadMSR(
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
  )
  {
    BOOL boolRet = FALSE ;
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
    I_CPUaccess * p_cpuaccess = //wxGetApp().
      gp_cpucontrolbase->
      GetCPUaccess() ;
    //May be NULL if CPUaccess' init failed
    if( p_cpuaccess )
    {
      boolRet = p_cpuaccess->RdmsrEx(
        dwIndex,
        p_dweax,// bit  0-31 (register "EAX")
        p_dwedx,
        //m_dwAffinityMask
        affinityMask
        ) ;
    }
    #ifdef _DEBUG
    //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::ReadMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << *p_dweax << " "
//        << *p_dwedx << " "
//        << affinityMask
//        << "\n" )
    #endif
    return boolRet ;
  }

  AM_LIB_EXPORT BOOL WriteMSR(
    DWORD dwIndex,    // MSR index
    DWORD dwEAX,     // bit  0-31
    DWORD dwEDX,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
    BOOL boolRet = FALSE ;
    DEBUGN("::WriteMSR(" << dwIndex
      << "," << dwEAX
      << "," << dwEDX
      << "," << affinityMask
      << ")")
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
    I_CPUaccess * p_cpuaccess = //wxGetApp().
      gp_cpucontrolbase->
      GetCPUaccess() ;
    DEBUGN("::WriteMSR(...)--CPUaccess pointer:" << p_cpuaccess )
    //May be NULL if CPUaccess' init failed
    if( p_cpuaccess )
    {
      boolRet = p_cpuaccess->WrmsrEx(
        dwIndex,
        dwEAX,// bit  0-31 (register "EAX")
        dwEDX,
        //m_dwAffinityMask
        affinityMask
        ) ;
    }
    DEBUGN("::WriteMSR(...) after p_cpuaccess->WrmsrEx(...)")
    #ifdef _DEBUG
    //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::WriteMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << dwEAX << " "
//        << dwEDX << " "
//        << affinityMask
//        << "\n" )
    #endif
    return boolRet ;
  }
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus

