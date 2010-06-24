#include "WinRing0_1_3RunTimeDynLinked.hpp"
//#include <winbase.h> //for ::GetProcAddress()
#include <Windows.h> //for ::GetProcAddress(), ::GetCurrentDirectory
//#include <strstream> //for std::ostrstream
#include <sstream> //for std::ostringstream
#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T(...)
#endif
#include <global.h> //for DEBUG()
#include <preprocessor_helper_macros.h>
#include <Controller/stdstring_format.hpp>
#include <Controller/tchar_conversion.h> //GetCharPointer(...)
#include <UserInterface/UserInterface.hpp> //for class "UserInterface"
#include <Windows/ErrorCodeFromGetLastErrorToString.h>

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

//void writeToMSR(BYTE byPstateID,BYTE VoltageID,BYTE FreqID, BYTE DivID)
//{
//  DWORD dwLow ;
//  DWORD dwHigh ;
//  unsigned long dwRegNr ;
//  fnWrMsrExDef fnwrmsrex ;
//  strFuncName = "WrmsrEx" ;
//  fnmsrex = (fnWrMsrExDef *)GetProcAddress(g_hinstanceWinRingDLL,strFuncName.c_str() );
//  dwLow = 
//  dwHigh = 
//  fnmsrex(
//    dwRegNr,
//    ,
//
//    )  
//}

