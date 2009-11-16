#pragma once

#include <Controller/I_CPUcontroller.hpp>
//#include <windows.h> //ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG

class PentiumM_Controller
  : public I_CPUcontroller
{
  //Model * mp_model ;
public:
  BYTE GetCurrentPstate(WORD & wFreqInMHz, float & Volt, BYTE byCoreID) ;
  BYTE GetCurrentPstate(
    BYTE & r_byFreqID
    , BYTE & r_byVoltageID
    , BYTE byCoreID
    ) ;
  PentiumM_Controller(void);
  BYTE Init() ;
  WORD GetNumberOfPstates() ;
  WORD GetMaximumFrequencyInMHz() ;
  WORD GetMinimumFrequencyInMHz() ;
  WORD GetMaximumVoltageID() ;
  WORD GetMinimumVoltageID() ;
  //WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) ;
  //bool GetPstateSafefy(
  //  WORD wFreqInMHz
  //  , float fVoltage 
  //  ) ;
  WORD GetVoltageID(float fVoltageInVolt ) ;
  float GetVoltageInVolt(WORD wVoltageID ) ;
  void IncreaseVoltageForCurrentPstate(BYTE byCoreID) ;
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
  BYTE SetVoltageAndFrequency( float fVolt, WORD wFreqInMHz ) ;
  void SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , BYTE byCoreID) ;
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
