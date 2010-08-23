#pragma once

#include "UsageGetterAndControllerBase.hpp"
#include <Controller/CPU-related/I_CPUcontroller.hpp>
//#include <windows.h> //ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG

//#define GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE

class I_CPUaccess ;

class PentiumM_Controller
  :
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
    public I_CPUcontroller
    ,
#endif
    public PentiumM::UsageGetterAndControllerBase //for PerformanceCounterValueDiff()
{
#ifdef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  I_CPUaccess * mp_cpuaccess ;
#endif //#ifdef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Model * mp_model ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  bool m_b2ndTimeOrLaterReadTSCandFIDchange ;
  double m_dCurrentTSCclockInHz ;
  DWORD m_dwTickCountInMilliseconds ;
  DWORD m_dwTickCountInMillisecondsPrev ;
  DWORD m_dwTickCountDiffInMilliseconds ;
  DWORD m_dwMinimumTimeSpanInMilliseconds ;
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  float m_fCurrentReferenceClockInMHz ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  ULONGLONG m_ullPerformanceEventCounterNumberOfFIDchange ;
  ULONGLONG m_ullNumberOfFIDchangePrevious ;
  ULONGLONG m_ullNumberOfFIDchangeDiff ;
  ULONGLONG m_ullTSCvalue ;
  ULONGLONG m_ullTSCdiff ;
  ULONGLONG m_ullTSCvaluePrevious ;
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
public:
  void DecreaseVoltageBy1Step(float & r_fVoltage) ;
//  BYTE GetCurrentPstate(WORD & wFreqInMHz, float & Volt, BYTE byCoreID) ;
  BYTE GetCurrentVoltageAndFrequency(
    float & r_fVoltage
    , float & r_fMultiplier
    , float & r_fReferencClockInMHz
    ) ;
  BYTE GetCurrentPstate(
    BYTE & r_byFreqID
    , BYTE & r_byVoltageID
    , BYTE byCoreID
    ) ;
  PentiumM_Controller(void);
  BYTE Init() ;
  void GetAllPossibleFrequencies(std::set<VoltageAndFreq> & 
    r_stdsetvoltageandfreq) ;
//  WORD GetNumberOfPstates() ;
//  WORD GetMaximumFrequencyInMHz() ;
//  WORD GetMinimumFrequencyInMHz() ;
  WORD GetMaximumVoltageID() ;
  WORD GetMinimumVoltageID() ;
  inline float GetMinimumVoltageInVolt() ;
  //WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
//  BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) ;
  //bool GetPstateSafefy(
  //  WORD wFreqInMHz
  //  , float fVoltage 
  //  ) ;
  WORD GetVoltageID(float fVoltageInVolt ) ;
  float GetVoltageInVolt(WORD wVoltageID ) ;
  void IncreaseVoltageBy1Step(float & r_fVoltage) ;
//  void IncreaseVoltageForCurrentPstate(BYTE byCoreID) ;
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
  inline void MonitorNumberOfFrequencyIDtransitions() ;
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
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
    bool bEnablePerformanceCounter,
    bool bInvertCounterMask ,
    BYTE byCounterMask
    ) ;
#ifdef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  void SetCPUaccess(
    //By using a pointer instead of a reference one can pass NULL
    //to show that there is no object.
    I_CPUaccess * p_cpuaccess )
  {
    mp_cpuaccess = p_cpuaccess ;
  }
#endif
  BYTE SetVoltageAndFrequency( 
    float fVolt , 
    WORD wFreqInMHz ,
    BYTE byCoreID 
    ) ;
  //BYTE SetFreqAndVoltageFromFreq(
  //  WORD wFreqInMHz 
  //  , BYTE byCoreID) ;
  BYTE SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , const std::set<VoltageAndFreq> & 
      cr_stdsetvoltageandfreqForInterpolation
    , BYTE byCoreID ) ;
  BYTE TooHot() ;
  BOOL // TRUE: success, FALSE: failure
    //WINAPI
    WrmsrEx(
      DWORD index,		// MSR index
      DWORD dwLow ,//eax,			// bit  0-31
      DWORD dwHigh, //edx,			// bit 32-63
      DWORD affinityMask	// Thread Affinity Mask
    ) ;
public:
  ~PentiumM_Controller(void);
};
