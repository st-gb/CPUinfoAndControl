// dllmain.cpp : Definiert den Einstiegspunkt f�r die DLL-Anwendung.

#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//#include <Controller\I_CPUcontroller.hpp>
#include <Controller/character_string/stdtstr.hpp> //GetStdString(...)
#include <Controller/I_CPUaccess.hpp> //passed as parameter in Init(...)
#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
#include <Controller/CPU-related/Intel/Intel_registers.h>
//include for PentiumM.hpp: //ReadMSR(...), WriteMSR(...)
#include <Windows/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#include <Controller/CPU-related/Intel/PentiumM/PentiumM.hpp>
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_registers.h>
//#include <Controller/CPU-related/Intel/PentiumM/PentiumM_Controller.hpp>

#include <Windows/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)

//#include <Controller/DynLibCPUcontroller.h>
//#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//#include <Controller/ExportedExeFunctionsCPUaccess.hpp> //ReadMSR(...) etc.
#include <ModelData/ModelData.hpp>
//#include <Windows/GetNumberOfLogicalCPUs.h>
#include <Windows/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#ifdef _DEBUG
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
#include <preprocessor_macros/preprocessor_helper_macros.h>

#include <tchar.h> //_T()
#include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION

extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;
extern float g_fReferenceClockInMHz ;

DWORD g_dwLowmostBits ;
DWORD g_dwHighmostBits ;
ULONGLONG g_ullPerformanceEventCounterNumberOfFIDchange ;

//PentiumM_Controller g_pentium_m_controller ;
//ExportedExeFunctionsCPUaccess g_exportedexefunctionscpuaccess ;

#ifdef _DEBUG
Logger g_logger ;
#endif

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
    std::string strExeFileNameWithoutDirs = GetStdString(
      GetExeFileNameWithoutDirs() ) ;
    std::string stdstrFilename = strExeFileNameWithoutDirs +
        ("PentiumM_DLL_log.txt") ;
    g_logger.OpenFile2( stdstrFilename ) ;
    DEBUGN("this Log file is open")
#endif
//	  MessageBox(NULL,"DLL_PROCESS_ATTACH","ii",MB_OK) ;
	  //for g_pfnreadmsr etc.
	  AssignPointersToExportedExeFunctions() ;
	  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
//    if( ! g_exportedexefunctionscpuaccess.AssignPointersToExportedExeFunctions()
//      )
	  {
      ::MessageBox(
        NULL,
        _T("Pointers could not be assigned to the execu-tables export functions\n"
        "Does the executable that loads this DLL have ReadMSR and WriteMSR"
        "export functions at all?(analyze this with a tool)")
        //Title
        ,_T("error")
        , MB_OK) ;
	    return FALSE ;
	  }
    GetReferenceClockAccordingToStepping() ;
//	  //force the cond. "< min. time diff" to become true.
//	  g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
//	  g_dwPreviousTickCountInMilliseconds -= 100 ; //->time diff > max. time diff
//    //The reference clock is needed for setting the current frequency. So it
//    //must be determined prior to any call of this function.
//    GetCurrentReferenceClock(12.0, 10 ,
//      //set max. to be not too high because for Pentium M the TSC changes with
//      //the current multiplier, so it gets more inexact the longer the
//      //timespan?!
//      10 ) ;
//    g_pentium_m_controller.SetCPUaccess( & g_exportedexefunctionscpuaccess ) ;
//    //For monitoring the number of FreqID transitions etc.
//    g_pentium_m_controller.Init() ;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//#define DLL_CALLING_CONVENTION __stdcall
#define DLL_CALLING_CONVENTION

//_declspec(dllexport)
//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
void
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  //WINAPI
  DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
  //CPUaccess object inside the exe.
  I_CPUaccess * pi_cpuaccess
//  Trie *
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
//  g_pi_cpuaccess = pi_cpuaccess ;
#ifdef _DEBUG
  //ReadMSR_func_type rdmsr = (ReadMSR_func_type) (void*) & pi_cpuaccess->RdmsrEx ;
  std::stringstream stdstrstream ;
  //For checking if the members are on the same RAM address between MSVC and MinGW:
  stdstrstream << "DLL::Init(...)--\naddress of I_CPUaccess:" << & pi_cpuaccess << "\n"
    << "address of I_CPUaccess::mp_model: " << & pi_cpuaccess->mp_model <<"\n"
    << "address in I_CPUaccess::mp_model: " << pi_cpuaccess->mp_model <<"\n"
    //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
    << "address of I_CPUaccess::mp_cpu_controller: " <<
      & pi_cpuaccess->mp_cpu_controller <<"\n"
    << "address in I_CPUaccess::mp_cpu_controller: " <<
      pi_cpuaccess->mp_cpu_controller ;
  MessageBoxA( NULL, stdstrstream.str().c_str() , //TEXT("")
    "", MB_OK) ;
