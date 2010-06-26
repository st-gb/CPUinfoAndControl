#pragma once

#include <Controller/I_CPUaccess.hpp>
#include <Controller/exported_functions.h>
//#include <windef.h> //BYTE et.c

typedef unsigned char BYTE ;
typedef unsigned short      WORD;
//typedef WORD near           *PWORD;
typedef WORD  *PWORD;
typedef unsigned long       DWORD;

//these typedefs can be used by
//-the code that e.g. uses wxDynLib or only the Windows API
//and attaches to the CPU controller DLLs 
//-and also by the CPU controller DLL source code.

//( * dll_GetCurrentPstate_type)
#define CALL_TYPE _cdecl
//#define CALL_TYPE __stdcall

//name_or_func_ptr may be 
//-"(*pfn)" for function pointer declarations or 
//-"func" for function declarations/ definitions
#define GET_CURRENT_PSTATE_SIG(name_or_func_ptr,suffix) \
  BYTE \
  #name_or_func_ptr \
  ( \
    PWORD p_wFreqInMHz \
    , \
      PWORD p_wMilliVolt \
    , WORD wCoreID  \
  ) ##suffix

typedef BOOL ReadMSRex_type (
  const I_CPUaccess * ,DWORD, DWORD*, DWORD*, long unsigned int ) ;

typedef void (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  //WINAPI
    CALL_TYPE
  *dll_init_type)(
      I_CPUaccess *
      //BYTE
//      , //I_CPUaccess::RdmsrEx
      , I_CPUaccess::ReadMSRex_type
      //::ReadMSR
      );
typedef DWORD (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
    CALL_TYPE
  * dll_getMulti_type)(WORD );
typedef WORD (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetMaximumFrequencyInMHz_type) (WORD wCPUcoreID) ;
typedef WORD (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetMaximumVoltageID_type) () ;
typedef WORD (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetMinimumVoltageID_type) () ;
typedef 
  BYTE (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetCurrentPstate_type) (
    PWORD p_wFreqInMHz 
    , //float & Volt
#ifdef GET_VOLTAGE_IN_MILLIVOLT
    //When the voltage was converted to an integer with milliVolt there were
    //rounding errors: 0.95 (float) became 940 (WORD) milliVolt
      PWORD p_wMilliVolt
#else
    float & r_f
#endif
    , WORD wCoreID 
  ) ;
typedef
  BYTE (//WINAPI
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * pfn_GetCurrentVoltageAndFrequency_type) (
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    //
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
  ) ;

  //GET_CURRENT_PSTATE_SIG("(* dll_GetCurrentPstate_type)",;)
typedef 
  WORD (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetNumberOfCPUcores_type) (
  ) ;
typedef 
  float (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetCPUusage_type) ( WORD wCoreID
  ) ;
typedef 
  float (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetTemperatureInCelsius_type) ( WORD wCoreID
  ) ;
typedef
  float (//WINAPI
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_PrepareForNextPerformanceCounting)
    ( DWORD dwAffMask
      , BYTE byPerformanceEventSelectRegisterNumber
    ) ;
//typedef BYTE (//WINAPI
//  //Calling convention--must be the same as in the DLL
//  //function signature that calls this function?!
//  CALL_TYPE
//  * dll_SetCurrentPstate_type) (
//    WORD wFreqInMHz
//    , //float & Volt
//      WORD p_wMilliVolt
//    , WORD wCoreID
//  ) ;
typedef BYTE (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
//  CALL_TYPE
//  WINAPI
//  __stdcall
  * pfnSetCurrentVoltageAndMultiplier_type) (
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  ) ;

typedef float * (
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  //the reference clock might change. So better get the possible multipliers.
  * pfnGetAvailableMultipliers_type) (
    WORD wCoreID
    , WORD * p_wNum
  ) ;
typedef
  float (//WINAPI
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetVoltageInVolt_type) ( WORD wCoreID
  ) ;
typedef
  WORD (//WINAPI
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetVoltageID_type) ( float fVoltageInVolt
  ) ;
//typedef void (
//  //Calling convention--must be the same as in the DLL
//  //function signature that calls this function?!
//  CALL_TYPE
//  * dynlib_SetPstateFromFreq) (
//    WORD wFreqInMHz
//    , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
//    , BYTE byCoreID ) ;
typedef BOOL ( // TRUE: success, FALSE: failure
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_WriteMSR_type) (
    DWORD index,		// MSR index
    DWORD dwLow ,//eax,			// bit  0-31
    DWORD dwHigh, //edx,			// bit 32-63
    DWORD affinityMask	// Thread Affinity Mask
    ) ;
