/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif
#include <Controller/character_string/stdtstr.hpp> //GetStdString(...)
//A I_CPUaccess pointer is passed as parameter in Init(...)
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
//GetCurrentReferenceClock(...)
//#include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
//Include for PentiumM.hpp: //ReadMSR(...), WriteMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#ifdef INSERT_DEFAULT_P_STATES
  #include <ModelData/ModelData.hpp>
#endif //#ifdef INSERT_DEFAULT_P_STATES
//For the inline functions used in this source file.
#include <Controller/CPU-related/Intel/PentiumM/PentiumM.hpp>
// Intel::Pentium_M::GetAvailableMultipliers(...)
#include <Controller/CPU-related/Intel/PentiumM/Pentium_M_and_Core1.hpp>
#include <Controller/CPU-related/Intel/ProcHotDetection.hpp>
//for EXPORT, APIENTRY preprocessor macros
#include <Controller/CPUcontrollerDynLibs/function_specifiers.h>
#include <preprocessor_macros/export_function_symbols.h> //EXPORT macro

#include "Controller/CPU-related/Intel/PentiumM/SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent.hpp"

//#include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
//#include <Windows/GetNumberOfLogicalCPUs.h>
//For AssignPointersToExportedExeMSRfunctions(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
#ifdef _DEBUG
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
  #include <Windows/Process/GetDLLfileName.hpp> //GetDLLfileName(...)
  static HMODULE g_hModule;
#endif

#include <tchar.h> //_T()
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for MessageBox(...), MB_OK
#endif //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)

/*extern*/ ReadMSR_func_type g_pfnreadmsr ;
/*extern*/ WriteMSR_func_type g_pfn_write_msr ;
//extern float g_fReferenceClockInMHz ;

bool gs_b2ndTimeOrLaterReadTSCandFIDchange ;
uint32_t g_ui32LowmostBits ;
uint32_t g_ui32HighmostBits ;
//ULONGLONG g_ullPerformanceEventCounterNumberOfFIDchange ;

//PentiumM_Controller g_pentium_m_controller ;
//ExportedExeFunctionsCPUaccess g_exportedexefunctionscpuaccess ;

#ifdef _DEBUG
//Logger g_logger ;
//Logger g_loggerDynLib ;
#endif

#define DYN_LIB_CALLING_CONVENTION
//Pentium Ms are not hyperthreaded.
bool g_IsHyperThreaded = false;
#include <Controller/CPUcontrollerDynLibs/Intel/ODCM/ThrottleRatioFunctions.hpp>

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
bool InitWindows()
{
  #ifdef _DEBUG
#ifdef _WIN32 //win 32 or 64 bit
  std::tstring stdtstrDLLfileName  = GetDLLfileName(g_hModule);
#endif //#ifdef _WIN32
  std::string strCurrentProcessFileNameWithoutDirs //= GetStdString(
    = CurrentProcess::GetFileNameWithoutDirs_inline();
  std::string stdstrFilename = strCurrentProcessFileNameWithoutDirs +
    //("PentiumM_DLL_log.txt") ;
    stdtstrDLLfileName + "_log.txt";
  g_logger.OpenFileA( stdstrFilename ) ;
  g_logger.AddExcludeFromLogging(
    "BYTE ReadMSR(DWORD, uint32_t*, uint32_t*, DWORD_PTR)--index:412");
  DEBUGN("this Log file is open")
  //  DEBUGN("" << pi_cpuaccess->GetNumberOfCPUCores() )
  #endif
  //gp_nehalem_clocksnothaltedcpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
  //  ) ;
  //Reaches here when compiled with MSC but not when MinGW?!
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr , g_pfn_write_msr ) ;
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
  return true ;
}
#endif

EXPORT
BOOL APIENTRY DllMain(
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
    HMODULE hModule,
   DWORD  ul_reason_for_call,
   LPVOID lpReserved
#endif //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
   )
{
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	  //According to Dr. Pohl the DllMain while execution this function is
	  //blocked for other processes. So execute initializations in Init(...).
  {
#ifdef _DEBUG
      //see http://stackoverflow.com/questions/846044/how-to-get-the-filename-of-a-dll:
      g_hModule = hModule;
#endif //#ifdef _DEBUG
//    std::string strExeFileNameWithoutDirs = GetStdString(
//      GetExeFileNameWithoutDirs() ) ;
#endif
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
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  }
  break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	  break;
	case DLL_PROCESS_DETACH:
	  DEBUGN( FULL_FUNC_NAME << " DLL_PROCESS_DETACH")
	  //EndPROCHOTdetectionThread();
		break;
	}
#endif //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
	return TRUE;
}

//Calling convention--must be the same as in the DLL
//function signature that calls this function?!
//#define DLL_CALLING_CONVENTION __stdcall
#define DLL_CALLING_CONVENTION

