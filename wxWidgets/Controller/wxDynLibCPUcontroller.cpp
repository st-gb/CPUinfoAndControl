#include "wxDynLibCPUcontroller.hpp"
#include "Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Controller/exported_functions.h> //ReadMSR
#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/DLLloadError.hpp>
#include <wx/msgdlg.h>
#include <limits> //float>::min()
//#include <float.h> //FLT_MIN

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
    wxString wxstrFuncName //= wxT
      ("Init") ;
    if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) 
      )
    //dll_init_type pfnInit = (dll_init_type)
    //  ::GetProcAddress(hinstanceCPUctlDLL,"Init");
    //FARPROC __stdcall pfn = ::GetProcAddress(hinstanceCPUctlDLL,"nNehalemControllerDLL");
    //if( pfnInit )
    {
//#ifdef _DEBUG
//      wxMessageBox( wxString::Format( "CPU access address: %x "
//        ", adress of ReadMSR fct:%x"
//        , p_cpuaccess
//        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
//        , & ::ReadMSR
//        ) ) ;
//#endif
      wxDYNLIB_FUNCTION(dll_init_type, Init, m_wxdynamiclibraryCPUctl) ;
      //void * wxdynamiclibraryCPUctl.GetSymbol(wxT("Init")) ;
      (*pfnInit)( //wxGetApp().mp_i_cpuaccess 
        p_cpuaccess
        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
        , & ::ReadMSR
        //1 
        ) ;
      m_pfnGetMaximumFrequencyInMHz = (dll_GetMaximumFrequencyInMHz_type) 
        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMaximumFrequencyInMHz") 
        ) ;
      m_pfnGetMinimumFrequencyInMHz = (dll_GetMaximumFrequencyInMHz_type) 
        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMinimumFrequencyInMHz") 
        ) ;
    
      //Do not use wxDYNLIB_FUNCTION: it shows a wxWidgets error message if 
      // a DLL function does not exist.
    //  wxDYNLIB_FUNCTION(dll_GetCurrentPstate_type, GetCurrentPstate, 
    //  m_wxdynamiclibraryCPUctl) ;
    //m_pfngetcurrentpstate = pfnGetCurrentPstate ;
    m_pfngetcurrentpstate = (dll_GetCurrentPstate_type) 
      m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetCurrentPstate") ) ;

    //wxDYNLIB_FUNCTION(dll_SetCurrentPstate_type, SetCurrentPstate, 
    //  m_wxdynamiclibraryCPUctl) ;
    //m_pfnsetcurrentpstate = pfnSetCurrentPstate ;
    m_pfnsetcurrentpstate = (dll_SetCurrentPstate_type) 
      m_wxdynamiclibraryCPUctl.GetSymbol( wxT("SetCurrentPstate") ) ;

    //wxDYNLIB_FUNCTION(dll_GetNumberOfCPUcores_type, GetNumberOfCPUcores, 
    //  m_wxdynamiclibraryCPUctl) ;
    //m_pfnGetNumberOfCPUcores = pfnGetNumberOfCPUcores ;
    m_pfnGetNumberOfCPUcores = (dll_GetNumberOfCPUcores_type)
      m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetNumberOfCPUcores") ) ;
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
//      DEBUG_COUT( "wxDynLibCPUcontroller::GetCurrentPstate("
//        << r_wFreqInMHz << ","
//        << r_fVolt << ","
//        << (WORD) byCoreID
//        << "):" <<
//        r_wFreqInMHz << " " << r_fVolt << "\n" ) ;
       return by ;
    }
    return 0 ; 
  }

WORD wxDynLibCPUcontroller::GetMaximumFrequencyInMHz()
{
  WORD wRet = 0 ;
  //wxDYNLIB_FUNCTION(dll_GetMaximumFrequencyInMHz_type, 
  //  GetMaximumFrequencyInMHz, m_wxdynamiclibraryCPUctl) ;
  //if( pfnGetMaximumFrequencyInMHz )
  if( m_pfnGetMaximumFrequencyInMHz )
  {
    wRet = (*m_pfnGetMaximumFrequencyInMHz)(0) ;
    DEBUG_COUT( "wxDynLibCPUcontroller::GetMaximumFrequencyInMHz():" <<
      wRet << "\n" ) ;
  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetMinimumFrequencyInMHz()
{
  WORD wRet = 0 ;
  if( m_pfnGetMinimumFrequencyInMHz )
  {
    wRet = (*m_pfnGetMinimumFrequencyInMHz)(0) ;
  }
  return wRet ;
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

float wxDynLibCPUcontroller::GetTemperatureInCelsius( WORD wCoreID )
{
  if( m_pfngettemperatureincelsius )
  {
    return ( * m_pfngettemperatureincelsius ) ( wCoreID ) ;
  }
  return std::numeric_limits<float>::min() ;
    //FLT_MIN
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
