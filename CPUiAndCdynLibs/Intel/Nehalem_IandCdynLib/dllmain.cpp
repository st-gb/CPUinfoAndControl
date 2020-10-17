/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//for CALLING_CONVENTION
#include <Controller/CPUcontrollerDynLib/calling_convention.h>
//For GetCurrentReferenceClock(...)
#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
#include <Controller/CPU-related/Intel/Intel_registers.h>
//A I_CPUaccess pointer is passed as parameter in Init(...)
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#include <preprocessor_macros/show_via_GUI.h> //SHOW_VIA_GUI(...)
#include <Controller/AssignPointersToExportedExeFunctions/AssignPointersToExportedExeMSRfunctions.h>

//#include <Aladdin_Enterprises_md5_imlementation/GenerateMD5checksum.hpp>

//Used by "Nehalem.hpp". The alternative would be:
// -in "Nehalem.hpp" that may be used in both I_CPUcontroller-derived class and
// this DLL source code would need to include the appropriate version of
// the 2 "inline_register_access_functions.hpp"
// -or: include <inline_register_access_functions.hpp> and set include path
// for DLL to "-I Windows/AssignPointersToExportedExeFunctions/\_
// inline_register_access_functions.hpp", for I_CPUcontgroller-derived to
// "-I Controller/.../_
// inline_register_access_functions.hpp"

//#include <Windows/AssignPointersToExportedExeFunctions/\_
//inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)

//For PrepareForNextPerformanceCountingNehalem(...) (etc.)
#include <Controller/CPU-related/Intel/Nehalem/Nehalem.hpp>
//Intel::IsHyperThreaded()
#include <Controller/CPU-related/Intel/HyperThreading.hpp>
#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
#ifdef _DEBUG
  #include <Controller/Logger/Logger.hpp> //class Logger
#endif
//#include <ModelData/ModelData.hpp>

//for BITMASK_FOR_LOWMOST_5BIT
#include <preprocessor_macros/bitmasks.h>
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
//#define _DEBUG
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>

#include <float.h> //for FLT_MIN
#include <sstream> //class std::stringstream
#include <tchar.h> //_T()
#include <windef.h> //for APIENTRY
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for MessageBox(...)
#endif
#ifdef _DEBUG
  //This logger variable _must_ have another name than in the executable this
  //dynamic library is attached to under _Linux_ . Else when this dyn lib is
  //unloaded the exe's logger destructor is called.
//  Logger g_loggerDynLib ;
#endif

//#define INSERT_DEFAULT_P_STATES
#ifdef INSERT_DEFAULT_P_STATES
  #define BUILD_WITH_ATTRIBUTES
  #include <ModelData/ModelData.hpp>
#endif

//defined in "/Windows/AssignPointersToExportedExeFunctions/
// AssignPointersToExportedExeMSRfunctions.cpp
ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;

//  extern float g_fReferenceClockInMHz ;
//Init to the default reference clock in MHz.
float g_fReferenceClockInMHz = 133.3 ;
float gs_fTimeStampCounterMultiplier = 0.0 ;
bool g_IsHyperThreaded = false;

#ifdef WESTMERE
  float lowestMultiplier = 9.0; //The minimum multiplier for Westmere is 9.
#else
  float lowestMultiplier = 7.0 ; //The minimum multiplier for Nehalem is 7.
#endif
#ifdef USE_ODCM
  #define ARRAY_INDEX_FOR_FIRST_MULTIPLIER NUMBER_OF_ODCM_VALUES
#else
  #define ARRAY_INDEX_FOR_FIRST_MULTIPLIER 0
#endif

//Use global vars instead of allocating them for each function call (->faster)
BYTE g_byValue1 , g_byValue2 ;
//Use uint32_t to ensure its 32 bit on either platform (32, 64bit)
uint32_t g_ui32Value1 , g_ui32Value2 ;

#include <preprocessor_macros/export_function_symbols.h> //EXPORT
#include <preprocessor_macros/dll_main_front_signature.h> //DLLMAIN_FRONT_SIGNATURE
#include "Nehalem_Windows.h" //InitWindows(...)

