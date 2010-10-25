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

//extern CPUcontrolBase * gp_cpucontrolbase ;
extern I_CPUaccess * g_p_cpuaccess ;

////from http://stackoverflow.com/questions/2094427/dll-export-as-a-c-c-function:
//#ifdef _WIN32
//  #define AM_LIB_EXPORT __declspec(dllexport)
//#else
//  #define AM_LIB_EXPORT __attribute__ ((visibility("default")))
//#endif // _WIN32

//from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
//("Calling an Exported Function in an EXE from Within a DLL"):
// Do exactly as you would export a DLL...
#ifdef  __cplusplus
extern "C"
{
#endif //#ifdef __cplusplus

  //see http://en.wikipedia.org/wiki/PCI_configuration_space:
  AM_LIB_EXPORT BOOL ReadPCIconfigSpace(
    BYTE byPCIbus , //"8-bit PCI bus",
    BYTE byDeviceAndFunction ,//"5-bit device, and 3-bit function"
    DWORD dwRegisterAddress ,
    PDWORD p_dwValue
  )
  {
//    LOGN("ReadPCIconfigSpace")
//    BOOL boolRet = FALSE ;
    //http://en.wikipedia.org/wiki/Static_variable:
    //"Static local variables: variables declared as static inside a function
    // are statically allocated while having the same scope as automatic local
    // variables. Hence whatever values the function puts into its static
    //local variables during one call will still be present when the function
    //is called again."
    //By using static: not created on stack (->faster)
    static BOOL boolRet ;
    boolRet = FALSE ;
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
    //May be NULL if CPUaccess' init failed
    if( //p_cpuaccess
        g_p_cpuaccess )
    {
      boolRet = //p_cpuaccess->RdmsrEx(
        g_p_cpuaccess->ReadPciConfigDwordEx(
          ( byPCIbus << 8 ) | byDeviceAndFunction ,
          dwRegisterAddress,
          p_dwValue
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

  //EXPORT
  AM_LIB_EXPORT BOOL ReadMSR(
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
  )
  {
//    LOGN("ReadMSR")
//    BOOL boolRet = FALSE ;
    //http://en.wikipedia.org/wiki/Static_variable:
    //"Static local variables: variables declared as static inside a function
    // are statically allocated while having the same scope as automatic local
    // variables. Hence whatever values the function puts into its static
    //local variables during one call will still be present when the function
    //is called again."
    //By using static: not created on stack (->faster)
    static BOOL boolRet ;
    boolRet = FALSE ;
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
    DEBUGN("Exe's exported ReadMSR("
      << "MSR index:" << dwIndex
      << " " << p_dweax
      << " " << p_dwedx
      << " Aff mask:" << affinityMask
      << ")"
      << " g_p_cpuaccess:" << g_p_cpuaccess
      )
    //May be NULL if CPUaccess' init failed
    if( //p_cpuaccess
      g_p_cpuaccess )
    {
      boolRet = //p_cpuaccess->RdmsrEx(
        g_p_cpuaccess->RdmsrEx(
        dwIndex,
        p_dweax,// bit  0-31 (register "EAX")
        p_dwedx,
        //m_dwAffinityMask
        affinityMask
        ) ;
      DEBUGN( "exe::ReadMSR after g_p_cpuaccess->RdmsrEx(Index,affinityMask): "
        << dwIndex << " "
        << * p_dweax << " "
        << * p_dwedx << " "
        << affinityMask
        << "\n" )
    }
    #ifdef _DEBUG
    //if( dwIndex == 0x1AD )
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
//    BOOL boolRet = FALSE ;
    //http://en.wikipedia.org/wiki/Static_variable:
    //"Static local variables: variables declared as static inside a function
    // are statically allocated while having the same scope as automatic local
    // variables. Hence whatever values the function puts into its static
    //local variables during one call will still be present when the function
    //is called again."
    //By using static: not created on stack (->faster)
    static BOOL boolRet ;
    boolRet = FALSE ;
//    LOGN("WriteMSR")
    DEBUGN("::WriteMSR(" << dwIndex
      << "," << dwEAX
      << "," << dwEDX
      << "," << affinityMask
      << ")")
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
    DEBUGN("::WriteMSR(...)--CPUaccess pointer:" << //p_cpuaccess
      g_p_cpuaccess )
    //May be NULL if CPUaccess' init failed
    if( //p_cpuaccess
        g_p_cpuaccess )
    {
      boolRet = //p_cpuaccess->WrmsrEx(
        g_p_cpuaccess->WrmsrEx(
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
