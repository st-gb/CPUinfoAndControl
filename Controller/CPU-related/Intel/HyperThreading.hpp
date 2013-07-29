#ifndef HYPERTHREADING_HPP_
#define HYPERTHREADING_HPP_

#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //CPUID(...)

namespace Intel
{
  inline bool IsHyperThreaded()
  {
    DWORD dwEAX, dwEBX, dwECX , dwEDX ;
    bool bRet = CPUID(
      //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
      //EDX bit 28 ht   Hyper-threading
      0x00000001
      , & dwEAX
      , & dwEBX
      , & dwECX
      , & dwEDX
      , 1
      ) ;
    if( bRet )
    { //TODO use something like "(dwEDX & 0x8000000)" 
      if( (dwEDX >> 28) & 1 )
        return true;
    }
    return false;
  }
} //namespace Intel

#endif //HYPERTHREADING
