#include "WinRing0_1_3LoadTimeDynLinked.hpp"
#include <Windows.h> //for ::GetProcAddress(), ::GetCurrentDirectory
#include <strstream> //for std::ostrstream
#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T(...)
#endif
#include <global.h> //for DEBUG()
#include <preprocessor_helper_macros.h>
#include <Controller/stdstring_format.hpp>
#include <Controller/tchar_conversion.h> //GetCharPointer(...)
#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <UserInterface.hpp> //for class "UserInterface"

//WinRing0 documentation:
//"Load-Time Dynamic Linking 
//1. Add WinRing0.lib or WinRing0x64.lib to your project. 
//2. Add #include "OlsApi.h" statement to your source file. 
//3. Call InitializeOls(). 
//4. Call GetDllStatus() to check error. 
//5. Call the library's functions. 
//6. Call DeinitializeOls()."
#include <WinRing0_1_3_1b/source/dll/OlsApi.h>
#include <WinRing0_1_3_1b/source/dll/OlsDef.h> //OLS_DLL_NO_ERROR etc.

#if defined(X64) || defined(_M_X64 ) || defined(_WIN64 )
  #define DLL_NAME "WinRing0x64.dll"
  #define TSTR_DLL_NAME _T("WinRing0x64.dll")
  #define WINRING0_DRIVER_NAME "WinRing0x64.sys"
#else
  #define DLL_NAME "WinRing0.dll"
  #define TSTR_DLL_NAME _T("WinRing0.dll")
  #define WINRING0_DRIVER_NAME "WinRing0.sys"
#endif

#ifdef _M_X64
#pragma comment(lib, "WinRing0x64.lib")
#else
#ifdef _DEBUG
//#pragma comment(lib, "WinRing0Debug.lib")
//#pragma comment(lib, "C:\Libs\WinRing0_1_3_1b/release/WinRing0Debug.lib")
#else
#pragma comment(lib, "WinRing0.lib")
#endif
#endif


WinRing0_1_3LoadTimeDynLinked::~WinRing0_1_3LoadTimeDynLinked()
{
  DeinitializeOls() ;
}

WinRing0_1_3LoadTimeDynLinked::WinRing0_1_3LoadTimeDynLinked(UserInterface * p_ui)
{
  DWORD dwValue ;
  mp_userinterface = p_ui ;
  //WinRing0 docu: 
  //"3. Call InitializeOls(). 
  // 4. Call GetDllStatus() to check error. "
  if( InitializeOls() )
  {
    dwValue = GetDllStatus() ;
    //DEBUG("result of InitializeOls:%lu\n", dwValue);
    LOG("result of InitializeOls:" << dwValue //<< "\n"
      )
    if( dwValue == OLS_DLL_NO_ERROR )
    {
       LOG("WinRing0 successfully initialized\n") ;
       //UIconfirm("WinRing0 successfully initialized") ;
    }
    else
    {
      DWORD dwLastErrorCode = ::GetLastError() ;
      std::ostrstream ostrstream;
      if( mp_userinterface )
      {
        DWORD dw =
        //http://msdn.microsoft.com/en-us/library/aa364934(VS.85).aspx:
        //If the buffer that is pointed to by lpBuffer is not large 
        //enough, the return value specifies the required size of 
        //the buffer, in characters, including the null-terminating 
        //character.
        ::GetCurrentDirectory(
          //__in   DWORD nBufferLength,
          0 ,
          //__out  LPTSTR lpBuffer
          NULL
        );
        LPTSTR lptstrCurrentDirectory = new 
          TCHAR[dw] ;
        if( 
          //If the function succeeds, the return value specifies the 
          //number of characters that are written to the buffer, not 
          //including the terminating null character.
          ::GetCurrentDirectory( 
            //The length of the buffer for the current directory string, 
            //in TCHARs. The buffer length must include room for a 
            //terminating null character.
            dw
            , lptstrCurrentDirectory
            ) == (dw - 1)
          )
          ostrstream << "current working dir:\"" 
            << //GetCharPointer(
            GetStdString(
              //is "wchar_t" if using unicode
              lptstrCurrentDirectory
              ) << "\"\n" ;
        ostrstream << "WinRing0 error:" ;
        delete [] lptstrCurrentDirectory ;
        DWORD dwErrorAsSystemError ;
        switch( dwValue )
        {
          case OLS_DLL_DRIVER_NOT_LOADED:
          ostrstream << 
            "The CPU access driver was not loaded.\n"
            "This may be due to this programs needs high "
            "privileges for the direct CPU access (=ring 0 access)\n"
            "Possible solution(s):\n"
            "-Run this program as administrator/ with elevated/ "
            "administrative privileges\n"
            "-Place the CPU access driver \"" WINRING0_DRIVER_NAME
            "\" into the same directory as the DLL \"" DLL_NAME "\""
            " (and this exe)\n" ;
          dwErrorAsSystemError = ERROR_FILE_NOT_FOUND ;
          break;
          case OLS_DLL_DRIVER_NOT_FOUND:
          ostrstream << 
            "The CPU access driver was not found.\n"
            "Possible solution:\n"
            "-Place the CPU access driver \"" WINRING0_DRIVER_NAME
            "\" into the same directory as the DLL \"" DLL_NAME "\""
            " (and this exe)\n"
            "-file system: run THIS program from an ordinary file system"
            " such as NTFS, and NOT from virtual file systems like"
            " Truecrypt drives\n" ;
          dwErrorAsSystemError = ERROR_FILE_NOT_FOUND ;
          break;
      case OLS_DLL_UNKNOWN_ERROR:
          ostrstream << "Unknown error\n" ;
          break;
      case OLS_DLL_UNSUPPORTED_PLATFORM: 
          ostrstream << "Unsupported Platform\n" ;
          break;
      case OLS_DLL_DRIVER_UNLOADED: 
          ostrstream << "Driver unloaded by other process\n" ;
          break;
      case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
          ostrstream << "Driver not loaded because of executing on "
              "Network Drive (1.0.8 or later)\n";
          break;
        }
        std::string stdstrErrMsg = ::GetLastErrorMessageString( 
          dwLastErrorCode ) ;
        ostrstream << "Last Windows error code: " << dwLastErrorCode ;
        if( ! stdstrErrMsg.empty() )
        {
          ostrstream << " (error message: \"" << stdstrErrMsg << "\")" ;
        }
        ostrstream << "\n" ;
        mp_userinterface->Confirm(ostrstream);
  #ifdef _DEBUG
        //Breakpoint possibility
        int i = 0 ;
  #endif
      } //if( mp_userinterface )
      //When throwing an exception by creating the exception object on the
      //stack VS 2005 reported an "stack buffer error"?.
      //Oh, that was rather because the exception was not catched.
     throw //new 
       CPUaccessException(ostrstream.str() ) ;
    }
  }
  else
  {
    if( mp_userinterface )
      mp_userinterface->Confirm("WinRing0's InitializeOls() failed\n");
    throw CPUaccessException("WinRing0's InitializeOls() failed" ) ;
  }
}

