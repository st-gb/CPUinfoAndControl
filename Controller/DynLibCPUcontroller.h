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
typedef 
  BYTE (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_GetCurrentPstate_type) (
    PWORD p_wFreqInMHz 
    , //float & Volt
      PWORD p_wMilliVolt
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
typedef BYTE (//WINAPI 
  //Calling convention--must be the same as in the DLL
  //function signature that calls this function?!
  CALL_TYPE
  * dll_SetCurrentPstate_type) (
    WORD wFreqInMHz 
    , //float & Volt
      WORD p_wMilliVolt
    , WORD wCoreID 
  ) ;
