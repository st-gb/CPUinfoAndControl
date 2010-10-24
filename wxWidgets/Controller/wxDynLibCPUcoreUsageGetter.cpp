#include "wxDynLibCPUcoreUsageGetter.hpp"
//GetErrorMessageFromErrorCodeA(...)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/GetLastErrorCode.hpp>//OperatingSystem::GetLastErrorCode()
#include <Controller/I_CPUaccess.hpp>
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <Windows/ErrorCodeFromGetLastErrorToString.h>
  #include <Windows/DLLloadError.hpp>
#endif //#ifdef _WIN32
#include <string>
#include <global.h>

wxDynLibCPUcoreUsageGetter::wxDynLibCPUcoreUsageGetter(
  //std::string & strDLLname 
  wxString & r_wxstrFilePath 
  //, std::string & strDLLfunctionName
  , I_CPUaccess * p_cpuaccess
  , CPUcoreData & cpucoredata
  )
{
  mp_cpuaccess = p_cpuaccess ;
  bool bSuccess = false ;
  //m_bDLLsuccessfullyLoaded = m_wxdynamiclibraryCPUcoreUsage.Load(
  //  strDLLname//, int flags = wxDL_DEFAULT
  //) ;
  //http://docs.wxwidgets.org/2.8.7/wx_wxdynamiclibrary.html
  // #wxdynamiclibraryload:
  //"Returns true if the library was successfully loaded, false otherwise."
  if( m_wxdynamiclibraryCPUcoreUsage.Load( r_wxstrFilePath
    //strDLLname 
    ) 
    )
  {
    mp_cpucoredata = & cpucoredata ;
    ////function signature (type), function name, wxDynLib object
    //wxDYNLIB_FUNCTION( _GetCPUcoreUsage, strDLLfunctionName , 
    //  m_wxdynamiclibraryCPUcoreUsage);
    if( m_wxdynamiclibraryCPUcoreUsage.IsLoaded() )
    {
      LOGN("CPU core usage getter Dyn lib successfully loaded")
      m_pfngetcpucoreusage = (_GetCPUcoreUsage) 
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol( //strDLLfunctionName
          //Use wxT() macro to enable to compile with both unicode and ANSI.
          wxT( "GetCPUcoreUsage" ) ) ;
      if( m_wxdynamiclibraryCPUcoreUsage.HasSymbol(
          //Use wxT() macro to enable to compile with both unicode and ANSI.
          wxT("GetNumberOfLogicalCPUcores")
          )
        )
        m_pfn_dll_usage_getter_num_logical_cpu_cores_type =
          (dll_usage_getter_num_logical_cpu_cores_type)
          m_wxdynamiclibraryCPUcoreUsage.GetSymbol( //strDLLfunctionName
            //Use wxT() macro to enable to compile with both unicode and ANSI.
            wxT("GetNumberOfLogicalCPUcores") ) ;
      else
        m_pfn_dll_usage_getter_num_logical_cpu_cores_type = NULL ;
      m_pfn_dll_init_type = (dll_usage_getter_init_type)
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol(
          //Use wxT() macro to enable to compile with both unicode and ANSI.
          wxT(INIT_LITERAL) ) ;
      LOGN("address of CPU access object: " << mp_cpuaccess )
      if( m_pfn_dll_init_type )
        (*m_pfn_dll_init_type) ( //p_cpuaccess 
        //p_cpuacces->wNumLogicalCPUcores 
        //cpucoredata.m_byNumberOfCPUCores

        //By passing a pointer to an object the DLL that receives this pointer
        //has to interpret the struct (an object is a data/ a struct +
        //functions) in the same way as this executable.
        //This means that the members of the struct must both have the
        //same alignment and (relative) position.
        //For instance there were crashes when exe was compiled with MinGW
        //and the DLL was compiled with MSVC because of the "vtable"?:
        //http://de.wikipedia.org/wiki/Tabelle_virtueller_Methoden  /
        //http://en.wikipedia.org/wiki/Virtual_method_table
        // meaning (some) function addresses of I_CPUaccess between MSVC and g++
        // were probably different.
//#ifndef _DEBUG
//        //TODO
//        0 //just for testing what happens if the DLL wants to call operations
            // of I_CPUaccess on a NULL pointer
//#else
        mp_cpuaccess
//#endif
        ) ;
      LOGN("after calling DLL's " << INIT_LITERAL <<  "function")
      if( m_pfngetcpucoreusage //&& m_pfn_dll_init_type
          )
      {
        //(*m_pfn_dll_init_type) ( p_cpuaccess ) ;
        bSuccess = true ;
        LOGN("CPU core usage getter dyn lib: function pointers to its "
          "functions assigned")
      }
    }
  }
  if( ! bSuccess )
  {
    std::string stdstrErrMsg = "loading the dynamic library failed:" ;
    DWORD dw = OperatingSystem::GetLastErrorCode() ;
    stdstrErrMsg += GetErrorMessageFromErrorCodeA(dw) ;
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//    DWORD dw = ::GetLastError() ;
    //std::string stdstrErrMsg = ::LocalLanguageMessageFromErrorCodeA( dw) ;
//    stdstrErrMsg += ::GetLastErrorMessageString(dw) ;
    stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
    //::wxMessageBox( wxT("Error message: ") + wxString(stdstrErrMsg) , wxT("loading DLL failed") ) ;
#else
#endif //#ifdef _WIN32
    LOGN("loading CPU core usage getter dynamic library failed:" <<
        stdstrErrMsg )
    //It does not make a sense to use this class instance because the 
    //initialisation failed. So throw an exception.
    throw CPUaccessException(stdstrErrMsg) ;
  }
  LOGN("End of dyn lib CPU core usage getter constructor")
}

