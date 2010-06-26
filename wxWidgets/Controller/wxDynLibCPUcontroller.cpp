#include "wxDynLibCPUcontroller.hpp"
#include "Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Controller/exported_functions.h> //ReadMSR
#include <ModelData/ModelData.hpp>
#include <UserInterface/UserInterface.hpp>
#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/DLLloadError.hpp>
#include <wx/msgdlg.h>
#include <binary_search.cpp> //GetClosestLessOrEqual
#ifdef _MSC_VER
  #include <float.h> //FLT_MIN
#else
  #include <limits> //float>::min()
#endif

wxDynLibCPUcontroller::wxDynLibCPUcontroller(
  wxString & r_wxstrFilePath 
  , I_CPUaccess * p_cpuaccess 
  , UserInterface * p_userinterface
  )
  :
//  m_fReferenceClockInMHz(0.0) ,
  mp_userinterface (p_userinterface)
  , m_wNumberOfLogicalCPUcores ( 1 )
{
  mp_model = p_cpuaccess->mp_model ;
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
    wxString wxstrFuncName (
      //Use wxT() to enable to compile with both unicode and ANSI.
        wxT("Init" ) ) ;
//    LOGN("Dyn Lib " << r_wxstrFilePath << " successfully loaded")
//    if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName )
//      )
//    //dll_init_type pfnInit = (dll_init_type)
//    //  ::GetProcAddress(hinstanceCPUctlDLL,"Init");
//    //FARPROC __stdcall pfn = ::GetProcAddress(hinstanceCPUctlDLL,"nNehalemControllerDLL");
//    //if( pfnInit )
//    {
//      LOGN("Dyn Lib symbol " << wxstrFuncName << " exists")
////#ifdef _DEBUG
////      wxMessageBox( wxString::Format( "CPU access address: %x "
////        ", adress of ReadMSR fct:%x"
////        , p_cpuaccess
////        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
////        , & ::ReadMSR
////        ) ) ;
////#endif
//      wxDYNLIB_FUNCTION(dll_init_type, Init, m_wxdynamiclibraryCPUctl) ;
//      LOGN("Dyn Lib assigned fct ptr to symbol " << wxstrFuncName )
//      LOGN("Dyn Lib before calling " << wxstrFuncName )
//      DEBUGN("dyn lib: p_cpuaccess: " << p_cpuaccess)
//
////      //TODO
////      p_cpuaccess->mp_cpucontroller = NULL ;
//      //void * wxdynamiclibraryCPUctl.GetSymbol(wxT("Init")) ;
//      (*pfnInit)( //wxGetApp().mp_i_cpuaccess
//        p_cpuaccess
//        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
//        , & ::ReadMSR
//        //1
//        ) ;

#ifdef _DEBUG
      //TODO is that possible: change the cpu controller class inside the DLL's
      //init() function the a controller class in that module?
      //e.g. INit(CPUaccess * p_cpuaccess ) {
      //   p_cpuaccess->m_cpucontroller = & g_NehalemCPUcontroller ;
      // }
      //This would avoid writing export functions.
//      p_cpuaccess->mp_cpu = p_cpuaccess
#endif
//      LOGN("Dyn Lib after calling " << wxstrFuncName )
//      m_pfnGetMaximumFrequencyInMHz = (dll_GetMaximumFrequencyInMHz_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMaximumFrequencyInMHz")
//        ) ;
//      m_pfnGetMinimumFrequencyInMHz = (dll_GetMaximumFrequencyInMHz_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMinimumFrequencyInMHz")
//        ) ;
      m_pfnGetAvailableMultipliers = (pfnGetAvailableMultipliers_type)
        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetAvailableMultipliers")
        ) ;
