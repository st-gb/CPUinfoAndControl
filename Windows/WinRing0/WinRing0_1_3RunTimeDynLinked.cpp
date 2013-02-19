/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "WinRing0_1_3RunTimeDynLinked.hpp"
//#include <winbase.h> //for ::GetProcAddress()
#include <Windows.h> //for ::GetProcAddress(), ::GetCurrentDirectory
#include <sstream> //for std::ostringstream
#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T(...)
#endif
//#include <global.h> //for DEBUG()
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//for BITMASK_FOR_LOWMOST_7BIT
#include <preprocessor_macros/bitmasks.h>
#include <Controller/character_string/stdtstr.hpp> //GetStdString(...)
 //convertToStdString(typename )
#include <Controller/character_string/stdstring_format.hpp>
//#include <Controller/character_string/tchar_conversion.h> //GetCharPointer(...)
#include <UserInterface/UserInterface.hpp> //for class "UserInterface"
#include <Windows/ErrorCode/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/HardwareAccess/HardwareAccessError/\
GetHardwareAccessErrorDescription.hpp>
#include <Windows/FileSystem/GetCurrentDirectory/GetCurrentDirectory.hpp>

//WinRing0 docu:
//Run-Time Dynamic Linking
//Add #include "OlsApiInit.h" statement to your source file.
//Call InitOpenLibSys().
//Call GetDllStatus() to check error.
//Call the library's functions. *
//Call DeinitOpenLibSys().

//#include <OlsApiInitDef.h> //for _IsMsr
#include <WinRing0_1_3_1b/source/dll/OlsApiInit.h>

//#ifdef X64 || 
#if defined(X64) || defined(_M_X64 ) || defined(_WIN64 )
  #define DLL_NAME "WinRing0x64.dll"
  #define TSTR_DLL_NAME _T("WinRing0x64.dll")
  #define WINRING0_DRIVER_NAME "WinRing0x64.sys"
#else
  #define DLL_NAME "WinRing0.dll"
  #define TSTR_DLL_NAME _T("WinRing0.dll")
  #define WINRING0_DRIVER_NAME "WinRing0.sys"
#endif

#define SUCCESS 1
#define FAILURE 0

WinRing0_1_3RunTimeDynLinked::WinRing0_1_3RunTimeDynLinked(UserInterface * pui)
  //: //C++ style initialisations
  //,mp_userinterface(pui)
  //mp_userinterface(NULL)
{
//#ifdef _DEBUG
//  std::stringstream stdstrstream ;
  //For checking if the members are on the same RAM address between MSVC and MinGW:
  LOGN_DEBUG( "WinRing0_1_3RunTimeDynLinked c'tor (...)--Address of this:"
    << this
    << "address of I_CPUaccess::mp_model: " << & mp_model <<"\n"
    << "address in I_CPUaccess::mp_model: " << mp_model
    //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
    << "address of I_CPUaccess::mp_cpu_controller: " << & mp_cpu_controller
    << "address in I_CPUaccess::mp_cpu_controller: "
    << mp_cpu_controller
    ) ;
//#endif //#ifdef _DEBUG
  Init(pui) ;
  LOGN( FULL_FUNC_NAME << "--end")
}
void WinRing0_1_3RunTimeDynLinked::Init(UserInterface * pui)
{
  //Initialize just to avoid (g++) compiler warning.
  DWORD dwDllStatus = 0 ;
//  char * archDLLName = DLL_NAME ;
  mp_userinterface = pui ;

  ::SetLastError(0) ;

  //WinRing0 Run-Time Dynamic Linking docu step 2: "Call InitOpenLibSys(). "
  BOOL boolInit = InitOpenLibSys( & m_hModuleWinRing0DLL ) ;

  //WinRing0 documentation "How to Use"->"Run-Time Dynamic Linking"
  // step 3: "Call GetDllStatus() to check error."
  dwDllStatus = GetDllStatus() ;
  if( dwDllStatus == OLS_DLL_NO_ERROR )
  {
     LOGN_TYPE("WinRing0 successfully initialized",
       LogLevel::success)
     //UIconfirm("WinRing0 successfully initialized") ;
  }
  else
  {
//        DWORD dw = ::GetLastError() ;
//        _GetDetailedWinRing0Error pfnGetDetailedWinRing0Error =
//          (_GetDetailedWinRing0Error)
//          ::GetProcAddress(m_hModuleWinRing0DLL,"GetDetailedWinRing0Error");
    DLLerror(dwDllStatus);
  }
  //TRUE  if loading WinRing0 DLL succeeded and all function pointers are
  // assigned/ <> NULL.
  if( boolInit )
  {
    BYTE major;
    BYTE minor;
    BYTE revision;
    BYTE release ;
    std::string strFuncName ;
    LOGN_TYPE( "InitOpenLibSys succeeded", LogLevel::success )

    GetDllVersion (& major, & minor, & revision, & release);
    LOGN("using WinRing0 DLL version "
      << (WORD)major << " "
      << (WORD)minor << " "
      << (WORD)revision << " "
      << (WORD) release
      )
  }
  else
  {
//    InitOpenLibSysFailed();
  }
  LOGN( FULL_FUNC_NAME << "--end")
}

