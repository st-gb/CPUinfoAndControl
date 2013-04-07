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
//#include <Windows_compatible_typedefs.h> //for ULONGLONG
#include <winnt.h> //for ULONGLONG
#include <limits.h>
//#include <Controller/GriffinController.hpp>
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
//#include <ModelData/ModelData.hpp>

//pre-declare
class I_CPUaccess ;
//class NehalemController ;
class Model ;
//class ICPUcoreUsageGetter ;

namespace Nehalem
{

  class ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts
  {
  public :
    DWORD m_dwTickCount ; //for storing the  milliseconds passed
    ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    ULONGLONG m_ullPreviousTimeStampCounterValue ;
  } ;

  class ClocksNotHaltedCPUcoreUsageGetter
    : public ICPUcoreUsageGetter
  {
  private :
    BYTE m_byPerfCounterBitWidth ;
    BYTE m_byPerfCounterNumber ;
    BYTE m_byNumberOfLogicalCPUcores ;
    //bool m_bAtLeastSecondTime ;
    double m_dReferenceClockInHz ;
    DWORD m_dwAtMask2ndTimeCPUcoreMask ;
    DWORD m_dwLowmostBits , m_dwHighmostBits ;
    DWORD m_dwAffinityMask ;
    DWORD m_dwTickCount ; //for getting the milliseconds passed
    DWORD m_dwTickCountDiff ;
    //Model * mp_model ;
    //NehalemController * mp_nehalem_controller ;
    ULONGLONG m_ullPerformanceEventCounter3Diff ;
    I_CPUaccess * mp_cpuaccess ;
    //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    ULONGLONG m_ull ;
    //ULONGLONG m_ullPreviousPerformanceEventCounter3 ;
    //ULONGLONG m_ullPreviousTimeStampCounterValue ;
    ULONGLONG m_ullTimeStampCounterValueDiff ;
    ULONGLONG m_ullPerformanceEventCounter3 ;
    ULONGLONG m_ullMaximumPerfCounterValue ;
  public:
    void AllocateCPUcoreAttributeArray() ;
    //Use the variable declared as member and not as local variable, 
    //so they aren't created for every method call
    ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts * m_ar_cnh_cpucore_ugpca ;
    //ClocksNotHaltedCPUcoreUsageGetter() ;
    ClocksNotHaltedCPUcoreUsageGetter(
      //BYTE byCoreID ,
      DWORD dwAffinityMask
      //, NehalemController * p_nehalemcontroller
      , I_CPUaccess *
      , BYTE byNumCPUcores
      ) ;
    ClocksNotHaltedCPUcoreUsageGetter() ;
    ~ClocksNotHaltedCPUcoreUsageGetter()
    {
      if( m_ar_cnh_cpucore_ugpca )
        delete [] m_ar_cnh_cpucore_ugpca ;
    }
    I_CPUaccess * GetCPUaccess( )
    {
      return mp_cpuaccess ; 
    }
    BYTE GetCurrentPstate(
      BYTE & r_byFreqID
      , BYTE & r_byVoltageID
      , BYTE byCoreID
      ) ;
    BYTE GetPercentalUsageForAllCores(float arf[] ) ; //{ return 0 ; } ;
    float GetPercentalUsageForCore(BYTE byCoreID) ;
    void GetPerfCounterAttributes() ;
    BYTE Init() ;
    void Init( DWORD dwAffinityMask , WORD wNumLogicalCPUcores ) ;
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
    void SetCPUaccess( I_CPUaccess * p_i_cpuaccess ) ;
    void SetNumberOfLogicalCPUcores( BYTE byNum ) ;
  } ; //end class
}; //end namespace PentiumM
