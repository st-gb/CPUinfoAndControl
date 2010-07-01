#pragma once //incl. guard
#include <global.h>
//#include "I_CPUcontroller.hpp"

class I_CPUcontroller ;
class Model ;

//This interface exists because there are many possib. to
//get the CPU core usage.
class ICPUcoreUsageGetter
{
public:
  //Must be a pointer because this is the base class,
  I_CPUcontroller * mp_cpucontroller ;
  Model * mp_model ;
  //virtual float GetUsageForCore(BYTE byCore) = 0 ;
  //virtual //float 
  //  BYTE GetPercentalUsageForBothCores(float r_arf[] ) = 0 ;

  //Must be virtual for the destructor of subclasses to be called?!
  virtual ~ICPUcoreUsageGetter()
  //Also needs to be defined (else when linking: undefined reference to it)
  {
    DEBUGN("~ICPUcoreUsageGetter()")
  } ;
  virtual WORD GetNumberOfLogicalCPUcores() { return 0 ; }
  virtual //float 
    BYTE GetPercentalUsageForAllCores(float arf[] ) = 0 ;
  //virtual float Init() = 0 ;//{return 0.0f; }
  virtual BYTE Init() //= 0 ;
    {return 0 ; }
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
  void SetAttributeData( Model * p_model )
  {
    mp_model = p_model ;
  }
};