void WinRing0_1_3RunTimeDynLinked::InitOpenLibSysFailed()
{
  LOGN_TYPE("InitOpenLibSys failed", LogLevel::error)
  //Is NULL if the WinRing0 DLL could not be loaded.
  //m_hModuleWinRing0DLL == NULL (WinRing0 DLL could not be loaded)
  if( m_hModuleWinRing0DLL )
  {
    //TODO error message
  //        std::ostrstream ostrstream;
  //        ostrstream << "Error loading the necessary library \"" <<
  //          archDLLName << "\"" ;
  //        if( mp_userinterface )
  //          mp_userinterface->Confirm(ostrstream);
  //        throw CPUaccessException( ) ;
    const char * pchErrorMessage =
      "Other error initializing the CPU/hardware access library "
      "\"WinRing0\":\n this may be due to "
      "insufficient rights, so running this program as administrator "
      "may help\nIf you do not run this program elevated then it could/"
      "should not work properly, especially when it is not connected to"
      " the service.";
    LOGN_TYPE("Throwing a CPUaccessException:" << pchErrorMessage,
      LogLevel::error )
    //When throwing an exception by creating the exception object on the
    //stack VS 2005 reported an "stack buffer error"?.
    //Oh, that was rather because the exception was not caught.
     throw //new
       CPUaccessException(
         //TODO Mysterious g++ error
         //"error: `ostrstreamErrorDesc' was not declared in this scope"
         //ostrstreamErrorDesc.str()
         //Avoid g++ warning "deprecated conversion from string constant to
         //'char*'"
         (char *)
         pchErrorMessage
         ) ;
  }
  else
  {
    //  if (m_hinstanceWinRingDLL == 0)
    //  {
    //    std::ostrstream ostrstream;
    //#ifndef __MINGW32__
    //    ostrstream << "Error loading the necessary library \"" <<
    //        archDLLName << "\"\n-Possible solution: put this library into the "
    //        "same directory as this executable's (working) directory or into a "
    //        "Windows system directory" ;
    //    if( mp_userinterface )
    //      mp_userinterface->Confirm(ostrstream);
    //    throw DLLnotLoadedException(ostrstream.str() ) ;
    //#else
    //    throw DLLnotLoadedException( "error" ) ;
    //
    // #endif
    //  }
    //  else
    std::string stdstrErrorMsg("the CPU access DLL \"" //<< //TSTR_DLL_NAME
      DLL_NAME
      //<<
      "\" failed to load/ "
      "InitOpenLibSys failed\n"
      "Is \"" //<< TSTR_DLL_NAME <<
      DLL_NAME "\" in the Windows system path or in the "
      "current directory of this program?") ;
  //      mp_userinterface->Confirm( stdstrErrorMsg ) ;
    LOGN( stdstrErrorMsg )
    //When throwing an exception by creating the exception object on the
    //stack VS 2005 reported an "stack buffer error"?.
    //Oh, that was rather because the exception was not caught.
     throw //new
       CPUaccessException(
         //TODO Mysterious g++ error
         //"error: `ostrstreamErrorDesc' was not declared in this scope"
         //ostrstreamErrorDesc.str()
         stdstrErrorMsg
         ) ;
  }
}