void InitOtherOSthanWindows()
{
  #ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = "" ;
    //GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("NehalemControllerDLL_log.txt") ;
//  g_loggerDynLib.OpenFile2( stdstrFilename ) ;
  g_logger.OpenFileA( stdstrFilename ) ;
  DEBUGN_LOGGER_NAME(//g_loggerDynLib,
    g_logger, "this Log file is open")
  //  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
  #endif //#ifdef _DEBUG
  AssignPointersToExportedExeMSRfunctions_inline(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  DEBUGN_LOGGER_NAME(//g_loggerDynLib,
    g_logger, "g_pfnreadmsr:" << g_pfnreadmsr
    << "g_pfn_write_msr:" << g_pfn_write_msr)
  gs_fTimeStampCounterMultiplier = Intel::Nehalem::GetTimeStampCounterMultiplier() ;
}

DLLMAIN_FRONT_SIGNATURE DllMain(
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved
#endif //#ifdef _WIN32
  )
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {
    return InitWindows() ;
  }
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
#else //#ifdef _WIN32
  InitOtherOSthanWindows() ;
#endif //#ifdef _WIN32
}
//#endif //#ifdef _WIN32

//Calling convention--must be the same as in the DLL
//function signature that calls this function?!
//WINAPI
//#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
#define NEHALEM_DLL_CALLING_CONVENTION

#include "../Intel/ODCM/ThrottleRatioFunctions.hpp"

EXPORT
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  NEHALEM_DLL_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetAvailableMultipliers(
  WORD wCoreID
  , WORD * p_wNumberOfArrayElements
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static fastestUnsignedDataType maximimMulti = 0;
//    BYTE byMaxMultiplier = 0 ;
//  DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-generated version has no problems
//#ifndef _MSC_VER
//    std::stringstream stdstrstream ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //<MaximumTurboRatioLimit1C startbit="0" bitlength="8"/>
//  //<MaximumTurboRatioLimit4C startbit="24" bitlength="8"/>
//  #endif
   //g_pi_cpuaccess->RdmsrEx(

  g_byValue1 = Intel::Nehalem::GetMaximumMultiplier(wCoreID, maximimMulti);
  DEBUGN("Dyn Lib--return value of Exe's ReadMSR:" << (WORD) g_byValue1)
  if( g_byValue1 ) //successfully read from MSR
  {
    DEBUGN("Dyn Lib--max multi:" << (WORD) maximimMulti)
#ifdef JUST1MULTIPLIER
    BYTE byNumMultis = 1;
#else
    BYTE byNumMultis = maximimMulti -
      //min. multi - 1
      6 ;
#ifdef USE_ODCM
    byNumMultis += NUMBER_OF_ODCM_VALUES; //for OCDM cycles
#endif
#endif
    float * ar_f = new float[byNumMultis] ;
    //If allocating the array on the heap succeeded.
    if( ar_f )
    {
      * p_wNumberOfArrayElements = byNumMultis ;
#ifdef USE_ODCM
      AddODCMdutyCyclesAsMultipliers(ar_f, lowestMultiplier);
#endif
      unsigned lowestMultiplier = Intel::Nehalem::GetLowestMultiplier();
      float fMulti = lowestMultiplier;
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
      for( BYTE by = ARRAY_INDEX_FOR_FIRST_MULTIPLIER; by < byNumMultis ; ++ by )
      {
        DEBUGN( FULL_FUNC_NAME << "--adding multplier " << fMulti )
        ar_f[by] = fMulti ++ ;
//       stdstrstream << fMulti << " " ;
      }
    }
//     MessageBox(NULL, stdstrstream.str().c_str(), "info", MB_OK) ;
#ifdef _DEBUG
  //if( wCoreID == 0 )
  //{
  // str.clear() ;
  //str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess
  //  << "multiplier:" << (WORD)byMaxMultiplier ;
  ////MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //}
#endif
    DEBUGN( FULL_FUNC_NAME << "--returning " << ar_f)
    return ar_f ;
  }
  * p_wNumberOfArrayElements = 0 ;
  DEBUGN( FULL_FUNC_NAME << "--returning NULL")
  return NULL ;
}

//For exporting this function with the same name as here in the source file.
EXPORT
/** Returns the _adjustable_ voltages.
 * The array pointed to by the return value must be freed by the caller (i.e.
x86I&C GUI or service) of this function. */
float *
  NEHALEM_DLL_CALLING_CONVENTION
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
//    BYTE byMaxMultiplier = 0 ;
//    DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-generated version has no problems
//#ifndef _MSC_VER
//    std::stringstream stdstrstream ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  #endif
   //g_pi_cpuaccess->RdmsrEx(
   BYTE byNumVoltages = 2 ;
   float * ar_f = /*new float[byNumVoltages]*/ NULL;
   //If allocating the array on the heap succeeded.
   if( ar_f )
   {
     ar_f[0] = 0.65 ;
     ar_f[1] = 0.9 ;
     *p_wNum = byNumVoltages ;
   }
   else
     *p_wNum = 0 ;
