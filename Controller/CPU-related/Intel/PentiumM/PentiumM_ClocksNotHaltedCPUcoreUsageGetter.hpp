/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <global.h> //for DWORD
//#include <Windows.h> //for ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG
#include "UsageGetterAndControllerBase.hpp"
#include <limits.h>
//#include <Controller/GriffinController.hpp>
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <ModelData/ModelData.hpp>

//pre-declare
class PentiumM_Controller ;
class Model ;
//class ICPUcoreUsageGetter ;

namespace PentiumM
{

  class ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts
  {
  public :
    ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    ULONGLONG m_ullPreviousTimeStampCounterValue ;
  } ;

  class ClocksNotHaltedCPUcoreUsageGetter
    : public ICPUcoreUsageGetter
    , public UsageGetterAndControllerBase //for PerformanceCounterValueDiff()
  {
  private :
    BYTE m_byPerfCounterBitWidth ;
    BYTE m_byPerfCounterNumber ;
    //bool m_bAtLeastSecondTime ;
    DWORD m_dwAtMask2ndTimeCPUcoreMask ;
    DWORD m_dwLowmostBits , m_dwHighmostBits ;
    DWORD m_dwAffinityMask ;
    Model * mp_model ;
    //GriffinController * mp_griffincontroller ;
    PentiumM_Controller * mp_pentium_m_controller ;
    ULONGLONG m_ullPerformanceEventCounter3Diff ;
    //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    ULONGLONG m_ull ;
    //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    //ULONGLONG m_ullPreviousTimeStampCounterValue ;
    ULONGLONG m_ullTimeStampCounterValueDiff ;
    ULONGLONG m_ullPerformanceEventCounter3 ;
    ULONGLONG m_ullMaximumPerfCounterValue ;
  public:
    //Use the variable declared as member and not as local variable, 
    //so they aren't created for every method call
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts * m_ar_cnh_cpucore_ugpca ;
    //ClocksNotHaltedCPUcoreUsageGetter() ;
    ClocksNotHaltedCPUcoreUsageGetter(
      //BYTE byCoreID ,
      DWORD dwAffinityMask
      //, GriffinController * p_griffincontroller 
      , PentiumM_Controller * p_pentium_m_controller
      ) ;
    ~ClocksNotHaltedCPUcoreUsageGetter()
    {
      delete [] m_ar_cnh_cpucore_ugpca ;
    }
    BYTE GetPercentalUsageForAllCores(float arf[] ) ; //{ return 0 ; } ;
    BYTE Init() ;

    float GetPercentalUsageForCore(BYTE byCoreID) ;

  } ; //end class
}; //end namespace PentiumM
