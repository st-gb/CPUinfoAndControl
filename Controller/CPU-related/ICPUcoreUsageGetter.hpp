/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl. guard
//#include <global.h>

////class CPUcoreUsageGetterAndControllerBase
//#include <Controller/CPU-related/UsageGetterAndControllerBase.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#include <windef.h> //for BYTE, WORD
//#include "I_CPUcontroller.hpp"

class I_CPUcontroller ;
class Model ;

//This interface exists because there are many possibilities to
//get the CPU core usage.
class ICPUcoreUsageGetter
//  :
//  public CPUcoreUsageGetterAndControllerBase
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