WinRing0_1_3RunTimeDynLinked::WinRing0_1_3RunTimeDynLinked(UserInterface * pui)
  //: //C++ style initialisations
  //m_hinstanceWinRingDLL ( NULL ) ,
  ////m_pfnreadpciconfigdwordex ( NULL ) ,
  ////m_pfnwritepciconfigdwordex ( NULL ) ,
  //m_pfnrdmsrex ( NULL ) ,
  //m_pfnwrmsrex (NULL) ,
  //m_pfncpuidex ( NULL ) 
  //,mp_userinterface(pui)
  //mp_userinterface(NULL)
{
//#ifdef _DEBUG
//  std::stringstream stdstrstream ;
  //For checking if the members are on the same RAM address between MSVC and MinGW:
  LOGN( "WinRing0_1_3RunTimeDynLinked c'tor (...)--Address of this:"
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
}
void WinRing0_1_3RunTimeDynLinked::Init(UserInterface * pui)
{
//  char * archDLLName = DLL_NAME ;
  //m_hinstanceWinRingDLL = NULL ;
  m_pfnrdmsrex = NULL ;
  m_pfnwrmsrex = NULL ;
  m_pfncpuidex = NULL ;
  m_pfnreadpciconfigdwordex = NULL ;
  mp_userinterface = pui ;
  //m_pfnrdmsrex = NULL ;
  //m_hinstanceWinRingDLL = ::LoadLibraryA(archDLLName);
//  m_hinstanceWinRingDLL = ::LoadLibrary(TSTR_DLL_NAME);
  //wxMessageBox(//wxstrDLLName 
  //  wxstrDLLName + " DLL NOT loaded");
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
  {
    //DEBUG("%s library successfully loaded\n", archDLLName);
    //LOG( archDLLName << " library successfully loaded\n" );
    //wxMessageBox(//wxstrDLLName 
    //  wxstrDLLName + " DLL successfulled loaded");
    //wxString wxstrFuncName="InitializeOls";
    //
    //void * pfn=wxdynamiclibraryTrans.
    //  //Returns pointer to symbol name in the library or NULL if the library contains no such symbol.
    //  GetSymbol(wxstrFuncName,&bSuccess);
    //void * pfn = ::GetProcAddress(m_hinstanceWinRingDLL,"InitializeOls");
    ::SetLastError(0) ;
//    _InitializeOls pfnInitializeOls = (_InitializeOls)
//      ::GetProcAddress(m_hinstanceWinRingDLL,"InitializeOls");

    //WinRing0 Run-Time Dynamic Linking docu step 2: "Call InitOpenLibSys(). "
    BOOL boolInit = InitOpenLibSys( & m_hModuleWinRing0DLL ) ;
    //FALSE e.g. if the WinRing0 DLL could not be loaded.
    if( boolInit )
//    InitOpenLibSys( & m_hModuleWinRing0DLL ) ;
    //if( pfn == NULL )
//    if( pfnInitializeOls == NULL )
//    {
//      //wxMessageBox("no " +wxstrFuncName);
//      DEBUG("no InitializeOls() in DLL library\n");
//      //InitializeOls() ausfhren.
//      //(*pfn)();
//    if( ! boolInit )
//    {
//      std::ostrstream ostrstream;
//      ostrstream << "Error loading the necessary library \"" <<
//        archDLLName << "\"" ;
//      if( mp_userinterface )
//        mp_userinterface->Confirm(ostrstream);
//      throw CPUaccessException( ) ;
//    }
//    else
    {
      BYTE major;
      BYTE minor;
      BYTE revision;
      BYTE release ;
      std::string strFuncName ;
      //Initialize just to avoid (g++) compiler warning.
      DWORD dwValue = 0 ;
//      _IsMsr pfnMSR ;
//      DEBUG("function InitializeOls exists\n");
//      //(*(void (*)())pfn)();
//      dwValue = (*pfnInitializeOls)() ;
      //DEBUG("result of InitializeOls:%lu\n", dwValue);
      //LOGN("result of InitializeOls:" << dwValue );

      GetDllVersion (& major, & minor, & revision, & release);
      LOGN("using WinRing0 DLL version "
        << (WORD)major << " "
        << (WORD)minor << " "
        << (WORD)revision << " "
        << (WORD) release
        )
      //WinRing0 Run-Time Dynamic Linking docu step 2:
      //   "Call GetDllStatus() to check error."
      dwValue = GetDllStatus() ;
      if( dwValue == OLS_DLL_NO_ERROR )
      {
         LOG("WinRing0 successfully initialized\n") ;
         //UIconfirm("WinRing0 successfully initialized") ;
      }
      else
      {
        std::string stdstrFunctionName = "GetDriverPath" ;
//        DWORD dw = ::GetLastError() ;
//        _GetDetailedWinRing0Error pfnGetDetailedWinRing0Error =
//          (_GetDetailedWinRing0Error)
//          ::GetProcAddress(m_hModuleWinRing0DLL,"GetDetailedWinRing0Error");

        _GetWinRing0DriverPath pfnGetWinRing0DriverPath =
          (_GetWinRing0DriverPath)
          ::GetProcAddress(m_hModuleWinRing0DLL,stdstrFunctionName.c_str());
        //only available at own compiled WinRing0 DLL version.
        if( //pfnGetDetailedWinRing0Error
            pfnGetWinRing0DriverPath )
        {
          TCHAR * p_tch = (*pfnGetWinRing0DriverPath) () ;
          LOGN("driver path expected  by WinRing0:" << p_tch )
        }
        else
          LOGN( "function" << stdstrFunctionName <<
              " does not exist in WinRing0 DLL")
          //std::ostrstream ostrstreamErrorDesc;
          std::ostringstream ostrstreamErrorDesc;
          std::string stdstrErrorMsg //("WinRing0 error ") ;
            ;
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
              {
                //TODO mysterious g++ error
                //"error: `ostrstreamErrorDesc' was not declared in this scope"
//                ostrstreamErrorDesc <<
//                    "current working dir:\""
//                  << //GetCharPointer(
//                  GetStdString(
//                    //is "wchar_t" if using unicode
//                    lptstrCurrentDirectory
//                    ) << "\"\n" ;
                  stdstrErrorMsg +=
                    "current working dir:\""
                    + GetStdString(
                      //is "wchar_t" if using unicode
                      lptstrCurrentDirectory
                      ) + "\"\n" ;
              }
              //TODO mysterious g++ error
              //"error: `ostrstreamErrorDesc' was not declared in this scope"
//              ostrstreamErrorDesc <<
                stdstrErrorMsg +=
                  "WinRing0 error:" ;
              delete [] lptstrCurrentDirectory ;
              DWORD dwErrorAsSystemError ;
              switch( dwValue )
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
              //TODO mysterios g++ error
              //"error: `ostrstreamErrorDesc' was not declared in this scope"
            mp_userinterface->Confirm(
                //ostrstreamErrorDesc
                stdstrErrorMsg);
#ifdef _DEBUG
//            //Breakpoint possibility
//            int i = 0 ;
#endif
          }
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
          //When throwing an exception by creating the exception object on the
          //stack VS 2005 reported an "stack buffer error"?.
          //Oh, that was rather because the exception was not catched.
         throw //new 
           CPUaccessException(
               //TODO Mysterios g++ error
               //"error: `ostrstreamErrorDesc' was not declared in this scope"
              //ostrstreamErrorDesc.str()
               stdstrErrorMsg
               ) ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
      }
//      pfnMSR = (_IsMsr)GetProcAddress(m_hinstanceWinRingDLL,"IsMsr");
//      //call "BOOL IsMsr();"
//      //if( (*(BOOL (*)())pfn)() )
//      if((*pfnMSR)() )
//      {
//        DEBUG("MSR exist.\n");
//      }
//      else
//      {
//        DEBUG("MSR DO NOT exist.\n");
//      }
      //strFuncName = "RdmsrEx" ;
      //g_pfnRdMsrEx = //(pfnRdMsrExDef)
      //  (fnRdMsrExDef *)GetProcAddress(m_hinstanceWinRingDLL,strFuncName.c_str() );
      ////If the function exists in the DLL.
      //if( g_pfnRdMsrEx )
      //{
      //  DEBUG("Function \"%s\" exists within DLL\n", strFuncName.c_str() ) ;
      //  bOk = true ;
      //}
//      m_pfnrdpmcex = (_RdpmcTx)::GetProcAddress( m_hinstanceWinRingDLL,"RdpmcEx");
      //if( ! m_pfnreadpmcex )
        //throw DynLinkToDLLfunctionException("RdpmcEx") ;
    }
    else
    {
      std::string stdstrErrorMsg("WinRing0: InitOpenLibSys failed\n"
          "Is the WinRing0 DLL in the Windows system path or in the current"
          "directory of this program?") ;
      LOGN( stdstrErrorMsg )
      //When throwing an exception by creating the exception object on the
      //stack VS 2005 reported an "stack buffer error"?.
      //Oh, that was rather because the exception was not catched.
       throw //new
         CPUaccessException(
             //TODO Mysterios g++ error
             //"error: `ostrstreamErrorDesc' was not declared in this scope"
            //ostrstreamErrorDesc.str()
             stdstrErrorMsg
             ) ;
    }
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
//  _DeinitializeOls pfnDeinitializeOls = (_DeinitializeOls)
//    ::GetProcAddress(m_hinstanceWinRingDLL,"DeinitializeOls");
//  if( pfnDeinitializeOls )
//  {
//    (*pfnDeinitializeOls)() ;
//  }
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
  //DEBUG("WRDL--CPUIDex--begin m_pfncpuidex: %x",m_pfncpuidex);
  //LOG( "WRDL--CPUIDex--begin m_pfncpuidex: " << m_pfncpuidex << "\n" );
  //LOGN( "CpuidEx function pointer address: " << m_pfncpuidex );
  //If it does not point to the function yet.
//  if( ! m_pfncpuidex )
//  {
//    std::string strFuncName =
//      //"T" stands for set >>T<<hread affinity mask.
//      "CpuidTx" ;
//    //_CpuidEx m_pfncpuidex;
//    m_pfncpuidex = (_CpuidTx) ::GetProcAddress( m_hinstanceWinRingDLL,
//      strFuncName.c_str() );
//    if(m_pfncpuidex)
//    {
//      //DEBUG("CPUID function pointer successfully assigned.\n");
//    }
//    else
//    {
//      DEBUG("CPUID function pointer NOT successfully assigned.\n");
//    }
//  }
  //else
  //  DEBUG("CPUID function pointer not assigned yet.\n");
//  if(m_pfncpuidex)
  {
//    bReturn = (*m_pfncpuidex)(dwFunctionIndex,
//      p_dwEAX,p_dwEBX,p_dwECX,p_dwEDX,dwAffinityMask) ;
    bReturn = CpuidTx(dwFunctionIndex,
      p_dwEAX,p_dwEBX,p_dwECX,p_dwEDX,dwAffinityMask ) ;
    if( bReturn )
    {
      //DEBUG("Calling CPUID function succeeded.\n");
    }
    else
      DEBUG("Calling CPUID function failed.\n");
  }
  //else
  //  DEBUG("CPUID function pointer could not be assigned.\n");
  return bReturn ;
}

BYTE WinRing0_1_3RunTimeDynLinked::GetNumberOfCPUCores()
{
  //return 2 ;
  BYTE byCoreNumber = 255 ;
  DWORD dwEAX;
  DWORD dwEBX;
  DWORD dwECX;
  DWORD dwEDX;
  DEBUG("WRDL--getting number of CPU cores\n");
  if( CpuidEx(
    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
    0x80000008,
    &dwEAX,
    &dwEBX,
    &dwECX,
    &dwEDX,
    1
      ) 
    )
  {
    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT ) 
      //"ECX 7:0 NC: number of physical cores - 1. 
      //The number of cores in the processor is NC+1 (e.g., if
      //NC=0, then there is one core). 
      //See also section 2.9.2 [Number of Cores and Core Number]."
      + 1 ;
    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
    DEBUGN( "WinRing0: Number of CPU cores according to AMDs calculation with "
      "CPUID: " << (WORD) byCoreNumber );
  }
  DEBUG("WRDL--end of getting number of CPU cores\n");
  return byCoreNumber ;
}

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
//  //If it does not point to the function yet.
//  if( ! m_pfnrdmsrex )
//  {
//    std::string strFuncName =
//      //"T" stands for set >>T<<hread affinity mask.
//      "RdmsrTx" ;
//    m_pfnrdmsrex = (_RdmsrTx ) ::GetProcAddress(m_hinstanceWinRingDLL,
//      strFuncName.c_str() );
//    if( m_pfnrdmsrex )
//    {
//      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
//      LOG( "Function pointer to \"" << strFuncName.c_str() <<
//          "\" successfully assigned.\n" );
//    }
//    else
//    {
//      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
//      LOG( "Function pointer to \"" << strFuncName.c_str() << "\" NOT assigned.\n");
//    }
//  }
//  if( m_pfnrdmsrex )
  {
    bReturn = 
      //"If the function succeeds, the return value is TRUE."
      //(*m_pfnrdmsrex)(dwIndex,p_dweax,p_dwedx,affinityMask);
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
      std::string strErrorMessage ;
      ::GetLastErrorMessageString(
        dwErrorCode ,
        strErrorMessage
        ) ;
      std::string stdstrAdditionalInfo = "" ;
      switch(dwErrorCode)
      {
      case ERROR_GEN_FAILURE:
        stdstrAdditionalInfo = "This error code could mean that MSR address \"" + 
          to_stdstring<DWORD>(dwIndex) + "\" is not avaiable on the CPU" ;
          break ;
      }
        //Show the detailed error message here because the details 
        //(how the access to the MSR is realised: via WinRing0)
        //depend on the implementation.
      UIconfirm(
        "Reading from MSR failed. "
        "\nerror message: \"" + strErrorMessage + "\"(error code: " 
        + to_stdstring<DWORD>( //(WORD) byModel 
          dwErrorCode //, std::hex
          )
        + ")\n"
        + stdstrAdditionalInfo + "\n"
        "Possible solutions:\n"
        "-This program needs elevated privileges for ring 0 / CPU "
        "access. So run it as administrator.\n"
        "-This program uses "
        "a DLL to communicate with the CPU for higher privileges. Does the "
        "file \"WinRing0[...].sys\" lay within the same directory as "
        "\"WinRing0[...].dll\"?\n");
      throw ReadMSRexception() ;
    }
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
  if( m_pfnrdpmcex )
  {
    //RdpmcEx seemed to cause a blue screen (maybe because of wrong param values)
    //bReturn =
    //(*m_pfnrdpmcex)(
    //   dwIndex
    //   ,p_dweax
    //   ,p_dwedx
    //   ,affinityMask
    //  );
  }
  return bReturn ;
}

