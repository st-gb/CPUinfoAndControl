// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"

//#include <Controller\I_CPUcontroller.hpp>
#include <Controller\I_CPUaccess.hpp>
#include <Controller\Intel_registers.h>
#include <Controller/DynLibCPUcontroller.h> 

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

I_CPUaccess * g_pi_cpuaccess ;
ReadMSR_func_type g_pfnreadmsr ;

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
    //from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
    //("Calling an Exported Function in an EXE from Within a DLL"):
    g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
      GetModuleHandle(NULL),
      "ReadMSR");
    if( ! g_pfnreadmsr )
      return FALSE ;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

_declspec(dllexport) 
void Init( //I_CPUcontroller * pi_cpu
  I_CPUaccess * pi_cpuaccess 
  //BYTE by 
  )
{
  g_pi_cpuaccess = pi_cpuaccess ;
  ////from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
  ////("Calling an Exported Function in an EXE from Within a DLL"):
  //g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
  //  GetModuleHandle(NULL),
  //  "ReadMSR");
}

_declspec(dllexport) 
  DWORD WINAPI GetMultiplier(WORD wCoreID )
{
  DWORD dwLowmostBits , dwHighmostBits ;
  g_pi_cpuaccess->RdmsrEx(
  //(*g_pfnreadmsr) (
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

_declspec(dllexport) 
  BYTE GetCurrentPstate(
    PWORD p_wFreqInMHz 
    , //float & Volt
      PWORD p_wMilliVolt
    , WORD wCoreID 
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  DWORD dwLowmostBits , dwHighmostBits ;
  BYTE byRet = g_pi_cpuaccess->RdmsrEx(
    //(*g_pfnwritemsr) (
    IA32_PERF_STATUS,
    & dwLowmostBits,// bit  0-31 (register "EAX")
    & dwHighmostBits, 
    //m_dwAffinityMask
    1 << wCoreID
    ) ;
  *p_wFreqInMHz = (BYTE) ( dwLowmostBits & 255 ) * 133 ;
  *p_wMilliVolt = 0 ;
  return byRet ;
}

_declspec(dllexport) 
  BYTE SetCurrentPstate(
    WORD wFreqInMHz 
    , //float & Volt
      WORD wMilliVolt
    , WORD wCoreID 
  )
  //dll_GetCurrentPstate_type
  //GET_CURRENT_PSTATE_SIG(GetCurrentPstate , )
{
  DWORD dwLowmostBits , dwHighmostBits ;
  dwLowmostBits = 
    //Freq / "FSB in MHz" = multiplier
    wFreqInMHz / 133 ;
  BYTE byRet = g_pi_cpuaccess->WrmsrEx(
    //(*g_pfnwritemsr) (
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

BYTE GetCPUcoreUsage( WORD wCPUcoreID )
{
  return 100 ;
}
