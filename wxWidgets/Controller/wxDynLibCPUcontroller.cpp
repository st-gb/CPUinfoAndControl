#include "wxDynLibCPUcontroller.hpp"
#include <Controller/exported_functions.h> //for "::ReadMSR(...)"
//GetErrorMessageFromErrorCodeA(...)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/GetLastErrorCode.hpp>//OperatingSystem::GetLastErrorCode()
//#include "Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp"
#include <Controller/I_CPUaccess.hpp> //for passing to dyn libs "Init()"
#include <ModelData/ModelData.hpp> //class Model
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <UserInterface/UserInterface.hpp> //class UserInterface
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <Windows/ErrorCodeFromGetLastErrorToString.h>
  #include <Windows/DLLloadError.hpp>//DLLloadError::GetPossibleSolution(DWORD)
#endif //#ifdef _WIN32
//for GetStdString(wxString &)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

#include <wx/msgdlg.h> //for ::wxMessageBox(...)
#include <wx/stopwatch.h> //::wxGetLocalTimeMillis()
//GetClosestLessOrEqual(...), GetClosestGreaterOrEqual(...)
#include <algorithms/binary_search/binary_search.cpp>
#ifdef _MSC_VER
  #include <float.h> //FLT_MIN
#else
  #include <limits> //float>::min()
#endif

void wxDynLibCPUcontroller::GetCurrentReferenceClock()
{
  //Get the reference clock at first for being able to interpolate default
  //voltages.
  float fDummy;
  LOGN("Before calling wxDynLibCPUcontroller::GetCurrentVoltageAndFrequency "
    "for getting current reference clock.")
  GetCurrentVoltageAndFrequency( fDummy, fDummy, m_fReferenceClockInMHz, 0);
  LOGN("After calling wxDynLibCPUcontroller::GetCurrentVoltageAndFrequency "
    "for getting current reference clock:" << m_fReferenceClockInMHz
    << " MHz")
}

wxDynLibCPUcontroller::wxDynLibCPUcontroller(
  wxString & r_wxstrFilePath 
  , I_CPUaccess * p_cpuaccess 
  , UserInterface * p_userinterface
  , Model * p_model
  )
  :
//  m_fReferenceClockInMHz(0.0) ,
  mp_userinterface (p_userinterface)
  , m_wNumberOfLogicalCPUcores ( 1 )
{
  if( ! p_cpuaccess )
  {
    std::string std_str = "can't create CPU controller because the hardware/ "
      "CPU access not initialized/ the pointer is NULL";
    p_userinterface->Confirm(std_str);
    throw new CPUaccessException(std_str);
  }
  //TODO crashes here if the CPU access initialization failed.
//  mp_model = p_cpuaccess->mp_model ;
  mp_model = p_model ;
  //m_wxdynamiclibraryCPUctl ;

  //http://docs.wxwidgets.org/2.8.7/wx_wxdynamiclibrary.html
  // #wxdynamiclibraryload :
  //"Returns true if the library was successfully loaded, false otherwise."
  if( m_wxdynamiclibraryCPUctl.Load(r_wxstrFilePath) 
    )
  {
    //wxdynamiclibraryCPUctl.
    wxString wxstrFuncName (
      //Use wxT() to enable to compile with both unicode and ANSI.
        wxT("Init" ) ) ;
//    LOGN("Dyn Lib " << r_wxstrFilePath << " successfully loaded")
    if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName )
      )
    {
      LOGN("Dyn Lib symbol " <<
        //wxString may be wide char -> to ANSI string.
        GetStdString( wxstrFuncName ) << " exists")
//#ifdef _DEBUG
//      wxMessageBox( wxString::Format( "CPU access address: %x "
//        ", adress of ReadMSR fct:%x"
//        , p_cpuaccess
//        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
//        , & ::ReadMSR
//        ) ) ;
//#endif
      wxDYNLIB_FUNCTION(dll_init_type, Init, m_wxdynamiclibraryCPUctl) ;
      LOGN("Dyn Lib assigned fct ptr to symbol "
        //Convert to std::string, else g++ linker error:
        //"undefined reference to `operator<<(std::ostream&, wxString const&)'"
        << ::GetStdString( wxstrFuncName )
        << " address of function:" << (void *) pfnInit;
        )
      LOGN("dyn lib: p_cpuaccess: " << p_cpuaccess)
      LOGN("dyn lib: p_cpuaccess->model: " << p_cpuaccess->mp_model)

      LOGN("Dyn Lib before calling "
        //Convert to std::string, else g++ linker error:
        //"undefined reference to `operator<<(std::ostream&, wxString const&)'"
        << ::GetStdString( wxstrFuncName )
        )
//      //TODO
//      p_cpuaccess->mp_cpucontroller = NULL ;
      //void * wxdynamiclibraryCPUctl.GetSymbol(wxT("Init")) ;

      //IMPORTANT: the class "Model" should have the same structure as in the
      //dynamic (link) library, else the executable file (i.e. GUI or service)
      //may malfunction:
      //it happened that the executable ran into an endless loop
      //(e.g. because the instruction pointer pointed to wrong data or the data
      //pointer for an operator for an operation pointed to a wrong address)
      //after a member
      //was added to class "CPUcoreData" whose instance is a member of class
      //"ModelData" and only the executable (and not the dynamic library)
      //included this change.
      ( * pfnInit)( //wxGetApp().mp_i_cpuaccess
        //Pass pointer to I_CPUaccess in order for the DLL to be able to
        // access the model via "p_cpuaccess->model"
        p_cpuaccess
        //, & WinRing0_1_3RunTimeDynLinked::RdmsrEx
//        , & ::ReadMSR
//        )
        ) ;
      LOGN("Dyn Lib after calling "
        //Convert to std::string, else g++ linker error:
        //"undefined reference to `operator<<(std::ostream&, wxString const&)'"
        << ::GetStdString( wxstrFuncName )
        )
      LOGN("# of available default voltages:" << p_cpuaccess->mp_model->
        m_cpucoredata.m_stdsetvoltageandfreqDefault.size() )
      std::set<VoltageAndFreq> & r_stdset =  p_cpuaccess->mp_model->
        m_cpucoredata.m_stdsetvoltageandfreqDefault;
      std::set<VoltageAndFreq>::const_iterator c_iter = r_stdset.begin();
      WORD wIndex = 0 ;
      while( c_iter != r_stdset.end() )
      {
        LOGN("Default voltages #" << wIndex ++ << ":"
          << c_iter->m_fVoltageInVolt << " Volt for " << c_iter->m_wFreqInMHz
          << " MHz.")
        ++ c_iter;
      }
    }