//      GetAvailableMultipliers( mp_cpuaccess->mp_model->m_cpucoredata.
//        m_stdset_floatAvailableMultipliers) ;
//      m_pfnGetMaximumVoltageID = (dll_GetMaximumVoltageID_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMaximumVoltageID")
//        ) ;
//      m_pfnGetMinimumVoltageID = (dll_GetMinimumVoltageID_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetMinimumVoltageID")
//        ) ;
      m_pfnGetAvailableVoltages = (pfnGetAvailableMultipliers_type)
        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetAvailableVoltagesInVolt")
        ) ;
      LOGN("Dyn Lib after GetMinimumFrequencyInMHz")

      wxstrFuncName = wxT("PrepareForNextPerformanceCounting") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) )
        m_pfn_preparefornextperformancecounting =
          (dll_PrepareForNextPerformanceCounting)
          m_wxdynamiclibraryCPUctl.GetSymbol( wxstrFuncName ) ;
      else
        m_pfn_preparefornextperformancecounting = NULL ;
      //Do not use wxDYNLIB_FUNCTION: it shows a wxWidgets error message if 
      // a DLL function does not exist.
      //  wxDYNLIB_FUNCTION(dll_GetCurrentPstate_type, GetCurrentPstate,
      //  m_wxdynamiclibraryCPUctl) ;
      //m_pfngetcurrentpstate = pfnGetCurrentPstate ;
//      m_pfngetcurrentpstate = (dll_GetCurrentPstate_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetCurrentPstate") ) ;
      m_pfnGetCurrentVoltageAndFrequency = (
          pfn_GetCurrentVoltageAndFrequency_type )
        m_wxdynamiclibraryCPUctl.GetSymbol(
          wxT("GetCurrentVoltageAndFrequency") ) ;

      //wxDYNLIB_FUNCTION(dll_SetCurrentPstate_type, SetCurrentPstate,
      //  m_wxdynamiclibraryCPUctl) ;
      //m_pfnsetcurrentpstate = pfnSetCurrentPstate ;
//      m_pfnsetcurrentpstate = (dll_SetCurrentPstate_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("SetCurrentPstate") ) ;
      m_pfnSetCurrentVoltageAndMultiplier =
          (pfnSetCurrentVoltageAndMultiplier_type)
        m_wxdynamiclibraryCPUctl.GetSymbol(
          wxT("SetCurrentVoltageAndMultiplier") ) ;

//      m_pfn_GetVoltageID = (dll_GetVoltageID_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetVoltageID") ) ;
//
//      m_pfn_GetVoltageInVolt = (dll_GetVoltageInVolt_type)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetVoltageInVolt") ) ;

//      m_pfn_set_pstate_from_freq = (dynlib_SetPstateFromFreq)
//        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("SetPstateFromFreq") ) ;

      //m_pfn_too_hot = (dll_TooHot_type)
      //  m_wxdynamiclibraryCPUctl.GetSymbol( wxT("TooHot") ) ;

      wxstrFuncName = wxT("WriteMSR") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) )
        m_pfn_write_msr = (dll_WriteMSR_type)
          m_wxdynamiclibraryCPUctl.GetSymbol( wxT("WriteMSR") ) ;
      else
        m_pfn_write_msr = NULL ;
      //wxDYNLIB_FUNCTION(dll_GetNumberOfCPUcores_type, GetNumberOfCPUcores,
      //  m_wxdynamiclibraryCPUctl) ;

      wxstrFuncName = wxT("GetNumberOfCPUcores") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) )
//        m_pfnGetNumberOfCPUcores = pfnGetNumberOfCPUcores ;
        m_pfnGetNumberOfCPUcores = (dll_GetNumberOfCPUcores_type)
          m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetNumberOfCPUcores") ) ;
      else
        m_pfnGetNumberOfCPUcores = NULL ;

      m_pfngettemperatureincelsius = (dll_GetTemperatureInCelsius_type)
        m_wxdynamiclibraryCPUctl.GetSymbol( wxT("GetTemperatureInCelsius") ) ;

      wxstrFuncName = wxT("moreThan1CPUcorePowerPlane") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) )
        m_b1CPUcorePowerPlane = false ;
      LOGN("Dyn Lib after moreThan1CPUcorePowerPlane")

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
//    }
//    else
//    {
//      DWORD dw = ::GetLastError() ;
//      std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
//      stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
//      //ERROR_PROC_NOT_FOUND
//      /*::wxMessageBox(
//        wxString::Format("function %s was not found within DLL",
//          wxstrFuncName) + wxString(stdstrErrMsg) ,
//        wxT("Error ")
//        ) ;*/
//      throw CPUaccessException(stdstrErrMsg) ;
//    }
      //Because this may not be the 1st time a controller is attached, clear
      // previous multipliers, else the result is the intersection of the
      //current and the next multipliers.
      mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.clear() ;
      LOGN("before DLL::GetAvailableMultipliers")
      GetAvailableMultipliers(
        mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers) ;
      mp_model->m_cpucoredata.AvailableMultipliersToArray() ;

