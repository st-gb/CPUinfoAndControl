//#include <wxWidgets/App.hpp>
#include <Controller/CPUcontrolBase.hpp>
#include <Controller/I_CPUaccess.hpp>
#include "exported_functions.h"
//#include <windef.h>
//#include <Windows_compatible_typedefs.h> //DWORD etc.

//class wx86InfoAndControlApp ;
//extern wx86InfoAndControlApp & wxGetApp();
class CPUcontrolBase ;

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
  //EXPORT
  AM_LIB_EXPORT BOOL ReadMSR(
    DWORD dwIndex,		// MSR index
    PDWORD p_dweax,			// bit  0-31
    PDWORD p_dwedx,			// bit 32-63
    DWORD_PTR affinityMask	// Thread Affinity Mask
  )
  {
    //MessageBox(NULL,lpszMessage,"From Exe",MB_OK);
    I_CPUaccess * p_cpuaccess = //wxGetApp().
      gp_cpucontrolbase->
      GetCPUaccess() ;
    return p_cpuaccess->RdmsrEx(
      dwIndex,
      p_dweax,// bit  0-31 (register "EAX")
      p_dwedx,
      //m_dwAffinityMask
      affinityMask
      ) ;
  }
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus

