// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.

#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

#include <global.h> //DEBUGN()
//#include <Controller\I_CPUcontroller.hpp>
#include <Controller/AMD/Griffin/GriffinController.hpp>
#include <Controller/Logger.hpp> //class Logger
#include <Controller/DynLibCPUcontroller.h>
#include <Controller/ExportedExeFunctionsCPUaccess.hpp>
#include <Controller/I_CPUaccess.hpp>
#include <ModelData/ModelData.hpp>
//#include <Windows/GetNumberOfLogicalCPUs.h>
#include <preprocessor_helper_macros.h>
#include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION
#include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>

ExportedExeFunctionsCPUaccess g_exportedexefunctionscpuaccess ;
GriffinController g_griffincontroller ;
#ifdef _DEBUG
Logger g_logger ;
#endif

//Nehalem::ClocksNotHaltedCPUcoreUsageGetter 
//  g_nehalem_clocksnothaltedcpucoreusagegetter( 0, g_nehalemcontroller ) ;
typedef BOOL (//WINAPI 
  * ReadMSR_func_type)
   (
    DWORD dwIndex,		// MSR index
    PDWORD p_dweax,			// bit  0-31
    PDWORD p_dwedx,			// bit 32-63
    DWORD_PTR affinityMask	// Thread Affinity Mask
  ) ;
typedef BOOL (//WINAPI 
  * WriteMSR_func_type)
   (
    DWORD dwIndex,		// MSR index
    DWORD dwEAX,			// bit  0-31
    DWORD dwEDX,			// bit 32-63
    DWORD_PTR affinityMask	// Thread Affinity Mask
  ) ;

I_CPUaccess * g_pi_cpuaccess ;
ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;

void AssignExeFunctionPointers()
{
  //from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
  //("Calling an Exported Function in an EXE from Within a DLL"):
  g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
    GetModuleHandle(NULL),
    "ReadMSR");
  g_pfn_write_msr = (WriteMSR_func_type)::GetProcAddress(
    GetModuleHandle(NULL),
    "WriteMSR");
}

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved
  )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
      //Reaches here when compiled with MSC but not when MinGW?!
      AssignExeFunctionPointers() ;
      if( ! g_pfnreadmsr || ! g_pfn_write_msr )
        return FALSE ;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
            break;
  }
//#ifdef _DEBUG
//  MessageBox( NULL, "DllMain", TEXT("") , MB_OK) ;
//#endif
  return TRUE;
}

//#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
#define NEHALEM_DLL_CALLING_CONVENTION 

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
  GetMaximumFrequencyInMHz(WORD wCoreID) ;

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
  NEHALEM_DLL_CALLING_CONVENTION
  Init( //I_CPUcontroller * pi_cpu
  I_CPUaccess * pi_cpuaccess 
  , ReadMSR_func_type pfnreadmsr 
  //BYTE by 
  )
{
#ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = GetExeFileNameWithoutDirs() ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
      ("GriffinControllerDLL_log.txt") ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
//  LPSTR lpstrModuleName ;
//  CHAR ar_strModuleName[100] ;
//  DWORD dwChars =
//     GetModuleFileName(
//     //HINSTANCE
//     //NULL
//     GetModuleHandle(NULL)
////     ,NULL //LPSTR
//     , ar_strModuleName
//     ,99 //DWORD
//     ) ;
  DEBUGN("chars for module name needed:" //<< dwChars //<< ar_strModuleName
      << strExeFileNameWithoutDirs )
//  LPSTR = new STR[dwChars] ;
//  DEBUGN()
#endif
  g_pi_cpuaccess = pi_cpuaccess ;
  g_griffincontroller.mp_model = pi_cpuaccess->mp_model ;
#ifdef _DEBUG
//  std::ostringstream ostr ;
//  ostr << "address of model: " << pi_cpuaccess->mp_model ;
//  ostr << " address of cpucoredata: " << & pi_cpuaccess->mp_model->m_cpucoredata ;
  //MessageBox is problematic when starting with a service? -> OK can't be
  //pressed.
//  MessageBox( NULL, ostr.str().c_str() , "Init" , MB_OK) ;
//  DEBUGN( "Init(): " << ostr.str())
#endif
  AssignExeFunctionPointers() ;
  //g_pfnreadmsr = pfnreadmsr ;
  //MSC-gerated version has no problems
//#ifndef _MSC_VER
//  std::stringstream str ;
//  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//#endif
  ////from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
  ////("Calling an Exported Function in an EXE from Within a DLL"):
  //g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
  //  GetModuleHandle(NULL),
  //  "ReadMSR");
  //WORD w = GetMaximumFrequencyInMHz(0) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 , w ) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 ,931 ) ;
  //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
  g_exportedexefunctionscpuaccess.m_pfn_read_msr = //g_pfnreadmsr ;
    (ReadMSR_func_type)::GetProcAddress(
    GetModuleHandle(NULL),
    "ReadMSR");
  g_exportedexefunctionscpuaccess.m_pfn_write_msr = //g_pfn_write_msr ;
    (WriteMSR_func_type)::GetProcAddress(
    GetModuleHandle(NULL),
    "WriteMSR");
  DEBUGN("Init() pi_cpuaccess:" << pi_cpuaccess )
  g_griffincontroller.SetCPUaccess(
    //& g_exportedexefunctionscpuaccess
    pi_cpuaccess
    ) ;
  g_griffincontroller.FillAvailableCPUcoreFrequenciesList() ;
  //LOGN() ;
//#ifdef _DEBUG
//  //MessageBox is problematic when starting with a service? -> OK can't be
//  //pressed.
//  MessageBox( NULL, "after set CPU access",TEXT("") , MB_OK) ;
//#endif
  //g_clocksnothaltedcpucoreusagegetter.Init(0, wNumLogicalCPUcores ) ;
}