//      LOGN("after DLL::GetAvailableMultipliers")
      LOGN("Available multpliers:")
      for( WORD wIndex = 0 ; wIndex < mp_model->m_cpucoredata.
        m_stdset_floatAvailableMultipliers.size() ; ++ wIndex )
      {
        LOGN( mp_model->m_cpucoredata.m_arfAvailableMultipliers[ wIndex ]
          << " " )
      }
//      LOGN("")
      //Because this may not be the 1st time a controller is attached, clear
      // previous voltages, else the result is the intersection of the
      //current and the next voltages.
      mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.clear() ;
      GetAvailableVoltagesInVolt(
        mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt) ;
      mp_model->m_cpucoredata.AvailableVoltagesToArray() ;
      LOGN("Available voltages in Volt:")
      for( WORD wIndex = 0 ; wIndex < mp_model->m_cpucoredata.
        m_stdset_floatAvailableVoltagesInVolt.size() ; ++ wIndex )
      {
        LOGN( mp_model->m_cpucoredata.m_arfAvailableVoltagesInVolt[ wIndex ]
          << " " )
      }
//      LOGN("")
  }
  else
  {
    DWORD dw = ::GetLastError() ;
    //std::string stdstrErrMsg = ::LocalLanguageMessageFromErrorCodeA( dw) ;
    std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
    stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
    LOGN("loading CPU controller dynamic library failed:" <<
        stdstrErrMsg )
    //::wxMessageBox( wxT("Error message: ") + wxString(stdstrErrMsg) , wxT("loading DLL failed") ) ;
    throw CPUaccessException(stdstrErrMsg) ;
  }
}

wxDynLibCPUcontroller::~wxDynLibCPUcontroller()
{
  m_wxdynamiclibraryCPUctl.Unload() ;
  LOGN("Unloaded the CPU controller dynamic library")
}

void wxDynLibCPUcontroller::DecreaseVoltageBy1Step(float & r_fVoltage)
{

}

//TODO
void wxDynLibCPUcontroller::GetAvailableMultipliers(
  //Use a std::set because: in this set the elements are sorted, also:
  //not the releasing memory problem when using this container.
  std::set<float> & r_stdset_float )
{
  if( m_pfnGetAvailableMultipliers )
  {
    LOGN("getting available multipliers")
    float * arf ;
    WORD wNum ;
    arf = (*m_pfnGetAvailableMultipliers) (
        0 ,
        & wNum
        ) ;
    LOGN("float array address allocated by DLL:" << arf  << "num eles:"
      << wNum )
    //If alloc. by DLL succeeded.
    if( arf )
    {
      for( WORD wIndex = 0 ; wIndex < wNum ; ++ wIndex )
      {
        r_stdset_float.insert(arf[wIndex]) ;
      }
      LOGN("Before deleting the array that should have been allocated by the DLL")
      //Was dyn. allocated by the DLL.
      delete [] arf ;
    }
    LOGN("After deleting the array that should have been allocated by the DLL")
  }
}

void wxDynLibCPUcontroller::GetAvailableVoltagesInVolt(
  //Use a std::set because: in this set the elements are sorted, also:
  //not the releasing memory problem when using this container.
  std::set<float> & r_stdset_float )
{
  if( m_pfnGetAvailableVoltages )
  {
    LOGN("getting available voltages")
    float * arfVoltagesInVolt ;
    WORD wNum ;
    arfVoltagesInVolt = (*m_pfnGetAvailableVoltages) (
        0 ,
        & wNum
        ) ;
    LOGN("float array address allocated by DLL:" << arfVoltagesInVolt  << "num eles:"
      << wNum )
    //If alloc. by DLL succeeded.
    if( arfVoltagesInVolt )
    {
      for( WORD wIndex = 0 ; wIndex < wNum ; ++ wIndex )
      {
        r_stdset_float.insert(arfVoltagesInVolt[wIndex]) ;
      }
      LOGN("Before deleting the array that should have been allocated by the DLL")
      //Was dyn. allocated by the DLL.
      delete [] arfVoltagesInVolt ;
    }
    LOGN("After deleting the array that should have been allocated by the DLL")
  }
}

