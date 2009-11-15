#include "WinRing0dynlinked.hpp"
//#include <winbase.h> //for ::GetProcAddress()
#include <Windows.h> //for ::GetProcAddress(), ::GetCurrentDirectory
#include <strstream> //for std::ostrstream
#include "../global.h" //for DEBUG()
#include "../preprocessor_helper_macros.h"
#include <Controller/stdstring_format.hpp>
#include <Windows/ErrorCodeFromGetLastErrorToString.h>

//#include <OlsApiInitDef.h> //for _IsMsr

//#ifdef X64 || 
#if defined(X64) || defined(_M_X64 ) || defined(_WIN64 )
  #define DLL_NAME "WinRing0x64.dll"
  #define WINRING0_DRIVER_NAME "WinRing0x64.sys"
#else
  #define DLL_NAME "WinRing0.dll"
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

WinRing0dynLinked::WinRing0dynLinked(UserInterface * pui)
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
  Init(pui) ;
}
void WinRing0dynLinked::Init(UserInterface * pui)
{
  char * archDLLName = DLL_NAME ;
  m_hinstanceWinRingDLL = NULL ;
  m_pfnrdmsrex = NULL ;
  m_pfnwrmsrex = NULL ;
  m_pfncpuidex = NULL ;
  m_pfnreadpciconfigdwordex = NULL ;
  mp_userinterface = pui ;
  //m_pfnrdmsrex = NULL ;
  m_hinstanceWinRingDLL = ::LoadLibraryA(archDLLName);
  //wxMessageBox(//wxstrDLLName 
  //  wxstrDLLName + " DLL NOT loaded");
  if (m_hinstanceWinRingDLL == 0)
  {
    std::ostrstream ostrstream;
    ostrstream << "Error loading the necessary library \"" << 
        archDLLName << "\"\n-Possible solution: put this library into the "
        "same directory as this executable's (working) directory or into a "
        "Windows system directory" ;
    if( mp_userinterface )
      mp_userinterface->Confirm(ostrstream);
    throw DLLnotLoadedException(ostrstream.str() ) ;
  }
  else
  {
    //DEBUG("%s library successfully loaded\n", archDLLName);
    LOG( archDLLName << " library successfully loaded\n" );
    //wxMessageBox(//wxstrDLLName 
    //  wxstrDLLName + " DLL successfulled loaded");
    //wxString wxstrFuncName="InitializeOls";
    //
    //void * pfn=wxdynamiclibraryTrans.
    //  //Returns pointer to symbol name in the library or NULL if the library contains no such symbol.
    //  GetSymbol(wxstrFuncName,&bSuccess);
    //void * pfn = ::GetProcAddress(m_hinstanceWinRingDLL,"InitializeOls");
    _InitializeOls pfnInitializeOls = (_InitializeOls)
      ::GetProcAddress(m_hinstanceWinRingDLL,"InitializeOls");
    //if( pfn == NULL )
    if( pfnInitializeOls == NULL )
    {
      //wxMessageBox("no " +wxstrFuncName);
      DEBUG("no InitializeOls() in DLL library\n");
      //InitializeOls() ausfhren.
      //(*pfn)();
      std::ostrstream ostrstream;
      ostrstream << "Error loading the necessary library \"" << 
        archDLLName << "\"" ;
      if( mp_userinterface )
        mp_userinterface->Confirm(ostrstream);
    }
    else
    {
      std::string strFuncName ;
      DWORD dwValue;
      _IsMsr pfnMSR ;
      DEBUG("function InitializeOls exists\n");
      //(*(void (*)())pfn)();
      dwValue = (*pfnInitializeOls)() ;
      //DEBUG("result of InitializeOls:%lu\n", dwValue);
      LOG("result of InitializeOls:" << dwValue << "\n" );
      if( dwValue == OLS_DLL_NO_ERROR )
      {
         LOG("WinRing0 successfully initialized\n") ;
      }
      else
      {
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
                  << lptstrCurrentDirectory << "\"\n" ;
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
                      " (and this exe)\n" ;
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
            mp_userinterface->Confirm(ostrstream);
#ifdef _DEBUG
            int i = 0 ;
#endif
          }
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
          //When throwing an exception by creating the exception object on the
          //stack VS 2005 reported an "stack buffer error"?.
          //Oh, that was rather because the exception was not catched.
         throw //new 
           CPUaccessException(ostrstream.str() ) ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
      }
      pfnMSR = (_IsMsr)GetProcAddress(m_hinstanceWinRingDLL,"IsMsr");
      //call "BOOL IsMsr();"
      //if( (*(BOOL (*)())pfn)() )
      if((*pfnMSR)() )
      {
        DEBUG("MSR exist.\n");
      }
      else
      {
        DEBUG("MSR DO NOT exist.\n");
      }
      //strFuncName = "RdmsrEx" ;
      //g_pfnRdMsrEx = //(pfnRdMsrExDef)
      //  (fnRdMsrExDef *)GetProcAddress(m_hinstanceWinRingDLL,strFuncName.c_str() );
      ////If the function exists in the DLL.
      //if( g_pfnRdMsrEx )
      //{
      //  DEBUG("Function \"%s\" exists within DLL\n", strFuncName.c_str() ) ;
      //  bOk = true ;
      //}
      m_pfnrdpmcex = (_RdpmcEx)::GetProcAddress( m_hinstanceWinRingDLL,"RdpmcEx");
      //if( ! m_pfnreadpmcex )
        //throw DynLinkToDLLfunctionException("RdpmcEx") ;
    }
  }
}

