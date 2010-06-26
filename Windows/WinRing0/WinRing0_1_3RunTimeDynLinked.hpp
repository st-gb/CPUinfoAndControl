#pragma once //include guard
//#include <windef.h> //for  WINAPI in OlsApiInitDef.h
//to resolve WINAPI in OlsApiInitDef.h
////for use with "/MD[d]" else: fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
//#define _AFXDLL

//#define IMPORT typedef __declspec(dllimport)
//
//IMPORT double (*GET_VERSION)();
//#define WINAPI __stdcall
//#define WINAPI IMPORT

#include <Windows.h> //for WINAPI, ::Sleep(...)
//#include <OlsApiInitDef.h> //for _IsMsr etc.
//#include <OlsDef.h> //for InitializeOls return codes: OLS_DLL_NO_ERROR, ...
#include <WinRing0_1_3_1b/source/dll/OlsApiInitExt.h>

//#include <OlsApiInitDef.h> //for "_WrmsrEx" etc.
//typedef DWORD (WINAPI *_RdmsrEx) (DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR affinityMask);
//typedef DWORD (WINAPI *_WrmsrEx) (DWORD index, DWORD eax, DWORD edx, DWORD_PTR affinityMask);
//typedef DWORD (WINAPI *_InitializeOls) ();
//typedef VOID (WINAPI *_DeinitializeOls) ();
typedef BYTE ( WINAPI * _GetDetailedWinRing0Error) (PDWORD p_dwWinErrorCode, PBYTE
    p_byErrorLocation) ;
typedef TCHAR * ( WINAPI * _GetWinRing0DriverPath) () ;
//
//typedef BOOL 
////__stdcall
//( //The calling convention "__stdcall" is needed because
//              //else runtime error: something with CPU register "ESP".
//         #ifdef _WINDOWS
//             __stdcall 
//         #endif
//             *_RdmsrEx)(DWORD,PDWORD,PDWORD,DWORD_PTR);

//#include "../ISpecificController.hpp"
#include <Controller/I_CPUaccess.hpp>
//#include <Winbase.h> //for ::Sleep(...)

//Exception class. If catched: don't continue (with assigning function 
//pointers to DLL functions etc.).
class DLLnotLoadedException:
    public CPUaccessException
{
public:
  DLLnotLoadedException(char * p_ch)
    : CPUaccessException(p_ch)
  {
  }
};

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class UserInterface ;

//This class calls the winring0 functions by function pointers.
class WinRing0_1_3RunTimeDynLinked
  //: public ControllerBase
  : public //ISpecificController
  I_CPUaccess
  //: public wxWidgets::CalculationThread
{
private:
  //HINSTANCE m_hinstanceWinRingDLL ;
  HMODULE m_hModuleWinRing0DLL ;
  //_ReadPciConfigDwordEx m_pfnreadpciconfigdwordex ;
  //_WritePciConfigDwordEx m_pfnwritepciconfigdwordex;
  _CpuidTx m_pfncpuidex ;
  _RdmsrTx m_pfnrdmsrex ;
  _RdpmcTx m_pfnrdpmcex ;
  _ReadPciConfigDwordEx m_pfnreadpciconfigdwordex ;
  _WritePciConfigDwordEx m_pfnwritepciconfigdwordex ;
  _WrmsrTx m_pfnwrmsrex ;
  //UserInterface * mp_userinterface ;
public:
  WinRing0_1_3RunTimeDynLinked(UserInterface * pui) ;
  WinRing0_1_3RunTimeDynLinked() ;
  ~WinRing0_1_3RunTimeDynLinked() ;
  void UIconfirm(const std::string & str) ;
  BOOL CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  );
  BYTE GetNumberOfCPUCores() ;
  void Init(UserInterface * pui) ;

  BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI 
  RdmsrEx(
	  DWORD index,		// MSR index
	  PDWORD eax,			// bit  0-31
	  PDWORD edx,			// bit 32-63
	  DWORD_PTR affinityMask	// Thread Affinity Mask
  );
  inline BOOL
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    RdpmcEx(
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  BOOL ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    );
  BOOL ReadTSC(DWORD & r_dw , DWORD & r_dw ) ;
  void SetUserInterface(UserInterface * pui) ;
  void Sleep(WORD wMillis) //{ ::Sleep(wMillis) ; }
    ;

  BOOL 
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI 
    WritePciConfigDwordEx (
    DWORD pciAddress, 
    DWORD regAddress, 
    DWORD value
    );

  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI 
    WrmsrEx(
	  DWORD index,		// MSR index
	  DWORD eax,			// bit  0-31
	  DWORD edx,			// bit 32-63
	  DWORD affinityMask	// Thread Affinity Mask
  );
};
