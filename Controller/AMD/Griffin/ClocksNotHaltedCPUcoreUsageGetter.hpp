#pragma once

#include <global.h> //for DWORD
#include <Windows.h> //for ULONGLONG
#include <Controller/AMD/Griffin/GriffinController.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>
#include <ModelData/ModelData.hpp>

#ifndef _MSC_VER //if no MS compiler
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
class GriffinController ;
class Model ;
//class ICPUcoreUsageGetter ;

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
  //bool m_bAtLeastSecondTime ;
  DWORD m_dwAtMask2ndTimeCPUcoreMask ;
  DWORD m_dwLowmostBits , m_dwHighmostBits ;
  DWORD m_dwAffinityMask ;
  GriffinController * mp_griffincontroller ;
  ULONGLONG m_ullPerformanceEventCounter3Diff ;
  //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
  ULONGLONG m_ull ;
  //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
  //ULONGLONG m_ullPreviousTimeStampCounterValue ;
  ULONGLONG m_ullTimeStampCounterValueDiff ;
  ULONGLONG m_ullPerformanceEventCounter3 ;
public:
  //Use the variable declared as member and not as local variable, 
  //so they aren't created for every method call
  ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts * m_ar_cnh_cpucore_ugpca ;
  //ClocksNotHaltedCPUcoreUsageGetter() ;
  ClocksNotHaltedCPUcoreUsageGetter(
    //BYTE byCoreID ,
    WORD dwAffinityMask ,
    GriffinController * p_griffincontroller 
    ) ;
  ~ClocksNotHaltedCPUcoreUsageGetter()
  {
    delete [] m_ar_cnh_cpucore_ugpca ;
  }
  BYTE GetPercentalUsageForAllCores(float arf[] ) ; //{ return 0 ; } ;
  BYTE Init() ;

  float GetPercentalUsageForCore(BYTE byCoreID) ;

} ; //end class