BOOL WinRing0_1_3RunTimeDynLinked::ReadPciConfigDwordEx(
  DWORD dwPCIaddress
  , DWORD dwRegAddress
  , PDWORD p_dwValue
  )
{
  BOOL bReturn = FAILURE ;
//  //If it does not point to the function yet.
//  if( ! m_pfnreadpciconfigdwordex )
//  {
//    std::string strFuncName = "ReadPciConfigDwordEx" ;
//    m_pfnreadpciconfigdwordex = (_ReadPciConfigDwordEx ) ::GetProcAddress(
//      m_hinstanceWinRingDLL, strFuncName.c_str() );
//    if( m_pfnreadpciconfigdwordex )
//    {
//      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
//      LOG( "Function pointer to \"" << strFuncName.c_str() <<
//          "\" successfully assigned.\n" );
//    }
//    else
//    {
//      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
//      LOG( "Function pointer to " << strFuncName.c_str() << " NOT assigned.\n" );
//    }
//  }
//  if( m_pfnreadpciconfigdwordex )
  {
    bReturn =
      ////"If the function succeeds, the return value is TRUE."
      //(*m_pfnreadpciconfigdwordex)(
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
      UIconfirm("Accessing the CPU failed. possible causes:\n"
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

void WinRing0_1_3RunTimeDynLinked::SetUserInterface(UserInterface * pui)
{
  mp_userinterface = pui ;
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
//  //If it does not point to the function yet.
//  if( ! m_pfnwritepciconfigdwordex )
//  {
//    std::string strFuncName = "WritePciConfigDwordEx" ;
//    m_pfnwritepciconfigdwordex = (_WritePciConfigDwordEx ) ::GetProcAddress(
//      m_hinstanceWinRingDLL, strFuncName.c_str() );
//    if( m_pfnwritepciconfigdwordex )
//    {
//      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
//      LOG( "Function pointer to \"" << strFuncName.c_str() <<
//          "\" successfully assigned.\n" );
//    }
//    else
//    {
//      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
//      LOG("Function pointer to \"" << strFuncName.c_str() <<
//          "\" NOT assigned.\n" );
//    }
//  }
//  if( m_pfnwritepciconfigdwordex )
  {
    bReturn =
      //"If the function succeeds, the return value is TRUE."
      //(*m_pfnwritepciconfigdwordex)(
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
      UIconfirm("Accessing the CPU failed. possible causes:\n"
        "This program needs elevated privileges for ring 0 "
        "access. So run it as administrator.\n"
        "This program uses "
        "a DLL to communicate with the CPU for higher privileges. Does the "
        "file \"WinRing0[...].sys\" lay within the same directory as "
        "\"WinRing0[...].dll\"?\n");
  }
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
//  //If it does not point to the function yet.
//  if( ! m_pfnwrmsrex )
//  {
//    std::string strFuncName =
//      //"T" stands for set >>T<<hread affinity mask.
//      "WrmsrTx" ;
//    m_pfnwrmsrex = (_WrmsrTx )GetProcAddress(m_hinstanceWinRingDLL,
//      strFuncName.c_str() );
//    if(m_pfnwrmsrex)
//    {
//      DEBUG("pointer for writing to MSR succ. obtained.\n");
//    }
//    else
//    {
//      DEBUG("pointer for writing to MSR NOT succ. obtained.\n");
//    }
//  }
//  if( //m_pfnrdmsrex
//    m_pfnwrmsrex )
  {
    //DEBUG("Directly before writing to MSR\n");
    bReturn = 
//      //"If the function succeeds, the return value is TRUE."
//      (*m_pfnwrmsrex)(dwIndex,dwEAX,dwEDX,dwAffinityMask);
      ::WrmsrTx(dwIndex,dwEAX,dwEDX,dwAffinityMask);
    if( bReturn )
    {
      //DEBUG("Writing to MSR succeeded.\n");
    }
    else
      mp_userinterface->Confirm("writing to MSR failed. possible causes:\n"
        "This program needs elevated privileges for ring 0 "
        "access. So run it as administrator.\n"
        "This program uses "
        "a DLL to communicate with the CPU for higher privileges. Does the "
        "file \"WinRing0[...].sys\" lay within the same directory as "
        "\"WinRing0[...].dll\"?\n");  
  }
  return bReturn ;
}