wxDynLibCPUcoreUsageGetter::~wxDynLibCPUcoreUsageGetter()
{
  m_wxdynamiclibraryCPUcoreUsage.Unload() ;
  DEBUGN("~wxDynLibCPUcoreUsageGetter()")
  LOGN("Unloaded the CPU core usage getter dynamic library")
}

WORD wxDynLibCPUcoreUsageGetter::GetNumberOfCPUcores()
{
  return GetNumberOfLogicalCPUcores() ;
}

WORD wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores()
{
  //DEBUG_COUTN(
  DEBUGN( "wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores "
    "function pointer for # CPU cores : " <<
    m_pfn_dll_usage_getter_num_logical_cpu_cores_type )
  if( m_pfn_dll_usage_getter_num_logical_cpu_cores_type )
  {
    WORD wNumLogCPUcores =
        (*m_pfn_dll_usage_getter_num_logical_cpu_cores_type)() ;
    //DEBUG_COUTN(
    DEBUGN( "wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores "
      "# CPU cores : " << wNumLogCPUcores )
    return wNumLogCPUcores ;
  }
  return 0 ;
}

BYTE wxDynLibCPUcoreUsageGetter::GetPercentalUsageForAllCores( float arf [] )
{
  //m_pfngetcpucoreusage should be valid if an object could be created.
  //if( m_pfngetcpucoreusage )
  {
    for(BYTE byCPUcoreNumber = 0 ; byCPUcoreNumber < 
      mp_cpucoredata->GetNumberOfCPUcores() ; ++ byCPUcoreNumber 
      )
#ifdef _DEBUG
    {
      float f = (*m_pfngetcpucoreusage)(byCPUcoreNumber) ;
      LOGN( "wxDynLibCPUcoreUsageGetter::GetPercentalUsageForAllCores:"
        "usage for core" << (WORD)byCPUcoreNumber << ":" << f )
      arf[byCPUcoreNumber] = f ;
    }
#else
      arf[byCPUcoreNumber] = (*m_pfngetcpucoreusage)(byCPUcoreNumber);
#endif
  }
  return 1 ;
}
