#pragma once

#include <Controller/I_CPUaccess.hpp>
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

typedef void (//WINAPI 
  *dll_init_type)(
      I_CPUaccess *
      //BYTE
      );
typedef DWORD (*dll_getMulti_type)(WORD );
typedef WORD (*dll_GetMaximumFrequencyInMHz_type) () ;
typedef 
  BYTE (//WINAPI 
  * dll_GetCurrentPstate_type) (
    PWORD p_wFreqInMHz 
    , //float & Volt
      PWORD p_wMilliVolt
    , WORD wCoreID 
  ) ;
  //GET_CURRENT_PSTATE_SIG("(* dll_GetCurrentPstate_type)",;)
typedef 
  WORD (//WINAPI 
  * dll_GetNumberOfCPUcores_type) (
  ) ;
typedef BYTE (//WINAPI 
  * dll_SetCurrentPstate_type) (
    WORD wFreqInMHz 
    , //float & Volt
      WORD p_wMilliVolt
    , WORD wCoreID 
  ) ;
