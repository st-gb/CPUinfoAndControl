// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.

#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//#include <Controller\I_CPUcontroller.hpp>
#include <Controller/stdtstr.hpp> //GetStdString()
#include <Controller/I_CPUaccess.hpp>
#include <Controller/CPU-related/GetCurrentReferenceClock.h>
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_registers.h>
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_Controller.hpp>
#include <Controller/DynLibCPUcontroller.h>
#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
#include <Controller/ExportedExeFunctionsCPUaccess.hpp> //ReadMSR(...) etc.
//#include <ModelData/ModelData.hpp>
//#include <Windows/GetNumberOfLogicalCPUs.h>
#include <Windows/AssignPointersToExportedExeFunctions.h>
#ifdef _DEBUG
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
#include <preprocessor_helper_macros.h>
#include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION
#include <tchar.h> //_T()

extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;
extern float g_fReferenceClockInMHz ;

DWORD g_dwLowmostBits ;
DWORD g_dwHighmostBits ;
ULONGLONG g_ullPerformanceEventCounterNumberOfFIDchange ;

PentiumM_Controller g_pentium_m_controller ;
ExportedExeFunctionsCPUaccess g_exportedexefunctionscpuaccess ;

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
    std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
    std::string stdstrFilename = strExeFileNameWithoutDirs +
        ("PentiumM_DLL_log.txt") ;
    g_logger.OpenFile2( stdstrFilename ) ;
    DEBUGN("this Log file is open")
#endif
//	  MessageBox(NULL,"DLL_PROCESS_ATTACH","ii",MB_OK) ;
	  //Reaches here when compiled with MSC but not when MinGW?!
	  //for g_pfnreadmsr etc.
	  AssignPointersToExportedExeFunctions() ;
//	  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
    if( ! g_exportedexefunctionscpuaccess.AssignPointersToExportedExeFunctions()
      )
	  {
      MessageBox(NULL,
        _T("Pointers could not be assigned to the execu-tables export functions\n"
        "Does the executable that loads this DLL have ReadMSR and WriteMSR"
        "export functions at all?(analyze this with a tool)")
        //Title
        ,_T("error")
        , MB_OK) ;
	    return FALSE ;
	  }
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
    g_pentium_m_controller.SetCPUaccess( & g_exportedexefunctionscpuaccess ) ;
    //For monitoring the number of FreqID transitions etc.
    g_pentium_m_controller.Init() ;
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

////_declspec(dllexport)
////http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
////"When external names follow the C naming conventions, they must also be
////declared as extern "C" in C++ code, to prevent them from using C++ naming
////conventions."
//extern "C" __declspec(dllexport)
//void
//  //Calling convention--must be the same as in the DLL
//  //function signature that calls this function?!
//  //WINAPI
//  NEHALEM_DLL_CALLING_CONVENTION
//  Init( //I_CPUcontroller * pi_cpu
//  //CPUaccess object inside the exe.
//  I_CPUaccess * pi_cpuaccess
//  , ReadMSR_func_type pfnreadmsr
//  //BYTE by
//  )
//{
//  g_pi_cpuaccess = pi_cpuaccess ;
//#ifdef _DEBUG
//  //ReadMSR_func_type rdmsr = (ReadMSR_func_type) (void*) & pi_cpuaccess->RdmsrEx ;
//  std::stringstream stdstrstream ;
//  //For checking if the members are on the same RAM address between MSVC and MinGW:
//  stdstrstream << "DLL::Init(...)--\naddress of I_CPUaccess:" << & pi_cpuaccess << "\n"
//    << "address of I_CPUaccess::mp_model: " << & pi_cpuaccess->mp_model <<"\n"
//    << "address in I_CPUaccess::mp_model: " << pi_cpuaccess->mp_model <<"\n"
//    //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
//    << "address of I_CPUaccess::mp_cpu_controller: " << & pi_cpuaccess->mp_cpu_controller <<"\n"
//    << "address in I_CPUaccess::mp_cpu_controller: " << pi_cpuaccess->mp_cpu_controller ;
//  MessageBoxA( NULL, stdstrstream.str().c_str() , //TEXT("")
//    "", MB_OK) ;
//#endif
//  //g_pi_cpuaccess->mp_cpu_controller = & g_nehalemcontroller ;
//  AssignExeFunctionPointers() ;
//  //g_nehalemcontroller.SetCPUaccess( pi_cpuaccess ) ;
//  //g_pfnreadmsr = pfnreadmsr ;
//  //MSC-gerated version has no problems
////#ifndef _MSC_VER
////  std::stringstream str ;
////  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
////  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
////#endif
//  ////from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
//  ////("Calling an Exported Function in an EXE from Within a DLL"):
//  //g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
//  //  GetModuleHandle(NULL),
//  //  "ReadMSR");
//  //WORD w = GetMaximumFrequencyInMHz(0) ;
//  //pi_cpuaccess->mp_model->m_cpucoredata.
//  //  //Add to the set of default p-states and to the set of avail. p-states.
//  //  AddDefaultVoltageForFreq( 0.0 , w ) ;
//  //pi_cpuaccess->mp_model->m_cpucoredata.
//  //  //Add to the set of default p-states and to the set of avail. p-states.
//  //  AddDefaultVoltageForFreq( 0.0 ,931 ) ;
//  //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
//}

