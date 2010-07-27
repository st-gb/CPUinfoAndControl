/*
 * inline_register_access_functions.h
 *
 *  Created on: Jul 13, 2010
 *      Author: Stefan
 */

#ifndef INLINE_REGISTER_ACCESS_FUNCTIONS_H_
#define INLINE_REGISTER_ACCESS_FUNCTIONS_H_

//#include <>
#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.
#include <Controller/CPUindependentHelper.h> //::getBinaryRepresentation(...)
#include <preprocessor_macros/logging_preprocessor_macros.h>

extern ReadMSR_func_type g_pfnreadmsr ;
extern WriteMSR_func_type g_pfn_write_msr ;

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
      << "VID:" <<
        //"15:9 CpuVid: core VID"
        ( ( dwEAX >> 9 ) & BITMASK_FOR_LOWMOST_7BIT )
      << "DID:" <<
        //"8:6 CpuDid: core divisor ID."
        ( ( dwEAX >> 6 ) & BITMASK_FOR_LOWMOST_3BIT )
      << "FID:" <<
        //"5:0 CpuFid: core frequency ID"
        ( dwEAX & BITMASK_FOR_LOWMOST_6BIT )
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
