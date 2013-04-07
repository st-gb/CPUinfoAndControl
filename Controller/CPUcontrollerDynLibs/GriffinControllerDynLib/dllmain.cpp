/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * dllmain.cpp
 *
 *  Created on: Jul 10, 2010
 *      Author: Stefan
 */

  //If you want to publish (parts) this sourcecode or generated binaries for other
  // purposes than for a DLL for x86Info&Control you have to ask Trilobyte.
  //If you use (parts) of this sourcecode then this license text must be contained.
  #ifdef _MSC_VER //MS compiler
  #include "stdafx.h"
  #endif

//  #include <Controller/CPU-related/AMD/Griffin/AMD_family17.h>
  #include <Controller/CPU-related/AMD/Griffin/Griffin.hpp>

  #include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
  #include <preprocessor_macros/show_via_GUI.h> //SHOW_VIA_GUI
  #include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
  #include <windef.h> //BOOL
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointerToExportedExeReadPCIconfig.h>
#ifdef _DEBUG
//  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
#endif
  //#include <Windows/GetNumberOfLogicalCPUs.h>

  //for BITMASK_FOR_LOWMOST_5BIT
  #include <preprocessor_macros/preprocessor_helper_macros.h>

//  #define COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

  #if defined(COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT) || defined(_DEBUG)
    #define COMPILE_WITH_MODEL_DEFINITION
  #endif

  #ifdef COMPILE_WITH_MODEL_DEFINITION
    #include <Controller/I_CPUaccess.hpp>
//    #include <Controller/CPU-related/I_CPUcontroller.hpp>
    #include <ModelData/ModelData.hpp>
    #include <ModelData/VoltageAndFreq.hpp>
    #include <set>
//    I_CPUcontroller * g_p_cpucontroller ;
    I_CPUaccess * g_pi_cpuaccess ;
    std::set<VoltageAndFreq> * g_p_stdsetvoltageandfreqWanted ;
  #else
    //fwd decl.
    class I_CPUaccess ;
  #endif

#ifdef _DEBUG
#include <Controller/Logger/Logger.hpp>
//Logger g_logger ;
#endif

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windows.h> //for
  #include <winuser.h> //MessageBox
#endif
  #include <sstream> //std::stringstream
  #include <tchar.h> //_T()
//  #include <global.h> //logging
  #include <Controller/CPU-related/AMD/Griffin/GetAvailableMultipliers.hpp>

//  extern ReadMSR_func_type g_pfnreadmsr ;
//  extern WriteMSR_func_type g_pfn_write_msr ;
//  extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;
  ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;
//  extern float g_fReferenceClockInMHz ;

  BYTE g_byFreqID, g_byDivisorID ;
  BYTE g_byValue1 ;
  BYTE g_byValue2 ;
  BYTE g_byValue3 ;
  DWORD g_dwLowmostBits ;
  DWORD g_dwHighmostBits ;
  float g_fMaxMultiDiv2 ;
  float g_fValue1 ;
  uint32_t g_dwMSRhighmost, g_dwMSRlowmost ;
  //MSRC001_0071 COFVID Status Register
  //54:49 MainPllOpFreqIdMax: main PLL operating frequency ID maximum.
  float g_fMainPllOpFreqIdMax ;
  float g_fMaxMultiplier;
  float g_fMainPllOpFreqId ;

#ifdef MAX_MULTI_IS_MAIN_PLL_OP_FREQ_ID_MAX_PLUS_8
  float g_fReferenceClockInMHz = 100.0f ;
#else
  float g_fReferenceClockInMHz = 200.0f ;
#endif

  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

  void InitializeLogger()
  {
    std::string strExeFileNameWithoutDirs //= GetExeFileNameWithoutDirs() ;
      ;
    std::string stdstrFilename = strExeFileNameWithoutDirs +
        ("GriffinControllerDLL_log.txt") ;
#ifdef _DEBUG
    if( g_logger.OpenFileA( stdstrFilename ) )
  //        SHOW_VIA_GUI("log file is open")
  //      else
  //        SHOW_VIA_GUI("log file is closed")

  //      g_logger.Log("this Log file is open");
    g_logger.CreateFormatter("htm");
#endif
  //      if( g_logger.GetFormatter() )
  //        g_logger.Log("m_p_log_formatter is not NULL");
  //      std::string std_str = "std::string";
  //      g_logger.Log(std_str);
  //      g_logger.Log(std_str.c_str() );
  //      std::string std_str("DllMain:");
  //    //  std_str +=
  //      g_logger.Log(//"MainPllOpFreqIdMax:" << g_fMainPllOpFreqIdMax
  //        std_str );

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
  }

