// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.

#ifdef _MSC_VER //MS compiler
#include "stdafx.h"
#endif

//#include <Controller\I_CPUcontroller.hpp>
#include <Controller\I_CPUaccess.hpp>
#include <Controller\Intel_registers.h>
#include <Controller/DynLibCPUcontroller.h>
//#include <Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.hpp> 
//#include <ModelData/ModelData.hpp>
#include <Windows/GetNumberOfLogicalCPUs.h>
#include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION

//#include <Controller\Intel\Nehalem\NehalemClocksNotHaltedCPUcoreUsageGetter.hpp>
//
//Nehalem::ClocksNotHaltedCPUcoreUsageGetter * gp_nehalem_clocksnothaltedcpucoreusagegetter ;
//
//NehalemController g_nehalemcontroller ;
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
//Nehalem::ClocksNotHaltedCPUcoreUsageGetter g_clocksnothaltedcpucoreusagegetter ;
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

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
    //gp_nehalem_clocksnothaltedcpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
    //  ) ;
	  //Reaches here when compiled with MSC but not when MinGW?!
	  AssignExeFunctionPointers() ;
	  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
	    return FALSE ;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
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
  g_pi_cpuaccess = pi_cpuaccess ;
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
  //g_pi_cpuaccess->RdmsrEx(
  (*g_pfnreadmsr) (
    IA32_PERF_STATUS,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits, 
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
      
  //if(pfnreadmsr)
  //  (*pfnreadmsr) (
  //    IA32_PERF_STATUS,
  //    & dwLowmostBits,// bit  0-31 (register "EAX")
  //    & dwHighmostBits, 
  //    //m_dwAffinityMask
  //    1 << wCoreID
  //    ) ;
  //else
  //  MessageBox(NULL,TEXT("It Did Not work :("),TEXT("From DLL"),MB_OK);

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
  //Intel: 198H 408 IA32_PERF_STATUS
  BYTE byRet = 
    //g_pi_cpuaccess->RdmsrEx(
    (*g_pfnreadmsr) (
    IA32_PERF_STATUS,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits, 
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  //Intel: "15:0 Current performance State Value"
  //   "63:16 Reserved"
  *p_wFreqInMHz = (BYTE) ( dwLowmostBits & 255 ) * 133 ;
  *p_wMilliVolt = 0 ;
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
  BYTE byMaxMultiplier = 0 ;
  DWORD dwLowmostBits , dwHighmostBits ;
//#ifdef _DEBUG
  //MSC-gerated version has no problems
//#ifndef _MSC_VER
//  std::stringstream str ;
//  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
//  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
//  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
//  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
//  //<MaximumTurboRatioLimit1C startbit="0" bitlength="8"/>
//  //<MaximumTurboRatioLimit4C startbit="24" bitlength="8"/>
//#endif
   //g_pi_cpuaccess->RdmsrEx( 
  (*g_pfnreadmsr) (
     0x1AD , // name="MSR_TURBO_RATIO_LIMIT"
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits, 
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
   byMaxMultiplier = (BYTE) ( dwLowmostBits & 255 ) ;
#ifdef _DEBUG
  if( wCoreID == 0 )
  {
   str.clear() ;
  str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess 
    << "multiplier:" << (WORD)byMaxMultiplier ;
  //MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  }
#endif
  return (WORD) ( byMaxMultiplier * 
    //FrontSideBus in MHz
    133 ) ;
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
  return 931 ;
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
//  DWORD dwReturnLength = 0 ;
  WORD wNumberOfCPUcores = 0 ;
//  PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p_slpi = NULL ;
  //NtquerySystemInfor(SYSTEM_BASCI_INFO)
  //BOOL WINAPI 
  //get the number of bytes needed.
  //if( ! GetLogicalProcessorInformation(
  //  p_slpi //__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
  //  , & dwReturnLength //__inout  PDWORD ReturnLength
  //  )
  //  && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER
  //{
  //  p_slpi = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) new BYTE[dwReturnLength ];
  //  if( GetLogicalProcessorInformation(
  //    p_slpi //__out    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
  //    , & dwReturnLength //__inout  PDWORD ReturnLength
  //      ) 
  //    )
  //  {
  //    wNumberOfCPUcores = p_slpi->ProcessorMask ;
  //    delete [] 
  //  }
  //}
  wNumberOfCPUcores = GetNumLogicalCPUs() ;
  return wNumberOfCPUcores ;
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
  SetCurrentPstate(
    WORD wFreqInMHz 
    , //float & Volt
      WORD wMilliVolt
    , WORD wCoreID 
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  DWORD dwLowmostBits , dwHighmostBits = 0 ;
  //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
  // "15:0  Target performance State Value"
  //  "31:16  Reserved"
  //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA   since: 06_0FH (Mobile)
  //  "63:33 Reserved"
  dwLowmostBits = 
    //Freq / "FSB in MHz" = multiplier
    wFreqInMHz / 133 ;
  BYTE byRet = 
    //g_pi_cpuaccess->WrmsrEx(
    (*g_pfn_write_msr) (
    IA32_PERF_CTL,
    dwLowmostBits,// bit  0-31 (register "EAX")
    dwHighmostBits, 
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  return byRet ;
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
