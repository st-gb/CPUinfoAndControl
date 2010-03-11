#include "wxDynLibCPUcontroller.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/DLLloadError.hpp>

wxDynLibCPUcontroller::wxDynLibCPUcontroller(
  wxString & r_wxstrFilePath 
  , I_CPUaccess * p_cpuaccess 
  )
{
  //m_wxdynamiclibraryCPUctl ;

  //wxstrFilePath = wxT("T:\SourceCodeManagement\X86Info_and_Control\x86InfoAndControl_MSVC\VS2010\x86InfoAndControlGUI\Debug\NehalemControllerDLL") ;
  //http://docs.wxwidgets.org/2.8.7/wx_wxdynamiclibrary.html#wxdynamiclibraryload:
  //"Returns true if the library was successfully loaded, false otherwise."
  if( m_wxdynamiclibraryCPUctl.Load(r_wxstrFilePath) 
    )
/*   HINSTANCE hinstanceCPUctlDLL = ::LoadLibrary(wxstrFilePath);
  if ( hinstanceCPUctlDLL != 0)*/
  {
    //wxdynamiclibraryCPUctl.
    wxString wxstrFuncName = wxT("Init") ;
    if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) 
      )
    //dll_init_type pfnInit = (dll_init_type)
    //  ::GetProcAddress(hinstanceCPUctlDLL,"Init");
    //FARPROC __stdcall pfn = ::GetProcAddress(hinstanceCPUctlDLL,"nNehalemControllerDLL");
    //if( pfnInit )
    {
      wxDYNLIB_FUNCTION(dll_init_type, Init, m_wxdynamiclibraryCPUctl) ;
      //void * wxdynamiclibraryCPUctl.GetSymbol(wxT("Init")) ;
      (*pfnInit)( //wxGetApp().mp_i_cpuaccess 
        p_cpuaccess
        //1 
        ) ;
    wxDYNLIB_FUNCTION(dll_GetCurrentPstate_type, GetCurrentPstate, 
      m_wxdynamiclibraryCPUctl) ;
    m_pfngetcurrentpstate = pfnGetCurrentPstate ;
    wxDYNLIB_FUNCTION(dll_SetCurrentPstate_type, SetCurrentPstate, 
      m_wxdynamiclibraryCPUctl) ;
    m_pfnsetcurrentpstate = pfnSetCurrentPstate ;
    wxDYNLIB_FUNCTION(dll_GetNumberOfCPUcores_type, GetNumberOfCPUcores, 
      m_wxdynamiclibraryCPUctl) ;
    m_pfnGetNumberOfCPUcores = pfnGetNumberOfCPUcores ;
      //dll_getMulti_type pfnGetMultiplier = (dll_getMulti_type)
      //  ::GetProcAddress(hinstanceCPUctlDLL,"GetMultiplier") ;
      //if( pfnGetMultiplier)
      //{
      //  DWORD dwM = (*pfnGetMultiplier) (0) ;
      //  dwM = dwM ;
      //}
      //DWORD dwMulti = (*pfnGetMultiplier)(0) ;
      //dwMulti = dwMulti ;
      //dll_Init
    }
    else
    {
      DWORD dw = ::GetLastError() ;
      std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
      stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
      //ERROR_PROC_NOT_FOUND
      /*::wxMessageBox( 
        wxString::Format("function %s was not found within DLL", 
          wxstrFuncName) + wxString(stdstrErrMsg) , 
        wxT("Error ")
        ) ;*/
      throw CPUaccessException(stdstrErrMsg) ;
    }
  }
  else
  {
    DWORD dw = ::GetLastError() ;
    //std::string stdstrErrMsg = ::LocalLanguageMessageFromErrorCode( dw) ;
    std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
    stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
    //::wxMessageBox( wxT("Error message: ") + wxString(stdstrErrMsg) , wxT("loading DLL failed") ) ;
    throw CPUaccessException(stdstrErrMsg) ;
  }
}

wxDynLibCPUcontroller::~wxDynLibCPUcontroller()
{
  m_wxdynamiclibraryCPUctl.Unload() ;
}

void wxDynLibCPUcontroller::DecreaseVoltageBy1Step(float & r_fVoltage)
{

}

BYTE wxDynLibCPUcontroller::GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & r_fVolt
    , BYTE byCoreID 
    )
  {
    WORD wMilliVolt ;
    if( m_pfngetcurrentpstate )
    {
       BYTE by = (*m_pfngetcurrentpstate)(
        & r_wFreqInMHz
        , & wMilliVolt
        , byCoreID
        ) ;
       r_fVolt = wMilliVolt * 1000 ;
       return by ;
    }
    return 0 ; 
  }

WORD wxDynLibCPUcontroller::GetMaximumFrequencyInMHz()
{
  WORD wRet = 0 ;
  wxDYNLIB_FUNCTION(dll_GetMaximumFrequencyInMHz_type, 
    GetMaximumFrequencyInMHz, m_wxdynamiclibraryCPUctl) ;
  if( pfnGetMaximumFrequencyInMHz )
  {
    wRet = (*pfnGetMaximumFrequencyInMHz)() ;
  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetMinimumFrequencyInMHz()
{
  return 0 ;
}

WORD wxDynLibCPUcontroller::GetMaximumVoltageID()
{
  return 0 ;
}

WORD wxDynLibCPUcontroller::GetMinimumVoltageID()
{
  return 0 ;
}

WORD wxDynLibCPUcontroller::GetNumberOfCPUcores()
{
  if( m_pfnGetNumberOfCPUcores )
  {
    return (*m_pfnGetNumberOfCPUcores ) () ;
  }
  return 0 ;
}

float wxDynLibCPUcontroller::GetVoltageInVolt(WORD wVoltageID )
{
  return 0 ;
}

WORD wxDynLibCPUcontroller::GetVoltageID(float fVoltageInVolt )
{
  return 0 ;
}

BYTE wxDynLibCPUcontroller::
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
{
  if( m_pfnsetcurrentpstate )
  {
    WORD wMilliVolt = (WORD) (fVolt * 1000.0) ;
     BYTE by = (*m_pfnsetcurrentpstate)(
      wFreqInMHz
      , wMilliVolt
      , byCoreID
      ) ;
     return by ;
  }
  return 0 ;
}