#endif

  float fVoltageForLowestMulti ;
  float fLowestMulti ;
  float fVoltageForHighestMulti ;
  float fHighestMulti ;

  if( GetDefaultPstates(
      fVoltageForLowestMulti,
      fLowestMulti,
      fVoltageForHighestMulti,
      fHighestMulti
      )
    )
  {
    DEBUGN("adding default voltage " << fVoltageForLowestMulti << " for "
      << (WORD) fLowestMulti * g_fReferenceClockInMHz << "MHz" )
    pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
      AddDefaultVoltageForFreq( fVoltageForLowestMulti,
        (WORD) ( fLowestMulti * g_fReferenceClockInMHz )
        ) ;
    DEBUGN("adding default voltage " << fVoltageForHighestMulti << " for "
      << (WORD) fHighestMulti * g_fReferenceClockInMHz << "MHz" )
    pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
      AddDefaultVoltageForFreq( fVoltageForHighestMulti,
        (WORD) ( fHighestMulti * g_fReferenceClockInMHz)
        ) ;
  }

//  AssignExeFunctionPointers() ;
  //g_nehalemcontroller.SetCPUaccess( pi_cpuaccess ) ;
  //MSC-generated version has no problems
//#ifndef _MSC_VER
//  std::stringstream str ;
//  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//#endif
  //WORD w = GetMaximumFrequencyInMHz(0) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 , w ) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 ,931 ) ;
  //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
}

