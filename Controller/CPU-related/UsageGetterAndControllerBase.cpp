/*
 * CPUcoreUsageGetterAndControllerBase.cpp
 *
 *  Created on: 10.04.2010
 *      Author: Stefan
 */

#include <Controller/CPU-related/UsageGetterAndControllerBase.hpp>
#include <Controller/I_CPUaccess.hpp>

CPUcoreUsageGetterAndControllerBase::CPUcoreUsageGetterAndControllerBase()
  : mp_cpuaccess (NULL)
{
  // TODO Auto-generated constructor stub

}

CPUcoreUsageGetterAndControllerBase::~CPUcoreUsageGetterAndControllerBase() {
  // TODO Auto-generated destructor stub
}

I_CPUaccess * CPUcoreUsageGetterAndControllerBase::GetCPUaccess()
{
  return mp_cpuaccess ;
}

//If inline: linker error (maybe this method is NOT available
//in derived classes then).
//inline
bool // TRUE: success, FALSE: failure
  CPUcoreUsageGetterAndControllerBase::RdmsrEx(
  DWORD dwIndex,        // MSR index
  DWORD & dwLowmostBits,            // bit  0-31 (register "EAX")
  DWORD & dwHighmostBits,           // bit 32-63 (register "EDX")
  DWORD_PTR dwAffinityMask  // Thread Affinity Mask
  )
{
  if( mp_cpuaccess )
    return mp_cpuaccess->RdmsrEx(
      dwIndex,//&dwEAX
      & dwLowmostBits,//&dwEDX
      & dwHighmostBits,
      dwAffinityMask
      );
  return false ;
}

void CPUcoreUsageGetterAndControllerBase::SetCPUaccess(I_CPUaccess * p_cpuaccess)
{
  mp_cpuaccess = p_cpuaccess ;
}