//#endif
//      wxDYNLIB_FUNCTION(dll_init_type, Init, m_wxdynamiclibraryCPUctl) ;
//      LOGN("Dyn Lib assigned fct ptr to symbol " <<
//        //else g++: "undefined reference to `operator<<(std::ostream&,
//        //wxString const&)'"
//        GetStdString( wxstrFuncName) )
//      LOGN("Dyn Lib before calling " <<
//        //else g++: "undefined reference to `operator<<(std::ostream&,
//        //wxString const&)'"
//        GetStdString( wxstrFuncName ) )
      DEBUGN("dyn lib: p_cpuaccess: " << p_cpuaccess)

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
      LOGN("m_pfnGetAvailableMultipliers:" <<
        //see http://stackoverflow.com/questions/2064692/
        //  how-to-print-function-pointers-with-cout:
        // cast to (void*), else either 1 or 0 is outputted.
        (void*) m_pfnGetAvailableMultipliers )
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
      LOGN("m_pfnGetAvailableVoltages:" <<
        //see http://stackoverflow.com/questions/2064692/
        //  how-to-print-function-pointers-with-cout:
        // cast to (void*), else either 1 or 0 is outputted.
        (void*) m_pfnGetAvailableVoltages )
//      LOGN("Dyn Lib after GetMinimumFrequencyInMHz")

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
      LOGN("m_pfnGetCurrentVoltageAndFrequency before assigning fct ptrs:"
        << m_pfnGetCurrentVoltageAndFrequency )
      m_pfnGetCurrentVoltageAndFrequency = (
          pfn_GetCurrentVoltageAndFrequency_type )
        m_wxdynamiclibraryCPUctl.GetSymbol(
          wxT("GetCurrentVoltageAndFrequency") ) ;
      LOGN("m_pfnGetCurrentVoltageAndFrequency:" //<< std::ios::hex
        //see http://stackoverflow.com/questions/2064692/
        //  how-to-print-function-pointers-with-cout:
        // cast to (void*), else either 1 or 0 is outputted.
        << (void*) m_pfnGetCurrentVoltageAndFrequency )

      GetCurrentReferenceClock();

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

      wxstrFuncName = wxT("GetTemperatureInCelsius") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName) )
      {
#ifdef COMPILE_WITH_LOG
        std::string stdstringFunctionName = GetStdString(wxstrFuncName) ;
#endif
        bool bFunctionPointerSuccessfullyAssigned = false ;
        LOGN("The symbol \""
          << stdstringFunctionName
          << "\" exists in the dynamic library "
          //<< r_wxstrFilePath << "\""
          )
        m_pfngettemperatureincelsius = (dll_GetTemperatureInCelsius_type)
          m_wxdynamiclibraryCPUctl.GetSymbol( wxstrFuncName ,
            & bFunctionPointerSuccessfullyAssigned) ;
        LOGN("function pointer for function \"" << stdstringFunctionName
          << "\":" << (void *) m_pfngettemperatureincelsius )
        if( bFunctionPointerSuccessfullyAssigned )
          LOGN("successfully assigned pointer to function " <<
            stdstringFunctionName )
        else
          LOGN("Failed to assign pointer to function " <<
            GetStdString(wxstrFuncName) )
      }
      else
      {
        LOGN("The symbol \""
          << GetStdString(wxstrFuncName)
          << "\" does _not_ exist in the dynamic library "
          //<< r_wxstrFilePath << "\""
          )
        m_pfngettemperatureincelsius = NULL ;
      }
      wxstrFuncName = wxT("moreThan1CPUcorePowerPlane") ;
      if( m_wxdynamiclibraryCPUctl.HasSymbol( wxstrFuncName ) )
        m_b1CPUcorePowerPlane = false ;
      LOGN("Dyn Lib after moreThan1CPUcorePowerPlane")

