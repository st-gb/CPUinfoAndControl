// dllmain.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.
//Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//#include <Controller\I_CPUcontroller.hpp>
//  #include <Controller/I_CPUaccess.hpp>
#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
#include <Controller/CPU-related/Intel/Intel_registers.h>
//Used by "Nehalem.hpp". The alternative would be:
// -in "Nehalem.hpp" that may be used in both I_CPUcontroller-derived class and
// this DLL source code would need to include the appropriate version of
// the 2 "inline_register_access_functions.hpp"
// -or: include <inline_register_access_functions.hpp> and set include path
// for DLL to "-I Windows/AssignPointersToExportedExeFunctions/\_
// inline_register_access_functions.hpp", for I_CPUcontgroller-derived to
// "-I Conroller/.../_
// inline_register_access_functions.hpp"
#include <Windows/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/CPU-related/Intel/Nehalem/Nehalem.hpp>
#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//  #include <Controller/DynLibCPUcontroller.h>
//  #include <Controller/CPU-related/Intel/Nehalem/NehalemController.hpp>
//#include <ModelData/ModelData.hpp>
//for BITMASK_FOR_LOWMOST_5BIT
#include <preprocessor_macros/bitmasks.h>
#include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
//#define _DEBUG
#include <Windows/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>

#ifdef _DEBUG
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
//  #include <Windows/GetNumberOfLogicalCPUs.h>

//  #include <global.h> //for DEBUGN(...)

#include <float.h> //for FLT_MIN
//#include <sstream> //std::stringstream
#include <tchar.h> //_T()
#include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION
//  #include <winuser.h> //::MessageBox(...)

#ifdef _DEBUG
  Logger g_logger ;
#endif

//defined in "/Windows/AssignPointersToExportedExeFunctions/
// AssignPointersToExportedExeMSRfunctions.cpp
extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;
//  extern float g_fReferenceClockInMHz ;
float g_fReferenceClockInMHz ;

//Use global vars instead of allocating them for each function call (->faster)
BYTE g_byValue1 , g_byValue2 ;
DWORD g_dwValue1 , g_dwValue2 ;

#define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000
//#include <Controller\Intel\Nehalem\NehalemClocksNotHaltedCPUcoreUsageGetter.hpp>
//
//Nehalem::ClocksNotHaltedCPUcoreUsageGetter * gp_nehalem_clocksnothaltedcpucoreusagegetter ;
//
//NehalemController g_nehalemcontroller ;
//Nehalem::ClocksNotHaltedCPUcoreUsageGetter
//  g_nehalem_clocksnothaltedcpucoreusagegetter( 0, g_nehalemcontroller ) ;

//I_CPUaccess * g_pi_cpuaccess ;
//Nehalem::ClocksNotHaltedCPUcoreUsageGetter g_clocksnothaltedcpucoreusagegetter ;

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
//For exporting this function with the same name as here in the source file.
//Especially for MinGW this line is needed in order to be called automatically
//for DLL attach / detach etc. actions.
extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
           )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {
#ifdef _DEBUG
std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
std::string stdstrFilename = strExeFileNameWithoutDirs +
    ("NehalemControllerDLL_log.txt") ;
g_logger.OpenFile2( stdstrFilename ) ;
DEBUGN("this Log file is open")
//  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
#endif
    //gp_nehalem_clocksnothaltedcpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
    //  ) ;
    //Reaches here when compiled with MSC but not when MinGW?!
    AssignPointersToExportedExeFunctions() ;
    if( ! g_pfnreadmsr || ! g_pfn_write_msr )
    {
      MessageBox( NULL,
        //_T() macro: ANSI-> "", unicode: ->L""; for Microsoft's compiler
        //each line needs a _T() macro.
        _T("Pointers could not be assigned to the execu-tables export functions\n")
        _T("Does the executable that loads this DLL have ReadMSR and WriteMSR")
        _T("export functions at all?(analyze this with a tool)")
        //Title
        ,_T("error")
        , MB_OK) ;
      return FALSE ;
    }
    //Force the cond. "< min. time diff" to become true.
    g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
    g_dwPreviousTickCountInMilliseconds
      //->time diff gets > max. time diff, so it calcs a ref clock.
      -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );
    //The reference clock is needed for setting the current frequency. So it
    //must be determined prior to any call of this function.
    GetCurrentReferenceClock(
      12.0,
      g_fReferenceClockInMHz ,
      1000 ,
      MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
    DEBUGN("first calculated reference clock in MHz: "
      << g_fReferenceClockInMHz )
  }
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

//#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
#define NEHALEM_DLL_CALLING_CONVENTION

