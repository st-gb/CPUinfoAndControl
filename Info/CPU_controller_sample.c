  //This file is intellectual property of Trilobyte SE GmbH, Berlin, Germany.
  //Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany.
  //It must not be used commercially without the permission of Trilobyte 
  //SE GmbH, Berlin, Germany.
  //It may be used for educational / academic purposes for free.
  //If you use (parts) of this sourcecode then this license text must be contained.
  #ifdef _MSC_VER //MS compiler
  #include "stdafx.h"
  #endif

  #include <Controller/CPU-related/GetCurrentReferenceClock.h>
  #include <Controller/CPU-related/Intel/Intel_registers.h>
  #include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
  #include <Controller/value_difference.h> //ULONG_VALUE_DIFF
  #include <Windows/AssignPointersToExportedExeFunctions.h>
  //#include <Windows/GetNumberOfLogicalCPUs.h>
  #include <preprocessor_helper_macros.h>  //for BITMASK_FOR_LOWMOST_5BIT

  #include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION
  #include <winuser.h> //MessageBox
  #include <sstream> //std::stringstream
  #include <tchar.h> //_T()

  extern ReadMSR_func_type g_pfnreadmsr ;
  extern WriteMSR_func_type g_pfn_write_msr ;
  extern float g_fReferenceClockInMHz ;

  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

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
      AssignPointersToExportedExeFunctions() ;
      if( ! g_pfnreadmsr || ! g_pfn_write_msr )
      {
        MessageBox(NULL,
          "Pointers could not be assigned to the execu-tables export functions\n"
          "Does the executable that loads this DLL have ReadMSR and WriteMSR"
          "export functions at all?(analyze this with a tool)"
          //Title
          ,"error"
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
      GetCurrentReferenceClock(12.0, 100 , MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
    }
    return TRUE;
  }

  //#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
  #define NEHALEM_DLL_CALLING_CONVENTION

  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  //For exporting this function with the same name as here in the source file.
  extern "C" __declspec(dllexport)
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
    BYTE byMaxMultiplier = 0 ;
    DWORD dwLowmostBits , dwHighmostBits ;
    (*g_pfnreadmsr) (
       0x1AD , // name="MSR_TURBO_RATIO_LIMIT"
      & dwLowmostBits,// bit  0-31 (register "EAX")
      & dwHighmostBits,
      1 << wCoreID //m_dwAffinityMask
      ) ;
     byMaxMultiplier = (BYTE) ( dwLowmostBits & 255 ) ;
     BYTE byNumMultis = byMaxMultiplier -
         //min. multi - 1
         5 ;
     float * ar_f = new float[byNumMultis] ;
     //If allocating the array on the heap succeeded.
     if( ar_f )
     {
       *p_wNumberOfArrayElements = byNumMultis ;
       float fMulti = 7.0 ; //The minimum multiplier for Nehalem is 7.
  //     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
       for( BYTE by = 0 ; by < byNumMultis ; ++ by )
       {
         ar_f[by] = fMulti ;
  //       stdstrstream << fMulti << " " ;
         ++ fMulti ;
       }
     }
     else
       *p_wNumberOfArrayElements = 0 ;    
     return ar_f ;
  }

  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  extern "C" __declspec(dllexport)
  //The array pointed to by the return value must be freed by the caller (i.e.
  //x86I&C GUI or service) of this function.
  float *
    NEHALEM_DLL_CALLING_CONVENTION
    GetAvailableVoltagesInVolt(
      WORD wCoreID
      , WORD * p_wNum )
  {
     BYTE byNumVoltages = 2 ;
     float * ar_f = new float[byNumVoltages] ;
     //If allocating the array on the heap succeeded.
     if( ar_f )
     {
       ar_f[0] = 0.65 ;
       ar_f[1] = 0.9 ;
       *p_wNum = byNumVoltages ;
     }
     else
       *p_wNum = 0 ;
    return ar_f ;
  }
  
  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  //For exporting this function with the same name as here in the source file.
  extern "C" __declspec(dllexport)
    BYTE
    NEHALEM_DLL_CALLING_CONVENTION
    GetCurrentVoltageAndFrequency(
      float * p_fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float * p_fMultiplier
      , float * p_fReferenceClockInMHz
      , WORD wCoreID
    )
  {
    DWORD dwLowmostBits , dwHighmostBits ;
  //  //Intel: 198H 408 IA32_PERF_STATUS
    BYTE byRet =
      (*g_pfnreadmsr) (
      IA32_PERF_STATUS,
      & dwLowmostBits,// bit  0-31 (register "EAX")
      & dwHighmostBits,
      1 << wCoreID //m_dwAffinityMask
      ) ;

    * p_fVoltageInVolt = 0 ;
    //Intel: "15:0 Current performance State Value"
    //   "63:16 Reserved"
    * p_fMultiplier = ( dwLowmostBits & 255 ) ;

    //This call sets g_fReferenceClockInMHz to the current reference clock.
    //This update of the value would be senseful for setting the CPU core
    //via "frequency" as parameter value the next time.
    GetCurrentReferenceClock(12.0 ,
      1 , //min. timespan in ms
      10000 ) ;
    *p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
    
    return byRet ;
  }

  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  //For exporting this function with the same name as here in the source file.
  extern "C" __declspec(dllexport)
  float
    NEHALEM_DLL_CALLING_CONVENTION
    GetTemperatureInCelsius ( WORD wCoreID
    )
  {
    BYTE byDigitalReadout ;
    BYTE byTempInDegCelsius ;
    BYTE byTempTarget;
    BYTE byResolutionInDegreesCelsius ;
    DWORD dwLowmostBits ;
    DWORD dwHighmostBits ;
    (*g_pfnreadmsr) (
       MSR_TEMPERATURE_TARGET ,
       & dwLowmostBits,// bit  0-31 (register "EAX")
       & dwHighmostBits,
       //m_dwAffinityMask
       1 << wCoreID
       ) ;
    //TemperatureTarget startbit="16" bitlength="8"/
    byTempTarget = ( dwLowmostBits >> 16 ) & 255 ;
    (*g_pfnreadmsr) (
       IA32_THERM_STATUS ,
       & dwLowmostBits,// bit  0-31 (register "EAX")
       & dwHighmostBits,
       //m_dwAffinityMask
       1 << wCoreID
       ) ;
    //Intel: "22:16 Digital Readout (RO)"
    byDigitalReadout = ( dwLowmostBits >> 16 ) & 127 ;
    //Intel: "30:27 Resolution in Degrees Celsius (RO)"
    byResolutionInDegreesCelsius = ( dwLowmostBits >> 27 ) &
        BITMASK_FOR_LOWMOST_5BIT ;
    byTempInDegCelsius = byTempTarget - byDigitalReadout ;
    return (float) byTempInDegCelsius ;
  }

  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  //For exporting this function with the same name as here in the source file.
  extern "C" __declspec(dllexport)
    BYTE
    NEHALEM_DLL_CALLING_CONVENTION //can be omitted.
    SetCurrentVoltageAndMultiplier(
      float fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float fMultiplier
      , WORD wCoreID
    )
  {
    DWORD dwLowmostBits , dwHighmostBits = 0 ;
    //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA   since: 06_0FH (Mobile)
    //  "63:33 Reserved"
    dwLowmostBits =
      //Freq / "FSB in MHz" = multiplier
      (BYTE) fMultiplier ;
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << dwLowmostBits ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
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