BYTE wxDynLibCPUcontroller::GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & r_fVoltageInVolt
    , BYTE byCoreID 
    )
  {
//    WORD wMilliVolt ;
//    DEBUGN("wxDynLibCPUcontroller::GetCurrentPstate(...) begin")
//    if( m_pfngetcurrentpstate )
    if( m_pfnGetCurrentVoltageAndFrequency )
    {
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      float fMultiplier ;
      //
//      float fReferenceClockInMHz ;
       BYTE byReturn =
//        (*m_pfngetcurrentpstate)(
//        & r_wFreqInMHz
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//        , & wMilliVolt
//#else
//        , r_fVoltageInVolt
//#endif
//        , byCoreID
//        ) ;
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//       r_fVoltageInVolt = (float) wMilliVolt / 1000.0 ;
//#endif
         (*m_pfnGetCurrentVoltageAndFrequency) (
         & r_fVoltageInVolt
         , & fMultiplier
//         , & fReferenceClockInMHz
         , & m_fReferenceClockInMHz
         , byCoreID
         ) ;
       r_wFreqInMHz = (WORD) ( fMultiplier * //fReferenceClockInMHz
           m_fReferenceClockInMHz ) ;
//       DEBUGN("wxDynLibCPUcontroller::GetCurrentPstate(...):"
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//           << " millivolt:" << wMilliVolt
//#endif
//           << " Volt:" << r_fVoltageInVolt  )
//      DEBUG_COUT( "wxDynLibCPUcontroller::GetCurrentPstate("
//        << r_wFreqInMHz << ","
//        << r_fVolt << ","
//        << (WORD) byCoreID
//        << "):" <<
//        r_wFreqInMHz << " " << r_fVolt << "\n" ) ;
       return byReturn ;
    }
    return 0 ; 
  }

BYTE wxDynLibCPUcontroller::GetCurrentVoltageAndFrequency(
  float & r_fVoltageInVolt
  , float & r_fMultiplier
  , float & r_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  if( m_pfnGetCurrentVoltageAndFrequency )
  {
     BYTE byReturn =
//        (*m_pfngetcurrentpstate)(
//        & r_wFreqInMHz
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//        , & wMilliVolt
//#else
//        , r_fVoltageInVolt
//#endif
//        , byCoreID
//        ) ;
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//       r_fVoltageInVolt = (float) wMilliVolt / 1000.0 ;
//#endif
       (*m_pfnGetCurrentVoltageAndFrequency) (
       & r_fVoltageInVolt
       , & r_fMultiplier
//         , & fReferenceClockInMHz
       , & r_fReferenceClockInMHz
       , wCoreID
       ) ;
     if( mp_model->m_bCollectPstatesAsDefault )
     {
       //Only collect for each multi, because the reference clock may vary,
       // and so an enormous number could rise.
       std::pair <std::map<float,VoltageAndFreq>::iterator, bool> pair_ =
         mp_model->m_cpucoredata.m_stdmap_fMultiplier2voltageandfreq.insert(
         std::pair<float,VoltageAndFreq>
           ( r_fMultiplier,
             VoltageAndFreq( r_fVoltageInVolt,
               //"(WORD)" to avoid g++ warning "passing `float' for converting 2
               //of `bool CPUcoreData::AddDefaultVoltageForFreq(float, WORD)' "
               (WORD) ( r_fReferenceClockInMHz * r_fMultiplier )
               )
           )
         ) ;
       if( mp_userinterface &&
         //http://www.cplusplus.com/reference/stl/map/insert/:
         //"The pair::second element in the pair is set to true if a new
         //element was inserted [...].
         pair_.second
         )
       {
         mp_model->m_cpucoredata.AddDefaultVoltageForFreq( r_fVoltageInVolt,
           //"(WORD)" to avoid g++ warning "passing `float' for converting 2
           //of `bool CPUcoreData::AddDefaultVoltageForFreq(float, WORD)' "
           (WORD) ( r_fReferenceClockInMHz * r_fMultiplier ) ) ;
         mp_userinterface->RedrawEverything() ;
       }
     }
     m_fReferenceClockInMHz = r_fReferenceClockInMHz ;
//       DEBUGN("wxDynLibCPUcontroller::GetCurrentPstate(...):"
//#ifdef GET_VOLTAGE_IN_MILLIVOLT
//           << " millivolt:" << wMilliVolt
//#endif
//           << " Volt:" << r_fVoltageInVolt  )
//      DEBUG_COUT( "wxDynLibCPUcontroller::GetCurrentPstate("
//        << r_wFreqInMHz << ","
//        << r_fVolt << ","
//        << (WORD) byCoreID
//        << "):" <<
//        r_wFreqInMHz << " " << r_fVolt << "\n" ) ;
     return byReturn ;
  }
  return 0 ;
}

