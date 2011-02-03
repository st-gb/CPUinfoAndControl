/*
 * dllmain.cpp
 *
 *  Created on: Jul 10, 2010
 *      Author: Stefan
 */

  //This file is intellectual property of Trilobyte SE GmbH, Berlin, Germany.
  //Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany.
  //It must not be used commercially without the permission of Trilobyte
  //SE GmbH, Berlin, Germany.
  //It may be used for educational / academic purposes for free.
  //It may be used for personal use for free.
  //If you want to publish (parts) this sourcecode or generated binaries for other
  // purposes than for a DLL for x86Info&Control you have to ask Trilobyte.
  //If you use (parts) of this sourcecode then this license text must be contained.
  #ifdef _MSC_VER //MS compiler
  #include "stdafx.h"
  #endif

  #include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
  #include <Controller/CPU-related/AMD/Griffin/AMD_family17.h>
  #include <Controller/CPU-related/AMD/Griffin/Griffin.hpp>
  //for EXPORT, APIENTRY preprocessor macros
  #include <Controller/CPUcontrollerDynLibs/function_specifiers.h>
  #include <Controller/CPUindependentHelper.h> //::getBinaryRepresentation(...)
  #include <Controller/ExportedExeFunctions.h> //ReadMSR(...) etc.
  #include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointerToExportedExeReadPCIconfig.h>
  #include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>
  //#include <Windows/GetNumberOfLogicalCPUs.h>
  //for BITMASK_FOR_LOWMOST_5BIT
  #include <preprocessor_macros/preprocessor_helper_macros.h>

//  #define COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

  #if defined(COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT) || defined(_DEBUG)
    #define COMPILE_WITH_MODEL_DEFINITION
  #endif

  #ifdef COMPILE_WITH_MODEL_DEFINITION
    #include <Controller/I_CPUaccess.hpp>
    #include <Controller/CPU-related/I_CPUcontroller.hpp>
    #include <ModelData/ModelData.hpp>
    #include <ModelData/VoltageAndFreq.hpp>
    #include <set>
    I_CPUcontroller * g_p_cpucontroller ;
    I_CPUaccess * g_pi_cpuaccess ;
    std::set<VoltageAndFreq> * g_p_stdsetvoltageandfreqWanted ;
  #else
    //fwd decl.
    class I_CPUaccess ;
  #endif

#ifdef _DEBUG
#include <Controller/Logger/Logger.hpp>
Logger g_logger ;
#endif


  #include <windows.h> //for PSYSTEM_LOGICAL_PROCESSOR_INFORMATION
  #include <winuser.h> //MessageBox
  #include <sstream> //std::stringstream
  #include <tchar.h> //_T()
  #include <global.h> //logging

  extern ReadMSR_func_type g_pfnreadmsr ;
  extern WriteMSR_func_type g_pfn_write_msr ;
//  extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;
  ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;
  extern float g_fReferenceClockInMHz ;

  BYTE g_byValue1 ;
  BYTE g_byValue2 ;
  BYTE g_byValue3 ;
  DWORD g_dwLowmostBits ;
  DWORD g_dwHighmostBits ;

  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

//  //Inline-> replaced instead function call
//  //inline
//  BYTE ReadMSR(
//    DWORD dwRegisterIndex,   // MSR index
//    PDWORD p_dwEAX,     // bit  0-31
//    PDWORD p_dwEDX,     // bit 32-63
//         //1bin =core 0; 10bin=2dec= core 1
//   DWORD_PTR affinityMask  // Thread Affinity Mask
//    )
//  {
//    DEBUGN("DLL's ReadMSR")
//    return (*g_pfnreadmsr)(
//      dwRegisterIndex ,
//      p_dwEAX ,
//      p_dwEDX ,
//      affinityMask
//      ) ;
//  }
//
//  //Inline-> replaced instead function call
////  inline
//  BYTE WriteMSR(
//    DWORD dwRegisterIndex,   // MSR index
//    DWORD dwEAX,     // bit  0-31
//    DWORD dwEDX,     // bit 32-63
//         //1bin =core 0; 10bin=2dec= core 1
//   DWORD_PTR affinityMask  // Thread Affinity Mask
//    )
//  {
//#ifdef _DEBUG
//    DEBUGN("WriteMSR("
//      << dwRegisterIndex
//      << "low:" << getBinaryRepresentation(dwEAX)
//      << "high:" << getBinaryRepresentation(dwEDX)
//      << affinityMask
//      << ")"
//      )
//#endif
////    return (*g_pfn_write_msr)(
////      dwRegisterIndex ,
////      dwEAX ,
////      dwEDX ,
////      affinityMask
////      ) ;
//      return 0 ;
//  }

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
#ifdef _DEBUG
      {
      std::string strExeFileNameWithoutDirs //= GetExeFileNameWithoutDirs() ;
        ;
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
      }
#endif
      DEBUGN("after GetMainPllOpFreqIdMax")
      AssignPointersToExportedExeMSRfunctions(g_pfnreadmsr, g_pfn_write_msr) ;
      AssignPointerToExportedExeReadPCIconfig(g_pfnReadPCIconfigSpace) ;
      //Pointers to Exe's fct must be assigned prior to the call of
      //GetMainPllOpFreqIdMax() !
      GetMainPllOpFreqIdMax() ;
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
    DEBUGN("DLL's GetAvailableMultipliers")
    return GetAvailableMultipliers(p_wNumberOfArrayElements) ;
  }

  //http://en.wikipedia.org/wiki/Dynamic-link_library#C_and_C.2B.2B:
  //"When external names follow the C naming conventions, they must also be
  //declared as extern "C" in C++ code, to prevent them from using C++ naming
  //conventions."
  extern "C" __declspec(dllexport)
  //The array pointed to by the return value must be freed by the caller (i.e.
  //x86I&C GUI or service) of this function.
  float * GetAvailableVoltagesInVolt(
      WORD wCoreID
      , WORD * p_wNumberOfArrayElements )
  {
    return GetAvailableVoltages(*p_wNumberOfArrayElements) ;
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
    return GetCurrentVoltageAndFrequencyAMDGriffin(
      p_fVoltageInVolt,
      p_fMultiplier ,
      p_fReferenceClockInMHz ,
      wCoreID
      ) ;
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
    DWORD dwValue ;
    float fTempInDegCelsius ;
    g_byValue1 = ( 24 << 3 ) | 3 ;
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
    AssignPointersToExportedExeMSRfunctions(
      g_pfnreadmsr , g_pfn_write_msr ) ;
#endif
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
  #ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
    g_pi_cpuaccess = pi_cpuaccess ;
    g_p_cpucontroller = pi_cpuaccess->mp_cpu_controller ;
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
    DEBUGN("DLL fct SetCurrentVoltageAndMultiplier")
//    DWORD dwLowmostBits , dwHighmostBits = 0 ;
//    dwLowmostBits =
//      //Freq / "FSB in MHz" = multiplier
//      (BYTE) fMultiplier ;
////    std::stringstream ss ;
////    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
////        << dwLowmostBits ;
////    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
//    g_byValue1 = GetVoltageID( fVoltageInVolt ) ;
    SetVoltageAndMultiplier( fVoltageInVolt , fMultiplier , (BYTE) wCoreID ) ;
    return 0 ;
  }