////_declspec(dllexport)
////http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
////"When external names follow the C naming conventions, they must also be
////declared as extern "C" in C++ code, to prevent them from using C++ naming
////conventions."
//extern "C" __declspec(dllexport)
//  DWORD
//  DLL_CALLING_CONVENTION
//  GetMultiplier(WORD wCoreID )
//{
//  DWORD dwLowmostBits , dwHighmostBits ;
////  g_pi_cpuaccess->RdmsrEx(
//  (*g_pfnreadmsr) (
//    IA32_PERF_STATUS,
//    & dwLowmostBits,// bit  0-31 (register "EAX")
//    & dwHighmostBits,
//    //m_dwAffinityMask
//    1 << wCoreID
//    ) ;
//  return dwLowmostBits ;
//}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
//The array pointed to by the return value must be freed by the caller of this function.
float *
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  DLL_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNum )
{
  BYTE byMaxMultiplier = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
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
  (*g_pfnreadmsr) (
    //MIN_AND_MAX_FID ,
    //According to the MSR walker of CrystalCPUID:
    //for Pentium M reg. addr. 0x198:
    //Bit 24-32 showed hex "0E" for a max. multipl. "14" for 1.86 133 MHz FSB.
    //Bit 24-32 showed hex "0C" for a max. multipl. "12" for 1.6 133 MHz FSB.
    IA32_PERF_STATUS ,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits,
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
   byMaxMultiplier = //(BYTE) ( dwLowmostBits & 255 ) ;
     ( dwHighmostBits >> 8 ) & 255 ;
   //According to the MSR walker of CrystalCPUID the min. multi is at the
   //highmost byte: was "06" for every tested CPU (1.6GHz FSB133,
   // 1.86 GHz FSB 133, 1.8GHz FSB 100)
   BYTE byMinMultiplier = (BYTE) ( dwHighmostBits >> 24 ) & 255 ;
   BYTE byNumMultis = byMaxMultiplier - byMinMultiplier
       //add "+1" : if min and max identical, the array size must be "1"
       + 1 ;
   float * ar_f = new float[byNumMultis] ;
   //Allocation of storage succeeded.
   if( ar_f )
   {
     *p_wNum = byNumMultis ;
     float fMulti = byMinMultiplier ;
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
     for( BYTE by = 0 ; by < byNumMultis ; ++ by )
     {
       ar_f[by] = fMulti ;
//       stdstrstream << fMulti << " " ;
       ++ fMulti ;
     }
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
//The array pointed to by the return value must be freed by the caller of this function.
float *
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  DLL_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
  BYTE byMaxVoltageID = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
//  #ifdef _DEBUG
  //MSC-gerated version has no problems
//#ifndef _MSC_VER
//    std::stringstream stdstrstream ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  #endif
  if(
  (*g_pfnreadmsr) (
    //MIN_AND_MAX_FID ,
    //According to the MSR walker of CrystalCPUID:
    //for Pentium M reg. addr. 0x198:
    //Bit 16-24 showed hex "29" for a max. 1.356 V for 1.86 133 MHz FSB.
    //Bit 16-24 showed hex "26" for a max. 1.308 V for 1.6 133 MHz FSB.
    IA32_PERF_STATUS ,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits,
    //m_dwAffinityMask
    1 << wCoreID
    )
    )
  {
    byMaxVoltageID = ( dwHighmostBits & 255 ) ;
     BYTE byNumVoltages = //(1.340 - 0.7 / 0.016) = 0.64 / 0.016 = 40
         //if e.g. only 1 voltage: 0.7-0.7/0.016 = 0, so add "1"
         byMaxVoltageID
         + 1 ;
         //41 ;
     float * ar_f = new float[byNumVoltages] ;
     if( ar_f ) //Allocation of storage succeeded.
     {
       for( BYTE by = 0 ; by < byNumVoltages ; ++ by )
       {
         ar_f[by] = //Do not: "+ 0.016" for each iteration because this could
             //cause rounding errors.
             0.7 + 0.016 * by ;
  //       stdstrstream << fMulti << " " ;
       }
       *p_wNum = byNumVoltages ;
     }
     else
       *p_wNum = 0 ;
     return ar_f ;
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
  return NULL ;
}

//_declspec(dllexport)
//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
  BYTE 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  DLL_CALLING_CONVENTION
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
//  //This call sets g_fReferenceClockInMHz to the current reference clock.
//  //This update of the value would be senseful for setting the CPU core
//  //via "frequency" as parameter value the next time.
//  GetCurrentReferenceClock(
//    //For Pentium M the TimeStampCounter has the same frequency as the CPU core
//    //clock-> must divide by the _current_ multiplier.
//    * p_fMultiplier
//    , 200 //min timespan in ms for diffs
//    //If set too high, this may happen (example):
//    //  1st TSC value reading when TSC freq was 600 MHz: read "10.000.000"
//    //  shortly before 2nd reading the TSC value the TSC freq is 1800 MHz
//    //  2nd time value reading after 10 ms is "16.500.000"
//    // then taking the diff: 16.500.000 - 10.000.000 =
//    //  6.500.000 -> ~ the TSC for multiplier "6", but the current multiplier
//    // is 18, so 6.500.000 / 18 = 361,111.1 , 361,111.1 * 100 = 36,111,111.1
//    //, what would be only 36.1 MHz !!
//    , 1500 //max. time diff
//    ) ;
  return //g_pentium_m_controller.GetCurrentVoltageAndFrequency(
    GetCurrentVoltageAndFrequencyPentium_M(
    * p_fVoltageInVolt
    , * p_fMultiplier
    , * p_fReferenceClockInMHz
    ) ;
//  if( g_pentium_m_controller.m_fReferenceClockInMHz != 0.0 )
//  {
//    *p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
//  }
//  return byRet ;
}

////http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
////"When external names follow the C naming conventions, they must also be
////declared as extern "C" in C++ code, to prevent them from using C++ naming
////conventions."
//extern "C" __declspec(dllexport)
//float GetCurrentReferenceClock()
//{
//
//}

////_declspec(dllexport)
////http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
////"When external names follow the C naming conventions, they must also be
////declared as extern "C" in C++ code, to prevent them from using C++ naming
////conventions."
//extern "C" __declspec(dllexport)
//WORD
//  //Calling convention--must be the same as in the DLL
//  //function signature that calls this function?!
//  DLL_CALLING_CONVENTION
//  GetNumberOfCPUcores()
//{
//  return 1 ;
//}

//_declspec(dllexport)
//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
  BYTE
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  DLL_CALLING_CONVENTION
  SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  return //byRet ;
    SetCurrentVoltageAndMultiplierPentiumM(
      fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , fMultiplier
      , wCoreID
      ) ;
}