//_declspec(dllexport)
//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
  DWORD 
  NEHALEM_DLL_CALLING_CONVENTION 
  GetMultiplier(WORD wCoreID )
{
  DWORD dwLowmostBits , dwHighmostBits ;

  return dwLowmostBits ;
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
  GetCurrentPstate(
    PWORD p_wFreqInMHz 
    , //float & Volt
      PWORD p_wMilliVolt
    , WORD wCoreID 
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  DWORD dwLowmostBits , dwHighmostBits ;
  float fVoltageInVolt ;
  BYTE byRet = 
    g_griffincontroller.GetCurrentPstate(
    * p_wFreqInMHz ,
    fVoltageInVolt,
    wCoreID ) ;
  *p_wMilliVolt = (WORD) (fVoltageInVolt * 1000.0 ) ;
  return byRet ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION 
  GetMaximumFrequencyInMHz(WORD wCoreID)
{
//#ifdef _DEBUG
//  MessageBox( NULL, "GetMaximumFrequencyInMHz", TEXT("") , MB_OK ) ;
//#endif
  WORD wMaximumFrequencyInMHz =
    g_griffincontroller.GetMaximumFrequencyInMHz() ;
  return wMaximumFrequencyInMHz ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION 
  GetMinimumFrequencyInMHz(WORD wCoreID)
{
  WORD wMinimumFrequencyInMHz =
    g_griffincontroller.GetMinimumFrequencyInMHz() ;
  return wMinimumFrequencyInMHz ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  GetMaximumVoltageID()
{
  WORD wMaximumFrequencyInMHz =
    g_griffincontroller.GetMaximumVoltageID() ;
  return wMaximumFrequencyInMHz ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION
  GetMinimumVoltageID()
{
  WORD wMinimumFrequencyInMHz =
    g_griffincontroller.GetMinimumVoltageID() ;
  return wMinimumFrequencyInMHz ;
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
  GetNumberOfCPUcores()
{
  WORD wNumberOfCPUcores = 0 ;

  wNumberOfCPUcores = //GetNumLogicalCPUs() ;
    g_griffincontroller.GetNumberOfCPUCores() ;
  return wNumberOfCPUcores ;
}

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
  float fTempInDegCelsius ;
  g_griffincontroller.GetCurrentTempInDegCelsius(fTempInDegCelsius) ;
  return fTempInDegCelsius ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
WORD GetVoltageID(float fVoltageInVolt )
{
  WORD wVoltageID = g_griffincontroller.GetVoltageID(fVoltageInVolt) ;
  return wVoltageID ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
float GetVoltageInVolt(WORD wVoltageID )
{
  return g_griffincontroller.GetVoltageInVolt(wVoltageID) ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
//When a function with this name exists in the DLL the dyn volt n freq scal thread
//scales per core.
void moreThan1CPUcorePowerPlane()
{
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
  BYTE byRet = 0 ;
  g_griffincontroller.PrepareForNextPerformanceCounting(
    dwAffMask, byPerformanceEventSelectRegisterNumber  ) ;
  return byRet ;
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
  SetCurrentPstate(
    WORD wFreqInMHz 
    , //float & Volt
      WORD wMilliVolt
    , WORD wCoreID 
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  float fVoltageInVolt = (float) wMilliVolt / 1000.0 ;
  DEBUGN("SetCurrentPstate("
      << wFreqInMHz
      << "," << wMilliVolt
      << "," << wCoreID )
  BYTE byRet = 
    g_griffincontroller.SetVoltageAndFrequency(
      fVoltageInVolt,
      wFreqInMHz ,
      wCoreID
    ) ;
  return byRet ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be
//declared as extern "C" in C++ code, to prevent them from using C++ naming
//conventions."
extern "C" __declspec(dllexport)
void SetPstateFromFreq (
    WORD wFreqInMHz
    , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
    , BYTE byCoreID )
{

}

//_declspec(dllexport) 
  void InitCPUcoreUsageGet()
{
}

float 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  NEHALEM_DLL_CALLING_CONVENTION 
  GetCPUcoreUsage( WORD wCPUcoreID )
{
  //float fCPUusage = g_clocksnothaltedcpucoreusagegetter.
  //  GetPercentalUsageForCore( wCPUcoreID ) ;
  //return //100 ;
  //  (BYTE) ( fCPUusage * 100 ) ;
  return -1.0 ;
}
