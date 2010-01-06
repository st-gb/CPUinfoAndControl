#pragma once

#include <Controller/I_CPUcontroller.hpp>
//#include <windows.h> //ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG

#define IA32_APERF 0xE8 //dec. 232

class NehalemController
  : public I_CPUcontroller
{
  //Model * mp_model ;
public:
  void DecreaseVoltageBy1Step(float & r_fVoltage) ;
  BYTE GetCurrentPstate(WORD & wFreqInMHz, float & Volt, BYTE byCoreID) ;
  BYTE GetCurrentPstate(
    BYTE & r_byFreqID
    , BYTE & r_byVoltageID
    , BYTE byCoreID
    ) ;
  NehalemController(void);
  BYTE Init() ;
  void GetAllPossibleFrequencies(std::set<VoltageAndFreq> & 
    r_stdsetvoltageandfreq) ;
  WORD GetNumberOfPstates() ;
  //Intel soft dev man vol 3 A  8.6
  //Addressable IDs for Logical processors in the same Package
//(CPUID.1:EBX[23:16]) — Indicates the maximum number of addressable
//ID for logical processors in a physical package. Within a physical package,
//there may be addressable IDs that are not occupied by any logical
//processors. This parameter does not represents the hardware capability of
//the physical processor.2

  WORD GetNumberOfCPUcores() {
    //not always correct
    return 8 ; }
  WORD GetMaximumFrequencyInMHz() ;
  WORD GetMinimumFrequencyInMHz() ;
  WORD GetMaximumVoltageID() ;
  WORD GetMinimumVoltageID() ;
  inline float GetMinimumVoltageInVolt() ;
  //WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) ;
  //bool GetPstateSafefy(
  //  WORD wFreqInMHz
  //  , float fVoltage 
  //  ) ;
  WORD GetVoltageID(float fVoltageInVolt ) ;
  float GetVoltageInVolt(WORD wVoltageID ) ;
  void IncreaseVoltageBy1Step(float & r_fVoltage) ;
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
  ~NehalemController(void);
};
