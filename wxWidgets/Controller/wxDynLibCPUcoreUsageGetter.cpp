#include "wxDynLibCPUcoreUsageGetter.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/DLLloadError.hpp>
#include <string>

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
  //http://docs.wxwidgets.org/2.8.7/wx_wxdynamiclibrary.html#wxdynamiclibraryload:
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
          "GetCPUcoreUsage" ) ;
      m_pfn_dll_usage_getter_num_logical_cpu_cores_type =
        (dll_usage_getter_num_logical_cpu_cores_type)
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol( //strDLLfunctionName
          "GetNumberOfLogicalCPUcores" ) ;
      m_pfn_dll_init_type = (dll_usage_getter_init_type)
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol(
          "Init" ) ;
      LOGN("address of CPU access object: " << mp_cpuaccess )
      if( m_pfn_dll_init_type )
        (*m_pfn_dll_init_type) ( //p_cpuaccess 
        //p_cpuacces->wNumLogicalCPUcores 
        //cpucoredata.m_byNumberOfCPUCores
        //By passing a pointer to an object the DLL that receives this pointer
        //has to interpret the struct (an object is a data/ a struct +
        //functions) in the same way as this executable.
        //This means that the members of the struct must both habe the
        //same alignment and (relative) position.
        //For instance there were crashes when exe was compiled with MinGW
        //and the DLL was compiled with MSVC.
        mp_cpuaccess
        ) ;
      if( m_pfngetcpucoreusage //&& m_pfn_dll_init_type
          )
      {
        //(*m_pfn_dll_init_type) ( p_cpuaccess ) ;
        bSuccess = true ;
        LOGN("CPU core usage getter dyn lib: function pointers to its functions assigned")
      }
    }
  }
  if( ! bSuccess )
  {
    DWORD dw = ::GetLastError() ;
    //std::string stdstrErrMsg = ::LocalLanguageMessageFromErrorCode( dw) ;
    std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
    stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
    //::wxMessageBox( wxT("Error message: ") + wxString(stdstrErrMsg) , wxT("loading DLL failed") ) ;
    LOGN("loading CPU core usage getter dynamic library failed:" <<
        stdstrErrMsg )
    //It does not make a sense to use this class instance because the 
    //initialisation failed. So throw an exception.
    throw CPUaccessException(stdstrErrMsg) ;
  }  
}

wxDynLibCPUcoreUsageGetter::~wxDynLibCPUcoreUsageGetter()
{
  m_wxdynamiclibraryCPUcoreUsage.Unload() ;
  DEBUGN("~wxDynLibCPUcoreUsageGetter()")
  LOGN("Unloaded the CPU core usage getter dynamic library")
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
      //DEBUG_COUTN(
      DEBUGN( "wxDynLibCPUcoreUsageGetter::GetPercentalUsageForAllCores:"
        "usage for core" << (WORD)byCPUcoreNumber << ":" << f )
      arf[byCPUcoreNumber] = f ;
    }
#else
      arf[byCPUcoreNumber] = (*m_pfngetcpucoreusage)(byCPUcoreNumber);
#endif
  }
  return 1 ;
}