//      DWORD dw = ::GetLastError() ;
//      std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
//      stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
//      //ERROR_PROC_NOT_FOUND
//      /*::wxMessageBox(
//        wxString::Format("function %s was not found within DLL",
//          wxstrFuncName) + wxString(stdstrErrMsg) ,
//        wxT("Error ")
//        ) ;*/

      //Because this may not be the 1st time a controller is attached, clear
      // previous multipliers, else the result is the intersection of the
      //current and the next multipliers.
      mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers.clear() ;
      LOGN("before DLL::GetAvailableMultipliers")
      GetAvailableMultipliers(
        mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers) ;
      mp_model->m_cpucoredata.AvailableMultipliersToArray() ;

//      LOGN("after DLL::GetAvailableMultipliers")
      LOGN("Available multipliers:")
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
    DWORD dw = OperatingSystem::GetLastErrorCode() ;
    std::string stdstrErrMsg = //EnglishMessageFromLastErrorCode() ;
      "loading the CPU controller dynamic library\""
      + GetStdString( r_wxstrFilePath )
      + "\" failed:"
      + GetErrorMessageFromErrorCodeA(dw) ;
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//    DWORD dw = ::GetLastError() ;
    //std::string stdstrErrMsg = ::LocalLanguageMessageFromErrorCodeA( dw) ;
//    std::string stdstrErrMsg = ::GetLastErrorMessageString(dw) ;
    stdstrErrMsg += DLLloadError::GetPossibleSolution( dw ) ;
#else //#ifdef _WIN32
#endif //#ifdef _WIN32
    LOGN( stdstrErrMsg )
    //::wxMessageBox( wxT("Error message: ") + wxString(stdstrErrMsg) ,
//    wxT("loading DLL failed") ) ;
    mp_userinterface->Confirm(stdstrErrMsg) ;
    throw CPUaccessException(stdstrErrMsg) ;
  }
}

wxDynLibCPUcontroller::~wxDynLibCPUcontroller()
{
  LOGN("Before Unloading the CPU controller dynamic library")
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
      LOGN("Before deleting the array that should have been allocated by the "
        "dynamic library")
      //Was dyn. allocated by the DLL.
      delete [] arf ;
      LOGN("After deleting the array that should have been allocated by the "
        "dynamic library")
    }
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