//     MessageBox(NULL, stdstrstream.str().c_str(), "info", MB_OK) ;
#ifdef _DEBUG
  //if( wCoreID == 0 )
  //{
  // str.clear() ;
  //str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess
  //  << "multiplier:" << (WORD)byMaxMultiplier ;
  ////MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //}
#endif
  DEBUGN( FULL_FUNC_NAME << "--returning " << ar_f )
  return ar_f ;
}

EXPORT
  BYTE
  //NEHALEM_DLL_CALLING_CONVENTION
//  _stdcall
//  _cdecl
//  CALLING_CONVENTION
  GetCurrentVoltageAndFrequency(
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    //
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
//  static float fReferenceClockInMHz = 0.0;
  static uint32_t ui32Value = 0;

//  SHOW_VIA_GUI( _T("GetCurrentVoltageAndFrequency begin") )

//  //Intel: 198H 408 IA32_PERF_STATUS
  g_byValue1 =
//    g_pi_cpuaccess->RdmsrEx(
    (* g_pfnreadmsr) (
    IA32_PERF_STATUS,
    & g_ui32Value1,// lowmost bit 0-31 (register "EAX")
    & ui32Value, //highmost bit 32-63
    1 << wCoreID //m_dwAffinityMask
    ) ;
//  SHOW_VIA_GUI( _T("GetCurrentVoltageAndFrequency after rdmsr") )

  if( g_byValue1 )
  {
    * p_fVoltageInVolt = 0.0f ;
#ifdef USE_ODCM
    float fODCMdutyCyclePercentage = GetODCMdutyCyclePercentage(wCoreID);
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
    * p_fMultiplier = ( g_ui32Value1 & 255 ) * fODCMdutyCyclePercentage;
//    if( fODCMdutyCyclePercentage)
#else //#ifdef USE_ODCM
    * p_fMultiplier = (float) g_ui32Value1;
#endif //#ifdef USE_ODCM
    DEBUGN(//"dyn lib GetCurrentVoltageAndFrequency"
      FULL_FUNC_NAME
      << "--multiplier:"
      << * p_fMultiplier
      << "--voltage:"
      << * p_fVoltageInVolt
      )
//#if !defined(_DEBUG) && defined(__linux_)
#ifdef STATIC_133MHZ_REFERENCE_CLOCK
    * p_fReferenceClockInMHz = 133.3 ;
#else
//    SHOW_VIA_GUI( _T("GetCurrentVoltageAndFrequency before "
//      "GetCurrentReferenceClock") )

    //This call sets g_fReferenceClockInMHz to the current reference clock.
    //This update of the value would be senseful for setting the CPU core
    //via "frequency" as parameter value the next time.
    GetCurrentReferenceClock(
      ////720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
//      12.0 ,
      gs_fTimeStampCounterMultiplier ,
//      * p_fReferenceClockInMHz ,
      * p_fReferenceClockInMHz,
      1000 , //min. timespan in ms
      10000 ) ;

    SHOW_VIA_GUI( _T("GetCurrentVoltageAndFrequency after "
      "GetCurrentReferenceClock") )
//    * p_fReferenceClockInMHz = fReferenceClockInMHz;
//    * p_fReferenceClockInMHz = 133.0;
#endif //#ifdef STATIC_133MHZ_REFERENCE_CLOCK
    std::stringstream std_strstream;
    std_strstream << "dyn lib GetCurrentVoltageAndFrequency--ref clock:"
        << * p_fReferenceClockInMHz;
    DEBUGN( std_strstream.str() )
    SHOW_VIA_GUI( std_strstream.str().c_str() )

    //time span for calculating the reference clock was too high or too low.
    if( * p_fReferenceClockInMHz == 0.0 )
      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
    else
      g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;

    DEBUGN("for core " << wCoreID << ": calculated reference clock in MHz: "
      << //g_fReferenceClockInMHz
      * p_fReferenceClockInMHz )
//      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
  }
  else
  {
    DEBUGN( FULL_FUNC_NAME << "--failed to read from MSR index "
      << IA32_PERF_STATUS)
  }
  SHOW_VIA_GUI( _T("GetCurrentVoltageAndFrequency end") )
  return g_byValue1 ;
}

//  extern "C" __declspec(dllexport)
//  WORD
//    //Calling convention--must be the same as in the DLL
//    //function signature that calls this function?!
//    NEHALEM_DLL_CALLING_CONVENTION
//    GetNumberOfCPUcores()
//  {
//    WORD wNumberOfCPUcores = 0 ;
//    wNumberOfCPUcores = GetNumLogicalCPUs() ;
//    return wNumberOfCPUcores ;
//  }

