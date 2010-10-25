/*
 * inline_register_access_functions.h
 *
 *  Created on: Jul 13, 2010
 *      Author: Stefan
 */

#ifndef INLINE_REGISTER_ACCESS_FUNCTIONS_H_
#define INLINE_REGISTER_ACCESS_FUNCTIONS_H_

//The functions within this file are inline because:
// -they are used by the CPU controller sourcecode:
//  ..."ReadMSR"...
//  -if the code should be compiled as a dynamic library, then _this_ file
//   should be included
//  -if the code should be compiled as a _built-in_ CPU controller then
//   another header file that defines the same functions as here should be
//   used/ included
//  -> this allows the "Don't Repeat Yourself" (DRY) principle ("don't write the same
//   code twice or more") _without_ a function overhead/ with the maximum speed.

#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.
//::getBinaryRepresentation(...)
#include <Controller/character_string/format_as_string.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //for DEBUGN(...)

extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;

inline BYTE CPUID(
  DWORD dwIndex,
  PDWORD p_dwEAX,
  PDWORD p_dwEBX,
  PDWORD p_dwECX,
  PDWORD p_dwEDX,
  DWORD_PTR affinityMask
  )
{
  DEBUGN("before executing the CPUID instruction")
  //TODO return error if calling CPUID fails
  //from http://www.ibm.com/developerworks/library/l-ia.html:
  asm ("cpuid"
        : "=a" (*p_dwEAX),
          "=b" (*p_dwEBX),
          "=c" (*p_dwECX),
          "=d" (*p_dwEDX)
        : "a" (dwIndex));
  DEBUGN("after executing the CPUID instruction: "
    << "EAX: " << * p_dwEAX
    << "EBX: " << * p_dwEBX
    << "ECX: " << * p_dwECX
    << "EDX: " << * p_dwEDX
    )
  return 1 ;
}

  //Inline-> replaced instead function call
  inline
  BYTE ReadMSR(
    DWORD dwRegisterIndex,   // MSR index
    PDWORD p_dwEAX,     // bit  0-31
    PDWORD p_dwEDX,     // bit 32-63
         //1bin =core 0; 10bin=2dec= core 1
   DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
    DEBUGN("inline ReadMSR")
    return (*g_pfnreadmsr)(
      dwRegisterIndex ,
      p_dwEAX ,
      p_dwEDX ,
      affinityMask
      ) ;
  }

  //Inline-> replaced instead function call
  inline
  BYTE WriteMSR(
    DWORD dwRegisterIndex,   // MSR index
    DWORD dwEAX,     // bit  0-31
    DWORD dwEDX,     // bit 32-63
         //1bin =core 0; 10bin=2dec= core 1
   DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
    DEBUGN("inline WriteMSR("
      << dwRegisterIndex
      << "low:" << getBinaryRepresentation(dwEAX)
      << "high:" << getBinaryRepresentation(dwEDX)
      << affinityMask
      << ")"
      )
    return (*g_pfn_write_msr)(
      dwRegisterIndex ,
      dwEAX ,
      dwEDX ,
      affinityMask
      ) ;
      return 1 ;
  }

#endif /* INLINE_REGISTER_ACCESS_FUNCTIONS_H_ */