//WinRing0dynLinked::WinRing0dynLinked(UserInterface * pui)
WinRing0dynLinked::WinRing0dynLinked()
  //: mp_userinterface(pui)
{
  //WinRing0dynLinked(NULL);
  Init(NULL) ;
}

WinRing0dynLinked::~WinRing0dynLinked()
{
  _DeinitializeOls pfnInitializeOls = (_DeinitializeOls)
    ::GetProcAddress(m_hinstanceWinRingDLL,"InitializeOls");
  if( pfnInitializeOls )
  {
    (*pfnInitializeOls)() ;
  }
}

BOOL WinRing0dynLinked::CpuidEx(
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
  LOG( "WRDL--CPUIDex--begin m_pfncpuidex: " << m_pfncpuidex << "\n" );
  //If it does not point to the function yet.
  if( ! m_pfncpuidex )
  {
    std::string strFuncName = "CpuidEx" ;
    //_CpuidEx m_pfncpuidex;
    m_pfncpuidex = (_CpuidEx) ::GetProcAddress( m_hinstanceWinRingDLL,
      strFuncName.c_str() );
    if(m_pfncpuidex)
    {
      DEBUG("CPUID function pointer succ. assigned.\n");
    }
    else
    {
      DEBUG("CPUID function pointer NOT succ. assigned.\n");
    }
  }
  //else
  //  DEBUG("CPUID function pointer not assigned yet.\n");
  if(m_pfncpuidex)
  {
    bReturn = (*m_pfncpuidex)(dwFunctionIndex,
      p_dwEAX,p_dwEBX,p_dwECX,p_dwEDX,dwAffinityMask) ;
    if( bReturn )
    {
      DEBUG("Calling CPUID function succeeded.\n");
    }
    else
      DEBUG("Calling CPUID function failed.\n");
  }
  //else
  //  DEBUG("CPUID function pointer could not be assigned.\n");
  return bReturn ;
}

BYTE WinRing0dynLinked::GetNumberOfCPUCores()
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
    LOG( "Number of CPU cores: " << (WORD) byCoreNumber << "\n" );
  }
  DEBUG("WRDL--end of getting number of CPU cores\n");
  return byCoreNumber ;
}