//  //_declspec(dllexport)
//  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//  //"When external names follow the C naming conventions, they must also be
//  //declared as extern "C" in C++ code, to prevent them from using C++ naming
//  //conventions."
//  extern "C" __declspec(dllexport)
//  void
//    //Calling convention--must be the same as in the DLL
//    //function signature that calls this function?!
//    //WINAPI
//    NEHALEM_DLL_CALLING_CONVENTION
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
//    //g_pi_cpuaccess->mp_cpu_controller = & g_nehalemcontroller ;
//    AssignPointersToExportedExeFunctions() ;
//    //g_nehalemcontroller.SetCPUaccess( pi_cpuaccess ) ;
//    //g_pfnreadmsr = pfnreadmsr ;
//    //MSC-gerated version has no problems
//  //#ifndef _MSC_VER
//  //  std::stringstream str ;
//  //  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
//  //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //#endif
//    ////from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
//    ////("Calling an Exported Function in an EXE from Within a DLL"):
//    //g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
//    //  GetModuleHandle(NULL),
//    //  "ReadMSR");
//    //WORD w = GetMaximumFrequencyInMHz(0) ;
//    //pi_cpuaccess->mp_model->m_cpucoredata.
//    //  //Add to the set of default p-states and to the set of avail. p-states.
//    //  AddDefaultVoltageForFreq( 0.0 , w ) ;
//    //pi_cpuaccess->mp_model->m_cpucoredata.
//    //  //Add to the set of default p-states and to the set of avail. p-states.
//    //  AddDefaultVoltageForFreq( 0.0 ,931 ) ;
//    //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
//  }

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  //The reference clock might change, also during runtime.
  //This is why it is a good idea to get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNumberOfArrayElements )
{
//    BYTE byMaxMultiplier = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
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
  g_byValue1 = (*g_pfnreadmsr) (
    MSR_TURBO_RATIO_LIMIT ,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits,
    1 << wCoreID //m_dwAffinityMask
    ) ;
  if( g_byValue1 ) //successfully read from MSR
  {
    //max.  multiplier
    g_byValue1 = (BYTE) ( dwLowmostBits & 255 ) ;
    BYTE byNumMultis = g_byValue1 -
      //min. multi - 1
      6 ;
    float * ar_f = new float[byNumMultis] ;
    //If allocating the array on the heap succeeded.
    if( ar_f )
    {
      *p_wNumberOfArrayElements = byNumMultis ;
      float fMulti = 7.0 ; //The minimum multiplier for Nehalem is 7.
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
      for( BYTE by = 0 ; by < byNumMultis ; ++ by )
      {
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
    return ar_f ;
  }
  * p_wNumberOfArrayElements = 0 ;
  return NULL ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
//For exporting this function with the same name as here in the source file.
extern "C" __declspec(dllexport)
//The array pointed to by the return value must be freed by the caller (i.e.
//x86I&C GUI or service) of this function.
float *
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
//    BYTE byMaxMultiplier = 0 ;
//    DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-gerated version has no problems
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
   BYTE byNumVoltages = 2 ;
   float * ar_f = new float[byNumVoltages] ;
   //If allocating the array on the heap succeeded.
   if( ar_f )
   {
     ar_f[0] = 0.0 ;
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
  return ar_f ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
  BYTE
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
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
//  //Intel: 198H 408 IA32_PERF_STATUS
  g_byValue1 =
//    g_pi_cpuaccess->RdmsrEx(
    (*g_pfnreadmsr) (
    IA32_PERF_STATUS,
    & g_dwValue1,// lowmost bit 0-31 (register "EAX")
    & g_dwValue2, //highmost bit 32-63
    1 << wCoreID //m_dwAffinityMask
    ) ;
  if( g_byValue1 )
  {
    * p_fVoltageInVolt = 0 ;
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
    * p_fMultiplier = ( g_dwValue1 & 255 ) ;
    //This call sets g_fReferenceClockInMHz to the current reference clock.
    //This update of the value would be senseful for setting the CPU core
    //via "frequency" as parameter value the next time.
    GetCurrentReferenceClock(
      ////720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
      12.0 ,
      * p_fReferenceClockInMHz ,
      1000 , //min. timespan in ms
      10000 ) ;
    //Timespan too high or too low.
    if( * p_fReferenceClockInMHz == 0.0 )
      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
    else
      g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;
    DEBUGN("for core " << wCoreID << ": calculated reference clock in MHz: "
      << //g_fReferenceClockInMHz
      * p_fReferenceClockInMHz )
//      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
  }
  return g_byValue1 ;
}

//  //_declspec(dllexport)
//  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//  //"When external names follow the C naming conventions, they must also be
//  //declared as extern "C" in C++ code, to prevent them from using C++ naming
//  //conventions."
//  extern "C" __declspec(dllexport)
//  WORD
//    //Calling convention--must be the same as in the DLL
//    //function signature that calls this function?!
//    NEHALEM_DLL_CALLING_CONVENTION
//    GetNumberOfCPUcores()
//  {
//  //  DWORD dwReturnLength = 0 ;
//    WORD wNumberOfCPUcores = 0 ;
//  //  PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p_slpi = NULL ;
//    //NtquerySystemInfor(SYSTEM_BASCI_INFO)
//    //BOOL WINAPI
//    //get the number of bytes needed.
//    //if( ! GetLogicalProcessorInformation(
//    //  p_slpi //__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
//    //  , & dwReturnLength //__inout  PDWORD ReturnLength
//    //  )
//    //  && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER
//    //{
//    //  p_slpi = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) new BYTE[dwReturnLength ];
//    //  if( GetLogicalProcessorInformation(
//    //    p_slpi //__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
//    //    , & dwReturnLength //__inout  PDWORD ReturnLength
//    //      )
//    //    )
//    //  {
//    //    wNumberOfCPUcores = p_slpi->ProcessorMask ;
//    //    delete []
//    //  }
//    //}
//    wNumberOfCPUcores = GetNumLogicalCPUs() ;
//    return wNumberOfCPUcores ;
//  }

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
float
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  //WINAPI
  GetTemperatureInCelsius ( WORD wCoreID
  )
{
//    BYTE byDigitalReadout ;
//    BYTE byTempInDegCelsius ;
//    BYTE byTempTarget;
//    BYTE byResolutionInDegreesCelsius ;
////    DWORD dwLowmostBits ;
////    DWORD dwHighmostBits ;
//    (*g_pfnreadmsr) (
//       MSR_TEMPERATURE_TARGET , //Intel "B-12Vol. 3"
//       & g_dwValue1, // bits 0-31 (register "EAX")
//       & g_dwValue2, // bits 32-63 (register "EDX")
//       //m_dwAffinityMask
//       1 << wCoreID
//       ) ;
//    //Intel B-76Vol. 3 : "23:16 Temperature Target. (R)"
//    // "The minimum temperature at which PROCHOT# will be asserted.
//    //  The value is degree C."
//    byTempTarget = ( g_dwValue1 >> 16 ) & 255 ;

  g_byValue1 = (*g_pfnreadmsr) (
     IA32_THERM_STATUS , //Address: 1A2H
     & g_dwValue1, // bits 0-31 (register "EAX")
     & g_dwValue2,
     //m_dwAffinityMask
     1 << wCoreID
     ) ;
  if( g_byValue1 )
  {
    //Intel: "22:16 Digital Readout (RO)"
    g_byValue1 = ( g_dwValue1 >> 16 ) & BITMASK_FOR_LOWMOST_7BIT ;

//    //Intel: "30:27 Resolution in Degrees Celsius (RO)"
//    byResolutionInDegreesCelsius = ( g_dwValue1 >> 27 ) &
//      BITMASK_FOR_LOWMOST_5BIT ;

    // TemperatureTarget - "Digital Readout"
//    byTempInDegCelsius = byTempTarget - g_byValue1 ;
    g_byValue2 = 100 - g_byValue1 ;
    return (float) g_byValue2 ;
  }
  return FLT_MIN ;
}

//_declspec(dllexport)
//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  PrepareForNextPerformanceCounting(
   DWORD dwAffMask
   , BYTE byPerformanceEventSelectRegisterNumber
   )
{
  return PrepareForNextPerformanceCountingNehalem(
    dwAffMask,
    byPerformanceEventSelectRegisterNumber) ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
//For exporting this function with the same name as here in the source file.
extern "C" __declspec(dllexport)
  BYTE
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
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
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << g_dwValue1 ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
  DEBUGN("before calling Exe's WriteMSR function")
  g_byValue1 =
    //g_pi_cpuaccess->WrmsrEx(
    (*g_pfn_write_msr) (
    //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W) When set to 1: disengages IDA since:
    //     06_0FH (Mobile)
    //  "63:33 Reserved"
    IA32_PERF_CTL,
    (BYTE) fMultiplier , // bits 0-31 (register "EAX")
    g_dwValue2, //bits 0-31 (register "EDX")
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  DEBUGN("after calling Exe's WriteMSR function. return value: "
    << (WORD) g_byValue1 )
  return g_byValue1 ;
}

//  //_declspec(dllexport)
//    void InitCPUcoreUsageGet()
//  {
//  }
//
//  float
//    //Calling convention--must be the same as in the DLL
//    //function signature that calls this function?!
//    NEHALEM_DLL_CALLING_CONVENTION
//    GetCPUcoreUsage( WORD wCPUcoreID )
//  {
//    //float fCPUusage = g_clocksnothaltedcpucoreusagegetter.
//    //  GetPercentalUsageForCore( wCPUcoreID ) ;
//    //return //100 ;
//    //  (BYTE) ( fCPUusage * 100 ) ;
//    return -1.0 ;
//  }
