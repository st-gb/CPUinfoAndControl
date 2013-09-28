/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "wxDynLibCPUcoreUsageGetter.hpp"
//GetErrorMessageFromErrorCodeA(...)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/GetLastErrorCode.hpp>//OperatingSystem::GetLastErrorCode()
#include <Controller/I_CPUaccess.hpp>
#include <Controller/Logger/LogLevel.hpp>
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <Windows/ErrorCodeFromGetLastErrorToString.h>
  #include <Windows/DLLloadError.hpp>
#endif //#ifdef _WIN32
#include <string>
//#include <global.h>

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
      LOGN_TYPE("CPU core usage getter Dyn lib successfully loaded",
        LogLevel::success)

      PossiblyAssignFunctionPointers();

      LOGN("address of CPU access object: " << mp_cpuaccess )
      if( m_pfn_dll_init_type )
      {
        if( ! mp_cpuaccess )
          LOGN_TYPE( "pointer to hardware access object is "
            "0->may crash when dyn lib's \"" << INIT_LITERAL << "\" function "
            "derefers it", LogLevel::warning)
        BYTE byRet = (*m_pfn_dll_init_type) ( //p_cpuaccess
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
        LOGN( "return value of DLL's " << INIT_LITERAL
          << " function:" << (WORD) byRet)
        if( byRet )
          LOGN_TYPE( "DLL's " << INIT_LITERAL
            << " function failed", LogLevel::error)
      }
      LOGN("after calling DLL's " << INIT_LITERAL <<  " function")
      if( m_pfngetcpucoreusage //&& m_pfn_dll_init_type
          )
      {
        //(*m_pfn_dll_init_type) ( p_cpuaccess ) ;
        bSuccess = true ;
        LOGN("CPU core usage getter dyn lib: assigned function pointers to its "
          "functions")
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
    LOGN_TYPE("loading CPU core usage getter dynamic library failed:" <<
        stdstrErrMsg, LogLevel::error)
    //It does not make a sense to use this class instance because the 
    //initialisation failed. So throw an exception.
    throw CPUaccessException(stdstrErrMsg) ;
  }
  LOGN("End" /*" of dyn lib CPU core usage getter constructor"*/ )
}

wxDynLibCPUcoreUsageGetter::~wxDynLibCPUcoreUsageGetter()
{
  DEBUGN(/*"~wxDynLibCPUcoreUsageGetter()"*/ "begin")
  m_wxdynamiclibraryCPUcoreUsage.Unload() ;
  //TODO crashed here under Linux if dyn lib uses global logger as in this exe
  // (the logger object is destroyed at end of dyn lib and then this exe tries
  // to access it)
  LOGN("Unloaded the CPU core usage getter dynamic library")
}

WORD wxDynLibCPUcoreUsageGetter::GetNumberOfCPUcores()
{
  return GetNumberOfLogicalCPUcores() ;
}

WORD wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores()
{
  //DEBUG_COUTN(
  DEBUGN( //"wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores "
    "function pointer for # CPU cores : " <<
    m_pfn_dll_usage_getter_num_logical_cpu_cores_type )
  if( m_pfn_dll_usage_getter_num_logical_cpu_cores_type )
  {
    WORD wNumLogCPUcores =
        (*m_pfn_dll_usage_getter_num_logical_cpu_cores_type)() ;
    //DEBUG_COUTN(
    DEBUGN( //"wxDynLibCPUcoreUsageGetter::GetNumberOfLogicalCPUcores "
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
    for(fastestUnsignedDataType CPUcoreNumber = 0 ; CPUcoreNumber <
      mp_cpucoredata->GetNumberOfCPUcores() ; ++ CPUcoreNumber
      )
#ifdef _DEBUG
    {
      float f = (*m_pfngetcpucoreusage)(CPUcoreNumber) ;
      LOGN( //"wxDynLibCPUcoreUsageGetter::GetPercentalUsageForAllCores:"
        "usage for core" << CPUcoreNumber << ":" << f )
      arf[CPUcoreNumber] = f ;
    }
#else
      arf[CPUcoreNumber] = (*m_pfngetcpucoreusage)(CPUcoreNumber);
#endif
  }
  return 1 ;
}

void wxDynLibCPUcoreUsageGetter::PossiblyAssignFunctionPointers()
{
  m_pfngetcpucoreusage = (_GetCPUcoreUsage)
    m_wxdynamiclibraryCPUcoreUsage.GetSymbol( //strDLLfunctionName
      //Use wxT() macro to enable to compile with both unicode and ANSI.
      wxT( "GetCPUcoreUsage" ) ) ;

  PossiblyAssignFunctionPointer(
    //Use wxT() macro to enable to compile with both unicode and ANSI.
    wxT("GetNumberOfLogicalCPUcores"),
    m_pfn_dll_usage_getter_num_logical_cpu_cores_type);

  m_pfn_dll_init_type = (dll_usage_getter_init_type)
    m_wxdynamiclibraryCPUcoreUsage.GetSymbol(
      //Use wxT() macro to enable to compile with both unicode and ANSI.
      wxT(INIT_LITERAL) ) ;
}

/** @brief template function for ease to assign function pointers from a dyn lib
 *  @param p_function must be passed in order to get its type */
template <typename func_type> void wxDynLibCPUcoreUsageGetter::
  PossiblyAssignFunctionPointer(
  const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
  )
{
  if( m_wxdynamiclibraryCPUcoreUsage.HasSymbol( wxstrFuncName ) )
    p_function = (func_type) m_wxdynamiclibraryCPUcoreUsage.GetSymbol(
      wxstrFuncName );
  else
    p_function = NULL ;
}