WORD wxDynLibCPUcontroller::GetMaximumFrequencyInMHz()
{
  WORD wRet = 0 ;
//  WORD wNumber ;
  //wxDYNLIB_FUNCTION(dll_GetMaximumFrequencyInMHz_type, 
  //  GetMaximumFrequencyInMHz, m_wxdynamiclibraryCPUctl) ;
  //if( pfnGetMaximumFrequencyInMHz )
//  if( m_pfnGetMaximumFrequencyInMHz )
//  if( m_pfnGetAvailableMultipliers )
//  {
//    float * arfMultipliers = (*m_pfnGetAvailableMultipliers) (
//        //core ID
//        0
//        , & wNumber
//        ) ;
//    arfMultipliers
//    wRet = (*m_pfnGetMaximumFrequencyInMHz)(0) ;
//    DEBUG_COUT( "wxDynLibCPUcontroller::GetMaximumFrequencyInMHz():" <<
//      wRet << "\n" ) ;
//  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetMinimumFrequencyInMHz()
{
  WORD wRet = 0 ;
//  if( m_pfnGetMinimumFrequencyInMHz )
//  {
//    wRet = (*m_pfnGetMinimumFrequencyInMHz)(0) ;
//  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetMaximumVoltageID()
{
  WORD wRet = 0 ;
//  if( m_pfnGetMaximumVoltageID )
//  {
//    wRet = (*m_pfnGetMaximumVoltageID)() ;
//  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetMinimumVoltageID()
{
  WORD wRet = 0 ;
//  if( m_pfnGetMinimumVoltageID )
//  {
//    wRet = (*m_pfnGetMinimumVoltageID)() ;
//  }
  return wRet ;
}

WORD wxDynLibCPUcontroller::GetNumberOfCPUcores()
{
  m_wNumberOfLogicalCPUcores = 0 ;
  if( m_pfnGetNumberOfCPUcores )
  {
    m_wNumberOfLogicalCPUcores = (*m_pfnGetNumberOfCPUcores ) () ;
  }
  return m_wNumberOfLogicalCPUcores ;
}

float wxDynLibCPUcontroller::GetTemperatureInCelsius( WORD wCoreID )
{
  if( m_pfngettemperatureincelsius )
  {
    return ( * m_pfngettemperatureincelsius ) ( wCoreID ) ;
  }
  return 
#ifdef _MSC_VER
    FLT_MIN ;
#else
    std::numeric_limits<float>::min() ;
#endif
}

float wxDynLibCPUcontroller::GetVoltageInVolt(WORD wVoltageID )
{
//  if( m_pfn_GetVoltageInVolt )
//    return (*m_pfn_GetVoltageInVolt)(wVoltageID) ;
  return 0.0 ;
}

//Called after changing the p-state identifier in the GUI: get the voltage ID
//for the voltage for the p-state x
WORD wxDynLibCPUcontroller::GetVoltageID(float fVoltageInVolt )
{
//  if( m_pfn_GetVoltageID )
//    return (*m_pfn_GetVoltageID)(fVoltageInVolt) ;
  return 0 ;
}

void wxDynLibCPUcontroller::PrepareForNextPerformanceCounting(
   DWORD dwAffinityBitMask
   , BYTE byPerformanceEventSelectRegisterNumber
   )
{
  if( m_pfn_preparefornextperformancecounting )
  {
    ( * m_pfn_preparefornextperformancecounting )
        ( dwAffinityBitMask, byPerformanceEventSelectRegisterNumber ) ;
  }
}

BYTE wxDynLibCPUcontroller::
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Scaling"
    SetVoltageAndFrequency(
      float fVoltageInVolt
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
{
  DEBUGN("dyn lib CPU controller--address of DLL's set multi fct:"
    << m_pfnSetCurrentVoltageAndMultiplier
    << " reference clock:" << m_fReferenceClockInMHz )
//  DEBUGN("wxDynLibCPUcontroller::SetVoltageAndFrequency(...)")
//  if( m_pfnsetcurrentpstate )
  CPUcoreData & r_cpucoredata = mp_model->m_cpucoredata ;
  std::set<float> & r_stdset_fAvailableMultipliers =
      r_cpucoredata.m_stdset_floatAvailableMultipliers ;
  float * p_fAvailableMultipliers = r_cpucoredata.m_arfAvailableMultipliers ;
  if( m_pfnSetCurrentVoltageAndMultiplier && m_fReferenceClockInMHz
      && //! r_cpucoredata.m_stdset_floatAvailableMultipliers.empty()
      //! r_stdset_fAvailableMultipliers.empty()
      p_fAvailableMultipliers
      )
  {
//    WORD wMilliVolt = (WORD) (fVolt * 1000.0) ;
//    BYTE byLowerMultiplier ; //, byHigherMultiplier , byMultiplierToUse ;
//    DWORD dwLowmostBits , dwHighmostBits = 0 ;
//    float fHigherFreq , fLowerFreq , fLowerFreqDiff, fHigherFreqDiff ;
    //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA   since: 06_0FH (Mobile)
    //  "63:33 Reserved"

    //Get the multiplier for the frequency that is closest to the wanted
    // frequency.
    //If e.g. one wants to set 930 MHz: if current ref. clock is 133.3 MHz:
    // 930 / 133.3 ~= 6.99 -> rounded down to 6; 6*133.3 = 799.8.

    //TODO choose from multipliers from attribute data, use float data type
//    byLowerMultiplier =
//      //Freq / "FSB in MHz" = multiplier
//        wFreqInMHz / m_fReferenceClockInMHz ;

    float fCalculatedMultiplier = (float) wFreqInMHz / m_fReferenceClockInMHz ;
    DEBUGN("dyn lib CPU controller--calculated multiplier: " <<
      fCalculatedMultiplier )

//    std::set<float>::const_iterator c_iterGreaterOrEqual =
//        r_stdset_fAvailableMultipliers.lower_bound( fMultiplier ) ;
//    std::set<float>::const_iterator c_iterGreaterOrEqual =
//        r_stdset_fAvailableMultipliers.lower_bound( fMultiplier ) ;
//    if( c_iterGreaterOrEqual != r_stdset_fAvailableMultipliers.end() )
    WORD wArraySize = r_stdset_fAvailableMultipliers.size() ;
    float fMultiplierLessOrEqual = GetClosestLessOrEqual(
      p_fAvailableMultipliers ,
      wArraySize,
      fCalculatedMultiplier ) ;
    DEBUGN("dyn lib CPU controller-- < or = multiplier closest to calculated "
      "multiplier: " << fMultiplierLessOrEqual )
    float fMultiplierGreaterOrEqual = GetClosestGreaterOrEqual(
      p_fAvailableMultipliers ,
      wArraySize,
      fCalculatedMultiplier) ;
    DEBUGN("dyn lib CPU controller-- > or = multiplier closest to calculated "
       "multiplier: " << fMultiplierGreaterOrEqual )
    float fGreaterOrEqualDiff = fMultiplierGreaterOrEqual -
      fCalculatedMultiplier ;
    float fLessOrEqualDiff = fCalculatedMultiplier - fMultiplierLessOrEqual ;
    fCalculatedMultiplier = fGreaterOrEqualDiff < fLessOrEqualDiff ?
      fMultiplierGreaterOrEqual : fMultiplierLessOrEqual ;
    DEBUGN(" <= to calculated multiplier diff:" << fLessOrEqualDiff
      << " >= to calculated multiplier diff:" << fGreaterOrEqualDiff
      << "-> using multiplier " << fCalculatedMultiplier )
//    byHigherMultiplier = byLowerMultiplier + 1 ;
//    fLowerFreq = byLowerMultiplier * m_fReferenceClockInMHz ;
//    fHigherFreq = byHigherMultiplier * m_fReferenceClockInMHz ;
//    fLowerFreqDiff = wFreqInMHz - fLowerFreq ;
//    fHigherFreqDiff = fHigherFreq - wFreqInMHz ;
//    byMultiplierToUse = fLowerFreqDiff < fHigherFreqDiff ?
//      byLowerMultiplier : byHigherMultiplier ;

//    DEBUGN("wxDynLibCPUcontroller::SetVoltageAndFrequency(...) "
//      "before calling fct. at address " << m_pfnsetcurrentpstate <<
//      " with args (" << wFreqInMHz << "," << wMilliVolt << ","
//      << (WORD) byCoreID << ")")
    DEBUGN("before calling DLL's function SetCurrentVoltageAndMultiplier( "
      << fVoltageInVolt << "," << //(WORD) byMultiplierToUse
      fCalculatedMultiplier
      << ","
      << (WORD) byCoreID << ")" )
     BYTE by = //(*m_pfnsetcurrentpstate)(
//      wFreqInMHz
//      , wMilliVolt
//      , byCoreID
//      ) ;
       (*m_pfnSetCurrentVoltageAndMultiplier)(
          fVoltageInVolt
         //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
         , //byMultiplierToUse
         fCalculatedMultiplier
         , byCoreID ) ;
    DEBUGN("return value of DLL's function SetCurrentVoltageAndMultiplier( "
      << fVoltageInVolt << "," << //(WORD) byMultiplierToUse
      fCalculatedMultiplier
      << ","
      << (WORD) byCoreID << ") : " << (WORD) by )
//    DEBUGN("wxDynLibCPUcontroller::SetVoltageAndFrequency(...) res.:"
//     << (WORD) by )
    return by ;
  }
  return 0 ;
}

//BYTE wxDynLibCPUcontroller::SetFreqAndVoltageFromFreq(
//  WORD wFreqInMHz
//  , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
//  , BYTE byCoreID )
//{
//  BYTE byRet = 0 ;
//  float fVoltageInVolt ;
//  DEBUGN("wxDynLibCPUcontroller::SetFreqAndVoltageFromFreq(WORD, std::set, BYTE"
//      "address of model: " << mp_model )
//  if( m_pfn_set_pstate_from_freq )
//  {
//
//  }
//  return byRet ;
//}

BYTE wxDynLibCPUcontroller::TooHot() 
{ 
  BYTE by = 0 ;
  float fTemperatureInCelsius ;
  m_wNumberOfLogicalCPUcores = mp_model->m_cpucoredata.m_byNumberOfCPUCores ;
  PerCPUcoreAttributes * arp_percpucoreattributes =
      mp_model->m_cpucoredata.m_arp_percpucoreattributes ;
  for( WORD wCPUcoreIdx = 0 ; wCPUcoreIdx < m_wNumberOfLogicalCPUcores ; 
    ++ wCPUcoreIdx )
  {
    fTemperatureInCelsius = GetTemperatureInCelsius( wCPUcoreIdx ) ;
    arp_percpucoreattributes[wCPUcoreIdx].m_fTempInDegCelsius =
      fTemperatureInCelsius ;
    if( fTemperatureInCelsius > //90.0
      mp_model->m_cpucoredata.m_fThrottleTempInDegCelsius
      )
    {
      by = 1 ;
      DEBUGN("temperature of CPU core " << wCPUcoreIdx << ":"
        << fTemperatureInCelsius
        << "is > throttle temp:"
        << mp_model->m_cpucoredata.m_fThrottleTempInDegCelsius )
//      break ; //at least 1 core too hot->break loop.
    }
  }
  return by ;
}

BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI
  wxDynLibCPUcontroller::WrmsrEx(
    DWORD index,		// MSR index
    DWORD dwLow ,//eax,			// bit  0-31
    DWORD dwHigh, //edx,			// bit 32-63
    DWORD affinityMask	// Thread Affinity Mask
    )
{
  BOOL boolRet = FALSE ;
  if( m_pfn_write_msr )
  {
  }
  else
    //May be NULL (e.g. if CPUacces init failed).
    if( mp_cpuaccess )
      boolRet = mp_cpuaccess->WrmsrEx(
        index,		// MSR index
        dwLow ,//eax,			// bit  0-31
        dwHigh, //edx,			// bit 32-63
        affinityMask	// Thread Affinity Mask
        ) ;
  return boolRet ;
}
