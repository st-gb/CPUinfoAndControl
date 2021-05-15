#pragma once///Include guard

#include "I_CPUaccess.hpp"

class DummyCPUaccess
  : public I_CPUaccess
{
 public: 
  BOOL CpuidEx(
    DWORD dwIndex,
    uint32_t * p_EAX,
    uint32_t * p_EBX,
    uint32_t * p_ECX,
    uint32_t * p_EDX,
    DWORD_PTR affinityMask
   )
  {
    return false; 
  }
  BYTE GetNumberOfCPUCores(){return 1;}
  
  BOOL ReadTSC(uint32_t & r_lowmostBits , uint32_t & r_highmostBits ){
   return FALSE; }
  BOOL ReadTSCinOrder(
    uint32_t & r_lowmostBits ,
    uint32_t & r_highmostBits ,
    DWORD dwThreadAffinityMask
    )
  { return FALSE ; }
};

