/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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

//Inside the header file:
//The calling convention "__stdcall" is needed because
//else runtime error: something with CPU register "ESP".
#include <WinRing0_1_3_1b/source/dll/OlsApiInitExt.h>

//#include <OlsApiInitDef.h> //for "_WrmsrEx" etc.

//Additional function definitions for a modified version of the DLL:
typedef BYTE ( WINAPI * _GetDetailedWinRing0Error) (
    PDWORD p_dwWinErrorCode, PBYTE p_byErrorLocation) ;
typedef TCHAR * ( WINAPI * _GetWinRing0DriverPath) () ;

//#include "../ISpecificController.hpp"
#include <Controller/I_CPUaccess.hpp> //needed because it's the base class.
//#include <Winbase.h> //for ::Sleep(...)

//Exception class. If caught: don't continue (with assigning function
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
  HMODULE m_hModuleWinRing0DLL ;
  //UserInterface * mp_userinterface ;
public:
  WinRing0_1_3RunTimeDynLinked(UserInterface * pui) ;
  WinRing0_1_3RunTimeDynLinked() ;
  //use "virtual" to avoid "Class 'WinRing0_1_3RunTimeDynLinked' has virtual
  //method 'WritePciConfigDwordEx' but non-virtual destructor"
  virtual ~WinRing0_1_3RunTimeDynLinked() ;

  void UIconfirm(const std::string & str) ;
  BOOL CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  );
  void DLLerror( DWORD dwDllStatus);
  void GetDriverPath();
  static void GetErrorMessageForInitError(DWORD dwValue,
    std::string & stdstrErrorMsg );
//  BYTE GetNumberOfCPUCores() ;
  void Init(UserInterface * pui) ;
  void InitOpenLibSysFailed();

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
  BOOL ReadTSC(DWORD & r_dwLowEAX , DWORD & r_dwHighEDX ) ;
  BOOL ReadTSCinOrder(
    DWORD & r_dwLowEAX ,
    DWORD & r_dwHighEDX ,
    DWORD dwThreadAffinityMask
    ) ;
  void SetUserInterface(UserInterface * pui) ;
  BYTE SetThreadAffinityMask(DWORD dwThreadAffinityMask) ;
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
