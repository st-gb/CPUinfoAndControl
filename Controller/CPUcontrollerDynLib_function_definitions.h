/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcontrollerDynLib_function_definitions.h
 *
 *  Created on: Oct 30, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLLERDYNLIB_FUNCTION_DEFINITIONS_H_
#define CPUCONTROLLERDYNLIB_FUNCTION_DEFINITIONS_H_

//CALLING_CONVENTION
#include <Controller/CPUcontrollerDynLib/calling_convention.h>
#include <windef.h> //BOOL

//typedef unsigned char BYTE ;
typedef unsigned short WORD;
////typedef WORD near * PWORD;
typedef WORD * PWORD;
//typedef unsigned long DWORD;
//#include <windef.h> //BYTE et.c

//Must happen in 2 steps: 1st: expand , 2nd: call a second macro that stringifies
#define EXPAND_AND_STRINGIFY(var) MAKE_STRING(var)
#define MAKE_STRING(var) #var
#define MAKE_STRING_FROM_OSTREAM(var) #var

//these typedefs can be used by
//-the code that e.g. uses wxDynLib or only the Windows API
//and attaches to the CPU controller DLLs
//-and also by the CPU controller DLL source code.

//( * dll_GetCurrentPstate_type)


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

//Forward declarations
class I_CPUaccess ;

typedef BOOL ReadMSRex_type (
  const I_CPUaccess * ,DWORD, DWORD*, DWORD*, long unsigned int ) ;

//Used for both
// -function name in function declaration
// -for assigning a pointer to a dynlib function with this name
#define DYN_LIB_INIT_FUNCTION_NAME Init
typedef void (
  //WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_init_type)(
      I_CPUaccess *
      //BYTE
      );
//#define INIT_FUNCTION_DECLARATION(var_name) void ( I_CPUaccess * var_name)

typedef DWORD (
  DYN_LIB_CALLING_CONVENTION
  * dll_getMulti_type)(WORD );
typedef WORD (
  DYN_LIB_CALLING_CONVENTION
  * dll_GetMaximumFrequencyInMHz_type) (WORD wCPUcoreID) ;
typedef WORD (
  DYN_LIB_CALLING_CONVENTION
  * dll_GetMaximumVoltageID_type) () ;
typedef WORD (
  DYN_LIB_CALLING_CONVENTION
  * dll_GetMinimumVoltageID_type) () ;
typedef
  BYTE (//WINAPI
  DYN_LIB_CALLING_CONVENTION
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
  DYN_LIB_CALLING_CONVENTION
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
  DYN_LIB_CALLING_CONVENTION
  * dll_GetNumberOfCPUcores_type) (
  ) ;
typedef
  float (//WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_GetCPUusage_type) ( WORD wCoreID
  ) ;
//Used for both
// -function name in function declaration
// -for assigning a pointer to a dynlib function with this name
#define DYN_LIB_GET_TEMPERATURE_IN_CELSIUS_FUNCTION_NAME GetTemperatureInCelsius
typedef
  float (//WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_GetTemperatureInCelsius_type) ( WORD wCoreID
  ) ;
typedef
  float (//WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_PrepareForNextPerformanceCounting)
    ( DWORD dwAffMask
      , BYTE byPerformanceEventSelectRegisterNumber
    ) ;
//typedef BYTE (//WINAPI
//  DYN_LIB_CALLING_CONVENTION
//  * dll_SetCurrentPstate_type) (
//    WORD wFreqInMHz
//    , //float & Volt
//      WORD p_wMilliVolt
//    , WORD wCoreID
//  ) ;
typedef BYTE (
//  DYN_LIB_CALLING_CONVENTION
//  WINAPI
//  __stdcall
  * pfnSetCurrentVoltageAndMultiplier_type) (
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
  ) ;
/** unsigned = same bit size as CPU arch -> fast */
typedef BYTE ( * pfnSetThrottleLevel_type) (
    float fThrottleLevel
    //[0.0f ... 1.0f]
    , unsigned coreID
  ) ;

typedef float * (
  DYN_LIB_CALLING_CONVENTION
  //the reference clock might change. So better get the possible multipliers.
  * pfnGetAvailableMultipliers_type) (
    WORD wCoreID
    , WORD * p_wNum
  ) ;
typedef
  float (//WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_GetVoltageInVolt_type) ( WORD wCoreID
  ) ;
typedef
  WORD (//WINAPI
  DYN_LIB_CALLING_CONVENTION
  * dll_GetVoltageID_type) ( float fVoltageInVolt
  ) ;
//typedef void (
//  DYN_LIB_CALLING_CONVENTION
//  * dynlib_SetPstateFromFreq) (
//    WORD wFreqInMHz
//    , const std::set<VoltageAndFreq> & cr_stdsetvoltageandfreqForInterpolation
//    , BYTE byCoreID ) ;
typedef BOOL ( // TRUE: success, FALSE: failure
  DYN_LIB_CALLING_CONVENTION
  * dll_WriteMSR_type) (
    DWORD index,    // MSR index
    DWORD dwLow ,//eax,     // bit  0-31
    DWORD dwHigh, //edx,      // bit 32-63
    DWORD affinityMask  // Thread Affinity Mask
    ) ;

#endif /* CPUCONTROLLERDYNLIB_FUNCTION_DEFINITIONS_H_ */