void Init()
{
#ifdef _DEBUG
  InitializeLogger();
#endif
  AssignPointersToExportedExeMSRfunctions(
    g_pfnreadmsr, g_pfn_write_msr) ;
//#ifdef _WIN32
  AssignPointerToExportedExeReadPCIconfig(g_pfnReadPCIconfigSpace) ;
//#endif
//      LOGN("after GetMainPllOpFreqIdMax")
  if( ! g_pfnreadmsr || ! g_pfn_write_msr )
  {
#ifdef _WIN32
    MessageBox(NULL,
      "Pointers could not be assigned to the execu-tables export functions\n"
      "Does the executable that loads this DLL have ReadMSR and WriteMSR"
      "export functions at all?(analyze this with a tool)"
      //Title
      ,"error"
      , MB_OK) ;
    return /*FALSE*/;
#endif //#ifdef _WIN32
  }
  //Pointers to Exe's fct must be assigned prior to the call of
  //GetMainPllOpFreqIdMax() !
  AMD::K10_and_K11::GetMainPllOpFreqIdMax() ;
  DEBUGN("after GetMainPllOpFreqIdMax")
}

#ifdef _WIN32
  EXPORT BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
  {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
      Init();
//      g_pfnreadmsr(
//        HARDWARE_CONFIGURATION_REGISTER,
//        g_dwLowmostBits ,
//        g_dwHighmostBits ,
//        1
//        ) ;
//      //Force the cond. "< min. time diff" to become true.
//      g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
//      g_dwPreviousTickCountInMilliseconds
//        //->time diff gets > max. time diff, so it calcs a ref clock.
//        -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );
//      //The reference clock is needed for setting the current frequency. So it
//      //must be determined prior to any call of this function.
//      GetCurrentReferenceClock(12.0, 100 , MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
    }
    return TRUE;
  }
#endif

  //#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
  #define NEHALEM_DLL_CALLING_CONVENTION

  EXPORT
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
    DEBUGN("DLL's GetAvailableMultipliers")
    return GetAvailableMultipliersAMDfamilyFh(p_wNumberOfArrayElements) ;
//    return 0;
  }

  EXPORT
  //The array pointed to by the return value must be freed by the caller (i.e.
  //x86I&C GUI or service) of this function.
  float * GetAvailableVoltagesInVolt(
      WORD wCoreID
      , WORD * p_wNumberOfArrayElements )
  {
    return GetAvailableVoltagesAMDfamilyFh(*p_wNumberOfArrayElements) ;
//    return 0;
  }

  EXPORT
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
    DEBUGN( FULL_FUNC_NAME << "--begin")
    return AMD::K10_and_K11::GetCurrentVoltageAndFrequency(
      p_fVoltageInVolt,
      p_fMultiplier ,
      p_fReferenceClockInMHz ,
      wCoreID
      ) ;
//    return 0;
  }

//#ifdef _WIN32 //Under Linux accessing the PCI config space does not work yet.
  EXPORT
  float
    NEHALEM_DLL_CALLING_CONVENTION
    GetTemperatureInCelsius ( WORD wCoreID
    )
  {
    DWORD dwValue ;
    float fTempInDegCelsius ;
//    static BYTE g_byValue1 = ( 24 << 3 ) | 3 ;
    DEBUGN("GetTemperatureInCelsius(...) device and function:"
      << (WORD) g_byValue1 )
//    BYTE byRet = //GetCPUMiscControlDWORD(
      g_pfnReadPCIconfigSpace(
      0, //bus number
      //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
//      g_byValue1
      CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER
      ,//) ((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
      F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER ,
      & dwValue
      ) ;
    fTempInDegCelsius = (float)( dwValue >> 21 ) / 8.0f ;
    return fTempInDegCelsius ;
//      0.0 ;
  }
//#endif

  //#define DLL_CALLING_CONVENTION __stdcall
  #define DLL_CALLING_CONVENTION

  EXPORT
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
#ifdef __linux__
//    AssignPointersToExportedExeMSRfunction_inline(
//      g_pfnreadmsr , g_pfn_write_msr ) ;
    Init();
#endif
//    std::string stdstrFilename = //strExeFileNameWithoutDirs +
//      ("PentiumM_DLL_log.txt") ;
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
    SHOW_VIA_GUI(stdstrstream.str().c_str() )
  #endif
  #ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
    g_pi_cpuaccess = pi_cpuaccess ;
//    g_p_cpucontroller = pi_cpuaccess->mp_cpu_controller ;
    g_p_stdsetvoltageandfreqWanted = & g_pi_cpuaccess->mp_model->m_cpucoredata.
      m_stdsetvoltageandfreqWanted ;
  #endif //INSERT_DEFAULT_P_STATES
  //  AssignExeFunctionPointers() ;
    //g_nehalemcontroller.SetCPUaccess( pi_cpuaccess ) ;
    //MSC-generated version has no problems
  //#ifndef _MSC_VER
  //  std::stringstream str ;
  //  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
  //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //#endif
    //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
  }

  EXPORT
  //When a function with this name exists in the DLL the dyn volt n freq scal thread
  //scales per core.
  void moreThan1CPUcorePowerPlane()
  {
  }

  EXPORT
    BYTE
    NEHALEM_DLL_CALLING_CONVENTION //can be omitted.
    SetCurrentVoltageAndMultiplier(
      float fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float fMultiplier
      , WORD wCoreID
    )
  {
    DEBUGN("DLL fct SetCurrentVoltageAndMultiplier")
    AMD::K10_and_K11::SetVoltageAndMultiplier( fVoltageInVolt , fMultiplier ,
      (BYTE) wCoreID ) ;
    return 0 ;
  }