EXPORT void DLL_CALLING_CONVENTION DeInit()
{
  DEBUGN( FULL_FUNC_NAME << " begin")
  EndPROCHOTdetectionThread();
}

EXPORT
void
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
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  InitWindows() ;
#else
  //for g_pfnreadmsr etc.
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr , g_pfn_write_msr ) ;
#endif //#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  Intel::Pentium_M::GetReferenceClockAccordingToStepping() ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
  SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent() ;
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
//#endif
//#ifdef _DEBUG
//  g_pi_cpuaccess = pi_cpuaccess ;
//  AssignPointersToExportedExeMSRfunctions(
//    g_pfnreadmsr , g_pfn_write_msr ) ;
  //Intel::Pentium_M::GetReferenceClockAccordingToStepping() ;
  std::string stdstrFilename = //strExeFileNameWithoutDirs +
    ("PentiumM_DLL_log.txt") ;
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
  StartPROCHOTdetectionThread();
#ifdef INSERT_DEFAULT_P_STATES
  Intel::Pentium_M_and_Core1::InsertDefaultVoltagePstates(pi_cpuaccess);
#endif
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
  DEBUGN( FULL_FUNC_NAME << " end")
}

//EXPORT
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

EXPORT
//The array pointed to by the return value must be freed by the caller of this function.
float *
  DLL_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  GetAvailableMultipliers(
    WORD wCoreID
    , WORD * p_wNum )
{
  return Intel::Pentium_M_and_Core1::GetAvailableMultipliers(
    //wCoreID,
    p_wNum );
}

#include <Controller\CPUcontrollerDynLibs/Intel/ODCM/ThrottleRatioFunctions.hpp>

EXPORT
//The array pointed to by the return value must be freed by the caller of this
//function.
float *
  DLL_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNum )
{
  BYTE byMaxVoltageID = 0 ;
  uint32_t ui32LowmostBits , ui32HighmostBits ;
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
    ( * g_pfnreadmsr) (
      //MIN_AND_MAX_FID ,
      //According to the MSR walker of CrystalCPUID:
      //for Pentium M reg. addr. 0x198:
      //Bit 16-24 showed hex "29" for a max. 1.356 V for 1.86 133 MHz FSB.
      //Bit 16-24 showed hex "26" for a max. 1.308 V for 1.6 133 MHz FSB.
      IA32_PERF_STATUS ,
      & ui32LowmostBits,// bit  0-31 (register "EAX")
      & ui32HighmostBits,
      //m_dwAffinityMask
      1 << wCoreID
      )
    )
  {
    byMaxVoltageID = ( ui32HighmostBits & 255 ) ;
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

EXPORT BYTE DLL_CALLING_CONVENTION GetCurrentVoltageAndFrequency(
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
  return //g_pentium_m_controller.GetCurrentVoltageAndFrequency(
    Intel::Pentium_M::GetCurrentVoltageAndFrequency(
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

/** "14.5.5.2  Reading the Digital Sensor"
* My tests showed:
* The only reliable test to check if throttle temperature is exceeded is to
* check bit "Thermal Status" (bit 0, RO) _many_ times per second.
* Reading 1x/sec is not sufficient (it seems this bit is only set for some
* micro-/milliseconds), better is ca. 100 times/sec.
* Neither "Thermal Status Log" (bit 1, R/WC0) nor perf monitoring is reliable
* because both can be changed via another program:
* -"Thermal Status Log" can be set to "1" via software, so a maldetection may
*  occur, also this bit can be set to "0", so it may not be detected. */
EXPORT float DLL_CALLING_CONVENTION GetTemperatureInCelsius (
  WORD wCoreID
  )
{
  DEBUGN(FULL_FUNC_NAME << " begin")
  if( g_vbProcHotIsCurrentlyActive )
  {
    DEBUGN(FULL_FUNC_NAME << " g_vbProcHotIsCurrentlyActive")
    g_vbProcHotIsCurrentlyActive = false;
    return //CPU_TEMP_IS_PROCHOT;
      CPU_TEMP_IS_CRITICAL;
  }
  return //CPU_TEMP_IS_BELOW_PROCHOT;
    CPU_TEMP_IS_BELOW_CRITICAL;
//    GetTemperatureInDegCelsiusPentiumM() ;
}

//EXPORT
//float GetCurrentReferenceClock()
//{
//
//}

//EXPORT
//WORD
//  //Calling convention--must be the same as in the DLL
//  //function signature that calls this function?!
//  DLL_CALLING_CONVENTION
//  GetNumberOfCPUcores()
//{
//  return 1 ;
//}

EXPORT
  BYTE
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
    Intel::Pentium_M::SetCurrentVoltageAndMultiplier(
      fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , fMultiplier
      , wCoreID
      ) ;
}