//WinRing0_1_3RunTimeDynLinked::WinRing0_1_3RunTimeDynLinked(UserInterface * pui)
WinRing0_1_3RunTimeDynLinked::WinRing0_1_3RunTimeDynLinked()
  //: mp_userinterface(pui)
{
  //WinRing0_1_3RunTimeDynLinked(NULL);
  Init(NULL) ;
}

WinRing0_1_3RunTimeDynLinked::~WinRing0_1_3RunTimeDynLinked()
{
  //WinRing0 Run-Time Dynamic Linking docu Step 5: "Call DeinitOpenLibSys()."
  DeinitOpenLibSys( & m_hModuleWinRing0DLL ) ;
}

BOOL WinRing0_1_3RunTimeDynLinked::CpuidEx(
    DWORD dwFunctionIndex,
    PDWORD p_dwEAX,
    PDWORD p_dwEBX,
    PDWORD p_dwECX,
    PDWORD p_dwEDX,
    DWORD_PTR dwAffinityMask
  )
{
  BOOL bReturn = FAILURE ;
  bReturn =
    //Function pointer "CpuidTx" is <> NULL if "InitOpenLibSys" succeeded.
    ::CpuidTx( dwFunctionIndex, p_dwEAX,p_dwEBX,p_dwECX,p_dwEDX,
      dwAffinityMask ) ;
  if( bReturn )
  {
    //DEBUG("Calling CPUID function succeeded.\n");
  }
  else
    DEBUG("Calling CPUID function failed.\n");
  //else
  //  DEBUG("CPUID function pointer could not be assigned.\n");
  return bReturn ;
}

void WinRing0_1_3RunTimeDynLinked::DLLerror(DWORD dwDllStatus)
{
  LOGN_TYPE("WinRing0 failed to initialize (DLL status <> 0)",
    LogLevel::error)
  GetDriverPath();

  std::ostringstream ostrstreamErrorDesc;
  std::string stdstrErrorMsg //("WinRing0 error ") ;
    ;
  std::tstring std_tstrCurrentDirectory;
  if( ! Windows::GetCurrentDirectory(std_tstrCurrentDirectory) )
  {
      stdstrErrorMsg +=
        "current working dir:\""
        + GetStdString(
          std_tstrCurrentDirectory
          ) + "\"\n" ;
  }
  stdstrErrorMsg +=
    "WinRing0 error:" ;
  GetErrorMessageForInitError(dwDllStatus, stdstrErrorMsg);
//    if( mp_userinterface )
//    {
//    mp_userinterface->Confirm(
//        stdstrErrorMsg);
//    }
  LOGN_TYPE(stdstrErrorMsg, LogLevel::error)
  #ifdef _DEBUG
  //            //Breakpoint possibility
  //            int i = 0 ;
  #endif
    //When throwing an exception by creating the exception object on the
    //stack VS 2005 reported an "stack buffer error"?.
    //Oh, that was rather because the exception was not caught.
  throw //new
    CPUaccessException(
      stdstrErrorMsg
      ) ;
  }

void WinRing0_1_3RunTimeDynLinked::GetDriverPath()
{
  const std::string stdstrFunctionName = "GetDriverPath" ;
  LOGN( FULL_FUNC_NAME << "--checking if the WinRing0 DLL contains a \""
    << stdstrFunctionName << "\" function")
  _GetWinRing0DriverPath pfnGetWinRing0DriverPath =
    (_GetWinRing0DriverPath)
    ::GetProcAddress(m_hModuleWinRing0DLL, stdstrFunctionName.c_str() );
  //only available at own compiled WinRing0 DLL version.
  if( //pfnGetDetailedWinRing0Error
      pfnGetWinRing0DriverPath )
  {
  #ifdef COMPILE_WITH_LOG
    TCHAR * p_tch = (*pfnGetWinRing0DriverPath) () ;
    LOGN("driver path expected by WinRing0:" << p_tch )
  #endif //#ifdef COMPILE_WITH_LOG
  }
  else
  {
    LOGN_TYPE( "function \"" << stdstrFunctionName <<
      "\" does not exist in WinRing0 DLL -> unable to output expected "
      "driver path for diagnosing the initialize error ",
      LogLevel::warning)
  }
}