BOOL // TRUE: success, FALSE: failure
  WinRing0dynLinked::RdmsrEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;
  //If it does not point to the function yet.
  if( ! m_pfnrdmsrex )
  {
    std::string strFuncName = "RdmsrEx" ;
    m_pfnrdmsrex = (_RdmsrEx ) ::GetProcAddress(m_hinstanceWinRingDLL,
      strFuncName.c_str() );
    if( m_pfnrdmsrex )
    {
      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
      LOG( "Function pointer to \"" << strFuncName.c_str() << 
          "\" successfully assigned.\n" );
    }
    else
    {
      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
      LOG( "Function pointer to \"" << strFuncName.c_str() << "\" NOT assigned.\n");
    }
  }
  if( m_pfnrdmsrex )
  {
    bReturn = 
      //"If the function succeeds, the return value is TRUE."
      (*m_pfnrdmsrex)(dwIndex,p_dweax,p_dwedx,affinityMask);
    //"If the function succeeds, the return value is TRUE. "
    if( bReturn )
    {
      //DEBUG("Reading from MSR succeeded\n");
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

inline BOOL WinRing0dynLinked::RdpmcEx(
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

BOOL WinRing0dynLinked::ReadPciConfigDwordEx(
  DWORD dwPCIaddress
  , DWORD dwRegAddress
  , PDWORD p_dwValue
  )
{
  BOOL bReturn = FAILURE ;
  //If it does not point to the function yet.
  if( ! m_pfnreadpciconfigdwordex )
  {
    std::string strFuncName = "ReadPciConfigDwordEx" ;
    m_pfnreadpciconfigdwordex = (_ReadPciConfigDwordEx ) ::GetProcAddress(
      m_hinstanceWinRingDLL, strFuncName.c_str() );
    if( m_pfnreadpciconfigdwordex )
    {
      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
      LOG( "Function pointer to \"" << strFuncName.c_str() << 
          "\" successfully assigned.\n" );
    }
    else
    {
      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
      LOG( "Function pointer to " << strFuncName.c_str() << " NOT assigned.\n" );
    }
  }
  if( m_pfnreadpciconfigdwordex )
  {
    bReturn =
      //"If the function succeeds, the return value is TRUE."
      (*m_pfnreadpciconfigdwordex)(
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

void WinRing0dynLinked::SetUserInterface(UserInterface * pui)
{
  mp_userinterface = pui ;
}

void WinRing0dynLinked::Sleep(WORD wMillis)
{
  ::Sleep(wMillis) ;
}

void WinRing0dynLinked::UIconfirm(const std::string & str)
{
  if ( mp_userinterface )
    mp_userinterface->Confirm(str) ;
}

BOOL WINAPI WinRing0dynLinked::WritePciConfigDwordEx (
  DWORD dwPCIaddress, DWORD dwRegAddress, DWORD dwValue)
{
  BOOL bReturn = FAILURE ;
  //If it does not point to the function yet.
  if( ! m_pfnwritepciconfigdwordex )
  {
    std::string strFuncName = "WritePciConfigDwordEx" ;
    m_pfnwritepciconfigdwordex = (_WritePciConfigDwordEx ) ::GetProcAddress(
      m_hinstanceWinRingDLL, strFuncName.c_str() );
    if( m_pfnwritepciconfigdwordex )
    {
      //DEBUG("Function pointer to %s succ assigned.\n", strFuncName.c_str());
      LOG( "Function pointer to \"" << strFuncName.c_str() << 
          "\" successfully assigned.\n" );
    }
    else
    {
      //DEBUG("Function pointer to %s NOT assigned.\n", strFuncName.c_str());
      LOG("Function pointer to \"" << strFuncName.c_str() << 
          "\" NOT assigned.\n" );
    }
  }
  if( m_pfnwritepciconfigdwordex )
  {
    bReturn =
      //"If the function succeeds, the return value is TRUE."
      (*m_pfnwritepciconfigdwordex)(
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
WINAPI 
WinRing0dynLinked::WrmsrEx(
	DWORD dwIndex,		// MSR index
	DWORD dwEAX,			// bit  0-31
	DWORD dwEDX,			// bit 32-63
	DWORD dwAffinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;
  //If it does not point to the function yet.
  if( ! m_pfnwrmsrex )
  {
    std::string strFuncName = "WrmsrEx" ;
    m_pfnwrmsrex = (_WrmsrEx )GetProcAddress(m_hinstanceWinRingDLL,
      strFuncName.c_str() );
    if(m_pfnwrmsrex)
    {
      DEBUG("pointer for writing to MSR succ. obtained.\n");
    }
    else
    {
      DEBUG("pointer for writing to MSR NOT succ. obtained.\n");
    }
  }
  if( //m_pfnrdmsrex 
    m_pfnwrmsrex )
  {
    //DEBUG("Directly before writing to MSR\n");
    bReturn = 
      //"If the function succeeds, the return value is TRUE."
      (*m_pfnwrmsrex)(dwIndex,dwEAX,dwEDX,dwAffinityMask);
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
