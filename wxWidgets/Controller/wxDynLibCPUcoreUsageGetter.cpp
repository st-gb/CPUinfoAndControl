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
      m_pfngetcpucoreusage = (_GetCPUcoreUsage) 
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol( //strDLLfunctionName
          "GetCPUcoreUsage" ) ;
      m_pfn_dll_init_type = (dll_usage_getter_init_type)
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol( 
          "Init" ) ;
      if( m_pfngetcpucoreusage && m_pfn_dll_init_type )
      {
        (*m_pfn_dll_init_type) ( p_cpuaccess ) ;
        bSuccess = true ;
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

    //It does not make a sense to use this class instance because the 
    //initialisation failed. So throw an exception.
    throw CPUaccessException(stdstrErrMsg) ;
  }  
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
      arf[byCPUcoreNumber] = f ;
    }
#else
      arf[byCPUcoreNumber] = (*m_pfngetcpucoreusage)(byCPUcoreNumber);
#endif
  }
  return 0 ;
}
