// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"

#include <Controller\I_CPUaccess.hpp>
#include <Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.hpp>
//#include <Controller/Logger.hpp>

I_CPUaccess * g_pi_cpuaccess ;
Nehalem::ClocksNotHaltedCPUcoreUsageGetter g_clocksnothaltedcpucoreusagegetter ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) 
float GetCPUcoreUsage( WORD wCPUcoreID )
{
  //Is NULL if g_clocksnothaltedcpucoreusagegetter::SetCPUaccess() has not 
  //been called yet.
  if( g_clocksnothaltedcpucoreusagegetter.GetCPUaccess() )
  {
    float fCPUusage = g_clocksnothaltedcpucoreusagegetter.
      GetPercentalUsageForCore( wCPUcoreID ) ;
    return //100 ;
      //(BYTE) ( fCPUusage * 100 ) ;
      fCPUusage ;
  }
  return -1.0 ;
}

//http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
//"When external names follow the C naming conventions, they must also be 
//declared as extern "C" in C++ code, to prevent them from using C++ naming 
//conventions."
extern "C" __declspec(dllexport) 
void Init( //I_CPUcontroller * pi_cpu
  I_CPUaccess * pi_cpuaccess 
  //BYTE by 
  )
{
  g_pi_cpuaccess = pi_cpuaccess ;
  //from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
  //("Calling an Exported Function in an EXE from Within a DLL"):
  g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
    GetModuleHandle(NULL),
    "ReadMSR");
  //WORD w = GetMaximumFrequencyInMHz(0) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 , w ) ;
  //pi_cpuaccess->mp_model->m_cpucoredata.
  //  //Add to the set of default p-states and to the set of avail. p-states.
  //  AddDefaultVoltageForFreq( 0.0 ,931 ) ;
  g_clocksnothaltedcpucoreusagegetter.mp_model = pi_cpuaccess->mp_model ;
  g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
  g_clocksnothaltedcpucoreusagegetter.Init() ;
}

