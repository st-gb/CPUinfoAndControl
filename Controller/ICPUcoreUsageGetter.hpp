#pragma once //incl. guard
#include "../global.h"

//This interface exists because there are many possib. to
//get the CPU core usage.
class ICPUcoreUsageGetter
{
public:
  //virtual float GetUsageForCore(BYTE byCore) = 0 ;
  //virtual //float 
  //  BYTE GetPercentalUsageForBothCores(float r_arf[] ) = 0 ;
  virtual //float 
    BYTE GetPercentalUsageForAllCores(float arf[] ) = 0 ;
  //virtual float Init() = 0 ;//{return 0.0f; }
  virtual BYTE Init() = 0 ;//{return 0.0f; }
};