void WinRing0_1_3RunTimeDynLinked::GetErrorMessageForInitError(
  DWORD dwDllStatus,
  std::string & stdstrErrorMsg )
{
  DWORD dwErrorAsSystemError ;
  switch( dwDllStatus )
  {
  case OLS_DLL_DRIVER_NOT_LOADED:
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
    stdstrErrorMsg +=
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
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
    stdstrErrorMsg +=
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
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
      stdstrErrorMsg +=
          "Unknown error\n" ;
      break;
  case OLS_DLL_UNSUPPORTED_PLATFORM:
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
      stdstrErrorMsg +=
      "Unsupported Platform\n" ;
      break;
  case OLS_DLL_DRIVER_UNLOADED:
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
      stdstrErrorMsg +=
      "Driver unloaded by other process\n" ;
      break;
  case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
    //TODO Mysterios g++ error
    //"error: `ostrstreamErrorDesc' was not declared in this scope"
  //                  ostrstreamErrorDesc <<
      stdstrErrorMsg +=
          "Driver not loaded because of executing on "
          "Network Drive (1.0.8 or later)\n";
      break;
  }
}

//BYTE WinRing0_1_3RunTimeDynLinked::GetNumberOfCPUCores()
//{
//  //return 2 ;
//  BYTE byCoreNumber = 255 ;
//  DWORD dwEAX;
//  DWORD dwEBX;
//  DWORD dwECX;
//  DWORD dwEDX;
//  DEBUG("WRDL--getting number of CPU cores\n");
//  if( CpuidEx(
//    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
//    0x80000008,
//    &dwEAX,
//    &dwEBX,
//    &dwECX,
//    &dwEDX,
//    1
//      )
//    )
//  {
//    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT )
//      //"ECX 7:0 NC: number of physical cores - 1.
//      //The number of cores in the processor is NC+1 (e.g., if
//      //NC=0, then there is one core).
//      //See also section 2.9.2 [Number of Cores and Core Number]."
//      + 1 ;
//    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
//    DEBUGN( "WinRing0: Number of CPU cores according to AMDs calculation with "
//      "CPUID: " << (WORD) byCoreNumber );
//  }
//  DEBUG("WRDL--end of getting number of CPU cores\n");
//  return byCoreNumber ;
//}

BOOL // TRUE: success, FALSE: failure
  WinRing0_1_3RunTimeDynLinked::RdmsrEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;
//  DEBUG_COUT(
//    "WinRing0_1_3RunTimeDynLinked::ReadMSRex(Index,affinityMask): "
//    << dwIndex << " "
//    << *p_dweax << " "
//    << *p_dwedx << " "
//    << affinityMask
//    << "\n" )
  bReturn =
    //"If the function succeeds, the return value is TRUE."
    //Function pointer "RdmsrTx" is <> NULL if "InitOpenLibSys"
    //succeeded.
    ::RdmsrTx(dwIndex,p_dweax,p_dwedx,affinityMask);
  //"If the function succeeds, the return value is TRUE. "
  if( bReturn )
  {
    //DEBUG("Reading from MSR succeeded\n");
//      DEBUG_COUT(
//        "WinRing0_1_3RunTimeDynLinked::ReadMSRex(Index,affinityMask): "
//        << dwIndex << " "
//        << *p_dweax << " "
//        << *p_dwedx << " "
//        << affinityMask
//        << "\n" )
  }
  else
  {
    DWORD dwErrorCode = ::GetLastError( ) ;
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("Reading from MSR failed.--"
        "Last error code: " << dwErrorCode ) ;
    std::string strErrorMessage = GetHardwareAccessErrorDescription(
      dwErrorCode, "MSR" , dwIndex ) ;
    //Show the detailed error message here because the details
    //(how the access to the MSR is realised: via WinRing0)
    //depend on the implementation.
    UIconfirm(
      "Reading from MSR failed. "
      "\nerror message: \"" + strErrorMessage +
//      "\"(error code: "
//      + convertToStdString<DWORD>( //(WORD) byModel
//        dwErrorCode //, std::hex
//        )
      + ")\n"
//      + stdstrAdditionalInfo + "\n"
      "Possible solutions:\n"
      "-This program needs elevated privileges for ring 0 / CPU "
      "access. So run it as administrator.\n"
//      "-This program uses "
//      "a DLL to communicate with the CPU for higher privileges. Does the "
//      "file \"WinRing0[...].sys\" lay within the same directory as "
//      "\"WinRing0[...].dll\"?\n"
      );
//    throw ReadMSRexception() ;
  }
  return bReturn ;
}