//Get closest multiplier because if a client wants an unsupported multiplier
// then this multi is not used.
//TODO also set closest voltage?!
inline BYTE wxDynLibCPUcontroller::GetClosestMultplierAndSetVoltageAndMultiplier(
  float fVoltageInVolt ,
  float fMultiplier ,
  WORD wCoreID
  )
{
  BYTE by = 0 ;
  LOGN("wxDynLibCPUcontroller::GetClosestMultplierAndSetVoltageAndMultiplier("
    "voltage in Volt:" << fVoltageInVolt
    << "multiplier:" << fMultiplier
    << "core ID:" << wCoreID )
  CPUcoreData & r_cpucoredata = mp_model->m_cpucoredata ;
  float * p_fAvailableMultipliers = r_cpucoredata.m_arfAvailableMultipliers ;
  if( m_pfnSetCurrentVoltageAndMultiplier
      && //! r_cpucoredata.m_stdset_floatAvailableMultipliers.empty()
      //! r_stdset_fAvailableMultipliers.empty()
      p_fAvailableMultipliers
      )
  {
    std::set<float> & r_stdset_fAvailableMultipliers =
        r_cpucoredata.m_stdset_floatAvailableMultipliers ;
    WORD wArraySize = r_stdset_fAvailableMultipliers.size() ;
    float fMultiplierLessOrEqual = GetClosestLessOrEqual(
      p_fAvailableMultipliers ,
      wArraySize,
      fMultiplier ) ;
    DEBUGN("dyn lib CPU controller-- < or = multiplier closest to calculated "
      "multiplier: " << fMultiplierLessOrEqual )
    float fMultiplierGreaterOrEqual = GetClosestGreaterOrEqual(
      p_fAvailableMultipliers ,
      wArraySize,
      fMultiplier) ;
    DEBUGN("dyn lib CPU controller-- > or = multiplier closest to calculated "
       "multiplier: " << fMultiplierGreaterOrEqual )
    float fGreaterOrEqualDiff = fMultiplierGreaterOrEqual -
      fMultiplier ;
    float fLessOrEqualDiff = fMultiplier - fMultiplierLessOrEqual ;
    fMultiplier = fGreaterOrEqualDiff < fLessOrEqualDiff ?
      fMultiplierGreaterOrEqual : fMultiplierLessOrEqual ;
    DEBUGN(" <= to calculated multiplier diff:" << fLessOrEqualDiff
      << " >= to calculated multiplier diff:" << fGreaterOrEqualDiff
      << "-> using multiplier " << fMultiplier )
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
//    DEBUGN(
    LOGN("before calling Dynamic Library's function "
      "\"SetCurrentVoltageAndMultiplier( "
      << fVoltageInVolt << "," << //(WORD) byMultiplierToUse
      fMultiplier
      << ","
      << wCoreID << ")\"" )
    //The CPU controller should determine the closest voltage.
    //Else not every voltage may be available to the CPU controller:
    // If e.g. for a Pentium M the voltage to set is 0.768 V and the Voltage ID
    //should be got then the Voltage ID was the one of 0.746 V (because of
    //false rounding?)
    //If the closest voltage is determined here then this would be redundant.
//    if( r_cpucoredata.m_arfAvailableVoltagesInVolt )
//    {
//      WORD wArrayIndex = GetArrayIndexForClosestValue(
//        r_cpucoredata.m_arfAvailableVoltagesInVolt,
//        r_cpucoredata.m_stdset_floatAvailableVoltagesInVolt.size() ,
//        fVoltageInVolt
//        ) ;
//      if( wArrayIndex != MAXWORD )
//        fVoltageInVolt = r_cpucoredata.m_arfAvailableVoltagesInVolt[
//          wArrayIndex ] ;
//    }
    by = ( * m_pfnSetCurrentVoltageAndMultiplier)(
      fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , //byMultiplierToUse
      fMultiplier
      , wCoreID
      ) ;
    DEBUGN("return value of DLL's function SetCurrentVoltageAndMultiplier( "
      << fVoltageInVolt << "," << //(WORD) byMultiplierToUse
      fMultiplier
      << ","
      << wCoreID << ") : " << (WORD) by )
  }
  else
    LOGN("wxDynLibCPUcontroller::GetClosestMultplierAndSetVoltageAnd"
      "Multiplier(...): function pointer for settting voltage and multiplier "
      "is NULL or no available multipliers")
  return by ;
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
       BYTE byReturn = ( * m_pfnGetCurrentVoltageAndFrequency) (
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
  LOGN("wxDynLibCPUcontroller::GetCurrentVoltageAndFrequency("
    << wCoreID << ") begin"
    "--m_pfnGetCurrentVoltageAndFrequency:" //<< std::ios::hex
    //see http://stackoverflow.com/questions/2064692/
    //  how-to-print-function-pointers-with-cout:
    // cast to (void*), else either 1 or 0 is outputted.
    << (void *) m_pfnGetCurrentVoltageAndFrequency )
  if( m_pfnGetCurrentVoltageAndFrequency )
  {
    LOGN("dyn lib CPU controller: GetCurrentVoltageAndFrequency")
     BYTE byReturn = ( * m_pfnGetCurrentVoltageAndFrequency) (
       & r_fVoltageInVolt
       , & r_fMultiplier
       , & r_fReferenceClockInMHz
       , wCoreID
       ) ;
//    BYTE byReturn = 1 ;
    LOGN("dyn lib CPU controller: after calling DLL's "
      "GetCurrentVoltageAndFrequency"
//#ifdef _DEBUG
      << " voltage: " << r_fVoltageInVolt
      << " multi: " << r_fMultiplier
      << " r_fReferenceClockInMHz:" << r_fReferenceClockInMHz
      << " core: " << wCoreID
//#endif
      )
     if( r_fReferenceClockInMHz )
     {
       if( mp_model->m_bCollectPstatesAsDefault )
       {
         LOGN("mp_model->m_bCollectPstatesAsDefault" )
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
     }
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
     LOGN("dyn lib CPU controller: GetCurrentVoltageAndFrequency end")
     return byReturn ;
  }
  LOGN("dyn lib CPU controller: GetCurrentVoltageAndFrequency end")
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
#ifdef COMPILE_WITH_LOG
    float fTemp = ( * m_pfngettemperatureincelsius ) ( wCoreID ) ;
    LOGN("DynLibCPUcontroller::GetTemperatureInCelsius--temperature for core "
      << wCoreID << ":" << fTemp )
    return fTemp ;
#else
    return ( * m_pfngettemperatureincelsius ) ( wCoreID ) ;
#endif
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

BYTE wxDynLibCPUcontroller::SetCurrentVoltageAndMultiplier(
  float fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float fMultiplier
  , WORD wCoreID
  )
{
  LOGN("wxDynLibCPUcontroller::SetCurrentVoltageAndMultiplier begin")
//  if( m_pfnSetCurrentVoltageAndMultiplier
////      && //! r_cpucoredata.m_stdset_floatAvailableMultipliers.empty()
////      //! r_stdset_fAvailableMultipliers.empty()
////      p_fAvailableMultipliers
//    )
//  {
  return GetClosestMultplierAndSetVoltageAndMultiplier(
      fVoltageInVolt ,
      fMultiplier ,
      wCoreID
      ) ;
//  }
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
  BYTE by = 0 ;
  DEBUGN("dyn lib CPU controller--address of DLL's set multi fct:"
    << m_pfnSetCurrentVoltageAndMultiplier
    << " reference clock:" << m_fReferenceClockInMHz )
//  DEBUGN("wxDynLibCPUcontroller::SetVoltageAndFrequency(...)")
//  if( m_pfnsetcurrentpstate )
  if( //m_pfnSetCurrentVoltageAndMultiplier &&
      m_fReferenceClockInMHz
//      && //! r_cpucoredata.m_stdset_floatAvailableMultipliers.empty()
//      //! r_stdset_fAvailableMultipliers.empty()
//      p_fAvailableMultipliers
    )
  {
//    WORD wMilliVolt = (WORD) (fVolt * 1000.0) ;
//    BYTE byLowerMultiplier ; //, byHigherMultiplier , byMultiplierToUse ;
//    DWORD dwLowmostBits , dwHighmostBits = 0 ;
//    float fHigherFreq , fLowerFreq , fLowerFreqDiff, fHigherFreqDiff ;

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

    GetClosestMultplierAndSetVoltageAndMultiplier(
      fVoltageInVolt ,
      fCalculatedMultiplier ,
      byCoreID
      ) ;
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

  //mp_model->m_cpucoredata.m_critical_section_typeAllData.
  //Lock for concurrent write when the throttle temp should be changed in
  //another thread.
  wxCriticalSectionLocker cs_locker(mp_model->m_cpucoredata.
      m_wxcriticalsectionIPCdata);
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
//      mp_model->m_cpucoredata.m_llLastTimeTooHot =
//        ::wxGetLocalTimeMillis().GetValue();
      m_llLastTimeTooHot = ::wxGetLocalTimeMillis().GetValue();
      DEBUGN("temperature of CPU core " << wCPUcoreIdx << ":"
        << fTemperatureInCelsius
        << "is > throttle temp:"
        << mp_model->m_cpucoredata.m_fThrottleTempInDegCelsius )
#ifndef _DEBUG
      break ; //at least 1 core too hot->break loop.
#endif
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
