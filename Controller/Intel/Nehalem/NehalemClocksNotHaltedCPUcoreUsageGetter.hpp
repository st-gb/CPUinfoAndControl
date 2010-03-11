#pragma once

#include <global.h> //for DWORD
//#include <Windows.h> //for ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG
#include <limits.h>
//#include <Controller/GriffinController.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>
#include <ModelData/ModelData.hpp>

#ifndef _MSC_VER //MS compiler has _UI64_MAX defined, but not g++
  #define _UI64_MAX ULONG_LONG_MAX
#endif

#define ULONGLONG_VALUE_DIFF(current,prev) \
  /* the performance counter values are increasing in time except a value wrap
  occured */ \
  (current) < (prev) ? \
  /* for understanding this calculation: 
    example: wrap at 255: current value: 2 old value : 250 
    -> correct value is "maximum value" minus "old value" + 
    "current value" + 1 (because first value is "0")
    = 255 - 250 + 2 + 1 = 5 + 2 + 1 = 8 
    example: wrap at 255: current value: 0 old value : 255: 255-255 + 0 + 1 = 1
  */ \
  (_UI64_MAX - (prev) ) + (current) + 1 \
  : \
  (current) - (prev)

//pre-declare
//class GriffinController ;
class NehalemController ;
class Model ;
//class ICPUcoreUsageGetter ;

namespace Nehalem
{

  class ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts
  {
  public :
    ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    ULONGLONG m_ullPreviousTimeStampCounterValue ;
  } ;

  class ClocksNotHaltedCPUcoreUsageGetter
    : public ICPUcoreUsageGetter
  {
  private :
    BYTE m_byPerfCounterBitWidth ;
    BYTE m_byPerfCounterNumber ;
    //bool m_bAtLeastSecondTime ;
    double m_dReferenceClockInMhz ;
    DWORD m_dwAtMask2ndTimeCPUcoreMask ;
    DWORD m_dwLowmostBits , m_dwHighmostBits ;
    DWORD m_dwAffinityMask ;
    //Model * mp_model ;
    //NehalemController * mp_nehalem_controller ;
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
      , NehalemController * p_nehalemcontroller
      ) ;
    ~ClocksNotHaltedCPUcoreUsageGetter()
    {
      delete [] m_ar_cnh_cpucore_ugpca ;
    }
    BYTE GetCurrentPstate(
      BYTE & r_byFreqID
      , BYTE & r_byVoltageID
      , BYTE byCoreID
      ) ;
    BYTE GetPercentalUsageForAllCores(float arf[] ) ; //{ return 0 ; } ;
    float GetPercentalUsageForCore(BYTE byCoreID) ;
    BYTE Init() ;
    ULONGLONG PerformanceCounterValueDiff(
      ULONGLONG ullPerformanceCounterCurrent ,
      ULONGLONG ullPerformanceCounterPrevious ) ;
    void PerformanceEventSelectRegisterWrite(
      DWORD dwAffinityBitMask ,
      //Pentium M has 1 or 2 "Performance Event Select Register" from 
      //  MSR ... to MSR ...  for 
      // 1 or 2 "Performance Event Counter Registers" from 
      //  ... to ...
      //  that store the 48 bit counter value
      BYTE byPerformanceEventSelectRegisterNumber ,
      BYTE byEventSelect , //8 bit
      BYTE byUnitMask , // 8 bit
      bool bUserMode,
      bool bOSmode,
      bool bEdgeDetect,
      bool bPINcontrol,
      bool bEnableAPICinterrupt,
      //Intel vol. 3B (document # 253659):
      //"When set, performance counting is
      //enabled in the corresponding performance-monitoring counter; when clear, the
      //corresponding counter is disabled. The event logic unit for a UMASK must be
      //disabled by setting IA32_PERFEVTSELx[bit 22] = 0, before writing to
      //IA32_PMCx."
      bool bEnablePerformanceCounter,
      bool bInvertCounterMask ,
      BYTE byCounterMask
      ) ;
  } ; //end class
}; //end namespace PentiumM