BOOL WinRing0_1_3LoadTimeDynLinked::CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  )
{
  BOOL boolCpuidTx = CpuidTx(
    index,
    eax,
    ebx,
    ecx,
    edx,
    affinityMask
  );
  return boolCpuidTx ;
}

BYTE WinRing0_1_3LoadTimeDynLinked::GetNumberOfCPUCores()
{
  return 0 ;
}

inline BOOL 
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI 
  WinRing0_1_3LoadTimeDynLinked::RdpmcEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dwEAX,			// bit  0-31
  PDWORD p_dwEDX,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
  )
{
  BOOL boolRdpmcTx = RdpmcTx(
    dwIndex,
    p_dwEAX,
    p_dwEDX,
    affinityMask
  );
  return boolRdpmcTx ;
}

BOOL // TRUE: success, FALSE: failure
//In g++ virtual methods can't be declared as stdcall
//WINAPI 
WinRing0_1_3LoadTimeDynLinked::RdmsrEx(
  DWORD index,		// MSR index
  PDWORD eax,			// bit  0-31
  PDWORD edx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
)
{
  BOOL boolRdmsrTx = RdmsrTx(
    index,
    eax,
    edx,
    affinityMask
  );
  return boolRdmsrTx ;
}

void WinRing0_1_3LoadTimeDynLinked::Init(UserInterface * pui)
{
}

BOOL WinRing0_1_3LoadTimeDynLinked::ReadPciConfigDwordEx(
  DWORD dwPCIaddress
  , DWORD dwRegAddress
  , PDWORD p_dwValue
  )
{
  BOOL boolRet = ReadPciConfigDwordEx(
    dwPCIaddress , //DWORD pciAddress,
    dwRegAddress, //DWORD regAddress,
    p_dwValue //PDWORD value
    );
  return boolRet ;
}

void WinRing0_1_3LoadTimeDynLinked::SetUserInterface(UserInterface * pui)
{
    mp_userinterface = pui ;
}

void WinRing0_1_3LoadTimeDynLinked::Sleep(WORD wMillis)
{ 
  ::Sleep(wMillis) ; 
}

BOOL 
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI 
  WinRing0_1_3LoadTimeDynLinked::WritePciConfigDwordEx (
  DWORD dwPCIaddress, 
  DWORD dwRegisterAddress, 
  DWORD dwValue
  )
{
  BOOL boolRet = WritePciConfigDwordEx(
    dwPCIaddress ,//DWORD pciAddress,
    dwRegisterAddress , //DWORD regAddress,
    dwValue //DWORD value
    );
  return boolRet ;
}

BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI 
  WinRing0_1_3LoadTimeDynLinked::WrmsrEx(
  DWORD index,		// MSR index
  DWORD eax,			// bit  0-31
  DWORD edx,			// bit 32-63
  DWORD affinityMask	// Thread Affinity Mask
  )
{
  BOOL boolWrmsrTx = WrmsrTx(
    index,
    eax,
    edx,
    affinityMask
  );
  return boolWrmsrTx ;
}