inline BOOL WinRing0_1_3RunTimeDynLinked::RdpmcEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
  )
{
  BOOL bReturn = FAILURE ;
  bReturn =
    //RdpmcEx seemed to cause a blue screen? (maybe because of wrong param
    //values)
    //Function pointer "RdpmcTx" is <> NULL if "InitOpenLibSys"
    //succeeded.
    ::RdpmcTx( dwIndex, p_dweax, p_dwedx, affinityMask ) ;
  return bReturn ;
}

BOOL WinRing0_1_3RunTimeDynLinked::ReadPciConfigDwordEx(
  DWORD dwPCIaddress
  , DWORD dwRegAddress
  , PDWORD p_dwValue
  )
{
  BOOL bReturn = FAILURE ;
  bReturn =
    //"If the function succeeds, the return value is TRUE."
    //Function pointer "ReadPciConfigDwordEx" is <> NULL if "InitOpenLibSys"
    //succeeded.
    ::ReadPciConfigDwordEx(
      dwPCIaddress
      , dwRegAddress
      , p_dwValue
    );
  //"If the function succeeds, the return value is TRUE. "
  if( bReturn )
  {
    ;
  }
  else
  {
    DWORD dwErrorCode = ::GetLastError( ) ;
    std::string strErrorMessage = GetHardwareAccessErrorDescription(
      dwErrorCode, "PCI configuration space" , dwPCIaddress ) ;
    //bit description
//    16-31 Reserved
    UIconfirm("Reading from the PCI configuration space at\nPCI bus "
      + convertToStdString<WORD>(
      //WinRing0: "8-15 PCI Bus Number"
      (dwPCIaddress >> 8 ) & BITMASK_FOR_LOWMOST_8BIT )
      + "\n, device " + convertToStdString<WORD>(
      //WinRing0: "3- 7 Device Number"
      (dwPCIaddress >> 3 ) & BITMASK_FOR_LOWMOST_5BIT ) +
      + "\n, function " + convertToStdString<WORD>(
      //WinRing0: "0- 2 Function Number"
      dwPCIaddress & BITMASK_FOR_LOWMOST_3BIT )
      + "\n, register address " + convertToStdString<WORD>( dwRegAddress )
      + "\nfailed.\n"
      + strErrorMessage
      + "possible causes:\n"
      "This program needs elevated privileges for ring 0 "
      "access. So run it as administrator.\n"
      "This program uses "
      "a DLL to communicate with the CPU for higher privileges. Does the "
      "file \"WinRing0[...].sys\" lay within the same directory as "
      "\"WinRing0[...].dll\"?\n");
  }
  return bReturn ;
}

BOOL WinRing0_1_3RunTimeDynLinked::ReadTSC(
  DWORD & r_dwLowEAX ,
  DWORD & r_dwHighEDX
  )
{
  return Rdtsc( & r_dwLowEAX, & r_dwHighEDX ) ;
}

//BOOL WinRing0_1_3RunTimeDynLinked::ReadTSC(
//  DWORD & r_dwLowEAX ,
//  DWORD & r_dwHighEDX
//  )
//{
//  return Rdtsc( & r_dwLowEAX, & r_dwHighEDX ) ;
//}

BOOL WinRing0_1_3RunTimeDynLinked::ReadTSCinOrder(
  DWORD & r_dwLowEAX ,
  DWORD & r_dwHighEDX ,
  DWORD dwThreadAffinityMask
  )
{
  DWORD dw,dw2,dw3,dw4 ;
  //http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf:
  //This ensures all prev ops are executed before. See Intel doc.
  CpuidEx(0,&dw,&dw2,&dw3,&dw4,dwThreadAffinityMask) ;
  return RdtscTx( & r_dwLowEAX, & r_dwHighEDX , dwThreadAffinityMask) ;
}