////_declspec(dllexport)
////http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
////"When external names follow the C naming conventions, they must also be
////declared as extern "C" in C++ code, to prevent them from using C++ naming
////conventions."
//extern "C" __declspec(dllexport)
//  DWORD
//  NEHALEM_DLL_CALLING_CONVENTION
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
  NEHALEM_DLL_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNum )
{
  BYTE byMaxMultiplier = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
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
  NEHALEM_DLL_CALLING_CONVENTION
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

//inline void NumberOfFreqIDchanges()
//{
//  if(
//    (*g_pfnreadmsr)(
//    //IA32_PERFEVTSEL0
//    //Intel vol. 3B:
//    //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
//    //address space; the number of MSRs per logical processor is reported using
//    //CPUID.0AH:EAX[15:8]."
//    //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
//    //The bit width of an IA32_PMCx MSR is reported using the
//    //CPUID.0AH:EAX[23:16]
//    //
//    IA32_PMC1
//    , g_dwLowmostBits
//    , g_dwHighmostBits
//    , 1
//    )
//  {
//  g_ullPerformanceEventCounterNumberOfFIDchange = dwHigh ;
//  g_ullPerformanceEventCounterNumberOfFIDchange <<= 32 ;
//  g_ullPerformanceEventCounterNumberOfFIDchange |= dwLow ;
//  //For taking a difference we need to have recorded a value before.
//  if( m_b2ndTimeOrLaterReadTSCandFIDchange )
//  {
//    m_ullNumberOfFIDchangeDiff = PERFORMANCE_COUNTER_VALUE_DIFF(
//      m_ullPerformanceEventCounterNumberOfFIDchange ,
//      m_ullNumberOfFIDchangePrevious
//      ) ;
//    LOGN("m_ullNumberOfFIDchangeDiff:" << m_ullNumberOfFIDchangeDiff)
//    //no FID change
//    if( m_ullNumberOfFIDchangeDiff == 0 &&
//        //else: <expr> / 0 = infinite
//        m_dwTickCountDiffInMilliseconds > 0 )
//    {
//      //e.g. time diff is "100 ms", TSC diff is "90.000.000":
//      // 90.000.000 * 1000 / 100 = 90.000.000.000 / 100 = 900.000.000(900MHz)
//      double dMHz = (double) m_ullTSCdiff * 1000.0 /// byFID
//        / (double) m_dwTickCountDiffInMilliseconds ;
//  //        LOGN("TSC diff:" << m_ullTSCdiff
//  //          << " milliseconds diff:" << m_dwTickCountDiffInMilliseconds
//  //          << " ->MHz=TSC diff * 1000 / ms_diff="
//  //            << (double) m_ullTSCdiff * 1000.0
//  //            << "/ " << (double) m_dwTickCountDiffInMilliseconds
//  //          << "\n=" << dMHz
//  //          << "FSB(MHz) = MHz/ multiplier=" << dMHz << "/" << (float)byFID
//  //          << "\n=" << dMHz / (float)byFID
//  //          )
//      r_wFreqInMHz = dMHz ;
//    }
//  }
//  else
//    m_b2ndTimeOrLaterReadTSCandFIDchange = true ;
//  m_ullNumberOfFIDchangePrevious =
//      m_ullPerformanceEventCounterNumberOfFIDchange ;
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
//  DWORD dwLowmostBits , dwHighmostBits ;
////  //Intel: 198H 408 IA32_PERF_STATUS
//  BYTE byRet =
////    g_pi_cpuaccess->RdmsrEx(
//    (*g_pfnreadmsr) (
//    IA32_PERF_STATUS,
//    & dwLowmostBits,// bit  0-31 (register "EAX")
//    & dwHighmostBits,
//    //m_dwAffinityMask
//    1 << wCoreID
//    ) ;
//  //Intel: "15:0 Current performance State Value"
//  //   "63:16 Reserved"
//  * p_fMultiplier = ( dwLowmostBits >> 8 ) & 255 ;
//
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
  return g_pentium_m_controller.GetCurrentVoltageAndFrequency(
    * p_fVoltageInVolt
    , * p_fMultiplier
    , * p_fReferenceClockInMHz
    ) ;
//  if( g_pentium_m_controller.m_fReferenceClockInMHz != 0.0 )
//  {
//    *p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
//  }
//  BYTE byVoltageID = ( dwLowmostBits & 255 ) ;
//  //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
//  //63= 0.7V 62=0.716 V,...
//  //40=1.340 Volt
//  *p_fVoltageInVolt = 0.7 + //( 63 - byVoltageID )
//    byVoltageID * 0.016 ;

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
//  NEHALEM_DLL_CALLING_CONVENTION
//  GetNumberOfCPUcores()
//{
//  return 1 ;
//}

inline BYTE GetVoltageID(float fVoltageInVolt )
{
  //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  float fVoltageID = fVoltageMinusLowestVoltage / 0.016 ;
  WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //round up because at 1.18 V the voltage ID in float is 29.999998
    //and would get 29 (1.164 V) when converting to an integer.
    //for 1.164 voltage ID in float is 29.000004
    //ceil( fVoltageID );
    //Avoid g++ warning "warning: converting to `WORD' from `float'"
    (WORD) fVoltageID ;
  if( fVoltageID - (float) wVoltageID >=
      //e.g. if desired voltage is 7.014: 7.014 - 7 = 0.014; 0.014/0.016=0.875
      0.5 )
    ++ wVoltageID ;
  return wVoltageID ;
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
  NEHALEM_DLL_CALLING_CONVENTION
  SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  DWORD dwLowmostBits ; //, dwHighmostBits = 0 ;
  //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
  // "15:0  Target performance State Value"
  //  "31:16  Reserved"
  //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA   since: 06_0FH (Mobile)
  //  "63:33 Reserved"
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << dwLowmostBits ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
  //byVoltageID: 40 = 1.34 V    27 = 1.1320000
  //float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
  //float fVoltageID = fVoltageMinusLowestVoltage / 0.016 ;
  BYTE byVoltageID = //(fVolt - 0.7 ) / 0.016 ;
    //round up because at 1.18 V the voltage ID in float is 29.999998
    //and would get 29 (1.164 V) when converting to an integer.
    //ceil( fVoltageID );
    GetVoltageID(fVoltageInVolt) ;
    //fVoltageInVolt ;
  BYTE byFreqID = (BYTE) fMultiplier ;
  dwLowmostBits = byFreqID << 8 ;
  dwLowmostBits |= byVoltageID ;
//  MessageBox
  BYTE byRet =
    //g_pi_cpuaccess->WrmsrEx(
    (*g_pfn_write_msr) (
    IA32_PERF_CTL,
    dwLowmostBits,// bit  0-31 (register "EAX")
    0,
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  return byRet ;
}