//EXPORT void GetMD5(unsigned char ar_ch [] )
//{
////  Aladdin_Enterprises::md5_imlementation::GenerateMD5checkSum(ar_ch;
//}

EXPORT
float
  NEHALEM_DLL_CALLING_CONVENTION
  //WINAPI
  GetTemperatureInCelsius ( WORD wCoreID
  )
{
  static uint32_t ui32Value = 0;
//    BYTE byDigitalReadout ;
//    BYTE byTempInDegCelsius ;
//    BYTE byTempTarget;
//    BYTE byResolutionInDegreesCelsius ;
////    DWORD dwLowmostBits ;
////    DWORD dwHighmostBits ;
//    (*g_pfnreadmsr) (
//       MSR_TEMPERATURE_TARGET , //Intel "B-12Vol. 3"
//       & g_ui32Value1, // bits 0-31 (register "EAX")
//       & g_dwValue2, // bits 32-63 (register "EDX")
//       //m_dwAffinityMask
//       1 << wCoreID
//       ) ;
//    //Intel B-76Vol. 3 : "23:16 Temperature Target. (R)"
//    // "The minimum temperature at which PROCHOT# will be asserted.
//    //  The value is degree C."
//    byTempTarget = ( g_ui32Value1 >> 16 ) & 255 ;

  g_byValue1 = (*g_pfnreadmsr) (
     IA32_THERM_STATUS , //Address: 1A2H
     & g_ui32Value1, // bits 0-31 (register "EAX")
     & //g_ui32Value2,
     ui32Value,
     //m_dwAffinityMask
     1 << wCoreID
     ) ;
  if( g_byValue1 )
  {
    //Intel: "22:16 Digital Readout (RO)"
    g_byValue1 = ( g_ui32Value1 >> 16 ) & BITMASK_FOR_LOWMOST_7BIT ;

//    //Intel: "30:27 Resolution in Degrees Celsius (RO)"
//    byResolutionInDegreesCelsius = ( g_ui32Value1 >> 27 ) &
//      BITMASK_FOR_LOWMOST_5BIT ;

    // TemperatureTarget - "Digital Readout"
//    byTempInDegCelsius = byTempTarget - g_byValue1 ;
    g_byValue2 = 100 - g_byValue1 ;
    return (float) g_byValue2 ;
  }
  return FLT_MIN ;
}

EXPORT void NEHALEM_DLL_CALLING_CONVENTION Init( //I_CPUcontroller * pi_cpu
  //CPUaccess object inside the exe.
  I_CPUaccess * pi_cpuaccess
//  void * p_v
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
//  InitOtherOSthanWindows() ;
//  mp_model->m_cpucoredata.m_MD5checksum;
#ifdef BUILD_WITH_ATTRIBUTES
//  unsigned numberOfModelDataBytes = //sizeof(*pi_cpuaccess->mp_model);
//    pi_cpuaccess->mp_model->GetSizeForCPUcontroller();
  //TODO call function "GenerateMD5checksum" within executable (as exported
  //function or as a member of class "I_CPUaccess".
  BYTE MD5checkSum[16];
//  Aladdin_Enterprises::md5_implementation::GenerateMD5checkSum(
//    pi_cpuaccess->mp_model, numberOfModelDataBytes, MD5checkSum );
//  I_CPUaccess::GetMD5checkSum( (void*) pi_cpuaccess->mp_model, numberOfModelDataBytes,
//    MD5checkSum);
  pi_cpuaccess->mp_model->GetCPUcontrollerModelMD5checkSum(MD5checkSum);
  int md5comparison = memcmp(
    pi_cpuaccess->mp_model->m_cpucoredata.m_MD5checksum, MD5checkSum, 16);
  memcpy( pi_cpuaccess->mp_model->m_cpucoredata.m_MD5checksum, MD5checkSum, 16);
  if( md5comparison != 0 )
    return;
#endif
#ifdef INSERT_DEFAULT_P_STATES
  /*BYTE*/ fastestUnsignedDataType byMaxMulti;
  float fVoltageInVolt = 0.65;
  float fFrequencyInMHz = lowestMultiplier * 133.0 ;

  DEBUGN("adding default voltage " << fVoltageInVolt << " V for "
    << (WORD) fFrequencyInMHz << " MHz" )
  //IMPORTANT: the Model class should have the same structure as in the
  //executable, else the executable may malfunction:
  //it happened that the executable ran into an endless loop after a member
  //was added to CPUcoreData class whose instance is a member of ModelData
  //and only the executable included this change.
  pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
    AddDefaultVoltageForFreq_inline( fVoltageInVolt,
      (WORD) ( fFrequencyInMHz )
      ) ;
  fVoltageInVolt = 1.1;
  g_byValue1 = Intel::Nehalem::GetMaximumMultiplier(0, byMaxMulti);
  fFrequencyInMHz = byMaxMulti * 133.3 ;
  DEBUGN("adding default voltage " << fVoltageInVolt << " for "
    << (WORD) fFrequencyInMHz << "MHz" )
  pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
    AddDefaultVoltageForFreq_inline( fVoltageInVolt,
      (WORD) ( fFrequencyInMHz )
      ) ;
#endif //INSERT_DEFAULT_P_STATES
  g_IsHyperThreaded = Intel::IsHyperThreaded();
}

