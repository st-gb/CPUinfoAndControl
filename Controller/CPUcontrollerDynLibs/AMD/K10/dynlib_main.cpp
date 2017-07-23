
#include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointerToExportedExeReadPCIconfig.h>
#include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
#include <preprocessor_macros/bitmasks.h>

#define GET_REFERENCE_CLOCK_VIA_TSC_DIFF
/** 31116 Rev 3.62 - January 11, 2013AMD Family 10h Processor BKDG :
 *   "CPU COF = 100 MHz * (CpuFid + 10h) / (2^CpuDid)."
 *  => 100 MHz The default reference clock frequency for K10 CPUs. */
float g_fReferenceClockInMHz = 100.0f; 
#include <Controller/CPU-related/AMD/beginningWithFam10h/from_K10.h>
#include <Controller/CPU-related/AMD/beginningWithFam10h/GetAvailableMultipliers.hpp>

ReadMSR_func_type g_pfnreadmsr;
WriteMSR_func_type g_pfn_write_msr;
ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;

bool Init()
{
  //#ifdef COMPILE_WITH_LOG
#ifdef _DEBUG
  OpenLogFile() ;
  #endif
#ifdef _DEBUG
//  g_std_ofstream << "before AssignPointersToExportedExeMSRfunctions_inline(...)"
//    << std::endl;
//  g_std_ofstream.flush();
#endif
  AssignPointersToExportedExeMSRfunctions_inline(
    g_pfnreadmsr ,
    g_pfn_write_msr
    ) ;
  AssignPointerToExportedExeReadPCIconfig(g_pfnReadPCIconfigSpace) ;
//  g_fReferenceClockMultiplier = GetReferenceClockMultiplier(1);
  DEBUGN( "ReadMSR fct. pointer :" << (void *) g_pfnreadmsr
    << " WriteMSR fct. pointer :" << (void *) g_pfn_write_msr
    )
#ifdef _DEBUG
//  g_std_ofstream << "after AssignPointersToExportedExeMSRfunctions_inline(...)"
//    << std::endl;
//  g_std_ofstream.flush();
#endif
  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
  {
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
    ::MessageBox(
      NULL ,
      "Pointers could not be assigned to the executables export functions\n"
      "Does the executable that loads this DLL have ReadMSR and WriteMSR"
      "export functions at all?(analyze this with a tool)"
      //Title
      ,"error"
      , MB_OK) ;
#endif
    return FALSE ;
  }
  
  DEBUGN( /*FULL_FUNC_NAME <<*/ "ref. clock in MHz:" << g_fReferenceClockInMHz)

  //      //Force the cond. "< min. time diff" to become true.
  //      g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
  //      g_dwPreviousTickCountInMilliseconds
  //        //->time diff gets > max. time diff, so it calcs a ref clock.
  //        -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );

  //      //The reference clock is needed for setting the current frequency. So it
  //      //must be determined prior to any call of this function.
  //      GetCurrentReferenceClock(12.0, 100 , MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
  return true ;
}

/** This function is the 1st function which is called by the executable 
 *   (GUI or service/daemon) */
EXPORT void /*DLL_CALLING_CONVENTION*/ Init( //I_CPUcontroller * pi_cpu
#ifdef INSERT_DEFAULT_P_STATES
  /** CPUaccess object inside the exe.*/
  I_CPUaccess * pi_cpuaccess
#else
  void * p_v
#endif //#ifdef INSERT_DEFAULT_P_STATES
//  , ReadMSR_func_type pfnreadmsr
  //BYTE by
  )
{
  Init();
  //g_MaxCPU_Cof = AMD::fromK10::GetMaxCPU_COF();
  //g_fMaxMultiplier = AMD::fromK10::GetMaximumMultiplier(g_MaxCPU_Cof);
#ifdef INSERT_DEFAULT_P_STATES
  #ifdef _DEBUG
//  g_std_ofstream << "Init(*)--pi_cpuaccess:" << pi_cpuaccess
//      << " sizeof(*pi_cpuaccess):" << sizeof(*pi_cpuaccess)
//      md5(*pi_cpuaccess)
//      << std::endl;
//  g_std_ofstream.flush();
  #endif
#endif
#ifdef INSERT_DEFAULT_P_STATES
  InsertDefaultPstates(pi_cpuaccess);
#endif
#ifdef _DEBUG
//  g_std_ofstream << "Init(*)--end:" << std::endl;
//  g_std_ofstream.flush();
#endif
}

/** @return The array pointed to by the return value must be freed by the 
*    caller (i.e. x86I&C GUI or service) of this function. */
EXPORT float * /*DLL_CALLING_CONVENTION*/ GetAvailableVoltagesInVolt(
  WORD wCoreID
  , WORD * p_wNum )
{
  DEBUGN("begin")
  return  AMD::fromK10::GetAvailableVoltagesInVolt(wCoreID, p_wNum);
}

EXPORT float //DLL_CALLING_CONVENTION
  GetTemperatureInCelsius( WORD wCoreID
  )
{
  return AMD::fromK10::GetTemperatureInCelsius(wCoreID);
}

EXPORT BYTE /*DLL_CALLING_CONVENTION*/ GetCurrentVoltageAndFrequency(
  float * p_fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float * p_fMultiplier
  , float * p_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  return AMD::fromK10::GetCurrentVoltageAndFrequency(
      p_fVoltageInVolt
    , p_fMultiplier
    , p_fReferenceClockInMHz
    , wCoreID
    ) ;
}

/** @brief The reference clock might change, also during runtime.
 *  This is why it is a good idea to get the possible multipliers.
 * @return the array pointed to by the return value must be freed by the
 *   caller (i.e. x86I&C GUI or service) of this function. */
EXPORT float * /*DYN_LIB_CALLING_CONVENTION*/ GetAvailableMultipliers(
  WORD wCoreID
  , WORD * p_wNumberOfArrayElements
  )
{
  DEBUGN("dynamic library's GetAvailableMultipliers")
  return AMD::fromK10::GetAvailableMultipliers(p_wNumberOfArrayElements);
}