void WinRing0_1_3RunTimeDynLinked::SetUserInterface(UserInterface * pui)
{
  mp_userinterface = pui ;
}

BYTE WinRing0_1_3RunTimeDynLinked::SetThreadAffinityMask(
  DWORD dwThreadAffinityMask)
{
  DWORD_PTR dw_ptr =
    //http://msdn.microsoft.com/en-us/library/ms686247%28VS.85%29.aspx:
    //"If the function fails, the return value is zero."
    ::SetThreadAffinityMask( ::GetCurrentThread(), dwThreadAffinityMask ) ;
  return dw_ptr ;
  return 0 ;
}

void WinRing0_1_3RunTimeDynLinked::Sleep(WORD wMillis)
{
  ::Sleep(wMillis) ;
}

void WinRing0_1_3RunTimeDynLinked::UIconfirm(const std::string & str)
{
  if ( mp_userinterface )
    mp_userinterface->Confirm(str) ;
}

BOOL 
//In g++ virtual methods can't be declared as stdcall
//WINAPI 
WinRing0_1_3RunTimeDynLinked::WritePciConfigDwordEx (
  DWORD dwPCIaddress, DWORD dwRegAddress, DWORD dwValue)
{
  BOOL bReturn = FAILURE ;
  bReturn =
    //"If the function succeeds, the return value is TRUE."
    //Function pointer "WritePciConfigDwordEx" is <> NULL if "InitOpenLibSys"
    //succeeded.
    ::WritePciConfigDwordEx(
      dwPCIaddress
      , dwRegAddress
      , dwValue
    );
  //"If the function succeeds, the return value is TRUE. "
  if( bReturn )
  {
    ;
  }
  else
    UIconfirm("Writing to the CPU configuration space failed. possible causes:\n"
      "This program needs elevated privileges for ring 0 "
      "access. So run it as administrator.\n"
      "This program uses "
      "a DLL to communicate with the CPU for higher privileges. Does the "
      "file \"WinRing0[...].sys\" lay within the same directory as "
      "\"WinRing0[...].dll\"?\n");
  return bReturn ;
}

BOOL // TRUE: success, FALSE: failure
//In g++ virtual methods can't be declared as stdcall
//WINAPI 
WinRing0_1_3RunTimeDynLinked::WrmsrEx(
	DWORD dwIndex,		// MSR index
	DWORD dwEAX,			// bit  0-31
	DWORD dwEDX,			// bit 32-63
	DWORD dwAffinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;
  //DEBUG("Directly before writing to MSR\n");
  bReturn =
    //Function pointer "WrmsrTx" is <> NULL if "InitOpenLibSys" succeeded.
    //WinRing0 1.3.1b doc:
    //"This function executes "WRMSR" (Opcode : 0F 30) instruction with thread
    //affinity mask."
    //"If the function succeeds, the return value is TRUE."
    ::WrmsrTx(dwIndex,dwEAX,dwEDX,dwAffinityMask);
  if( bReturn )
  {
    //DEBUG("Writing to MSR succeeded.\n");
  }
  else
  {
    DWORD dwErrorCode = ::GetLastError( ) ;
    std::string strErrorMessage = GetHardwareAccessErrorDescription(
      dwErrorCode, "MSR" , dwIndex) ;
    std::wstring std_wstrMessage = L"writing to MSR failed. "
      + GetStdWstring(strErrorMessage) +
      L"possible causes:\n"
      //This happened when wanted to write to a read-only status register.
      L"-all or some bits within the register of this address are only readable\n"
      L"-this program needs elevated privileges for ring 0 "
      L"access. So run it as administrator.\n";
    LOGN_ERROR( std_wstrMessage)
#ifdef COMPILE_AS_GUI
    mp_userinterface->Confirm(
      std_wstrMessage
//      "This program uses "
//      "a DLL to communicate with the CPU for higher privileges. Does the "
//      "file \"WinRing0[...].sys\" lay within the same directory as "
//      "\"WinRing0[...].dll\"?\n"
      );
#endif
  }
  return bReturn ? hardware_access_succeeded : hardware_access_failed;
}