//  extern "C" __declspec(dllexport)
//  void
//    Init( //I_CPUcontroller * pi_cpu
//    //CPUaccess object inside the exe.
////    I_CPUaccess * pi_cpuaccess
//    void * p_v
//    , ReadMSR_func_type pfnreadmsr
//    //BYTE by
//    )
//  {
//  //  g_pi_cpuaccess = pi_cpuaccess ;
//  #ifdef _DEBUG
//    //ReadMSR_func_type rdmsr = (ReadMSR_func_type) (void*) & pi_cpuaccess->RdmsrEx ;
//    std::stringstream stdstrstream ;
//    //For checking if the members are on the same RAM address between MSVC and MinGW:
//    stdstrstream << "DLL::Init(...)--\naddress of I_CPUaccess:" << & pi_cpuaccess << "\n"
//      << "address of I_CPUaccess::mp_model: " << & pi_cpuaccess->mp_model <<"\n"
//      << "address in I_CPUaccess::mp_model: " << pi_cpuaccess->mp_model <<"\n"
//      //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
//      << "address of I_CPUaccess::mp_cpu_controller: " << & pi_cpuaccess->mp_cpu_controller <<"\n"
//      << "address in I_CPUaccess::mp_cpu_controller: " << pi_cpuaccess->mp_cpu_controller ;
//    MessageBoxA( NULL, stdstrstream.str().c_str() , //TEXT("")
//      "", MB_OK) ;
//  #endif
//    //MSC-generated version has no problems
//  //#ifndef _MSC_VER
//  //  std::stringstream str ;
//  //  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
//  //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //#endif
//  }

EXPORT
WORD
  NEHALEM_DLL_CALLING_CONVENTION
  PrepareForNextPerformanceCounting(
   DWORD dwAffMask
   , BYTE byPerformanceEventSelectRegisterNumber
   )
{
  return Intel::Nehalem::PrepareForNextPerformanceCounting(
    dwAffMask,
    byPerformanceEventSelectRegisterNumber) ;
}

//For exporting this function with the same name as here in the source file.
EXPORT
  BYTE
  //NEHALEM_DLL_CALLING_CONVENTION
  //WINAPI
//    __stdcall
  SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  static const uint32_t ui32Zero = 0;
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << g_ui32Value1 ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
  DEBUGN("before calling Exe's WriteMSR function")
  if( fMultiplier < lowestMultiplier )
  {
#ifdef USE_ODCM
    //e.g. 6.125/7 = 0.875 = 87.5% ODCM duty cycle 0.875 * 8 = ODCM value "7"
    // for 87.5% ODCM duty cycle
    const float ODCMdutyCycle = fMultiplier / lowestMultiplier / 0.125f;
    SetODCMdutyCycle(ODCMdutyCycle, wCoreID);
#endif //#ifdef USE_ODCM
    g_byValue1 = WriteMSR(IA32_PERF_CTL, (BYTE) lowestMultiplier, ui32Zero,
        1 << wCoreID);
    DEBUGN("after calling Exe's WriteMSR function. return value: "
      << (WORD) g_byValue1 )
  }
  else
  {
#ifdef USE_ODCM
    DisableODCM(wCoreID);
#endif //#ifdef USE_ODCM
    g_byValue1 =
      //g_pi_cpuaccess->WrmsrEx(
      WriteMSR(
      //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
      // "15:0  Target performance State Value"
      //  "31:16  Reserved"
      //  "32  IDA Engage. (R/W) When set to 1: disengages IDA since:
      //     06_0FH (Mobile)
      //  "63:33 Reserved"
      IA32_PERF_CTL,
      (BYTE) fMultiplier , // bits 0-31 (register "EAX")
      //bits 0-31 (register "EDX")
      //g_ui32Value2,
      ui32Zero,
      //m_dwAffinityMask
      1 << wCoreID
      ) ;
  }
  DEBUGN("after calling Exe's WriteMSR function. return value: "
    << (WORD) g_byValue1 )
  return g_byValue1 ;
}
